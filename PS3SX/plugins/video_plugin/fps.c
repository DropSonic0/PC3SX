/***************************************************************************
                          fps.c  -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#define _IN_FPS

#include <unistd.h>
#include <sched.h>
#include <sys/ppu_thread.h>

#include "externals.h"
#include "fps.h"
#include "gpu.h"
#include <sys/timer.h>
#include <sys/return_code.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>

// FPS stuff
float          fFrameRateHz=0;
DWORD          dwFrameRateTicks=16;
float          fFrameRate;
int            iFrameLimit;
int            UseFrameLimit=0;
int            UseFrameSkip=0;

// FPS skipping / limit
BOOL   bInitCap = TRUE;
float  fps_skip = 0;
float  fps_cur  = 0;

float  speed  = 1;

#define MAXLACE 16

void CALLBACK GPUsetSpeed(float newSpeed) {
 if (newSpeed > 0 && newSpeed <= 1000) {
  speed = newSpeed;
 }
}

void CheckFrameRate(void)
{                           
 if(UseFrameSkip)                                      // skipping mode?
  {
   if(!(dwActFixes&0x80))                              // not old skipping mode?
    {
     dwLaceCnt++;                                      // -> store cnt of vsync between frames
     if(dwLaceCnt>=MAXLACE && UseFrameLimit)           // -> if there are many laces without screen toggling,
      {                                                //    do std frame limitation
       if(dwLaceCnt==MAXLACE) bInitCap=TRUE;
       FrameCap();
      }
    }
   else if(UseFrameLimit) FrameCap();
   calcfps();                                          // -> calc fps display in skipping mode
  }                                                  
 else                                                  // non-skipping mode:
  {
   if(UseFrameLimit) FrameCap();                       // -> do it
  // if(ulKeybits&KEY_SHOWFPS) calcfps();                // -> and calc fps display
  }
}                      

////////////////////////////////////////////////////////////////////////
// LINUX VERSION
////////////////////////////////////////////////////////////////////////

#define TIMEBASE 100000

unsigned long timeGetTime()
{
 return sys_time_get_system_time() / 10;
}

void FrameCap (void)
{
	static uint64_t next_time = 0; // Target time for the next frame in microseconds
	uint64_t now_time;
	uint64_t fr_interval_us;
	int64_t time_to_wait;

	// Convert dwFrameRateTicks (which is in 10us units) to microseconds (us)
	fr_interval_us = (uint64_t)(dwFrameRateTicks / speed) * 10;
	
	now_time = sys_time_get_system_time();

	// Initialize or resync if we are way behind schedule (e.g., more than 4 frames)
	// to prevent the emulator from trying to fast-forward uncontrollably.
	if (next_time == 0 || now_time > next_time + (fr_interval_us * 4)) {
		next_time = now_time;
	}
	
	// Calculate the wait time until the next frame's target time
	time_to_wait = next_time - now_time;

	if (time_to_wait > 0)
	{
		// Hybrid sleep. If we have more than ~2ms to wait, call usleep.
		// This avoids burning CPU in a busy-wait loop for long waits.
		if (time_to_wait > 2000) {
			sys_timer_usleep(time_to_wait - 1000); // Sleep until ~1ms before the target time
		}

		// Cooperative busy-wait for the remaining time to achieve higher precision.
		// sys_ppu_thread_yield() allows other threads (like audio) to run.
		while(sys_time_get_system_time() < next_time) {
			sys_ppu_thread_yield();
		}
	}
	
	// Schedule the next frame from the last scheduled time to maintain a fixed step.
	// This ensures a consistent emulation speed even if there are minor timing hiccups.
	next_time += fr_interval_us;
}

#define MAXSKIP 120

void FrameSkip(void)
{
 static int   iNumSkips=0,iAdditionalSkip=0;           // number of additional frames to skip
 static DWORD dwLastLace=0;                            // helper var for frame limitation
 static DWORD curticks, lastticks, _ticks_since_last_update;
 int tickstogo=0;
 static int overslept=0;
 unsigned int frTicks = dwFrameRateTicks / speed;

 if(!dwLaceCnt) return;                                // important: if no updatelace happened, we ignore it completely

 if(iNumSkips)                                         // we are in skipping mode?
  {
   dwLastLace+=dwLaceCnt;                              // -> calc frame limit helper (number of laces)
   bSkipNextFrame = TRUE;                              // -> we skip next frame
   iNumSkips--;                                        // -> ok, one done
  }
 else                                                  // ok, no additional skipping has to be done...
  {                                                    // we check now, if some limitation is needed, or a new skipping has to get started
   DWORD dwWaitTime;

   if(bInitCap || bSkipNextFrame)                      // first time or we skipped before?
    {
     if(UseFrameLimit && !bInitCap)                    // frame limit wanted and not first time called?
      {
       DWORD dwT=_ticks_since_last_update;             // -> that's the time of the last drawn frame
       dwLastLace+=dwLaceCnt;                          // -> and that's the number of updatelace since the start of the last drawn frame

       curticks = timeGetTime();                       // -> now we calc the time of the last drawn frame + the time we spent skipping
       _ticks_since_last_update= dwT+curticks - lastticks;

       dwWaitTime=dwLastLace*frTicks;                  // -> and now we calc the time the real psx would have needed

       if(_ticks_since_last_update<dwWaitTime)         // -> we were too fast?
        {
         if((dwWaitTime-_ticks_since_last_update)>     // -> some more security, to prevent
            (60*frTicks))                              //    wrong waiting times
          _ticks_since_last_update=dwWaitTime;

         while(_ticks_since_last_update<dwWaitTime)    // -> loop until we have reached the real psx time
          {                                            //    (that's the additional limitation, yup)
           curticks = timeGetTime();
           _ticks_since_last_update = dwT+curticks - lastticks;
          }
        }
       else                                            // we were still too slow ?!!?
        {
         if(iAdditionalSkip<MAXSKIP)                   // -> well, somewhen we really have to stop skipping on very slow systems
          {
           iAdditionalSkip++;                          // -> inc our watchdog var
           dwLaceCnt=0;                                // -> reset lace count
           lastticks = timeGetTime();
           return;                                     // -> done, we will skip next frame to get more speed
          } 
        }
      }

     bInitCap=FALSE;                                   // -> ok, we have inited the frameskip func
     iAdditionalSkip=0;                                // -> init additional skip
     bSkipNextFrame=FALSE;                             // -> we don't skip the next frame
     lastticks = timeGetTime();                        // -> we store the start time of the next frame
     dwLaceCnt=0;                                      // -> and we start to count the laces 
     dwLastLace=0;
     _ticks_since_last_update=0;
     return;                                           // -> done, the next frame will get drawn
    }

   bSkipNextFrame=FALSE;                               // init the frame skip signal to 'no skipping' first

   curticks = timeGetTime();                           // get the current time (we are now at the end of one drawn frame)
   _ticks_since_last_update = curticks - lastticks;

   dwLastLace=dwLaceCnt;                               // store curr count (frame limitation helper)
   dwWaitTime=dwLaceCnt*frTicks;                       // calc the 'real psx lace time'
   if (dwWaitTime >= overslept)
   	dwWaitTime-=overslept;

   if(_ticks_since_last_update>dwWaitTime)             // hey, we needed way too long for that frame...
    {
     if(UseFrameLimit)                                 // if limitation, we skip just next frame,
      {                                                // and decide after, if we need to do more
       iNumSkips=0;
      }
     else
      {
       iNumSkips=_ticks_since_last_update/dwWaitTime;  // -> calc number of frames to skip to catch up
       iNumSkips--;                                    // -> since we already skip next frame, one down
       if(iNumSkips>MAXSKIP) iNumSkips=MAXSKIP;        // -> well, somewhere we have to draw a line
      }
     bSkipNextFrame = TRUE;                            // -> signal for skipping the next frame
    }
   else                                                // we were faster than real psx? fine :)
   if(UseFrameLimit)                                   // frame limit used? so we wait til the 'real psx time' has been reached
    {
     if(dwLaceCnt>MAXLACE)                             // -> security check
      _ticks_since_last_update=dwWaitTime;

     while(_ticks_since_last_update<dwWaitTime)        // -> just do a waiting loop...
      {
       curticks = timeGetTime();
       _ticks_since_last_update = curticks - lastticks;

	tickstogo = dwWaitTime - _ticks_since_last_update;
	if (tickstogo-overslept >= 200 && !(dwActFixes&16))
		sys_timer_sleep(tickstogo*10 - 200);
      }
    }
   overslept = _ticks_since_last_update - dwWaitTime;
   if (overslept < 0)
	overslept = 0;
   lastticks = timeGetTime();                          // ok, start time of the next frame
  }

 dwLaceCnt=0;                                          // init lace counter
}

void calcfps(void)
{
 static unsigned long curticks,_ticks_since_last_update,lastticks;
 static long   fps_cnt = 0;
 static unsigned long  fps_tck = 1;
 static long          fpsskip_cnt = 0;
 static unsigned long fpsskip_tck = 1;

  {
   curticks = timeGetTime();
   _ticks_since_last_update=curticks-lastticks;

   if(UseFrameSkip && !UseFrameLimit && _ticks_since_last_update)
    fps_skip=min(fps_skip,((float)TIMEBASE/(float)_ticks_since_last_update+1.0f));

   lastticks = curticks;
  }

 if(UseFrameSkip && UseFrameLimit)
  {
   fpsskip_tck += _ticks_since_last_update;

   if(++fpsskip_cnt==2)
    {
     fps_skip = (float)2000/(float)fpsskip_tck;
     fps_skip +=6.0f;
     fpsskip_cnt = 0;
     fpsskip_tck = 1;
    }
  }

 fps_tck += _ticks_since_last_update;

 if(++fps_cnt==20)
  {
   fps_cur = (float)(TIMEBASE*20)/(float)fps_tck;

   fps_cnt = 0;
   fps_tck = 1;

   //if(UseFrameLimit && fps_cur>fFrameRateHz)           // optical adjust ;) avoids flickering fps display
    //fps_cur=fFrameRateHz;
  }

}

void PCFrameCap (void)
{
 static unsigned long curticks, lastticks, _ticks_since_last_update;
 static unsigned long TicksToWait = 0;
 BOOL Waiting = TRUE;

 while (Waiting)
  {
   curticks = timeGetTime();
   _ticks_since_last_update = curticks - lastticks;
   if ((_ticks_since_last_update > TicksToWait) ||
       (curticks < lastticks))
    {
     Waiting = FALSE;
     lastticks = curticks;
     TicksToWait = (TIMEBASE/ (unsigned long)fFrameRateHz);
    }
  }
}

void PCcalcfps(void)
{
 static unsigned long curticks,_ticks_since_last_update,lastticks;
 static long  fps_cnt = 0;
 static float fps_acc = 0;
 float CurrentFPS=0;

 curticks = timeGetTime();
 _ticks_since_last_update=curticks-lastticks;
 if(_ticks_since_last_update)
      CurrentFPS=(float)TIMEBASE/(float)_ticks_since_last_update;
 else CurrentFPS = 0;
 lastticks = curticks;

 fps_acc += CurrentFPS;

 if(++fps_cnt==10)
  {
   fps_cur = fps_acc / 10;
   fps_acc = 0;
   fps_cnt = 0;
  }

 fps_skip=CurrentFPS+1.0f;
}

void SetAutoFrameCap(void)
{
 if(iFrameLimit==1)
  {
   fFrameRateHz = fFrameRate;
   dwFrameRateTicks=(TIMEBASE*100 / (unsigned long)(fFrameRateHz*100));
   return;
  }

 if(dwActFixes&32)
  {
   if (g_gpu.Interlaced)
        fFrameRateHz = g_gpu.PAL?50.0f:60.0f;
   else fFrameRateHz = g_gpu.PAL?25.0f:30.0f;
  }
 else
  {
   fFrameRateHz = g_gpu.PAL?50.0f:59.94f;
   dwFrameRateTicks=(TIMEBASE*100 / (unsigned long)(fFrameRateHz*100));
  }
}

void SetFPSHandler(void)
{
}

void InitFPS(void)
{
 if(!fFrameRate) fFrameRate=200.0f;
 if(fFrameRateHz==0) fFrameRateHz=fFrameRate;          // set user framerate
 dwFrameRateTicks=(TIMEBASE / (unsigned long)fFrameRateHz);
}


