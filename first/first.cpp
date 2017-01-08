#include "CL\opencl.h"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include<string>
#include <fstream>
#include <istream>
#include <iostream>
#include <iterator>
#pragma comment(lib,"OpenCL.lib")

char *Read(const char * path) {
	FILE * fp = NULL;
	fopen_s(&fp,path,"rb");
	if (fp == NULL) return NULL;
	fseek(fp,0,SEEK_END);
	long size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	void *buf = malloc(size + 2);
	if (buf == NULL) return NULL;
	memset(buf,0,size+2);

	fread(buf,1,size,fp);
	fclose(fp);

	return (char*)buf;
}

cl_program CreateProgram(cl_context context,const char * file) {
	char * Source = Read(file);
	if (Source == NULL) return NULL;

	//std::fstream fileOpen("./Kernal1.cl");
	//std::string programString(std::istreambuf_iterator<char>(fileOpen), (std::istreambuf_iterator<char>()));
	//const char *programCodeSource = programString.c_str();

	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&Source, NULL, NULL);
	if (program == NULL) return program;
	cl_int err = clBuildProgram(program, 0, 0, NULL, NULL, NULL);
	//cl_int err = clBuildProgram(program,1, &IDs[0],NULL,NULL,NULL);

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

int Process(cl_context context, std::vector<cl_device_id> IDs) {
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
	cl_command_queue cqueue = clCreateCommandQueueWithProperties(context, IDs[0], &properties, &err);
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

	return 0;
}

int main() {
	cl_uint num = 0;
	cl_int err = clGetPlatformIDs(0,0,&num);
	if (err < 0) {
		return -1;
	}
	std::vector<cl_platform_id> platforms(num);
	err = clGetPlatformIDs(num,&platforms[0],&num);
	if (err < 0) {
		return -1;
	}

	printf("PlatformIDs:%d \n",num);

	cl_context context = NULL;
	cl_device_id id = NULL;
	std::vector<cl_device_id> deviceIDs;

	for (cl_int i = 0; i < num; i++) {
		printf("%s %d:\n", "PLATFORM",i);
		char buf[1024] = { 0 };
		size_t size = 0;
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE,1024, buf,&size);
		if (err < 0) continue;
		printf("	PROFILE:%s\n",buf);
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 1024, buf, &size);
		if (err < 0) continue;
		printf("	VERSION:%s\n", buf);
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, buf, &size);
		if (err < 0) continue;
		printf("	NAME:%s\n", buf);
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 1024, buf, &size);
		if (err < 0) continue;
		printf("	VENDOR:%s\n", buf);
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 1024, buf, &size);
		if (err < 0) continue;
		printf("	EXTENSIONS:%s\n", buf);
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_HOST_TIMER_RESOLUTION, 1024, buf, &size);
		if (err < 0) continue;
		printf("	TIMER:%s\n", buf);

		cl_uint deviceNum = 0;
		err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL,0,NULL,&deviceNum);
		if (err < 0) continue;

		std::vector<cl_device_id> devices(deviceNum);
		err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceNum, &devices[0], &deviceNum);
		if (err < 0) continue;

		printf("	  Devices:%d\n",deviceNum);
		char deviceBuf[1024] = { 0 };
		size_t deviceSize = 0;
		for (cl_int j = 0; j < deviceNum; j++) {
			err = clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, 1024, deviceBuf, &deviceSize);
			if (err < 0) continue;
			printf("		TYPE:%s\n", deviceBuf);
			err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME,1024, deviceBuf,&deviceSize);
			if (err < 0) continue;
			printf("		NAME:%s\n",deviceBuf);

			if (context == NULL) {
				cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[i]), 0 };
				context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, &err);
				deviceIDs = devices;
			}
		}
	}
	
	if (context != NULL) {
		Process(context, deviceIDs);
		clReleaseContext(context);
	}

	return 0;
}