// 对比 CPU 和 OpenCL 实现的 log10 函数

#include <iostream>
#include <cmath>
#include <CL/opencl.hpp>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char **argv) {
    double min_value = 1e-7;
    if (argc > 1) {
        min_value = std::stof(argv[1]);
    }

    std::cout << "min_value: " << min_value << std::endl;

    int n = 1024;
    std::vector<float> data(n);

    // 从文件读取输入
    std::ifstream file("./input.txt");
    if (file.is_open()) {
        int j = 0;
        while (!file.eof()) {
            file >> data[j++];
        }
    }

    // CPU 实现
    auto start = std::chrono::steady_clock::now();
    std::vector<float> result(n);
    std::vector<float> result_gpu(n);
    for (int i = 0; i < n; i++) {
        result[i] = std::log(data[i]);
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "CPU time: " << elapsed.count() << " s" << std::endl;

    FILE *fp;
    char fileName[] = "./log_kernel.cl";
    char *source_str;
    size_t source_size;

    fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);


    // 使用 opencl cpp 接口实现
    cl::Platform platform = cl::Platform::getDefault();
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];
    cl::Context context(device);
    cl::CommandQueue queue(context, device);
    cl::Program::Sources sources;
    sources.push_back({source_str, source_size});

    // 设置编译选项
    std::stringstream ss;
    ss << min_value;
    std::string min_value_str = ss.str();
    std::string build_options = "-DMIN_VALUE=" + min_value_str;
    std::cout << "build_options: " << build_options << std::endl;

    cl::Program program(context, sources);
    if (program.build({device}, build_options.c_str()) != CL_SUCCESS) {
        std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        exit(1);
    }

    cl::Buffer buffer_data(context, CL_MEM_READ_ONLY, n * sizeof(float));
    cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, n * sizeof(float));

    queue.enqueueWriteBuffer(buffer_data, CL_TRUE, 0, n * sizeof(float), data.data());

    cl::Kernel kernel(program, "log_float");
    kernel.setArg(0, buffer_data);
    kernel.setArg(1, buffer_result);

    start = std::chrono::steady_clock::now();
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(n), cl::NullRange);
    queue.finish();
    queue.enqueueReadBuffer(buffer_result, CL_TRUE, 0, n * sizeof(float), result_gpu.data());
    end = std::chrono::steady_clock::now();

    elapsed = end - start;
    std::cout << "GPU time: " << elapsed.count() << " s" << std::endl;

    // 对比结果
    for (int i = 0; i < n; i++) {
        auto delta = std::abs(result[i] - result_gpu[i]);
        std::cout << i << " " << result[i] << " " << result_gpu[i] << " " << delta << std::endl;
    }

    return 0;
}
