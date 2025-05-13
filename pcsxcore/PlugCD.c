#include "plugins.h"
#include "PlugCD.h"

// gets track 
long getTN(unsigned char* buffer)
{
 int numtracks = getNumTracks();

//SysPrintf("start getTn()\n");

 if (-1 == numtracks)
 {
//        SysPrintf("end getTn()\n");
         return -1;
 }

 buffer[0]=1;
 buffer[1]=numtracks;

//   printf("getnumtracks %d %d\n", (int)buffer[0], (int)buffer[1]);
//SysPrintf("end getTn()\n");
   return 0;
}

 // if track==0 -> return total length of cd
 // otherwise return start in bcd time format
long getTD(int track, unsigned char* buffer)
{
      // lasttrack just keeps track of which track TD was requested last (go fig)
//	SysPrintf("start getTD()\n");

   if (track > CD.numtracks)
   {
//      printf("getTD bad %2d\n", track);
      return -1;
   }

   if (track == 0)
   {
      buffer[0] = CD.tl[track].end[0];
      buffer[1] = CD.tl[track].end[1];
      buffer[2] = CD.tl[track].end[2];
   }
   else
   {
      buffer[0] = CD.tl[track].start[0];
      buffer[1] = CD.tl[track].start[1];
      buffer[2] = CD.tl[track].start[2];
   }
//   printf("getTD %2d %02d:%02d:%02d\n", track, (int)buffer[0],
//         (int)buffer[1], (int)buffer[2]);

   // bcd encode it
   buffer[0] = intToBCD(buffer[0]);
   buffer[1] = intToBCD(buffer[1]);
   buffer[2] = intToBCD(buffer[2]);
//   SysPrintf("end getTD()\n");
   return 0;
}

// opens a binary cd image and calculates its length
void openBin(const char* filename)
{
    long end, size, blocks;

//    SysPrintf("start openBin()\n");

    CD.cd = fopen(filename, "rb");

    printf("CD.cd = %08x\n",CD.cd);

//	if (CD.cd == NULL) CD.cd = -1;
	
/*    if (CD.cd == 0)
    {
        SysPrintf("Error opening cd\n");
        exit(0);
    }
*/
    end = fseek(CD.cd, 0, SEEK_END);
    end = ftell(CD.cd);
    size = end;
    printf("size of CD in MB = %d\n",size/1048576);

    rc = fseek(CD.cd, 0, SEEK_SET);
    blocks = size / 2352;

    // put the track length info in the track list
    CD.tl = (Track*) malloc(sizeof(Track));

    CD.tl[0].type = Mode2;
    CD.tl[0].num = 1;
    CD.tl[0].start[0] = 0;
    CD.tl[0].start[1] = 0;
    CD.tl[0].start[2] = 0;
    CD.tl[0].end[2] = blocks % 75;
    CD.tl[0].end[1] = ((blocks - CD.tl[0].end[2]) / 75) % 60;
    CD.tl[0].end[0] = (((blocks - CD.tl[0].end[2]) / 75) - CD.tl[0].end[1]) / 60;

    CD.tl[0].start[1] += 2;
    CD.tl[0].end[1] += 2;

    normalizeTime(CD.tl[0].end);

    CD.numtracks = 1;

//    SysPrintf("end openBin()\n");
}

void addBinTrackInfo()
{
//    SysPrintf("start addBinTrackInfo()\n");

    CD.tl = realloc(CD.tl, (CD.numtracks + 1) * sizeof(Track));
    CD.tl[CD.numtracks].end[0] = CD.tl[0].end[0];
    CD.tl[CD.numtracks].end[1] = CD.tl[0].end[1];
    CD.tl[CD.numtracks].end[2] = CD.tl[0].end[2];
    CD.tl[CD.numtracks].start[0] = CD.tl[0].start[0];
    CD.tl[CD.numtracks].start[1] = CD.tl[0].start[1];
    CD.tl[CD.numtracks].start[2] = CD.tl[0].start[2];

//    SysPrintf("end addBinTrackInfo()\n");
}

// new file types should be added here and in the CDOpen function
void newCD(const char * filename)
{
//    SysPrintf("start newCD()\n");

    CD.type = Bin;
    openBin(filename);
    addBinTrackInfo();

    CD.bufferPos = 0x7FFFFFFF;
    seekSector(0,2,0);

//    SysPrintf("end newCD\n");
}


// return the sector address - the buffer address + 12 bytes for offset.
unsigned char* getSector()
{
//    SysPrintf("getSector()\n");
    return CD.buffer + (CD.sector - CD.bufferPos) + 12;
}

// returns the number of tracks
char getNumTracks()
{
//    SysPrintf("start getNumTracks()\n");
    // if there's no open cd, return -1
    if (CD.cd == 0) {
        return -1;
    }

//    printf("numtracks %d\n",CD.numtracks);
//    SysPrintf("end getNumTracks()\n");
    return CD.numtracks;
}

