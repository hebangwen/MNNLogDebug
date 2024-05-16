# MNN Log 算子调试

MNN Log 算子添加了最小值的限制，避免了输入为 0 时的 log 运算错误。但是在某些情况下，这种限制会导致输出为 0 的情况，这时候需要调整最小值限制。见[UnaryBufExection.cpp#L223](https://github.com/alibaba/MNN/blob/master/source/backend/opencl/execution/buffer/UnaryBufExecution.cpp#L223)。

log_kernel.cl 中是 log 算子的实现，其中有一个宏定义 MIN_VALUE，这个值就是最小值限制的值。我们可以通过修改这个值来调整最小值限制。

## 使用

不能交叉编译，需要在 Android 系统上编译运行，因为交叉编译的环境不支持 OpenCL 运行。需要使用 termux 等工具在 Android 系统上编译运行。可以直接在支持 OpenCL 的 Linux 系统上编译运行。

``` shell
# 编译 log_debug 程序
make 

# 运行 log_debug 程序，参数为最小值限制的值
# MNN Log 算子的最小值限制为 0.000001
./log_debug 1e-7

./log_debug 1e-8
```
