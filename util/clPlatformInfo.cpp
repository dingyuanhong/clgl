#include <stdlib.h>
#include <stdio.h>
#include "clPlatformInfo.h"

void printPlatformInfo(FILE * f,cl_platform_id platformid)
{
	fprintf(f,"Platform:------------------------------------------------------------\n");
	char buf[1024] = { 0 };
	size_t size = 0;
	cl_int err = clGetPlatformInfo(platformid, CL_PLATFORM_NAME, 1024, buf, &size);
	if (err >= 0 && size > 0) {
		fprintf(f, "Name:%s\n", buf);
	}
	size = 0;
	err = clGetPlatformInfo(platformid, CL_PLATFORM_PROFILE, 1024, buf, &size);
	if (err >= 0 && size > 0) {
		fprintf(f, "Profile:%s\n", buf);
	}
	size = 0;
	err = clGetPlatformInfo(platformid, CL_PLATFORM_VERSION, 1024, buf, &size);
	if (err >= 0 && size > 0) {
		fprintf(f, "Version:%s\n", buf);
	}
	size = 0;
	err = clGetPlatformInfo(platformid, CL_PLATFORM_VENDOR, 1024, buf, &size);
	if (err >= 0 && size > 0) {
		fprintf(f, "Vendor:%s\n", buf);
	}
	size = 0;
	err = clGetPlatformInfo(platformid, CL_PLATFORM_EXTENSIONS, 1024, buf, &size);
	if (err >= 0 && size > 0) {
		fprintf(f, "Extensions:%s\n", buf);
	}
	size = 0;
	err = clGetPlatformInfo(platformid, CL_PLATFORM_HOST_TIMER_RESOLUTION, 1024, buf, &size);
	if (err >= 0 && size > 0) {
		if (size == 8) {
			cl_long timer = *(cl_long*)buf;
			fprintf(f, "HostTimerResolution:%I64d\n", timer);
		}
		else if (size == 4) {
			cl_int timer = *(cl_int*)buf;
			fprintf(f, "HostTimerResolution:%d\n", timer);
		}
		else {
			fprintf(f, "HostTimerResolution:%s(%d)\n", buf,size);
		}
	}
	fprintf(f, "------------------------------------------------------------\n");
}

std::vector<cl_device_id> GetDeviceIDs(cl_platform_id platformid, cl_uint deviceType)
{
	std::vector<cl_device_id> deviceIDs;
	cl_uint deviceNum = 0;
	cl_int err = clGetDeviceIDs(platformid, deviceType, 0, NULL, &deviceNum);
	if (err < 0) return deviceIDs;

	std::vector<cl_device_id> devices(deviceNum);
	err = clGetDeviceIDs(platformid, deviceType, deviceNum, &devices[0], &deviceNum);
	if (err < 0) {
		devices.clear();
	}

	return devices;
}

std::vector<cl_platform_id> GetPlatformIDs()
{
	std::vector<cl_platform_id> platforms;
	cl_uint num = 0;
	cl_int err = clGetPlatformIDs(0, 0, &num);
	if (err < 0) {
		return platforms;
	}

	platforms.resize(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);
	if (err < 0) {
		platforms.clear();
		return platforms;
	}
	return platforms;
}