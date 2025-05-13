#ifndef __PSGLGRAPHICS_H__
#define __PSGLGRAPHICS_H__

#include <vector>
#include "IGraphics.h"

#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <cell/dbgfont.h>

class PSGLGraphics : IGraphics<GLfloat, GLint>
{
public:
	PSGLGraphics();
	~PSGLGraphics();

	virtual void Init();
	virtual void Init(uint32_t resolutionId, uint16_t pal60Hz);
	virtual void Deinit();

	virtual void InitDbgFont();
	virtual void DeinitDbgFont();

	virtual GLfloat GetDeviceAspectRatio();
	virtual GLint GetResolutionWidth();
	virtual GLint GetResolutionHeight();
	
	int CheckResolution(uint32_t resId);
	void SwitchResolution(uint32_t resId, uint16_t pal60Hz);
	uint32_t GetInitialResolution();
	uint32_t GetCurrentResolution();
	void PreviousResolution();
	void NextResolution();
	int AddResolution(uint32_t resId);
	CellVideoOutState GetVideoOutState();
	void GetAllAvailableResolutions();
	void SetResolution();
private:
	PSGLdeviceParameters InitCommon(uint32_t resolutionId, uint16_t pal60Hz);
	PSGLdevice* psgl_device;
	PSGLcontext* psgl_context;
	GLuint gl_width;
	GLuint gl_height;
	CellVideoOutState stored_video_state;
	int currentResolution;
	uint32_t initialResolution;

	CellDbgFontConsoleId dbg_id;
	std::vector<uint32_t> supportedResolutions;
	virtual int32_t ChangeResolution(uint32_t resId, uint16_t pal60Hz);
};

static CellDbgFontConsoleId dbg_id;

//FIXME: classify this
void dprintf_console(const char* fmt, ...);
void dprintf_noswap(float x, float y, float scale, const char* fmt, ...);
void write_fps(void);
void gl_dprintf(float x, float y, float scale, const char* fmt, ...);


#endif
