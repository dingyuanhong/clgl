#pragma once
#include "CL\opencl.h"
#include <vector>

void printPlatformInfo(cl_platform_id platformid);

std::vector<cl_device_id> GetDeviceIDs(cl_platform_id platformid,cl_uint deviceType);

std::vector<cl_platform_id> GetPlatformIDs();