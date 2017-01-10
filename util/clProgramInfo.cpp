#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include "clProgramInfo.h"

char *GetFileData(const char * path){
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
	char * Source = GetFileData(file);
	if (Source == NULL) return NULL;

	/*std::fstream fileOpen(file);
	std::string programString(std::istreambuf_iterator<char>(fileOpen), (std::istreambuf_iterator<char>()));
	const char *programCodeSource = programString.c_str();*/

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
			free(buffer);
		}
		else {
			printf("BuildProgram:Error:%d\n", err);
		}

		clReleaseProgram(program);
		return NULL;
	}

	return program;
}