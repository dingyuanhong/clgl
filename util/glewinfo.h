#pragma once

#include <GL/glew.h>
#if defined(GLEW_OSMESA)
#define GLAPI extern
#include <GL/osmesa.h>
#elif defined(GLEW_EGL)
#include <GL/eglew.h>
#elif defined(_WIN32)
#include <GL/wglew.h>
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLTypes.h>
#elif !defined(__HAIKU__)
#include <GL/glxew.h>
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
void glewVInitContext(GLContext* ctx);
GLboolean glewVCreateContext(GLContext* ctx);
#ifdef WIN32
GLboolean _glewVCreateContext(GLContext* ctx, int visual);
#elif defined(GLEW_OSMESA)
GLboolean _glewVCreateContext(GLContext* ctx, GLint osmWidth, GLint osmHeight, GLint osmFormat, GLubyte **osmPixels);
#endif
void glewVDestroyContext(GLContext* ctx);

//打印状态
void VisualInfoPrint(GLContext* ctx, FILE *file);

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

void glxewInfoPrint(FILE *f, struct createParams params);

