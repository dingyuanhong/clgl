#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <istream>

#include "cl\opencl.h"
#include "gl\glew.h"
#include "gl\gl.h"
#include "gl\glu.h"
#include "gl\glut.h"

#pragma comment(lib,"OpenCL.lib")
#pragma comment(lib,"Opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glut32.lib")

char *Read(const char * path) {
	FILE * fp = NULL;
	fopen_s(&fp, path, "rb");
	if (fp == NULL) return NULL;
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	void *buf = malloc(size + 2);
	if (buf == NULL) return NULL;
	memset(buf, 0, size + 2);

	fread(buf, 1, size, fp);
	fclose(fp);

	return (char*)buf;
}

cl_program CreateProgram(cl_context context, const char * file) {
	char * Source = Read(file);
	if (Source == NULL) return NULL;

	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&Source, NULL, NULL);
	if (program == NULL) return program;
	cl_int err = clBuildProgram(program, 0, 0, NULL, NULL, NULL);

	free(Source);

	if (err < 0) {
		size_t len = 0;
		clGetProgramBuildInfo(program, NULL, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		if (len > 0) {
			char *buffer = (char*)malloc(len);
			clGetProgramBuildInfo(program, NULL, CL_PROGRAM_BUILD_LOG, len, buffer, &len);
			if (len > 0) {
				printf("BuildProgram:Error:%s\n", buffer);
			}
		}
		else {
			printf("BuildProgram:Error:%d\n", err);
		}

		clReleaseProgram(program);
		return NULL;
	}

	return program;
}

cl_context_properties * GetGLContextProperties(cl_platform_id platform) {
	// Create the properties for this context.
	static cl_context_properties lContextProperties[] = {
		// We need to add information about the OpenGL context with
		// which we want to exchange information with the OpenCL context.
#if defined (WIN32)
		// We should first check for cl_khr_gl_sharing extension.
		CL_GL_CONTEXT_KHR , (cl_context_properties) wglGetCurrentContext() ,
		CL_WGL_HDC_KHR , (cl_context_properties) wglGetCurrentDC() ,
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

GLuint initVBO(int vbolen) {
	GLint bsize;
	GLuint vbo_buffer;
	glGenBuffers(1, &vbo_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_buffer);
	glBufferData(GL_ARRAY_BUFFER, vbolen*sizeof(float) * 4, NULL, GL_STREAM_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if ((GLuint)bsize != (vbolen*sizeof(float) * 4)) {
		printf("vbo Buffer size is error(%d)(%d).\n", vbo_buffer, bsize);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo_buffer;
}

int vboProcess(cl_context context, cl_device_id device, GLuint vbo) {
	int err = 0;
	cl_mem cl_vbo_mem = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, vbo, &err);

	cl_program program = CreateProgram(context, "./vboKernel.cl");
	if (program == NULL) return -1;

	int w = 640;
	int h = 480;
	int seq = 0;
	cl_mem cl_w = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &w, &err);
	cl_mem cl_h = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &h, &err);
	cl_mem cl_seq = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &seq, &err);

	cl_kernel kernel = clCreateKernel(program, "init_vbo_kernel", &err);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_vbo_mem);
	clSetKernelArg(kernel, 1, sizeof(cl_int), &cl_w);
	clSetKernelArg(kernel, 2, sizeof(cl_int), &cl_h);
	clSetKernelArg(kernel, 3, sizeof(cl_int), &cl_seq);

	glFinish();

	size_t tex_globalWorkSize[] = { 0 };
	size_t tex_localWorkSize[] = { 0 };

	cl_queue_properties properties = 0;
	cl_command_queue_properties prop = 0;
	//cl_command_queue queue = clCreateCommandQueue(context, device, prop ,&err);
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, &properties, &err);
	if (queue != NULL) {
		err = clEnqueueAcquireGLObjects(queue, 1, &cl_vbo_mem, 0, NULL, NULL);
		err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, tex_globalWorkSize, tex_localWorkSize, 0, NULL, NULL);
		clFinish(queue);
		clReleaseCommandQueue(queue);
		err = clEnqueueReleaseGLObjects(queue, 1, &cl_vbo_mem, 0, NULL, NULL);
	}

	clReleaseMemObject(cl_w);
	clReleaseMemObject(cl_h);
	clReleaseMemObject(cl_seq);

	clReleaseProgram(program);
	return 0;
}

int glProcess(GLuint vbo, int vbolen) {
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glDrawArrays(GL_LINES, 0, vbolen * 2);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	return 0;
}

GLuint vbo = NULL;
int Process(cl_context context, cl_device_id device) {
	vbo = initVBO(640 * 480);
	vboProcess(context, device,vbo);
	glProcess(vbo,640*480);
	return -1;
}

