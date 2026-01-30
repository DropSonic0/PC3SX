#include <stdint.h>
#include <mednafen/mednafen.h>
#include <mednafen/cdrom/cdromif.h>

struct CdrStat {
    uint32_t Type;
    uint32_t Status;
    unsigned char Time[3];
};

struct SubQ {
    char res0[12];
    unsigned char ControlAndADR;
    unsigned char TrackNumber;
    unsigned char IndexNumber;
    unsigned char TrackRelativeAddress[3];
    unsigned char Filler;
    unsigned char AbsoluteAddress[3];
    unsigned char CRC[2];
    char res1[72];
};

static uint8_t	Buffer[8192];

//BCD to int
#define btoi(b)     					((b) / 16 * 10 + (b) % 16)
//MSF to sector
#define MSF2SECT(m, s, f)               (((m) * 60 + (s) - 2) * 75 + (f))

#ifdef CDAUDIOTEST
// this thread plays audio data
void			pkSPUplayCDDAchannel	(short * m, int i);
//static uint16_t *iso_play_cdbuf;
//static uint16_t iso_play_bufptr;
//static uint32_t CDASector;
static MDFN_Thread*					AudioThread;
static volatile int32_t				AudioPlaying = 0;
static volatile uint32_t			AudioInitalOffset = 0;
static volatile uint32_t			AudioOffset = 0;
static uint8_t						AudioBuffer[2352 * 10];
#define CDDA_FRAMETIME			(1000 * (sizeof(sndbuffer) / 2352) / 75)
static int 							AudioThreadFunction				(void *param)
{
	long			d, t, i, s;
	unsigned char	tmp;
	int sec;

	t = MDFND_GetTime();

	iso_play_cdbuf = 0;
	iso_play_bufptr = 0;

	CD_TOC toc;
	CDIF_ReadTOC(&toc);


	while (playing) {
		d = t - (long)MDFND_GetTime();
		if (d <= 0) {
			d = 1;
		}
		else if (d > CDDA_FRAMETIME) {
			d = CDDA_FRAMETIME;
		}

		MDFND_Sleep(d);
		t = MDFND_GetTime() + CDDA_FRAMETIME;

		//TODO: Subchannels?

		CDIF_ReadRawSector(sndbuffer, AudioOffset ++);
		
		if(AudioOffset >= toc.tracks[100].lba)
		{
			playing = 0;
		}

		if(/*!cdr.Muted &&*/ playing)
		{
			//TODO: Wipe data?
			pkSPUplayCDDAchannel((short *)AudioBuffer, 2352);
		}

#if 0	
		// BIOS CD Player: Fast forward / reverse seek
		if( cdr.FastForward ) {
			// ~+0.25 sec
			cddaCurOffset += CD_FRAMESIZE_RAW * 75 * 3;

#if 0
			// Bad idea: too much static
			if( subChanInterleaved )
				fseek( cddaHandle, s * (CD_FRAMESIZE_RAW + SUB_FRAMESIZE), SEEK_SET );
			else
				fseek( cddaHandle, s * CD_FRAMESIZE_RAW, SEEK_SET );
#endif
		}
		else if( cdr.FastBackward ) {
			// ~-0.25 sec
			cddaCurOffset -= CD_FRAMESIZE_RAW * 75 * 3;
			if( cddaCurOffset & 0x80000000 ) {
				cddaCurOffset = 0;
				cdr.FastBackward = 0;

				playing = 0;
				fclose(cddaHandle);
				cddaHandle = NULL;
				initial_offset = 0;
				break;
			}

#if 0
			// Bad idea: too much static
			if( subChanInterleaved )
				fseek( cddaHandle, s * (CD_FRAMESIZE_RAW + SUB_FRAMESIZE), SEEK_SET );
			else
				fseek( cddaHandle, s * CD_FRAMESIZE_RAW, SEEK_SET );
#endif
		}
#endif
	
		// Vib Ribbon: decoded buffer IRQ
//		iso_play_cdbuf = sndbuffer;
//		iso_play_bufptr = 0;
	}

	return 0;
}
#endif


//Prototypes
extern "C"
{
	long			CDR__getStatus			(struct CdrStat *stat);
	long			pkCDRinit				(void);
	long			pkCDRshutdown			(void);
	long			pkCDRopen				(void);
	long			pkCDRclose				(void);
	long			pkCDRgetTN				(unsigned char * aBuffer);
	long			pkCDRgetTD				(unsigned char aTrack, unsigned char* aBuffer);
	long			pkCDRreadTrack			(unsigned char* aPosition);

	unsigned char*	pkCDRgetBuffer			(void);
	unsigned char*	pkCDRgetBufferSub		(void);
	long			pkCDRconfigure			(void);
	long			pkCDRtest				(void);
	void			pkCDRabout				(void);
	long			pkCDRplay				(unsigned char* aTime);
	long			pkCDRstop				(void);
	long			pkCDRsetfilename		(char* aFileName);
	long			pkCDRgetStatus			(struct CdrStat* aStatus);
	long			pkCDRreadCDDA			(unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer);
}


