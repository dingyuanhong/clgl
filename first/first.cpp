//用于GPU使用QUEUE
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include "CL\opencl.h"
#include "clProgramInfo.h"
#include "clPlatformInfo.h"
#include "clDeviceInfo.h"

#pragma comment(lib,"OpenCL.lib")

#include <vector>

int Process(cl_context context, cl_device_id id) {
	cl_program program = CreateProgram(context, "./Kernal1.cl");
	if (program == NULL) return -1;

#define DATA_SIZE 10
	//内存参数
	std::vector<float> a(DATA_SIZE), b(DATA_SIZE);
	for (int i = 0; i < DATA_SIZE; i++) {
		a[i] = i;
		b[i] = i;
	}

	cl_int err = 0;
	//准备参数
	cl_mem cl_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * DATA_SIZE, &a[0], &err);
	cl_mem cl_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * DATA_SIZE, &b[0], &err);
	cl_mem cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * DATA_SIZE, NULL, &err);
	//关联核参数
	cl_kernel adder = clCreateKernel(program, "hello_kernel", &err);
	err = clSetKernelArg(adder, 0, sizeof(cl_mem), &cl_a);
	err = clSetKernelArg(adder, 1, sizeof(cl_mem), &cl_b);
	err = clSetKernelArg(adder, 2, sizeof(cl_mem), &cl_res);

	cl_queue_properties properties = 0;
	//创建执行队列
	cl_command_queue cqueue = clCreateCommandQueueWithProperties(context, id, &properties, &err);
	if (err == CL_INVALID_DEVICE) {
		//http://stackoverflow.com/questions/37016928/clcreatecontext-succeeds-but-clcreatecommandqueue-fails-with-33

		cl_command_queue_properties prop = 0;
		cqueue = clCreateCommandQueue(context, id, prop, &err);
	}
	if (cqueue != NULL) {
		//执行核
		cl_uint work_dims = 2;
		size_t global_work_offset[] = { 0 , 5};
		size_t global_work_size[] = { 2 , 2 };
		size_t local_work_size[] = { 2 , 1 };
		err = clEnqueueNDRangeKernel(cqueue, adder, work_dims, global_work_offset, global_work_size, local_work_size, 0, 0, 0);
		//拷贝结果
		std::vector<float> res(DATA_SIZE);
		err = clEnqueueReadBuffer(cqueue, cl_res, CL_TRUE, 0, sizeof(float) * DATA_SIZE, &res[0], 0, 0, 0);
		err = clFinish(cqueue);
	}
	//释放核
	clReleaseCommandQueue(cqueue);
	clReleaseMemObject(cl_a);
	clReleaseMemObject(cl_b);
	clReleaseMemObject(cl_res);

	clReleaseProgram(program);

	if (err > 0) return 0;
	return err;
}

#include "glVersion.h"
#include "gl\glut.h"

#pragma comment(lib,"Opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glut32.lib")

void glInitWindow() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("测试");
	glewInit();
	glVersion();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glInitWindow();

	std::vector<cl_platform_id> platforms = GetPlatformIDs();
	printf("PlatformIDs:%d \n", platforms.size());

	std::vector<cl_platform_device_info> infos;
	for (cl_int i = 0; i < platforms.size(); i++) {
		printf("%s %d:\n", "PLATFORM",i);
		printPlatformInfo(platforms[i]);

		std::vector<cl_device_id> deviceIDs = GetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL);
		printf("Devices:%d\n", deviceIDs.size());

		for (cl_int j = 0; j < deviceIDs.size(); j++) {
			printf("ID:%d\n",j);
			printDeviceInfo(deviceIDs[j]);

			cl_platform_device_info info = { 0 };
			info.platform = platforms[i];
			info.device = deviceIDs[j];
			infos.push_back(info);
		}
	}

	if (infos.size() > 0) {
		int index = 2;
		int err = 0;
		cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(infos[index].platform), 0 };
		cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, &err);

		if (context != NULL) {
			printf("Context:\n");
			printContextInfo(context);
			err = Process(context, infos[index].device);
			clReleaseContext(context);

			if (err >= 0) {
				printf("执行成功.\n");
			}
			else {
				printf("执行失败.(%d)\n",err);
			}
		}
	}

	getchar();
	return 0;
}