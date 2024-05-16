
// natral log in opencl

__kernel void log_float(
    __global float *input,
    __global float *output)
{
    int i = get_global_id(0);
    // output[i] = native_log(input[i]);
    output[i] = native_log(input[i] > MIN_VALUE ? input[i] : MIN_VALUE);
}
