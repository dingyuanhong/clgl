__kernel void hello_kernel(__global const float *a,
                           __global const float *b,
                           __global float *result)
{
	int gid = get_global_id(0);
	if(gid%2 == 0){
		gid = get_global_id(1);
	}
	//result[gid] = a[gid] + b[gid];
	result[gid] = gid;
	//result[gid] = n + 1;
	//result[gid] = get_local_id(n) + 1;
	//result[gid] = get_group_id(n) + 1;
}