#ifndef _PLUGCD_H_
#define _PLUGCD_H_

#include <stdio.h>
#include <stdint.h>
#include "../../pcsxcore/plugins.h"

#define CHAR_LEN 256

// 2352 is a sector size, so cache is 50 sectors
#define BUFFER_SECTORS 50
#define BUFFER_SIZE BUFFER_SECTORS*2352
#define BZIP_BUFFER_SECTORS 10

//  74 minutes * 60 sex/min * 75 frames/sec * 96 bytes needed per frame
#define TOTAL_CD_LENGTH 74*60*75
#define BYTES_PER_SUBCHANNEL_FRAME 96
#define MAX_SUBCHANNEL_DATA TOTAL_CD_LENGTH*BYTES_PER_SUBCHANNEL_FRAME

typedef struct {
	char	dn[128];
	char	fn[128];
} cd_conf;

extern cd_conf CDConfiguration;

extern int rc;

enum TrackType
{
   unknown_track, Mode1_track, Mode2_track, Audio_track, Pregap_track = 0x80
};

enum CDType
{
   unk_cd, Bin_cd, Cue_cd, Rar_cd, IndexBZ_cd, IndexZ_cd, SBI_cd, M3S_cd
};

typedef struct
{
   enum TrackType type;
   char num;
   unsigned char start[3];
   unsigned char end[3];
} Track;

struct
{   
   FILE* cd;
   FILE* cdda;
   int numtracks;
   long bufferPos;
   long sector;
   Track* tl;
   unsigned char buffer[BUFFER_SIZE];
   enum CDType type;
} CD;

void CDDAclose(void);

// function headers for cdreader.c
void openCue(const char* filename);
void openBin(const char* filename);
void openIso(const char* filename);
char getNumTracks();
void seekSector(const unsigned char m, const unsigned char s, const unsigned char f);
unsigned char* getSector();
void newCD(const char * filename);
void readit(const unsigned char m, const unsigned char s, const unsigned char f);


// subtracts two times in integer format (non-BCD) ->  l - r = a
#define sub(l, r, a)\
   a[1] = 0;\
   a[0] = 0;\
   a[2] = l[2] - r[2];\
   if ((char)a[2] < 0)\
   {\
      a[2] += 75;\
      a[1] -= 1;\
   }\
   a[1] += l[1] - r[1];\
   if ((char)a[1] < 0)\
   {\
      a[1] += 60;\
      a[0] -= 1;\
   }\
   a[0] += l[0] - r[0];\

// converts a time like 17:61:00  to 18:01:00
#define normalizeTime(c)\
   while(c[2] > 75)\
   {\
      c[2] -= 75;\
      c[1] += 1;\
   }\
   while(c[1] > 60)\
   {\
      c[1] -= 60;\
      c[0] += 1;\
   }

// use btoi / itob from pcsxcore/CdRom.h via plugins.h -> psxcommon.h -> cdrom.h?
// Actually they are in CdRom.h which is included in psxcommon.h? No.
// Let's re-define them if needed or use the ones from psxcommon.h
#ifndef btoi
#define btoi(b)     ((b) / 16 * 10 + (b) % 16)
#endif
#ifndef itob
#define itob(i)     ((i) / 10 * 16 + (i) % 10)
#endif
#ifndef MSF2SECT
#define MSF2SECT(m, s, f)               (((m) * 60 + (s) - 2) * 75 + (f))
#endif

// Prototypes for cdr.c
long CDR__open(void);
long CDR__init(void);
long CDR__shutdown(void);
long CDR__close(void);
long CDR__getTN(unsigned char *buffer);
long CDR__getTD(unsigned char track, unsigned char *buffer);
long CDR__readTrack(unsigned char *time);
unsigned char *CDR__getBuffer(void);
unsigned char *CDR__getBufferSub(void);
long CDR__configure(void);
long CDR__test(void);
void CDR__about(void);
long CDR__play(unsigned char *time);
long CDR__stop(void);
long CDR__setfilename(char *filename);
long CDR__getStatus(struct CdrStat *stat);
long CDR__readCDDA(unsigned char m, unsigned char s, unsigned char f, unsigned char *buffer);

#endif
