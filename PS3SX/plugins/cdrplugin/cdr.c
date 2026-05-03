#include <stdlib.h>
#include <string.h>
#include <sys/timer.h>
#include <pthread.h>

#include "cdr.h"

// Defined in plugins.c
extern s64 cdOpenCaseTime;

extern void SPU_playCDDAchannel(short *m, int i);

static volatile int AudioPlaying = 0;
static volatile uint32_t AudioOffset = 0;
static pthread_t AudioThread;

static void* AudioThreadFunction(void *param) {
    unsigned char sndbuffer[2352];
    
    while (AudioPlaying) {
        if (CDR__readCDDA(0, 0, 0, sndbuffer) == 0) {
            SPU_playCDDAchannel((short *)sndbuffer, 2352);
            AudioOffset++;
            
            // Check end of track 0 (total size)
            uint32_t total_sectors = (uint32_t)CD.tl[0].end[0]*60*75 + (uint32_t)CD.tl[0].end[1]*75 + (uint32_t)CD.tl[0].end[2];
            if (AudioOffset >= total_sectors) {
                AudioPlaying = 0;
                break;
            }
        } else {
            AudioPlaying = 0;
            break;
        }
        
        sys_timer_usleep(13333); // 1/75 sec
    }
    
    return NULL;
}

long CDR__init(void) {
    memset(&CD, 0, sizeof(CD));
    return 0;
}

long CDR__shutdown(void) {
    CDR__stop();
    return 0;
}

long CDR__open(void) {
    char str[256];
    if (CDConfiguration.dn[0] != '\0') {
        strcpy(str, CDConfiguration.dn);
        strcat(str, "/");
        strcat(str, CDConfiguration.fn);
    } else {
        strcpy(str, CDConfiguration.fn);
    }
    newCD(str);
    return (CD.cd == NULL) ? -1 : 0;
}

long CDR__close(void) {
    CDR__stop();
    if (CD.cd) {
        fclose(CD.cd);
        CD.cd = NULL;
    }
    if (CD.tl) {
        free(CD.tl);
        CD.tl = NULL;
    }
    return 0;
}

long CDR__getTN(unsigned char *buffer) {
    char numtracks = getNumTracks();
    if (numtracks == -1) return -1;

    buffer[0] = 1;
    buffer[1] = (unsigned char)numtracks;
    return 0;
}

long CDR__getTD(unsigned char track, unsigned char *buffer) {
    if (track > CD.numtracks) return -1;

    unsigned char temp[3];
    if (track == 0) {
        temp[0] = CD.tl[0].end[0];
        temp[1] = CD.tl[0].end[1];
        temp[2] = CD.tl[0].end[2];
    } else {
        temp[0] = CD.tl[track].start[0];
        temp[1] = CD.tl[track].start[1];
        temp[2] = CD.tl[track].start[2];
    }

    // PCSX expects buffer[0]=min, buffer[1]=sec, buffer[2]=frames
    buffer[0] = itob(temp[0]);
    buffer[1] = itob(temp[1]);
    buffer[2] = itob(temp[2]);
    return 0;
}

long CDR__readTrack(unsigned char *time) {
    seekSector(btoi(time[0]), btoi(time[1]), btoi(time[2]));
    return 0;
}

unsigned char *CDR__getBuffer(void) {
    return getSector();
}

unsigned char *CDR__getBufferSub(void) {
    return NULL;
}

long CDR__configure(void) {
    return 0;
}

long CDR__test(void) {
    return 0;
}

void CDR__about(void) {
}

long CDR__play(unsigned char *time) {
    uint32_t offset = MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2]));
    
    if (AudioPlaying) {
        if (AudioOffset == offset) return 0;
        CDR__stop();
    }

    AudioOffset = offset;
    AudioPlaying = 1;
    pthread_create(&AudioThread, NULL, AudioThreadFunction, NULL);
    
    return 0;
}

long CDR__stop(void) {
    if (AudioPlaying) {
        AudioPlaying = 0;
        pthread_join(AudioThread, NULL);
    }
    return 0;
}

long CDR__setfilename(char *filename) {
    strncpy(CDConfiguration.fn, filename, 128);
    return 0;
}

long CDR__getStatus(struct CdrStat *stat) {
    if (CD.cd == NULL) return -1;

    // Based on plugins.c logic
    if (cdOpenCaseTime < 0)
        stat->Status = 0x10; // Shell open
    else
        stat->Status = AudioPlaying ? 0x80 : 0x00;

    stat->Type = Mode2_track; // Default for PSX

    uint32_t sectors = AudioOffset;
    stat->Time[0] = sectors / 75 / 60;
    sectors -= stat->Time[0] * 75 * 60;
    stat->Time[1] = sectors / 75;
    sectors -= stat->Time[1] * 75;
    stat->Time[2] = sectors;

    return 0;
}

long CDR__readCDDA(unsigned char m, unsigned char s, unsigned char f, unsigned char *buffer) {
    if (CD.cd == NULL) return -1;

    uint32_t sector;
    if (m == 0 && s == 0 && f == 0) {
        sector = AudioOffset;
    } else {
        sector = MSF2SECT(m, s, f);
    }

    fseek(CD.cd, (long)sector * 2352, SEEK_SET);
    if (fread(buffer, 1, 2352, CD.cd) != 2352) return -1;

    return 0;
}

// Helpers
void openBin(const char* filename) {
    CD.cd = fopen(filename, "rb");
    if (CD.cd == NULL) return;

    fseek(CD.cd, 0, SEEK_END);
    long size = ftell(CD.cd);
    fseek(CD.cd, 0, SEEK_SET);
    long blocks = size / 2352;

    CD.numtracks = 1;
    CD.tl = (Track*) malloc((CD.numtracks + 1) * sizeof(Track));

    // Track 0: Disk size
    CD.tl[0].type = Mode2_track;
    CD.tl[0].start[0] = 0;
    CD.tl[0].start[1] = 0;
    CD.tl[0].start[2] = 0;
    CD.tl[0].end[2] = blocks % 75;
    CD.tl[0].end[1] = ((blocks - CD.tl[0].end[2]) / 75) % 60;
    CD.tl[0].end[0] = (((blocks - CD.tl[0].end[2]) / 75) - CD.tl[0].end[1]) / 60;
    
    // Track 1
    CD.tl[1].type = Mode2_track;
    CD.tl[1].start[0] = 0;
    CD.tl[1].start[1] = 2;
    CD.tl[1].start[2] = 0;
}

void newCD(const char * filename) {
    openBin(filename);
    CD.bufferPos = 0x7FFFFFFF;
}

char getNumTracks() {
    if (CD.cd == NULL) return -1;
    return (char)CD.numtracks;
}

void seekSector(const unsigned char m, const unsigned char s, const unsigned char f) {
    CD.sector = (( (long)m * 60) + ((long)s - 2)) * 75 + f;
    long byteOffset = CD.sector * 2352;

    if ((byteOffset >= CD.bufferPos) && (byteOffset < (CD.bufferPos + BUFFER_SIZE))) {
        return;
    }
    readit(m, s, f);
}

void readit(const unsigned char m, const unsigned char s, const unsigned char f) {
    long byteOffset = CD.sector * 2352;
    fseek(CD.cd, byteOffset, SEEK_SET);
    fread(CD.buffer, 1, BUFFER_SIZE, CD.cd);
    CD.bufferPos = byteOffset;
}

unsigned char* getSector() {
    long byteOffset = CD.sector * 2352;
    return CD.buffer + (byteOffset - CD.bufferPos) + 12;
}
