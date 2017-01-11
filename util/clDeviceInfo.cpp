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
	//����
	PRINTTYPE(Type);
	//����
	PRINT(Name, NAME);
	//Platform
	PRINTHEX(Platform, PLATFORM);
	//��Ӧ��ID
	PRINTHEX(VendorID, VENDOR_ID);
	//��Ӧ��
	PRINT(Vendor, VENDOR);
	//�汾
	PRINT(Version, VERSION);
	//���
	PRINT(Profile, PROFILE);
	PRINT(Extensions, EXTENSIONS);
	//opencl�汾
	PRINT(OpenCL_CVersion, OPENCL_C_VERSION);
	PRINTDRIVER();

	//�������д�С
	PRINTINTGER(MaxComputeUnits, MAX_COMPUTE_UNITS);
	PRINTINTGER(MaxWorkItemDimensions, MAX_WORK_ITEM_DIMENSIONS);
	PRINTINTGER(MaxWorkGroupSize, MAX_WORK_GROUP_SIZE);
	PRINTINTGER(MaxWorkItemSizes, MAX_WORK_ITEM_SIZES);

	//��������С
	PRINTINTGER(MaxParameterSize, MAX_PARAMETER_SIZE);
	//��������
	PRINTINTGER(MaxSamplers, MAX_SAMPLERS);
	//��ַ������
	PRINTINTGER(AddressBits, ADDRESS_BITS);
	//�ڴ��ַ����ֵ
	PRINTINTGER(MemBaseAddrAlign, MEM_BASE_ADDR_ALIGN);
	//��С���������Ͷ����С
	PRINTINTGER(MinDataTypeAlignSize, MIN_DATA_TYPE_ALIGN_SIZE);
	//ȫ�ֻ�������
	PRINTMEMCACHETYPE(GlobalMemCacheType, GLOBAL_MEM_CACHE_TYPE);
	PRINTMEMORYSIZE(GlobalMemCacheLineSize, GLOBAL_MEM_CACHELINE_SIZE);
	PRINTMEMORYSIZE(GlobalMemCacheSize, GLOBAL_MEM_CACHE_SIZE);
	PRINTMEMORYSIZE(GlobalMemSize, GLOBAL_MEM_SIZE);
	//�������������С
	PRINTMEMORYSIZE(MaxConstantBufferSize, MAX_CONSTANT_BUFFER_SIZE);
	//��������������
	PRINTINTGER(MaxConstantArgs, MAX_CONSTANT_ARGS);
	//���ػ�������
	PRINTMEMCACHETYPE(LocalMemType, LOCAL_MEM_TYPE);
	PRINTMEMORYSIZE(LocalMemSize, LOCAL_MEM_SIZE);

	//�豸����
	PRINTINTGER(Available, AVAILABLE);
	//����������
	PRINTINTGER(CompilerAvailable, COMPILER_AVAILABLE);
	//����������
	PRINTINTGER(LinkAvailable, LINKER_AVAILABLE);
	//ִ������
	PRINTEXECCAPABILITY(ExcutionCapabilities, EXECUTION_CAPABILITIES);
	//�ں�֧�ֲ���
	PRINT(BuiltInKernels, BUILT_IN_KERNELS);

	//����ʸ�����
	PRINTINTGER(PreferredVectorWidthChar, PREFERRED_VECTOR_WIDTH_CHAR);
	PRINTINTGER(PreferredVectorWidthShort, PREFERRED_VECTOR_WIDTH_SHORT);
	PRINTINTGER(PreferredVectorWidthInt, PREFERRED_VECTOR_WIDTH_INT);
	PRINTINTGER(PreferredVectorWidthLong, PREFERRED_VECTOR_WIDTH_LONG);
	PRINTINTGER(PreferredVectorWidthFloat, PREFERRED_VECTOR_WIDTH_FLOAT);
	PRINTINTGER(PreferredVectorWidthDouble, PREFERRED_VECTOR_WIDTH_DOUBLE);
	PRINTINTGER(PreferredVectorWidthHalf, PREFERRED_VECTOR_WIDTH_HALF);
	//����ʸ�����
	PRINTINTGER(NativeVectorWidthChar , NATIVE_VECTOR_WIDTH_CHAR);
	PRINTINTGER(NativeVectorWidthShort , NATIVE_VECTOR_WIDTH_SHORT);
	PRINTINTGER(NativeVectorWidthInt , NATIVE_VECTOR_WIDTH_INT);
	PRINTINTGER(NativeVectorWidthLong , NATIVE_VECTOR_WIDTH_LONG);
	PRINTINTGER(NativeVectorWidthFloat , NATIVE_VECTOR_WIDTH_FLOAT);
	PRINTINTGER(NativeVectorWidthDouble , NATIVE_VECTOR_WIDTH_DOUBLE);
	PRINTINTGER(NativeVectorWidtHalf , NATIVE_VECTOR_WIDTH_HALF);
	
	//�Ƿ�֧��ͼ��
	PRINTINTGER(ImageSupport, IMAGE_SUPPORT);
	PRINTINTGER(MaxReadImageArgs, MAX_READ_IMAGE_ARGS);
	PRINTINTGER(MaxWriteImageArgs, MAX_WRITE_IMAGE_ARGS);
	PRINTINTGER(MaxReadWriteImageArgs, MAX_READ_WRITE_IMAGE_ARGS);
	//ͼ�񻺳���
	PRINTMEMORYSIZE(MaxMemAllocSize, MAX_MEM_ALLOC_SIZE);
	PRINTMEMORYSIZE(ImageMaxBufferSize, IMAGE_MAX_BUFFER_SIZE);
	PRINTINTGER(ImageMaxArraySize, IMAGE_MAX_ARRAY_SIZE);
	//ͼ�������г���
	PRINTINTGER(ImagePitchAlignment, IMAGE_PITCH_ALIGNMENT);
	//ͼ�����ַ����
	PRINTINTGER(ImageBaseAddressAlignment, IMAGE_BASE_ADDRESS_ALIGNMENT);
	//ͼ���С
	PRINTINTGER(Image2dMaxWidth, IMAGE2D_MAX_WIDTH);
	PRINTINTGER(Image2dMaxHeight, IMAGE2D_MAX_HEIGHT);
	PRINTINTGER(Image3dMaxWidth, IMAGE3D_MAX_WIDTH);
	PRINTINTGER(Image3dMaxHeight, IMAGE3D_MAX_HEIGHT);
	PRINTINTGER(Image3dMaxDepth, IMAGE3D_MAX_DEPTH);

	//��������
	PRINTINTGER(QueueProperties, QUEUE_PROPERTIES);
	//������������
	PRINTINTGER(QueueOnHostProperties, QUEUE_ON_HOST_PROPERTIES);
	//�������� 
	PRINTINTGER(PartitionProperties, PARTITION_PROPERTIES);
	//��������
	PRINTINTGER(PARTITION_TYPE , PARTITION_TYPE);	/*PARTITION_TYPE PartitionType*/ //����û��
	//�豸��Դ
	PRINTINTGER(ParentDevice, PARENT_DEVICE);
	//������������豸��
	PRINTINTGER(PartitionMaxSubDevices, PARTITION_MAX_SUB_DEVICES);
	//�������ϵ
	PRINTHEX(PartitionAffinityDomain, PARTITION_AFFINITY_DOMAIN);
	//���ü���
	PRINTINTGER(PeferenceCount, REFERENCE_COUNT);

	//�����û�������ͬ��
	PRINTINTGER(PreferredInteropUserSync , PREFERRED_INTEROP_USER_SYNC);
	
	{
		PRINTMEMORYSIZE(QUEUE_ON_DEVICE_PREFERRED_SIZE, QUEUE_ON_DEVICE_PREFERRED_SIZE); //����û��
		PRINTINTGER(GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE, GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE);
		PRINTINTGER(PREFERRED_PLATFORM_ATOMIC_ALIGNMENT, PREFERRED_PLATFORM_ATOMIC_ALIGNMENT);
		PRINTINTGER(PREFERRED_GLOBAL_ATOMIC_ALIGNMENT, PREFERRED_GLOBAL_ATOMIC_ALIGNMENT);
		PRINTINTGER(PREFERRED_LOCAL_ATOMIC_ALIGNMENT, PREFERRED_LOCAL_ATOMIC_ALIGNMENT);

		//���ȫ�ֱ�����С
		PRINTINTGER(MAX_GLOBAL_VARIABLE_SIZE, MAX_GLOBAL_VARIABLE_SIZE);
		//�豸��������
		PRINTINTGER(QUEUE_ON_DEVICE_PROPERTIES, QUEUE_ON_DEVICE_PROPERTIES);
		//�豸���г���
		PRINTINTGER(QUEUE_ON_DEVICE_MAX_SIZE, QUEUE_ON_DEVICE_MAX_SIZE);
		//�豸���г���
		PRINTINTGER(MAX_ON_DEVICE_QUEUES, MAX_ON_DEVICE_QUEUES);
		//�豸�¼���
		PRINTINTGER(MAX_ON_DEVICE_EVENTS, MAX_ON_DEVICE_EVENTS);
		//�ܵ����������
		PRINTINTGER(MAX_PIPE_ARGS, MAX_PIPE_ARGS);
		//�ܵ����������
		PRINTINTGER(PIPE_MAX_ACTIVE_RESERVATIONS, PIPE_MAX_ACTIVE_RESERVATIONS);
		//�ܵ�������С
		PRINTINTGER(PIPE_MAX_PACKET_SIZE, PIPE_MAX_PACKET_SIZE);
		//���������
		PRINTINTGER(MAX_NUM_SUB_GROUPS, MAX_NUM_SUB_GROUPS);
		//��������Ľ���
		PRINTINTGER(SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS, SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS);

		//�ڴ�汾
		PRINTINTGER(IL_VERSION, IL_VERSION);
		//����������
		PRINTINTGER(SVM_CAPABILITIES, SVM_CAPABILITIES);
	}

	//��������
	PRINTINTGER(SingleFPConfig, SINGLE_FP_CONFIG);
	PRINTINTGER(DoubleFpConfig, DOUBLE_FP_CONFIG);

	//�������֧��
	PRINTINTGER(ErrorCorrectionSupport, ERROR_CORRECTION_SUPPORT);
	//������ʱ���ķֱ���
	PRINTINTGER(ProfiLingTimerResolution, PROFILING_TIMER_RESOLUTION);
	//ʱ�����Ƶ��
	PRINTINTGER(MaxClockFrequency, MAX_CLOCK_FREQUENCY);
	//С���ֽ���
	PRINTINTGER(EndianLittle, ENDIAN_LITTLE);
	//�Ƿ�����ͳһ�ڴ�
	PRINTINTGER(HostUnifiedMemory, HOST_UNIFIED_MEMORY);
	//��ӡ���������
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