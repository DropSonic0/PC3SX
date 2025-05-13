#include "cell.h"
#include "ps3video.h"
#include "rom_list.h"

#include <vector>
#include <assert.h>

extern int is_running;

PS3Graphics::PS3Graphics() : PSGLGraphics(), gl_buffer(NULL), vertex_buf(NULL)
{
   	printf("PS3Graphics constructor\n");
	m_smooth = false;
	m_pal60Hz = false;
	m_overscan = false;
	m_overscan_amount = 0.0;
}

PS3Graphics::~PS3Graphics()
{
   	printf("PS3Graphics destructor\n");
	Deinit();
}

void PS3Graphics::Deinit()
{
   	printf("PS3Graphics::Deinit()\n");
	PSGLGraphics::Deinit();
	if(vertex_buf)
	{
		free(vertex_buf);
		vertex_buf = NULL;
	}
	if(gl_buffer)
	{
		free(gl_buffer);
		gl_buffer = NULL;
	}
}

void PS3Graphics::DeInitDbgFont()
{
	PSGLGraphics::DeinitDbgFont();
}

void PS3Graphics::InitDbgFont()
{
	PSGLGraphics::InitDbgFont();
}

void PS3Graphics::FlushDbgFont()
{
   	//printf("PS3Graphics::FlushDbgFont()\n");
	cellDbgFontDraw();
}

void PS3Graphics::Swap()
{
   	//printf("PS3Graphics::Swap()\n");
	psglSwap();
}



void PS3Graphics::Clear()
{
   	//printf("PS3Graphics::Clear()\n");
	glClear(GL_COLOR_BUFFER_BIT);
}

