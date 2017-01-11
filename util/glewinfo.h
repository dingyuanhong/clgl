#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#if defined(GLEW_OSMESA)
#define GLAPI extern
#include <GL/osmesa.h>
#elif defined(GLEW_EGL)
#include <GL/eglew.h>
#elif defined(WIN32)
#include <GL/wglew.h>
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLTypes.h>
#elif !defined(__HAIKU__)
#include <GL/glxew.h>
#endif

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#endif

#ifdef GLEW_REGAL
#include <GL/Regal.h>
#endif

#ifdef  __cplusplus  
extern "C" {
#endif 

typedef struct GLContextStruct
{
#if defined(GLEW_OSMESA)
	OSMesaContext ctx;
#elif defined(GLEW_EGL)
	EGLContext ctx;
	EGLDisplay  display;
#elif defined(_WIN32)
	HWND wnd;
	HDC dc;
	HGLRC rc;
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
	CGLContextObj ctx, octx;
#elif !defined(__HAIKU__)
	Display* dpy;
	XVisualInfo* vi;
	GLXContext ctx;
	Window wnd;
	Colormap cmap;
#endif
} GLContext;

//用于处理
void glewInitContextV(GLContext* ctx);
GLboolean glewCreateContextV(GLContext* ctx);
#ifdef WIN32
GLboolean _glewCreateContextV(GLContext* ctx, int visual);
#elif defined(GLEW_OSMESA)
GLboolean _glewCreateContextV(GLContext* ctx, GLint osmWidth, GLint osmHeight, GLint osmFormat, GLubyte **osmPixels);
#endif
void glewDestroyContextV(GLContext* ctx);

//打印状态
void glewVisualInfoPrint(FILE *file, GLContext* ctx);


struct createParams
{
#if defined(GLEW_OSMESA)
#elif defined(GLEW_EGL)
#elif defined(_WIN32)
	int         pixelformat;
#elif !defined(__APPLE__) && !defined(__HAIKU__) || defined(GLEW_APPLE_GLX)
	const char* display;
	int         visual;
#endif
	int         major, minor;  /* GL context version number */

							   /* https://www.opengl.org/registry/specs/ARB/glx_create_context.txt */
	int         profile;       /* core = 1, compatibility = 2 */
	int         flags;         /* debug = 1, forward compatible = 2 */
};
GLboolean glewCreateContext(GLContext* ctx, struct createParams* params);
void glewDestroyContext(GLContext* ctx);
void glewInfoPrint(FILE *f, struct createParams params);

#ifdef  __cplusplus  
}
#endif  /* end of __cplusplus */  