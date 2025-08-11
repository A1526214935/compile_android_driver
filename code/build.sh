#!/bin/bash


# 设置环境变量
export PATH=/root/Kernel/android13-5.15/prebuilts/clang/host/linux-x86/clang-r450784e/bin:$PATH

# 编译内核模块
make CROSS_COMPILE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1  ARCH=arm64 -C /root/Kernel/android12-5.10/out/android12-5.10/common M=$PWD -j8

# 将编译好的模块重命名
mv LGHUB.ko 510.ko

# 可选：再次编译内核模块
make CROSS_COMPILE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1  ARCH=arm64 -C /root/Kernel/android13-5.15/out/android13-5.15/common M=$PWD -j8

# 可选：再次重命名编译好的模块
mv LGHUB.ko 515.ko
#make CROSS_COMPILE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1  ARCH=arm64 -C /root/Kernel/android14-6.1/bazel-android14-6.1/common M=$PWD -j8

# 删除指定文件和目录
find . -type d -name '.thinlto-cache' -exec rm -r {} \; -prune
find . -type f -name '.*' -exec rm {} \;
find . -type f -name '*.mod*' -exec rm {} \;
find . -type f -name '*.lto*' -exec rm {} \;
find . -type f -name '*.o' -exec rm {} \;
find . -type f -name '*.symvers' -exec rm {} \;
find . -type f -name '*.order' -exec rm {} \;
