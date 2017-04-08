#pragma once
#include "CL\opencl.h"

typedef struct CL_PLATFORM_DEVICE_INFO {
	cl_platform_id platform;
	cl_device_id   device;
}cl_platform_device_info;

void printDeviceInfo(FILE * f,cl_device_id id);

void printContextInfo(FILE * f,cl_context context);

cl_device_type getDeviceType(cl_device_id deviceid);

int getDeviceCLVersion(cl_device_id deviceid);