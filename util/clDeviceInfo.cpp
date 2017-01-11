#include <stdio.h>
#include "clDeviceInfo.h"

const char* GetDeviceType(cl_device_type it)
{
	if (it == CL_DEVICE_TYPE_CPU)
		return "CPU";
	else if (it == CL_DEVICE_TYPE_GPU)
		return "GPU";
	else if (it == CL_DEVICE_TYPE_ACCELERATOR)
		return "ACCELERATOR";
	else if (it == CL_DEVICE_TYPE_CUSTOM) {
		return "CUSTOM";
	}
	else
		return "DEFAULT";
}

cl_device_type getDeviceType(cl_device_id id)
{
	char buffer[1024] = { 0 };
	size_t size = 0;
	int err = clGetDeviceInfo(id, CL_DEVICE_TYPE, 1024, buffer, &size);
	if (err >= 0 && size > 0) {
		cl_device_type type = *(cl_device_type*)buffer;
		return type;
	}
	return CL_DEVICE_TYPE_ALL;
}

const char* GetMemCacheType(cl_device_mem_cache_type it)
{
	if (it == CL_NONE)
		return "NONE Cache";
	else if (it == CL_READ_ONLY_CACHE)
		return "READONLY Cache";
	else if (it == CL_READ_WRITE_CACHE)
		return "READWRITE Cache";
	else
		return "UNKNOWN";
}

const char* GetExecCapability(cl_device_exec_capabilities it)
{
	if (it == CL_EXEC_KERNEL)
		return "EXEC KERNEL";
	else if (it == CL_EXEC_NATIVE_KERNEL)
		return "EXEC NATIVE KERNEL";
	else
		return "UNKNOWN";
}

//void printGB(cl_long value) 
#define printGB(v)	\
{															\
	cl_long value = (v);									\
	cl_int bValue = value % 1024;							\
	cl_int kbValue = (cl_int)(value / 1024) % 1024;			\
	cl_int mbValue = (cl_int)(value / (1024 * 1024)) % 1024;\
	cl_int gbValue = (cl_int)(value / (1024 * 1024)) / 1024;\
	if (gbValue > 0) {				\
		fprintf(f, "%dg.", gbValue);\
	}								\
	if (mbValue > 0) {				\
		fprintf(f, "%dm.", mbValue);\
	}								\
	if (kbValue > 0) {				\
		fprintf(f, "%dk.", kbValue);\
	}								\
	if (bValue > 0) {				\
		fprintf(f, "%db.", bValue);	\
	}								\
}

#define PRINTDRIVER()	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DRIVER_VERSION, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		fprintf(f, "  DriverVersion : %s\n", buffer);										\
	}

#define PRINT(name,type)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		fprintf(f,"  " #name ": %s\n", buffer);							\
	}

