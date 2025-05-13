/*
 * PSGLGraphics.cpp
 *
 *  Created on: Oct 26, 2010
 *      Author: halsafar
 */

#include "PSGLGraphics.h"

#include <stdio.h>
#include <string.h>
#include "cell.h"
#include <sys/sys_time.h>

PSGLGraphics::PSGLGraphics()
{
	printf("~PSGLGraphics - constructor");
	psgl_device = NULL;
	psgl_context = NULL;
	gl_width = 0;
	gl_height = 0;
}

PSGLGraphics::~PSGLGraphics()
{
	printf("~PSGLGraphics - destructor\n");

}

PSGLdeviceParameters PSGLGraphics::InitCommon(uint32_t resolutionId, uint16_t pal60Hz)
{
	printf("PSGLGraphics::InitCommon(%d, %d)\n", resolutionId, pal60Hz);
	PSGLinitOptions options =
	{
		enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE,
		maxSPUs: 1,
		initializeSPUs: false,
		persistentMemorySize: 0,
		transientMemorySize: 0,
		errorConsole: 0,
		fifoSize: 0,
		hostMemorySize: 8 * 1024 * 1024
	};

	printf("PSGLGraphics::InitCommon()->psglInit(&options)\n");
	psglInit(&options);

	PSGLdeviceParameters params;
	params.enable = PSGL_DEVICE_PARAMETERS_COLOR_FORMAT | PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT | PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE | PSGL_DEVICE_PARAMETERS_RESC_ADJUST_ASPECT_RATIO;
	params.colorFormat = GL_ARGB_SCE;
	params.depthFormat = GL_NONE;
	params.multisamplingMode = GL_MULTISAMPLING_NONE_SCE;

	if(pal60Hz)
	{
		printf("PSGLGraphics::InitCommon()->PAL60\n");
		params.enable |= PSGL_DEVICE_PARAMETERS_RESC_PAL_TEMPORAL_MODE;
		params.rescPalTemporalMode = RESC_PAL_TEMPORAL_MODE_60_INTERPOLATE;
	}

	if(resolutionId != NULL)
	{
		printf("PSGLGraphics::InitCommon()->resolutionId != NULL\n");
		//Resolution setting
		CellVideoOutResolution resolution;
		cellVideoOutGetResolution(resolutionId, &resolution);
		
		params.enable |= PSGL_DEVICE_PARAMETERS_WIDTH_HEIGHT;
		params.width = resolution.width;
		params.height = resolution.height;
	}

	params.enable |= PSGL_DEVICE_PARAMETERS_RESC_RATIO_MODE;
	params.rescRatioMode = RESC_RATIO_MODE_FULLSCREEN;
	printf("PSGLGraphics::InitCommon()->return params\n");
	return params;
}

void PSGLGraphics::Init()
{
	printf("PSGLGraphics::Init()\n");
	PSGLdeviceParameters myParams;
	myParams	= InitCommon(NULL, 0);

	printf("PSGLGraphics::Init()->psglCreateDeviceExtended(&myParams)\n");
	psgl_device = psglCreateDeviceExtended(&myParams);


	printf("PSGLGraphics::Init()->psglGetDeviceDimensions(psgl_device, %d, %d)\n", (int *)gl_width, (int *)gl_height);
	psglGetDeviceDimensions(psgl_device, &gl_width, &gl_height); // Get the dimensions of the screen in question, and do stuff with it :)

	printf("PSGLGraphics::Init()->psgl_context = psglCreateContext()\n");
	psgl_context = psglCreateContext(); // Create a context and bind it to the current display.
	

	printf("PSGLGraphics::Init()->psglMakeCurrent(psgl_context, psgl_device)\n");
	psglMakeCurrent(psgl_context, psgl_device);

	printf("PSGLGraphics::Init()->psglResetCurrentContext()\n");
	psglResetCurrentContext();
}

