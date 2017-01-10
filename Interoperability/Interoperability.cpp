#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <istream>

#include "glVersion.h"
#include "gl\glut.h"
#pragma comment(lib,"Opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glut32.lib")

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include "clPlatformInfo.h"
#include "clDeviceInfo.h"
#include "clProgramInfo.h"
#include "clContextInfo.h"
#pragma comment(lib,"OpenCL.lib")

cl_context CreateContext(cl_device_type type) {
	std::vector<cl_platform_id> platforms = GetPlatformIDs();

	printf("PlatformIDs:%d \n", platforms.size());

	cl_context context = NULL;
	cl_platform_device_info info = { 0 };

	for (cl_int i = 0; i < platforms.size(); i++) {
		char buf[1024] = { 0 };
		size_t size = 0;
		int err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 1024, buf, &size);
		if (err < 0) continue;

		if (strstr(buf, "cl_khr_gl_sharing") == NULL) {
			continue;
		}

		std::vector<cl_device_id> devices = GetDeviceIDs(platforms[i], type /*CL_DEVICE_TYPE_GPU*/);
		for (cl_int j = 0; j < devices.size(); j++) {
			if (context == NULL) {
				//cl_context_properties props[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[i]), 0 };
				//context = clCreateContext(props, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL, &err);

				cl_context_properties * props = GetGLContextProperties(platforms[i]);
				info.platform = platforms[i];
				info.device = devices[j];
				context = clCreateContext(props, 1, &info.device, NULL, NULL, &err);
			}
		}
	}

	return context;
}

typedef struct TaskArgs {
	cl_program program;
	cl_kernel kernel;
	cl_command_queue queue;
	cl_mem vbo_mem;
}TaskArgs;

TaskArgs *initTask(cl_context context, GLuint vbo,int w,int h,int seq) {
	int err = 0;
	cl_mem vbo_mem = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, vbo, &err);

	cl_program program = CreateProgram(context, "./vboKernel.cl");
	if (program == NULL) {
		clReleaseMemObject(vbo_mem);
		return NULL;
	}

	cl_kernel kernel = clCreateKernel(program, "init_vbo_kernel", &err);

	char buffer[1024] = { 0 };
	size_t size = 0;
	err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 1024, &buffer[0], &size);
	cl_device_id device = *(cl_device_id*)buffer;
	err = clGetDeviceInfo(device,CL_DEVICE_OPENCL_C_VERSION,1024, &buffer[0], &size);

	//printDeviceInfo(device);

	cl_queue_properties properties = CL_QUEUE_ON_DEVICE | CL_QUEUE_PROFILING_ENABLE;
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, &properties, &err);
	if (err == CL_INVALID_DEVICE) {
		cl_command_queue_properties prop = 0;
		queue = clCreateCommandQueue(context,device,prop,&err);
	}
	if (err != 0) {
		printf("Error:clCreateCommandQueueWithProperties:%d\n",err);
	}

	TaskArgs *args = (TaskArgs*)malloc(sizeof(TaskArgs));
	args->program = program;
	args->kernel = kernel;
	args->queue = queue;
	args->vbo_mem = vbo_mem;
	return args;
}

void uninitTask(TaskArgs *args) {
	if (args->queue != NULL) {
		clFinish(args->queue);
		clReleaseCommandQueue(args->queue);
		args->queue = NULL;
	}
	if (args->kernel != NULL) {
		clReleaseKernel(args->kernel);
		args->kernel = NULL;
	}
	if (args->program != NULL) {
		clReleaseProgram(args->program);
		args->program = NULL;
	}
	if (args->vbo_mem != NULL) {
		clReleaseMemObject(args->vbo_mem);
		args->vbo_mem = NULL;
	}
}