void PS3Graphics::SetViewports()
{
   printf("PS3Graphics::SetViewPorts()\n");
   float device_aspect = this->GetDeviceAspectRatio();
   GLuint width = this->GetResolutionWidth();
   GLuint height = this->GetResolutionHeight();
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // calculate the glOrtho matrix needed to transform the texture to the desired aspect ratio
   float desired_aspect = m_ratio;

   GLuint real_width = width, real_height = height;

   // If the aspect ratios of screen and desired aspect ratio are sufficiently equal (floating point stuff), 
   // assume they are actually equal.
   if ( (int)(device_aspect*1000) > (int)(desired_aspect*1000) )
   {
      float delta = (desired_aspect / device_aspect - 1.0) / 2.0 + 0.5;
      //glOrthof(0.5 - delta, 0.5 + delta, 0, 1, -1, 1);
      glViewport(width * (0.5 - delta), 0, 2.0 * width * delta, height);
      real_width = (int)(2.0 * width * delta);
   }

   else if ( (int)(device_aspect*1000) < (int)(desired_aspect*1000) )
   {
      float delta = (device_aspect / desired_aspect - 1.0) / 2.0 + 0.5;
      //glOrthof(0, 1, 0.5 - delta, 0.5 + delta, -1, 1);
      glViewport(0, height * (0.5 - delta), width, 2.0 * height * delta);
      real_height = (int)(2.0 * height * delta);
   }
   else
   {
      glViewport(0, 0, width, height);
   }

   if (m_overscan)
   {
      glOrthof(-m_overscan_amount/2, 1 + m_overscan_amount/2, -m_overscan_amount/2, 1 + m_overscan_amount/2, -1, 1);
   }
   else
   {
      glOrthof(0, 1, 0, 1, -1, 1);
   }

   _cgViewWidth = real_width;
   _cgViewHeight = real_height;
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void PS3Graphics::SetOverscan(bool will_overscan, float amount)
{
   	printf("PS3Graphics::SetOverscan(%d, %f)()\n", will_overscan, amount);
	m_overscan_amount = amount;
	m_overscan = will_overscan;
	SetViewports();
}

void PS3Graphics::SetPAL60Hz(bool pal60Hz)
{
	printf("PS3Graphics::SetPAL60Hz(%d)\n", pal60Hz);
	m_pal60Hz = pal60Hz;
}

bool PS3Graphics::GetPAL60Hz()
{
	printf("PS3Graphics::GetPAL60Hz()\n");
	return m_pal60Hz;
}

void PS3Graphics::SetAspectRatio(bool keep_aspect)
{
	printf("PS3Graphics::SetAspectRatio(%d)\n", keep_aspect);
	if (keep_aspect)
	{
		m_ratio = SCREEN_4_3_ASPECT_RATIO;
	}
	else
	{
		m_ratio = SCREEN_16_9_ASPECT_RATIO;
	}
	SetViewports();
}

void PS3Graphics::Draw(int width, int height, uint8_t* screen)
{
	Clear();
	
	glBufferSubData(GL_TEXTURE_REFERENCE_BUFFER_SCE, 0, height * EMU_SCREEN_PITCH, screen);
	glTextureReferenceSCE(GL_TEXTURE_2D, 1, width, height, 0, GL_ARGB_SCE, SCREEN_RENDER_TEXTURE_PITCH, 0);
	UpdateCgParams(width, height, width, height);
	
	glDrawArrays(GL_QUADS, 0, 4); 
	glFlush();
	
	if(Settings.Fps)
		write_fps();

	//Stop the funk
	cellSysutilCheckCallback();

	if(is_running == 0)
	{
		printf("Close and exit from %s\n",__FUNCTION__);
		Deinit();
	}
}

int32_t PS3Graphics::ChangeResolution(uint32_t resId, uint16_t pal60Hz)
{
	printf("PS3Graphics::ChangeResolution(%d)\n", resId);
	int32_t ret;

	PSGLGraphics::DeinitDbgFont();
	Deinit();
	
	PSGLGraphics::Init(resId, pal60Hz);
	PSGLInit();
	PSGLGraphics::InitDbgFont();
	PSGLGraphics::SetResolution();
}

void PS3Graphics::UpdateCgParams(unsigned width, unsigned height, unsigned tex_width, unsigned tex_height)
{
	printf("PS3Graphics::UpdateCgParams(%d, %d, %d, %d)\n", width, height, tex_width, tex_height);
	cgGLSetStateMatrixParameter(_cgpModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetParameter2f(_cgpVideoSize, width, height);
	cgGLSetParameter2f(_cgpTextureSize, tex_width, tex_height);
	cgGLSetParameter2f(_cgpOutputSize, _cgViewWidth, _cgViewHeight);
}

void PS3Graphics::InitScreenQuad(int width, int height)
{
	printf("PS3Graphics::InitScreenQuad(%d, %d)\n", width, height);
	screenQuad.v1.x = 0;
	screenQuad.v1.y = 0;
	screenQuad.v1.z = 0;

	screenQuad.v2.x = 0;
	screenQuad.v2.y = 1;
	screenQuad.v2.z = 0;

	screenQuad.v3.x = 1;
	screenQuad.v3.y = 1;
	screenQuad.v3.z = 0;

	screenQuad.v4.x = 1;
	screenQuad.v4.y = 0;
	screenQuad.v4.z = 0;

	screenQuad.t1.u = 0;
	screenQuad.t1.v = 1;

	screenQuad.t2.u = 0;
	screenQuad.t2.v = 0;

	screenQuad.t3.u = 1;
	screenQuad.t3.v = 0;

	screenQuad.t4.u = 1;
	screenQuad.t4.v = 1;

	memcpy(vertex_buf, ((float*)&screenQuad), 12 * sizeof(GLfloat));
	memcpy(vertex_buf + 128, ((float*)&screenQuad) + 12, 8 * sizeof(GLfloat));
}

void PS3Graphics::SetSmooth(bool smooth)
{
	printf("PS3Graphics::SetSmooth(%d)\n", smooth);
	m_smooth = smooth;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST);
}

CGerror CheckCgError(int line)
{
	printf("PS3Graphics::CheckCgError(%d)\n",line);
	CGerror err = cgGetError();

	if (err != CG_NO_ERROR)
	{
		//FIXME: Add logging stuff
		printf ("CG error:%d at line %d %s\n", err, line, cgGetErrorString (err));
	}

	return err;
}

CGprogram LoadShaderFromFile(CGcontext cgtx, CGprofile target, const char* filename, const char *entry)
{
	printf("PS3Graphics::LoadShaderFromFile()\n");
	CGprogram id = cgCreateProgramFromFile(cgtx, CG_BINARY, filename, target, entry, NULL);
	if(!id)
	{
		printf("Failed to load shader program >>%s<<\nExiting\n", filename);
		CheckCgError(__LINE__);
	}

	return id;
}

CGprogram LoadShaderFromSource(CGcontext cgtx, CGprofile target, const char* filename, const char *entry)
{
	printf("PS3Graphics::LoadShaderFromSource()\n");
	CGprogram id = cgCreateProgramFromFile(cgtx, CG_SOURCE, filename, target, entry, NULL);
	if(!id)
	{
		printf("Failed to load shader program >>%s<<\nExiting\n", filename);
		CheckCgError(__LINE__);
	}
	
	return id;
}



int32_t PS3Graphics::InitCg()
{
	printf("PS3Graphics::InitCg()\n");
	
	cgRTCgcInit();

	printf("PS3Graphics::InitCg() - About to create CgContext\n");
	_cgContext = cgCreateContext();
	if (_cgContext == NULL)
	{
		printf("Error creating Cg Context\n");
		return 1;
	}
	if (strlen(_curFragmentShaderPath.c_str()) > 0)
	{
		return LoadFragmentShader(_curFragmentShaderPath.c_str());
	}
	else
	{
		_curFragmentShaderPath = DEFAULT_SHADER_FILE;
		return LoadFragmentShader(_curFragmentShaderPath.c_str());
	}
}

int32_t PS3Graphics::LoadFragmentShader(std::string shaderPath)
{
   printf("PS3Graphics::LoadFragmentShader(%s)\n", shaderPath.c_str());

   // store the cur path
   _curFragmentShaderPath = shaderPath;

   _vertexProgram = LoadShaderFromSource(_cgContext, CG_PROFILE_SCE_VP_RSX, shaderPath.c_str(), "main_vertex");
   if (_vertexProgram <= 0)
   {
	   printf("Error loading vertex shader...");
	   return 1;
   }

   _fragmentProgram = LoadShaderFromSource(_cgContext, CG_PROFILE_SCE_FP_RSX, shaderPath.c_str(), "main_fragment");
   if (_fragmentProgram <= 0)
   {
	   printf("Error loading fragment shader...");
	   return 1;
   }

   // bind and enable the vertex and fragment programs
   cgGLEnableProfile(CG_PROFILE_SCE_VP_RSX);
   cgGLEnableProfile(CG_PROFILE_SCE_FP_RSX);
   cgGLBindProgram(_vertexProgram);
   cgGLBindProgram(_fragmentProgram);

   // acquire mvp param from v shader
   _cgpModelViewProj = cgGetNamedParameter(_vertexProgram, "modelViewProj");
   if (CheckCgError (__LINE__) != CG_NO_ERROR)
   {
	   // FIXME: WHY DOES THIS GIVE ERROR ON OTHER LOADS
	   // return 1;
   }

   _cgpVideoSize = cgGetNamedParameter(_fragmentProgram, "IN.video_size");
   _cgpTextureSize = cgGetNamedParameter(_fragmentProgram, "IN.texture_size");
   _cgpOutputSize = cgGetNamedParameter(_fragmentProgram, "IN.output_size");

   printf("SUCCESS - LoadFragmentShader(%s)\n", shaderPath.c_str());
   return CELL_OK;
}

int32_t PS3Graphics::PSGLInit()
{
	printf("PS3Graphics::PSGLInit()\n");
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDisable(GL_DITHER);
	glShadeModel(GL_FLAT);
	glEnable(GL_VSYNC_SCE);
	glEnable(GL_TEXTURE_2D);
	
	uint32_t ret = InitCg();
	if (ret != CELL_OK)
	{
		printf("Failed to InitCg: %d", __LINE__);
	}

	SetViewports();

	gl_buffer = (uint8_t*)memalign(128, SCREEN_RENDER_TEXTURE_HEIGHT * SCREEN_RENDER_TEXTURE_PITCH); // Allocate memory for texture.
   memset(gl_buffer, 0, SCREEN_RENDER_TEXTURE_HEIGHT * SCREEN_RENDER_TEXTURE_PITCH);
	vertex_buf = (uint8_t*)memalign(128, 256);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGenBuffers(2, vbo);

	glBindBuffer(GL_TEXTURE_REFERENCE_BUFFER_SCE, vbo[0]);
	glBufferData(GL_TEXTURE_REFERENCE_BUFFER_SCE, SCREEN_RENDER_TEXTURE_HEIGHT * SCREEN_RENDER_TEXTURE_PITCH, gl_buffer, GL_STREAM_DRAW);

	glTextureReferenceSCE(GL_TEXTURE_2D, 1, SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT, 0, GL_ARGB_SCE, SCREEN_RENDER_TEXTURE_PITCH, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	SetSmooth(m_smooth);

	// PSGL doesn't clear the screen on startup, so let's do that here.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	Clear();
	psglSwap();

   // Use some initial values for the screen quad.
	InitScreenQuad(SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 256, vertex_buf, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(2, GL_FLOAT, 0, (void*)128);
	glColorPointer(4,GL_BYTE,0,0);
				

	return CELL_OK;
}

void PS3Graphics::Init()
{
	printf("PS3Graphics::Init()\n");
	PSGLGraphics::Init(NULL, m_pal60Hz);
	int32_t ret = PSGLInit();

	if (ret == CELL_OK)
	{
		PSGLGraphics::SetResolution();
	}
	PSGLGraphics::GetAllAvailableResolutions();
}