void PSGLGraphics::Init(uint32_t resolutionId, uint16_t pal60Hz)
{
	printf("PSGLGraphics::Init(%d, %d)\n", resolutionId, pal60Hz);
	PSGLdeviceParameters myParams;
	myParams	= InitCommon(resolutionId, pal60Hz);

	printf("PSGLGraphics::Init(%d, %d)->psglCreateDeviceExtended(&myParams)\n", resolutionId, pal60Hz);
	psgl_device = psglCreateDeviceExtended(&myParams);

	printf("PSGLGraphics::Init(%d, %d)->psglGetDeviceDimensions(psgl_device, &gl_width, &gl_height)\n", resolutionId, pal60Hz);
	psglGetDeviceDimensions(psgl_device, &gl_width, &gl_height); // Get the dimensions of the screen in question, and do stuff with it :)

	printf("PSGLGraphics::Init(%d, %d)->psgl_context = psglCreateContext()\n", resolutionId, pal60Hz);
	psgl_context = psglCreateContext(); // Create a context and bind it to the current display.

	printf("PSGLGraphics::Init(%d, %d)->psglMakeCurrent(psgl_context, psgl_device)\n", resolutionId, pal60Hz);
	psglMakeCurrent(psgl_context, psgl_device);

	printf("PSGLGraphics::Init(%d, %d)->psglResetCurrentContext()\n", resolutionId, pal60Hz);
	psglResetCurrentContext();
}

void PSGLGraphics::GetAllAvailableResolutions()
{
	printf("PSGLGraphics::GetAllAvailableResolution()\n");
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_480))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_480);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_480;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_576))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_576);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_576;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_960x1080))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_960x1080);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_960x1080;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_720))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_720);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_720;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_1280x1080))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_1280x1080);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_1280x1080;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_1440x1080))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_1440x1080);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_1440x1080;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_1600x1080))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_1600x1080);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_1600x1080;
	}
	if(CheckResolution(CELL_VIDEO_OUT_RESOLUTION_1080))
	{
		AddResolution(CELL_VIDEO_OUT_RESOLUTION_1080);
		initialResolution = CELL_VIDEO_OUT_RESOLUTION_1080;
	}
	currentResolution = supportedResolutions.size()-1;
}

int PSGLGraphics::CheckResolution(uint32_t resId)
{
	printf("PSGLGraphics::CheckResolution(%d)\n", resId);
	return cellVideoOutGetResolutionAvailability(CELL_VIDEO_OUT_PRIMARY, resId, CELL_VIDEO_OUT_ASPECT_AUTO,0);
}

int PSGLGraphics::AddResolution(uint32_t resId)
{
	printf("PSGLGraphics::AddResolution(%d)\n", resId);
	supportedResolutions.push_back(resId);
}

void PSGLGraphics::NextResolution()
{
	printf("PSGLGraphics::NextResolution()\n");
	printf("supportedResolutions size: %d\n", supportedResolutions.size());
	if(currentResolution+1 < supportedResolutions.size())
	{
		currentResolution++;
		printf("currentResolution: %d\n", currentResolution);
	}
}

void PSGLGraphics::PreviousResolution()
{
	printf("PSGLGraphics::NextResolution()\n");
	printf("supportedResolutions size: %d\n", supportedResolutions.size());
	if(currentResolution > 0)
	{
		currentResolution--;
		printf("currentResolution: %d\n", currentResolution);
	}
}

void PSGLGraphics::SwitchResolution(uint32_t resId, uint16_t pal60Hz)
{
	printf("PSGLGraphics::SwitchResolution(%d)\n",resId);
	if(CheckResolution(resId))
	{
		printf("PSGLGraphics::SwitchResolution(%d)->ChangeResolution(%d)\n",resId, resId);
		ChangeResolution(resId, pal60Hz);
	}
}

uint32_t PSGLGraphics::GetInitialResolution()
{
	printf("PSGLGraphics::GetInitialResolution()\n");
	return initialResolution;
}