long			pkCDRinit				(void)
{
	return 0;
}

long			pkCDRshutdown			(void)
{
	return 0;
}

long			pkCDRopen				(void)
{
	return 0;
}

long			pkCDRclose				(void)
{
	return 0;
}

long			pkCDRgetTN				(unsigned char * aBuffer)
{
	CD_TOC toc;
	CDIF_ReadTOC(&toc);

	aBuffer[0] = toc.first_track;
	aBuffer[1] = toc.last_track;

	return 0;
}

long			pkCDRgetTD				(unsigned char aTrack, unsigned char* aBuffer)
{
	//TODO: Special case track zero to return size of disc?

	CD_TOC toc;
	CDIF_ReadTOC(&toc);

	uint32_t sectors = toc.tracks[aTrack + 1].lba - toc.tracks[aTrack].lba;

	aBuffer[2] = sectors / 75 / 60;
	sectors -= aBuffer[2] * 75 * 60;
	aBuffer[1] = sectors / 75;
	sectors -= aBuffer[1] * 75;
	aBuffer[0] = sectors;

	return 0;
}

long			pkCDRreadTrack			(unsigned char* aPosition)
{
	//TODO: Fix
	CD_TOC toc;
	CDIF_ReadTOC(&toc);

	int sector = MSF2SECT(btoi(aPosition[0]), btoi(aPosition[1]), btoi(aPosition[2]));
	sector += toc.tracks[1].lba;
	CDIF_ReadRawSector(Buffer, sector);

#if 0
    if (subChanMixed) {
        fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * (CD_FRAMESIZE_RAW + SUB_FRAMESIZE), SEEK_SET);
        fread(cdbuffer, 1, CD_FRAMESIZE_RAW, cdHandle);
        fread(subbuffer, 1, SUB_FRAMESIZE, cdHandle);

        if (subChanRaw) DecodeRawSubData();
    }
    else {
        if(isMode1ISO) {
            fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * MODE1_DATA_SIZE, SEEK_SET);
            fread(cdbuffer + 12, 1, MODE1_DATA_SIZE, cdHandle);
            memset(cdbuffer, 0, 12); //not really necessary, fake mode 2 header
            cdbuffer[0] = (time[0]);
            cdbuffer[1] = (time[1]);
            cdbuffer[2] = (time[2]);
            cdbuffer[3] = 1; //mode 1
        } else {
            fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * CD_FRAMESIZE_RAW, SEEK_SET);
            fread(cdbuffer, 1, CD_FRAMESIZE_RAW, cdHandle);
        }

        if (subHandle != NULL) {
            fseek(subHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * SUB_FRAMESIZE, SEEK_SET);
            fread(subbuffer, 1, SUB_FRAMESIZE, subHandle);

            if (subChanRaw) DecodeRawSubData();
        }
    }

    return 0;
#endif

	return 0;
}

unsigned char*	pkCDRgetBuffer			(void)
{
	return Buffer + 12;
}

unsigned char*	pkCDRgetBufferSub		(void)
{
	return 0;
}

long			pkCDRconfigure			(void)
{
	return 0;
}

long			pkCDRtest				(void)
{
	return 0;
}

void			pkCDRabout				(void)
{
	return;
}

long			pkCDRplay				(unsigned char* aTime)
{
#ifdef CDAUDIOTEST
	if(pkSPUplayCDDAchannel != NULL)
	{
		int32_t offset = MSF2SECT(time[0], time[1], time[2]);
		if(AudioPlaying)
		{
			if(AudioInitialOffset == offset)
			{
				return;
			}
			pkCDRstop();
		}

		AudioInitialOffset = offset;
		AudioCurrentOffset = AudioInitialOffset;

		AudioPlaying = 1;
		AudioThread = MDFND_CreateThread(AudioTheadFunction, 0);
	}
#endif
	return 0;
}

long			pkCDRstop				(void)
{
#ifdef CDAUDIOTEST
	if(AudioPlaying)
	{
		AudioPlaying = 0;

		MDFND_WaitThread(AudioThread);
		AudioInitialOffset = 0;
	}
#endif

	return 0;
}

long			pkCDRsetfilename		(char* aFileName)
{
	//NOTHING
	return 0;
}

long			pkCDRgetStatus			(struct CdrStat* aStatus)
{
    CDR__getStatus(aStatus);

#ifdef CDAUDIOTEST
	stat->Status |= AudioPlaying ? 0x80 : 0;
	sec2msf(AudioOffset, (u8*)stat->Time);

//TODO:
//BIOS - boot ID (CD type)
//    stat->Type = ti[1].type;
#endif

	return 0;
}

long			pkCDRreadCDDA			(unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer)
{
#ifdef CDAUDIOTEST
	int sector = MSF2SECT(aMinutes, aSeconds, aFrames);
	sector += CDIF_GetTrackStartPositionLBA(1);
	CDIF_ReadRawSector(aBuffer, sector);
#endif

	return 0;
}