int clInit() {
	cl_uint num = 0;
	cl_int err = clGetPlatformIDs(0, 0, &num);
	if (err < 0) {
		return -1;
	}
	std::vector<cl_platform_id> platforms(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);
	if (err < 0) {
		return -1;
	}

	printf("PlatformIDs:%d \n", num);

	cl_context context = NULL;
	cl_device_id id = NULL;
	std::vector<cl_device_id> deviceIDs;

	for (cl_int i = 0; i < num; i++) {
		printf("%s %d:\n", "PLATFORM", i);
		char buf[1024] = { 0 };
		size_t size = 0;
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 1024, buf, &size);
		if (err < 0) continue;
		printf("	EXTENSIONS:%s\n", buf);

		if (strstr(buf,"cl_khr_gl_sharing") == NULL) {
			continue;
		}

		//if (context == NULL) {
			//cl_context_properties props[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[i]), 0 };
			//cl_context_properties * props = GetGLContextProperties(platforms[i]);
			//context = clCreateContext(props, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL, &err);
		//}

		cl_uint deviceNum = 0;
		err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceNum);
		if (err < 0) continue;

		std::vector<cl_device_id> devices(deviceNum);
		err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceNum, &devices[0], &deviceNum);
		if (err < 0) continue;

		printf("	  Devices:%d\n", deviceNum);
		char deviceBuf[1024] = { 0 };
		size_t deviceSize = 0;
		for (cl_int j = 0; j < deviceNum; j++) {
			err = clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, 1024, deviceBuf, &deviceSize);
			if (err < 0) continue;
			cl_uint type = *((cl_uint*)deviceBuf);
			printf("		TYPE:%d\n", type);
			err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 1024, deviceBuf, &deviceSize);
			if (err < 0) continue;
			printf("		NAME:%s\n", deviceBuf);	

			if (context == NULL && (type == CL_DEVICE_TYPE_GPU || type == CL_DEVICE_TYPE_CPU)) {
				//cl_context_properties props[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[i]), 0 };
				cl_context_properties * props = GetGLContextProperties(platforms[i]);
				deviceIDs = devices;
				id = devices[j];
				context = clCreateContext(props, 1, &id, NULL, NULL, &err);
				//context = clCreateContext(props, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL, &err);
			}
		}
	}

	if (context != NULL) {
		Process(context, id);
		//clReleaseContext(context);
	}

	return 0;
}

void ShowVersion()
{
	const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字  
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台  
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号  
	const GLubyte* Extensions  =glGetString(GL_EXTENSIONS);  
	const GLubyte* gluVersion = gluGetString(GLU_VERSION); //返回当前GLU工具库版本  
	printf("OpenGL实现厂商的名字：%s\n", name);
	printf("渲染器标识符：%s\n", biaoshifu);
	printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
	printf("OpenGL支持的扩展：%s\n",Extensions );  
	printf("OGLU工具库版本：%s\n", gluVersion);
}

void MMain() {
	const int WIDTH = 500;
	const int HEIGHT = 500;

	// Create a memory DC compatible with the screen
	HDC hdc = CreateCompatibleDC(0);
	if (hdc == 0) std::cout << "Could not create memory device context";

	// Create a bitmap compatible with the DC
	// must use CreateDIBSection(), and this means all pixel ops must be synchronised
	// using calls to GdiFlush() (see CreateDIBSection() docs)
	BITMAPINFO bmi = {
		{ sizeof(BITMAPINFOHEADER), WIDTH, HEIGHT, 1, 32, BI_RGB, 0, 0, 0, 0, 0 },
		{ 0 }
	};
	DWORD *pbits; // pointer to bitmap bits
	HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void **)&pbits,
		0, 0);
	if (hbm == 0) std::cout << "Could not create bitmap";

	//HDC hdcScreen = GetDC(0);
	//HBITMAP hbm = CreateCompatibleBitmap(hdcScreen,WIDTH,HEIGHT);

	// Select the bitmap into the DC
	HGDIOBJ r = SelectObject(hdc, hbm);
	if (r == 0) std::cout << "Could not select bitmap into DC";

	// Choose the pixel format
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // struct size
		1, // Version number
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL, // use OpenGL drawing to BM
		PFD_TYPE_RGBA, // RGBA pixel values
		32, // color bits
		0, 0, 0, // RGB bits shift sizes...
		0, 0, 0, // Don't care about them
		0, 0, // No alpha buffer info
		0, 0, 0, 0, 0, // No accumulation buffer
		32, // depth buffer bits
		0, // No stencil buffer
		0, // No auxiliary buffers
		PFD_MAIN_PLANE, // Layer type
		0, // Reserved (must be 0)
		0, // No layer mask
		0, // No visible mask
		0 // No damage mask
	};
	int pfid = ChoosePixelFormat(hdc, &pfd);
	if (pfid == 0) std::cout << "Pixel format selection failed";

	// Set the pixel format
	// - must be done *after* the bitmap is selected into DC
	BOOL b = SetPixelFormat(hdc, pfid, &pfd);
	if (!b) std::cout << "Pixel format set failed";

	// Create the OpenGL resource context (RC) and make it current to the thread
	HGLRC hglrc = wglCreateContext(hdc);
	if (hglrc == 0) std::cout << "OpenGL resource context creation failed";
	wglMakeCurrent(hdc, hglrc);

	int ret = clInit();

	wglDeleteContext(hglrc); // Delete RC

	SelectObject(hdc, r); // Remove bitmap from DC
	DeleteObject(hbm); // Delete bitmap
	DeleteDC(hdc); // Delete DC
}

void Callback() {
	glProcess(vbo, 640 * 480);
}

int glMain(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("测试");

	glewInit();
	ShowVersion();

	int ret = clInit();

	glutDisplayFunc(Callback);
	glutMainLoop();
	return 0;
}

int main(int argc, char **argv) {
	glMain(argc,argv);

	//glewInit();
	//ShowVersion();
	//MMain();
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	
	MMain();
	return 0;
}