uint32_t PSGLGraphics::GetCurrentResolution()
{
	printf("PSGLGraphics::GetCurrentResolution()\n");
	return supportedResolutions[currentResolution];
}

int32_t PSGLGraphics::ChangeResolution(uint32_t resId, uint16_t pal60Hz)
{
	printf("PSGLGraphics::ChangeResolution(%d)\n", resId);
	int32_t ret;

	DeinitDbgFont();
	psglDestroyContext(psgl_context);
	psglDestroyDevice(psgl_device);
	
	Init(resId, pal60Hz);
	InitDbgFont();
	SetResolution();
}

void PSGLGraphics::SetResolution()
{
		cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &stored_video_state);
}

CellVideoOutState PSGLGraphics::GetVideoOutState()
{
   	printf("PSGLGraphics::GetVideoState()\n");
	return stored_video_state;
}

void PSGLGraphics::Deinit()
{
   printf("PSGLGraphics::Deinit()\n");
   glFinish();
   this->DeinitDbgFont();
   //free(gl_buffer);
   //free(vertex_buf);

   psglDestroyContext(psgl_context);
   psglDestroyDevice(psgl_device);
#ifdef PS3_SDK_4_75
   //FIXME: It will crash here for 1.92 - termination of the PSGL library - works fine for 3.41
   psglExit();
#else
   //for 1.92
   gl_width = 0;
   gl_height = 0;
   psgl_context = NULL;
   psgl_device = NULL;
#endif
   sys_process_exit(0);

}

void PSGLGraphics::InitDbgFont()
{
   printf("PSGLGraphics::InitDbgFont()\n");
	CellDbgFontConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.bufSize = 512;
	cfg.screenWidth = gl_width;
	cfg.screenHeight = gl_height;
	cellDbgFontInit(&cfg);
}


void PSGLGraphics::DeinitDbgFont()
{
   printf("PSGLGraphics::DeinitDbgFont()\n");
	cellDbgFontExit();
}


GLfloat PSGLGraphics::GetDeviceAspectRatio()
{
   printf("PSGLGraphics::GetDeviceAspectRatio()\n");
	return psglGetDeviceAspectRatio(psgl_device);
}


GLint PSGLGraphics::GetResolutionWidth()
{
   printf("PSGLGraphics::GetResolutionWidth()\n");
	return gl_width;
}


GLint PSGLGraphics::GetResolutionHeight()
{
   printf("PSGLGraphics::GetResolutionHeight()\n");
	return gl_height;
}




//FIXME: classify this
void dprintf_console(const char* fmt, ...)
{
	glClear(GL_COLOR_BUFFER_BIT);

	va_list ap;

	va_start(ap, fmt);
	cellDbgFontConsoleVprintf(dbg_id, fmt, ap);
	va_end(ap);

	cellDbgFontConsoleEnable(dbg_id);
	cellDbgFontDraw();
	psglSwap();	// added comment by SSNES author - should this be here?
}

void dprintf_noswap(float x, float y, float scale, const char* fmt, ...)
{
   char buffer[512];

   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buffer, 512, fmt, ap);
   cellDbgFontPuts(x, y, scale, 0xffffffff, buffer);
   va_end(ap);

   cellDbgFontDraw();
}

void write_fps(void)
{
   static float last_time = 0.0;
   float new_time = sys_time_get_system_time() / 1000000.0;
   float delta = new_time - last_time;
   last_time = new_time;

   dprintf_noswap(0.1, 0.1, 1.0, "FPS: %.2f\n", 1.0f/delta);
}


void gl_dprintf(float x, float y, float scale, const char* fmt, ...)
{
	glClear(GL_COLOR_BUFFER_BIT);

	char buffer[512];

	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buffer, 512, fmt, ap);
	printf("%s",buffer);
	cellDbgFontPuts(x, y, scale, 0xffffffff, buffer);
	va_end(ap);

	cellDbgFontDraw();
	psglSwap();
}
