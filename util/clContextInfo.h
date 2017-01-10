#pragma once

#include "CL/opencl.h"

#if defined (WIN32)
#include "windows.h"
#elif defined (__linux__)
#elif defined (__APPLE__)
#endif

inline cl_context_properties * GetGLContextProperties(cl_platform_id platform) {
	// Create the properties for this context.
	static cl_context_properties lContextProperties[] = {
		// We need to add information about the OpenGL context with
		// which we want to exchange information with the OpenCL context.
#if defined (WIN32)
		// We should first check for cl_khr_gl_sharing extension.
		CL_GL_CONTEXT_KHR , (cl_context_properties)wglGetCurrentContext() ,
		CL_WGL_HDC_KHR , (cl_context_properties)wglGetCurrentDC() ,
#elif defined (__linux__)
		// We should first check for cl_khr_gl_sharing extension.
		CL_GL_CONTEXT_KHR , (cl_context_properties)glXGetCurrentContext() ,
		CL_GLX_DISPLAY_KHR , (cl_context_properties)glXGetCurrentDisplay() ,
#elif defined (__APPLE__)
		// We should first check for cl_APPLE_gl_sharing extension.
#if 0
		// This doesn't work.
		CL_GL_CONTEXT_KHR , (cl_context_properties)CGLGetCurrentContext() ,
		CL_CGL_SHAREGROUP_KHR , (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()) ,
#else
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE , (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()) ,
#endif
#endif
		CL_CONTEXT_PLATFORM , (cl_context_properties)platform ,
		0 , 0 ,
	};

	lContextProperties[5] = (cl_context_properties)platform;

	return lContextProperties;
}