void readit(const unsigned char m, const unsigned char s, const unsigned char f)
{
//    SysPrintf("start readit()\n");
//    printf(" not cached %08x %08x\n",CD.sector - CD.bufferPos, BUFFER_SIZE);

    // fakie ISO support.  iso is just cd-xa data without the ecc and header.
    // read in the same number of sectors then space it out to look like cd-xa
    /*      if (CD.type == Iso)
          {
             unsigned char temptime[3];
             long tempsector = (( (m * 60) + (s - 2)) * 75 + f) * 2048;
             fs_seek(CD.cd, tempsector, SEEK_SET);
             fs_read(CD.buffer, sizeof(unsigned char), 2048*BUFFER_SECTORS, CD.cd);

             // spacing out the data here...
             for(tempsector = BUFFER_SECTORS - 1; tempsector >= 0; tempsector--)
             {
                memcpy(&CD.buffer[tempsector*2352 + 24],&CD.buffer[tempsector*2048], 2048);

                // two things - add the m/s/f flags in case anyone is looking
                // and change the xa mode to 1
                temptime[0] = m;
                temptime[1] = s;
                temptime[2] = f + (unsigned char)tempsector;
                
                normalizeTime(temptime);
                CD.buffer[tempsector*2352+12] = intToBCD(temptime[0]);
                CD.buffer[tempsector*2352+12+1] = intToBCD(temptime[1]);
                CD.buffer[tempsector*2352+12+2] = intToBCD(temptime[2]);
                CD.buffer[tempsector*2352+12+3] = 0x01;
             }
          }
          else */
    {
        rc = fseek(CD.cd, CD.sector, SEEK_SET);
//        printf(" seek1 rc %d\n", rc);
        rc = fread(CD.buffer, sizeof(unsigned char) * BUFFER_SIZE, 1, CD.cd);
//        printf(" seek2 rc %d\n", rc);
    }

    CD.bufferPos = CD.sector;

//    SysPrintf("end readit()\n");
}


void seekSector(const unsigned char m, const unsigned char s, const unsigned char f)
{
//    SysPrintf("start seekSector()\n");

    // calc byte to search for
    CD.sector = (( (m * 60) + (s - 2)) * 75 + f) * 2352;

//    printf("seek %d %02d:%02d:%02d",CD.sector, (int)m, (int)s, (int)f);

    // is it cached?
    if ((CD.sector >= CD.bufferPos) &&
            (CD.sector < (CD.bufferPos + BUFFER_SIZE)) ) {
//        printf(" cached %d %d\n",CD.sector - CD.bufferPos,BUFFER_SIZE);
//       SysPrintf("end seekSector()\n");
        return;
    }
    // not cached - read a few blocks into the cache
    else
    {
        readit(m,s,f);
    }
//    SysPrintf("end seekSector()\n");
}

long CDR__open(void)
{
	char str[256];
/*	
    SysPrintf("start CDR_open()\n");

    //	newCD("/cd/cd.bin");
	strcpy(str, CDConfiguration.dn);
	strcat(str, "/");
	strcat(str, CDConfiguration.fn);
	
	newCD(str);

    SysPrintf("end CDR_open()\n");
*/  
  return 0;
}

long CDR__init(void)
{

	return 0;
}

long CDR__shutdown(void) {
    return 0;
}

long CDR__close(void) {
    SysPrintf("start CDR_close()\n");
    fclose(CD.cd);
    free(CD.tl);
    SysPrintf("end CDR_close()\n");
    return 0;
}

long CDR__getTN(unsigned char *buffer) {
//    SysPrintf("start CDR_getTN()\n");
//    SysPrintf("end CDR_getTN()\n");
    return getTN(buffer);
//    return 0;
}

long CDR__getTD(unsigned char track, unsigned char *buffer) {
//    SysPrintf("start CDR_getTD()\n");
//    printf("getTD from track %d\n", track);

   unsigned char temp[3];
   int result = getTD((int)track, temp);

   if (result == -1) return -1;

   buffer[1] = temp[1];
   buffer[2] = temp[0];

//    SysPrintf("end CDR_getTD()\n");
    return 0;
}

long CDR__readTrack(unsigned char *time) {
//    SysPrintf("start CDR_readTrack()\n");
//    printf("readTrack at %02d:%02d:%02d\n", BCDToInt(time[0]), BCDToInt(time[1]), BCDToInt(time[2]));

    //	cdrom_reinit();

    seekSector(BCDToInt(time[0]), BCDToInt(time[1]), BCDToInt(time[2]));

//    SysPrintf("end CDR_readTrack()\n");
    return 0;
}

unsigned char *CDR__getBuffer(void) {
//    SysPrintf("start CDR_getBuffer()\n");
    return getSector();
}

/*
long (CALLBACK* CDRconfigure)(void);
long (CALLBACK* CDRtest)(void);
void (CALLBACK* CDRabout)(void);
long (CALLBACK* CDRplay)(unsigned char *);
long (CALLBACK* CDRstop)(void);
*/