int vboProcess(TaskArgs *args,int w,int h, int seq) {
	if(args->queue != NULL){
		clSetKernelArg(args->kernel, 0, sizeof(cl_mem), &args->vbo_mem);
		clSetKernelArg(args->kernel, 1, sizeof(cl_int), &w);
		clSetKernelArg(args->kernel, 2, sizeof(cl_int), &h);
		clSetKernelArg(args->kernel, 3, sizeof(cl_int), &seq);

		/*size_t tex_globalWorkSize[] = { (size_t)w*h };
		size_t tex_localWorkSize[] = { 0 };*/

		size_t tex_globalWorkSize[2] = { w, h };
		size_t tex_localWorkSize[2] = { 32, 4 };

		int err = clEnqueueAcquireGLObjects(args->queue, 1, &args->vbo_mem, 0, NULL, NULL);
		err = clEnqueueNDRangeKernel(args->queue, args->kernel, 1, NULL, tex_globalWorkSize, tex_localWorkSize, 0, NULL, NULL);
		err = clEnqueueReleaseGLObjects(args->queue, 1, &args->vbo_mem, 0, NULL, NULL);
		//clFinish(args->queue);

		return err;
	}
	else {
		return -1;
	}
}

void vboProcess(GLuint vbo) {
	cl_context context = CreateContext(CL_DEVICE_TYPE_GPU);
	TaskArgs *args = initTask(context, vbo, 640, 480, 0);
	glFinish();
	vboProcess(args, 640, 480, 0);
	if (args != NULL) {
		uninitTask(args);
		free(args);
	}
}

GLuint initVBO(int vbolen) {
	GLint bsize;
	GLuint vbo_buffer;
	glGenBuffers(1, &vbo_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_buffer);
	glBufferData(GL_ARRAY_BUFFER, vbolen*sizeof(float), NULL, GL_STREAM_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if ((GLuint)bsize != (vbolen*sizeof(float))) {
		printf("vbo Buffer size is error(%d)(%d).\n", vbo_buffer, bsize);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo_buffer;
}

GLuint initTexture(int width,int height) {
	GLuint tex = NULL;
	glGenTextures(1,&tex);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,tex);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,
		GL_RGBA32F_ARB,width,height,
		0,GL_LUMINANCE,GL_FLOAT,NULL);

	return tex;
}

void renderTexture(GLuint tex, int w, int h)
{
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(0, h);
		glVertex2f(0, h);
		glTexCoord2f(w, h);
		glVertex2f(w, h);
		glTexCoord2f(w, 0);
		glVertex2f(w, 0);
	glEnd();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void renderVBO(GLuint vbo, int vbolen) {
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glDrawArrays(GL_LINES, 0, vbolen * 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void DCProcess() {
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

	GLuint vbo = initVBO(640 * 480 * 4);
	vboProcess(vbo);

	wglDeleteContext(hglrc); // Delete RC

	SelectObject(hdc, r); // Remove bitmap from DC
	DeleteObject(hbm); // Delete bitmap
	DeleteDC(hdc); // Delete DC
}

int dcMain(int argc, char **argv) {
	glewInit();
	glVersion();
	DCProcess();
	return 0;
}

void glInitWindow() {
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("²âÊÔ");
	glewInit();
	glVersion();

	if (glewIsSupported("GL_VERSION_2_1"))
		printf("Ready for OpenGL 2.1\n");
	else {
		printf("Warning: Detected that OpenGL 2.1 not supported\n");
	}
}

GLuint tex = NULL;
GLuint vbo = NULL;
TaskArgs *args = NULL;
int seq = 0;

void renderScene() {
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glFinish();
	vboProcess(args, 640, 480, seq++ );

	renderTexture(tex,640,480);
	renderVBO(vbo, 640 * 480);
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int glMain(int argc, char **argv) {
	glInitWindow();
	vbo = initVBO(640*480*4);
	tex = initTexture(640,480);

	cl_context context = CreateContext(CL_DEVICE_TYPE_GPU);
	args = initTask(context,vbo,640,480,0);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(reshape);
	glutMainLoop();

	if (args != NULL) {
		uninitTask(args);
		free(args);
	}
	return 0;
}

int main(int argc, char **argv) {
	return glMain(argc,argv);
	return dcMain(argc,argv);
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	return main(0,NULL);
}