# pacPrism 构建指南

本指南详细说明如何在不同平台上构建pacPrism项目。

## 📋 系统要求

### 必需软件
- **CMake 3.14+**
- **C++23兼容编译器**:
  - GCC 13+ (Linux)
  - Clang 14+ (Linux/macOS)
  - MSVC 19.36+ (Windows)
- **Git** (用于版本信息)

### 依赖管理
- **vcpkg** (推荐)
- 或手动安装Boost.Beast 1.85.0+

## 🚀 快速开始

### 使用vcpkg (推荐)

1. **克隆仓库**
   ```bash
   git clone https://github.com/tzbkk/pacPrism.git
   cd pacPrism
   ```

2. **安装依赖**
   ```bash
   # Windows
   vcpkg install boost-beast
   vcpkg integrate install

   # Linux/macOS
   ./vcpkg install boost-beast
   ./vcpkg integrate install
   ```

3. **配置项目**
   ```bash
   # 自动处理依赖并生成构建文件
   cmake -B build
   ```

4. **构建项目**
   ```bash
   # 构建所有目标
   cmake --build build

   # 或使用多线程
   cmake --build build --parallel $(nproc)
   ```

5. **运行程序**
   ```bash
   # Windows
   ./build/bin/pacprism.exe

   # Linux/macOS
   ./build/bin/pacprism
   ```

### 验证构建

```bash
# 测试HTTP服务器
curl http://localhost:8080

# 预期输出
Hello from pacPrism!
```

## 🔧 详细构建选项

### 构建配置

```bash
# Debug构建
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Release构建
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 指定编译器
cmake -B build -DCMAKE_CXX_COMPILER=g++
cmake -B build -DCMAKE_CXX_COMPILER=clang++
```

### 构建特定目标

```bash
# 仅构建主可执行文件
cmake --build build --target pacprism

# 仅构建DHT库
cmake --build build --target node_dht

# 仅构建网络传输库
cmake --build build --target network_transmission

# 清理构建
cmake --build build --target clean
```

## 🏗️ 构建系统架构

### 模块化CMake设计

pacPrism使用模块化CMake架构：

```
CMakeLists.txt (根目录)
├── include(VersionConfig)     # 版本管理系统
├── include(BeastConfig)        # Boost.Beast配置
├── include(LibraryConfig)      # 库目标配置
└── include(BuildConfig)        # 构建系统配置
```

### 自动依赖管理

- **vcpkg集成**: 自动检测和安装Boost.Beast
- **版本控制**: 从CMake项目版本自动生成version.h
- **Git集成**: 自动提取提交信息和构建时间戳
- **跨平台**: Windows/Linux/macOS统一构建流程

### 目标结构

```
pacprism
├── libnode_dht.dll      # DHT操作库
├── libnetwork_transmission.dll  # HTTP网络库
└── pacprism.exe        # 主可执行文件
```

## 🐛 常见问题

### 依赖问题

**问题**: 找不到Boost.Beast
```bash
# 解决方案: 显式指定vcpkg工具链
cmake -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
```

**问题**: C++23编译错误
```bash
# 解决方案: 确保使用最新编译器
# GCC 13+
sudo apt update && sudo apt install gcc-13 g++-13

# 或下载最新MSVC (Windows)
```

### 构建问题

**问题**: 链接错误
```bash
# 解决方案: 清理并重新构建
rm -rf build
cmake -B build
cmake --build build
```

**问题**: 运行时错误
```bash
# 解决方案: 检查DLL路径
# Windows: 确保build/bin在PATH中
# Linux: 设置LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./build/bin:$LD_LIBRARY_PATH
```

## 🎯 性能优化

### Release构建优化

```bash
# 启用所有优化
cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native"

# 使用Ninja构建系统 (更快)
cmake -B build -G Ninja
ninja -C build
```

### 调试构建

```bash
# 包含调试信息
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-g -O0 -fsanitize=address"

# 运行调试版本
./build/bin/pacprism.exe
```

## 📊 构建输出

### 成功构建示例

```
-- Building for: Ninja
-- The CXX compiler identification is GNU 15.2.0
-- pacPrism Version Configuration:
--   Version: 0.1.0
--   Build Type: Release
--   Build Date: 2025-11-30
--   Git Version: c2f0ec0-dirty
--   Git Commit: c2f0ec0
--   Version Header: D:/Documents/projects/pacPrism/build/include/pacPrism/version.h
-- Found Boost.Beast from vcpkg: Boost::beast
-- Configuring done (1.2s)
-- Generating done (0.0s)
-- Build files have been written to: D:/Documents/projects/pacPrism/build

[6/6] Linking CXX executable bin\pacprism.exe
```

### 版本信息输出

```
pacPrism - Semi-decentralized Package Distribution System
Version 0.1.0
Build: 0.1.0 (Release, 2025-11-30)
Git: c2f0ec0-dirty [c2f0ec0]
Starting HTTP server...
Server started, listening on port 8080
```

## 🔄 开发工作流

### 日常开发

```bash
# 1. 修改代码
vim src/main.cpp

# 2. 增量构建
cmake --build build

# 3. 运行测试
./build/bin/pacprism.exe

# 4. 测试HTTP功能
curl http://localhost:8080
```

### 版本管理

```bash
# 1. 更新版本号 (CMakeLists.txt)
project(pacPrism VERSION 0.2.0 LANGUAGES CXX)

# 2. 重新配置
cmake -B build

# 3. 构建
cmake --build build

# 4. 验证版本
./build/bin/pacprism.exe
```

### Git工作流

```bash
# 1. 功能分支开发
git checkout -b feature/http-handling
git add .
git commit -m "feat: implement HTTP request handling"

# 2. 版本标记
git tag v0.2.0
git push origin v0.2.0

# 3. 版本信息自动包含在构建中
cmake --build build
./build/bin/pacprism.exe
# 输出显示新版本和Git信息
```

---

*📝 更新日期: 2025-11-30*
*🔧 构建系统: CMake 3.14+ with vcpkg*
*🎯 支持平台: Windows, Linux, macOS*