#define PRINTHEX(name,type)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		if(size == 4){														\
			cl_uint value = *((cl_uint*)buffer);							\
			fprintf(f,"  " #name ": %04X\n", value);					\
		}																	\
		else if (size == 8) {												\
			cl_long value = *((cl_long*)buffer);							\
			fprintf(f,"  " #name ": %0I64X\n", value);							\
		}																	\
		else {																\
			fprintf(f,"  " #name ": %s(%d)\n", buffer,size);					\
		}																	\
	}

#define PRINTINTGER(name , type)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		if(size == 4){														\
			cl_uint value = *((cl_uint*)buffer);							\
			fprintf(f,"  " #name ": %u\n", value);								\
		}																	\
		else if (size == 8) {												\
			cl_long value = *((cl_long*)buffer);							\
			fprintf(f,"  " #name ": %I64u\n", value);							\
		}																	\
		else {																\
			fprintf(f,"  " #name ": %s(%d)\n", buffer,size);					\
		}																	\
	}

#define PRINTMEMORYSIZE(name , type)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		if(size == 4){														\
			cl_uint v = *((cl_uint*)buffer);							\
			fprintf(f,"  " #name ": %u(",v);								\
			printGB(v);													\
			fprintf(f, ")\n");												\
		}																	\
		else if (size == 8) {												\
			cl_long v = *((cl_long*)buffer);							\
			fprintf(f,"  " #name ": %I64u(",v);								\
			printGB(v);													\
			fprintf(f, ")\n");												\
		}																	\
		else {																\
			fprintf(f,"  " #name ": %s(%d)\n", buffer,size);					\
		}																	\
	}

#define PRINTTYPE(name)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_TYPE, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		cl_device_type type = *((cl_device_type*)buffer);					\
		fprintf(f,"  " #name ": %s(%d)\n", GetDeviceType(type),(cl_uint)type);	\
	}

#define PRINTMEMCACHETYPE(name,type)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		cl_device_mem_cache_type value = *((cl_device_mem_cache_type*)buffer);\
		fprintf(f,"  " #name ": %s(%d)\n", GetMemCacheType(value),(cl_uint)value);\
	}

#define PRINTEXECCAPABILITY(name,type)	\
	err = 0;																\
	err = clGetDeviceInfo(deviceid, CL_DEVICE_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {												\
		cl_device_exec_capabilities value = *((cl_device_exec_capabilities*)buffer);\
		fprintf(f,"  " #name ": %s(%d)\n", GetExecCapability(value),(cl_uint)value);	\
	}


void printDeviceInfo(FILE * f,cl_device_id id)
{
	fprintf(f, "Opencl Device Information:\n");
	char buffer[1024] = { 0 };
	size_t size = 0;
	cl_int err = 0;
	cl_device_id deviceid = id;
	//类型
	PRINTTYPE(Type);
	//名称
	PRINT(Name, NAME);
	//Platform
	PRINTHEX(Platform, PLATFORM);
	//供应商ID
	PRINTHEX(VendorID, VENDOR_ID);
	//供应商
	PRINT(Vendor, VENDOR);
	//版本
	PRINT(Version, VERSION);
	//简况
	PRINT(Profile, PROFILE);
	PRINT(Extensions, EXTENSIONS);
	//opencl版本
	PRINT(OpenCL_CVersion, OPENCL_C_VERSION);
	PRINTDRIVER();

	//工作队列大小
	PRINTINTGER(MaxComputeUnits, MAX_COMPUTE_UNITS);
	PRINTINTGER(MaxWorkItemDimensions, MAX_WORK_ITEM_DIMENSIONS);
	PRINTINTGER(MaxWorkGroupSize, MAX_WORK_GROUP_SIZE);
	PRINTINTGER(MaxWorkItemSizes, MAX_WORK_ITEM_SIZES);

	//参数最大大小
	PRINTINTGER(MaxParameterSize, MAX_PARAMETER_SIZE);
	//最大采样数
	PRINTINTGER(MaxSamplers, MAX_SAMPLERS);
	//地址比特数
	PRINTINTGER(AddressBits, ADDRESS_BITS);
	//内存地址对其值
	PRINTINTGER(MemBaseAddrAlign, MEM_BASE_ADDR_ALIGN);
	//最小的数据类型对其大小
	PRINTINTGER(MinDataTypeAlignSize, MIN_DATA_TYPE_ALIGN_SIZE);
	//全局缓存类型
	PRINTMEMCACHETYPE(GlobalMemCacheType, GLOBAL_MEM_CACHE_TYPE);
	PRINTMEMORYSIZE(GlobalMemCacheLineSize, GLOBAL_MEM_CACHELINE_SIZE);
	PRINTMEMORYSIZE(GlobalMemCacheSize, GLOBAL_MEM_CACHE_SIZE);
	PRINTMEMORYSIZE(GlobalMemSize, GLOBAL_MEM_SIZE);
	//最大常数缓冲区大小
	PRINTMEMORYSIZE(MaxConstantBufferSize, MAX_CONSTANT_BUFFER_SIZE);
	//常数参数最大个数
	PRINTINTGER(MaxConstantArgs, MAX_CONSTANT_ARGS);
	//本地缓存类型
	PRINTMEMCACHETYPE(LocalMemType, LOCAL_MEM_TYPE);
	PRINTMEMORYSIZE(LocalMemSize, LOCAL_MEM_SIZE);

	//设备可用
	PRINTINTGER(Available, AVAILABLE);
	//编译器可用
	PRINTINTGER(CompilerAvailable, COMPILER_AVAILABLE);
	//链接器可用
	PRINTINTGER(LinkAvailable, LINKER_AVAILABLE);
	//执行能力
	PRINTEXECCAPABILITY(ExcutionCapabilities, EXECUTION_CAPABILITIES);
	//内核支持参数
	PRINT(BuiltInKernels, BUILT_IN_KERNELS);

	//优先矢量宽度
	PRINTINTGER(PreferredVectorWidthChar, PREFERRED_VECTOR_WIDTH_CHAR);
	PRINTINTGER(PreferredVectorWidthShort, PREFERRED_VECTOR_WIDTH_SHORT);
	PRINTINTGER(PreferredVectorWidthInt, PREFERRED_VECTOR_WIDTH_INT);
	PRINTINTGER(PreferredVectorWidthLong, PREFERRED_VECTOR_WIDTH_LONG);
	PRINTINTGER(PreferredVectorWidthFloat, PREFERRED_VECTOR_WIDTH_FLOAT);
	PRINTINTGER(PreferredVectorWidthDouble, PREFERRED_VECTOR_WIDTH_DOUBLE);
	PRINTINTGER(PreferredVectorWidthHalf, PREFERRED_VECTOR_WIDTH_HALF);
	//本地矢量宽度
	PRINTINTGER(NativeVectorWidthChar , NATIVE_VECTOR_WIDTH_CHAR);
	PRINTINTGER(NativeVectorWidthShort , NATIVE_VECTOR_WIDTH_SHORT);
	PRINTINTGER(NativeVectorWidthInt , NATIVE_VECTOR_WIDTH_INT);
	PRINTINTGER(NativeVectorWidthLong , NATIVE_VECTOR_WIDTH_LONG);
	PRINTINTGER(NativeVectorWidthFloat , NATIVE_VECTOR_WIDTH_FLOAT);
	PRINTINTGER(NativeVectorWidthDouble , NATIVE_VECTOR_WIDTH_DOUBLE);
	PRINTINTGER(NativeVectorWidtHalf , NATIVE_VECTOR_WIDTH_HALF);
	
	//是否支持图像
	PRINTINTGER(ImageSupport, IMAGE_SUPPORT);
	PRINTINTGER(MaxReadImageArgs, MAX_READ_IMAGE_ARGS);
	PRINTINTGER(MaxWriteImageArgs, MAX_WRITE_IMAGE_ARGS);
	PRINTINTGER(MaxReadWriteImageArgs, MAX_READ_WRITE_IMAGE_ARGS);
	//图像缓冲区
	PRINTMEMORYSIZE(MaxMemAllocSize, MAX_MEM_ALLOC_SIZE);
	PRINTMEMORYSIZE(ImageMaxBufferSize, IMAGE_MAX_BUFFER_SIZE);
	PRINTINTGER(ImageMaxArraySize, IMAGE_MAX_ARRAY_SIZE);
	//图像丢弃队列长度
	PRINTINTGER(ImagePitchAlignment, IMAGE_PITCH_ALIGNMENT);
	//图像基地址队列
	PRINTINTGER(ImageBaseAddressAlignment, IMAGE_BASE_ADDRESS_ALIGNMENT);
	//图像大小
	PRINTINTGER(Image2dMaxWidth, IMAGE2D_MAX_WIDTH);
	PRINTINTGER(Image2dMaxHeight, IMAGE2D_MAX_HEIGHT);
	PRINTINTGER(Image3dMaxWidth, IMAGE3D_MAX_WIDTH);
	PRINTINTGER(Image3dMaxHeight, IMAGE3D_MAX_HEIGHT);
	PRINTINTGER(Image3dMaxDepth, IMAGE3D_MAX_DEPTH);

	//队列性能
	PRINTINTGER(QueueProperties, QUEUE_PROPERTIES);
	//主机队列性能
	PRINTINTGER(QueueOnHostProperties, QUEUE_ON_HOST_PROPERTIES);
	//分区性能 
	PRINTINTGER(PartitionProperties, PARTITION_PROPERTIES);
	//分区类型
	PRINTINTGER(PARTITION_TYPE , PARTITION_TYPE);	/*PARTITION_TYPE PartitionType*/ //可能没有
	//设备起源
	PRINTINTGER(ParentDevice, PARENT_DEVICE);
	//分区的最大子设备数
	PRINTINTGER(PartitionMaxSubDevices, PARTITION_MAX_SUB_DEVICES);
	//分区域关系
	PRINTHEX(PartitionAffinityDomain, PARTITION_AFFINITY_DOMAIN);
	//引用计数
	PRINTINTGER(PeferenceCount, REFERENCE_COUNT);

	//引用用户互操作同步
	PRINTINTGER(PreferredInteropUserSync , PREFERRED_INTEROP_USER_SYNC);
	
	{
		PRINTMEMORYSIZE(QUEUE_ON_DEVICE_PREFERRED_SIZE, QUEUE_ON_DEVICE_PREFERRED_SIZE); //可能没有
		PRINTINTGER(GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE, GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE);
		PRINTINTGER(PREFERRED_PLATFORM_ATOMIC_ALIGNMENT, PREFERRED_PLATFORM_ATOMIC_ALIGNMENT);
		PRINTINTGER(PREFERRED_GLOBAL_ATOMIC_ALIGNMENT, PREFERRED_GLOBAL_ATOMIC_ALIGNMENT);
		PRINTINTGER(PREFERRED_LOCAL_ATOMIC_ALIGNMENT, PREFERRED_LOCAL_ATOMIC_ALIGNMENT);

		//最大全局变量大小
		PRINTINTGER(MAX_GLOBAL_VARIABLE_SIZE, MAX_GLOBAL_VARIABLE_SIZE);
		//设备队列性能
		PRINTINTGER(QUEUE_ON_DEVICE_PROPERTIES, QUEUE_ON_DEVICE_PROPERTIES);
		//设备队列长度
		PRINTINTGER(QUEUE_ON_DEVICE_MAX_SIZE, QUEUE_ON_DEVICE_MAX_SIZE);
		//设备队列长度
		PRINTINTGER(MAX_ON_DEVICE_QUEUES, MAX_ON_DEVICE_QUEUES);
		//设备事件数
		PRINTINTGER(MAX_ON_DEVICE_EVENTS, MAX_ON_DEVICE_EVENTS);
		//管道参数最大数
		PRINTINTGER(MAX_PIPE_ARGS, MAX_PIPE_ARGS);
		//管道最大活动保留数
		PRINTINTGER(PIPE_MAX_ACTIVE_RESERVATIONS, PIPE_MAX_ACTIVE_RESERVATIONS);
		//管道最大包大小
		PRINTINTGER(PIPE_MAX_PACKET_SIZE, PIPE_MAX_PACKET_SIZE);
		//子组最大数
		PRINTINTGER(MAX_NUM_SUB_GROUPS, MAX_NUM_SUB_GROUPS);
		//子组独立的进度
		PRINTINTGER(SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS, SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS);

		//内存版本
		PRINTINTGER(IL_VERSION, IL_VERSION);
		//向量机能力
		PRINTINTGER(SVM_CAPABILITIES, SVM_CAPABILITIES);
	}

	//单点配置
	PRINTINTGER(SingleFPConfig, SINGLE_FP_CONFIG);
	PRINTINTGER(DoubleFpConfig, DOUBLE_FP_CONFIG);

	//错误纠正支持
	PRINTINTGER(ErrorCorrectionSupport, ERROR_CORRECTION_SUPPORT);
	//分析定时器的分辨率
	PRINTINTGER(ProfiLingTimerResolution, PROFILING_TIMER_RESOLUTION);
	//时钟最大频率
	PRINTINTGER(MaxClockFrequency, MAX_CLOCK_FREQUENCY);
	//小端字节序
	PRINTINTGER(EndianLittle, ENDIAN_LITTLE);
	//是否主机统一内存
	PRINTINTGER(HostUnifiedMemory, HOST_UNIFIED_MEMORY);
	//打印输出缓冲区
	PRINTMEMORYSIZE(PrintfBufferSize, PRINTF_BUFFER_SIZE);

	fprintf(f, "------------------------------------------------------------\n");
}


#undef PRINT
#undef PRINTHEX
#undef PRINTINTGER
#undef PRINTMEMORYSIZE
#undef PRINTMEMCACHETYPE
#undef PRINTEXECCAPABILITY

#define PRINTINTGER(name , type)	\
	size = 0;	\
	err = clGetContextInfo(context, CL_CONTEXT_##type, 1024, buffer, &size);	\
	if (err >= 0 && size > 0) {									\
		if (size == 4) {										\
			cl_uint value = *((cl_uint*)buffer);				\
			fprintf(f,"  " #name ": %u\n", value);						\
		}														\
		else if (size == 8) {									\
			cl_long value = *((cl_long*)buffer);				\
			fprintf(f,"  " #name ": %0I64u\n", value);					\
		}														\
		else {													\
			fprintf(f,"  " #name ": %s(%d)\n", buffer, size);			\
		}														\
	}

void printContextInfo(FILE * f, cl_context context)
{
	fprintf(f, "Opencl Context Information:\n");
	char buffer[1024] = { 0 };
	size_t size = 0;
	cl_int err = 0;

	PRINTINTGER(REFERENCE_COUNT, REFERENCE_COUNT);
	PRINTINTGER(PROPERTIES, PROPERTIES);

	cl_int deviceNum = 0;
	err = clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, 1024, buffer, &size);
	if (err >= 0) {
		deviceNum = *((cl_int*)buffer);
	}
	err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 1024, buffer, &size);
	if (err >= 0 && size > 0) {
		cl_device_id *value = ((cl_device_id*)buffer);
		printf("  DEVICES:");
		for (cl_int i = 0; i < deviceNum; i++) {
			fprintf(f, " %d:0x%p",i, value[i]);
		}
		fprintf(f, "\n");
	}

	fprintf(f, "--------------------------------------------------------------------------------\n");
}