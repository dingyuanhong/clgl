
// TODO: Add OpenCL kernel code here.

#define PI 3.1415926

__kernel void init_vbo_kernel(__global float4 *vbo,int w,int h,int seq)
{
	int gid = get_global_id(0);
	float4 linepts;
	float f = 1.0f;
	float a  = (float)h/4.0f;
	float b = w/2.0f;
	linepts.x = gid;
	linepts.y = b + a*sin( PI*2.0*( (float)gid / (float)w*f + (float)seq/(float)w ) ) ;
	linepts.z = gid + 1.0f;
	linepts.w = b + a*sin(PI*2.0*( (float)(gid + 1.0f)/(float)w*f + (float)seq/(float)w ) );

	vbo[gid] = linepts;
}