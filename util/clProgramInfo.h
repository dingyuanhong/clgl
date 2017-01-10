#pragma once

#include "CL\opencl.h"

char *GetFileData(const char * path);

cl_program CreateProgram(cl_context context, const char * file);