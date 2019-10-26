//g++ -std=c++0x -o rotate rotate.c -lOpenCL
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include<math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main(void) {
    int i,j;
    const float W = 25, H = 25;

    float **ip = (float**)malloc(H*sizeof(float));
    for(i = 0; i < H; i++) {
        ip[i] = (float *)malloc(W*sizeof(float));
        for(j = 0; j < W; j++){
            ip[i][j] = j;
        }
    }

    float **op = (float**)malloc(H*sizeof(float));
    for(i = 0; i < H; i++) {
        op[i] = (float *)malloc(W*sizeof(float));
        for(j = 0; j < W; j++){
            op[i][j] = j;
        }
    }
  
    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("image_rotate.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1, 
            &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector 
    //We assume that A,B,C are float arrays which 
    //have been declared and initialized 
    cl_mem d_ip = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            W*H * sizeof(float), NULL, &ret);

    cl_mem d_op = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
             W*H * sizeof(float), NULL, &ret);

    // Copy the lists A and B to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, d_ip, CL_TRUE, 0,
             W*H * sizeof(float), ip, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "image_rotate", &ret);

    float theta = 32 * 3.14159265359 / 180;
    //The angle of rotation is theta 
    float cos_theta=(cos(45));
    float sin_theta=(sin(45));

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&d_op);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&d_ip);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&W);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&H);
    ret = clSetKernelArg(kernel, 4, sizeof(cl_float), (void *)&cos_theta);
    ret = clSetKernelArg(kernel, 5, sizeof(cl_float), (void *)&sin_theta);
    
    // Set local and global workgroup sizes
    size_t globalws[2] = {W,H};
    size_t localws[2] = {16,16};

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, 
            globalws, localws, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    //int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
    ret = clEnqueueReadBuffer(command_queue, d_op, CL_TRUE, 0, 
            W * H * sizeof(float), op, 0, NULL, NULL);

    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            for(int k = 0; k < W; k++){
                printf("%.2f  %.2f\n", ip[i][k],op[k][j]);

            }
        }
    }

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(d_ip);
    ret = clReleaseMemObject(d_op);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(ip);
    free(op);
    return 0;
        
}