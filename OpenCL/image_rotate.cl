__kernel void image_rotate(__global float *dest_data, __global float *src_data, int W, int H, float sinTheta,float cosTheta)
{
	const int ix = get_global_id(0);
	const int iy = get_global_id(1);

	float xpos = ((float)ix * cosTheta + ((float)iy)*sinTheta);
	float ypos = -1.0*((float)ix) * sinTheta + ((float)iy)*cosTheta;

	if(((int)xpos>=0) && ((int)xpos < W) && ((int)ypos>=0) && ((int)ypos<H))
	{
       dest_data[(int)ypos*W+(int)xpos] = src_data[iy*W+ix];
	}
}