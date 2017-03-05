#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <OpenCL/opencl.h>
#include <pthread.h>
#include "information.h"
#include "readCLFile.h"
#include "swap.h"


//Length of vectors
unsigned int n = 4096;


cl_platform_id cpPlatform;        // OpenCL platform
cl_device_id device_id;           // device ID
cl_context context;               // context
cl_command_queue queue;           // command queue
cl_program program;               // program
cl_kernel kernel;                 // kernel


int main() {
    // Size, in bytes, of each vector
    size_t bytes = 2*n*sizeof(float);
    size_t elementSize = n*sizeof(kfloat);
    size_t globalWorkSize = n/2;    //Anzahl zuverwendenen Threads
    
    cl_mem bufA = 0;
    cl_mem bufB = 0;
    int k;
    int temp;
    int i;
    
    kfloat* A = (kfloat *)malloc(n*sizeof(kfloat));
    kfloat* swapped = (kfloat *)malloc(n*sizeof(kfloat));
    kfloat* result = (kfloat *)malloc(n*sizeof(kfloat));

    size_t globalSize, localSize;
    cl_int err;
    
    // Number of work items in each local work group
    localSize = 64;
    
    // Number of total work items - localSize must be devisor
    globalSize = ceil(n/(float)localSize)*localSize;
    
    // Bind to platform
    err = clGetPlatformIDs(1, &cpPlatform, NULL);
    
    // Get ID for the device
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    
    // Create a context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    
    // Create a command queue
    queue = clCreateCommandQueue(context, device_id, 0, &err);
    
    const char *clSource = ksrc("/Users/michael/Dropbox/Fachhochschule/Master/Semester2/PPP/fft/fft.cl");
    
    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char**)&clSource, NULL, &err);
    
    // Build the program executable
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    
    // Kernel bauen, mit der Funktion reduction
    kernel=clCreateKernel(program,"fft",&err);
    
    // Buffer anlegen, für die Übergabe an die Grafikkarte
    bufA = clCreateBuffer(context,CL_MEM_READ_ONLY,bytes,NULL,&err);
    bufB = clCreateBuffer(context,CL_MEM_WRITE_ONLY,bytes,NULL,&err);

    // Arrays füllen
    double werte= (double) 2*M_PI / n;
    for(i=0;i<n;i++)
    {
        A[i].real = cos(werte*i);
        A[i].imagin = 0;
        swapped[i].real=0;
        swapped[i].imagin = 0;
    }
    
    //Werte reihenfolge tauschen
    swap(A,swapped, n);
    
    // Write Buffer anlegen
    err = clEnqueueWriteBuffer(queue,bufA,CL_TRUE,0,elementSize,swapped,0,NULL,NULL);
    
    //Kernel Argumenete Setzten
    err = clSetKernelArg(kernel, 0, sizeof(cl_int), &n);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufA);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufB);

    err = clEnqueueNDRangeKernel(queue,kernel,1,NULL,&globalWorkSize,NULL,0,NULL,NULL);

    // Ergebniss Buffer wieder einlesen
    clEnqueueReadBuffer(queue,bufB,CL_TRUE,0,elementSize,result,0,NULL,NULL);
    
    //Ausgabe
    for(i=0; i< n; i++)
    {
        printf("%d:\t%f\n",i,result[i].real);
    }

    // Zeit zurück setzen
    clFinish(queue);

    free(A);
    free(swapped);
    free(result);
    free(clSource);
    
    return EXIT_SUCCESS;
}
