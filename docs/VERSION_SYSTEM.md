# pacPrism 版本系统

pacPrism 使用了一个模块化的CMake控制的版本管理系统，自动生成版本信息并在构建时注入。

## 🏗️ 系统架构

### CMake 配置文件

1. **VersionConfig.cmake** - 主要版本配置模块
   - 从 `project()` 命令解析版本号
   - 提取Git信息（如果可用）
   - 生成编译时定义
   - 创建 version.h 头文件

2. **cmake/version.h.in** - 版本头模板文件
   - 定义版本相关的常量和函数
   - 使用CMake变量替换
   - 提供 C++ API 接口

3. **vcpkg.json** - 包依赖管理
   - 定义项目版本
   - 管理外部依赖

## 📦 版本信息组成

### 自动提取的信息

- **项目版本**: 从 `project(pacPrism VERSION 0.1.0)` 提取
- **构建类型**: 从 `${CMAKE_BUILD_TYPE}` 获取
- **构建日期**: 自动生成当前日期
- **Git信息**: 如果是Git仓库，自动提取
  - Git标签和提交描述
  - 短提交哈希

### 生成的常量

```cpp
namespace pacprism {
    // 版本号
    constexpr int VERSION_MAJOR = 0;
    constexpr int VERSION_MINOR = 1;
    constexpr int VERSION_PATCH = 0;

    // 版本字符串
    constexpr const char* VERSION_FULL = "0.1.0";
    constexpr const char* VERSION_SHORT = "0.1";

    // 构建信息
    constexpr const char* BUILD_TYPE = "Release";
    constexpr const char* BUILD_DATE = "2025-11-30";

    // Git信息
    constexpr const char* GIT_VERSION = "v0.1.0-1-gabc1234";
    constexpr const char* GIT_COMMIT = "abc1234";
}
```

## 🔧 使用方法

### 1. 修改项目版本

在根目录 `CMakeLists.txt` 中：

```cmake
project(pacPrism VERSION 1.2.3 LANGUAGES CXX)
#                           ^^^^^^
#                           新版本号
```

### 2. 在代码中使用版本信息

```cpp
#include "pacPrism/version.h"

void print_version() {
    std::cout << "pacPrism v" << pacprism::getVersionFull() << std::endl;
    std::cout << "Build: " << pacprism::getBuildInfo() << std::endl;
    std::cout << "Git: " << pacprism::getGitInfo() << std::endl;

    // 或者直接使用常量
    std::cout << "Major: " << pacprism::VERSION_MAJOR << std::endl;
}
```

### 3. 编译时定义

所有版本信息都作为编译时定义注入到目标中：

```cpp
#ifdef PACPRISM_VERSION_FULL
    std::cout << "Version: " << PACPRISM_VERSION_FULL << std::endl;
#endif

#ifdef PACPRISM_BUILD_TYPE
    std::cout << "Build: " << PACPRISM_BUILD_TYPE << std::endl;
#endif
```

## 📁 文件结构

```
pacPrism/
├── CMakeLists.txt              # 主CMake配置（包含版本）
├── vcpkg.json                 # 包版本信息
├── scripts/
│   └── VersionConfig.cmake     # 版本配置模块
├── cmake/
│   └── version.h.in           # 版本头模板
├── build/include/pacPrism/
│   └── version.h              # 生成的版本头
└── src/main.cpp               # 使用版本信息
```

## 🔄 构建流程

1. **CMake配置**:
   ```bash
   cmake -B build
   # 生成: build/include/pacPrism/version.h
   ```

2. **编译构建**:
   ```bash
   cmake --build build
   # 所有目标自动包含版本定义
   ```

3. **版本信息输出**:
   ```bash
   ./build/bin/pacprism.exe
   # pacPrism - Semi-decentralized Package Distribution System
   # Version 0.1.0
   # Build: 0.1.0 (Release, 2025-11-30)
   # Git: v0.1.0-dirty [c2f0ec0]
   ```

## ✨ 特性

- **自动版本管理**: 从CMake项目版本自动提取
- **Git集成**: 自动检测Git仓库状态和提交信息
- **构建信息**: 包含构建类型和日期
- **模块化设计**: 易于扩展和维护
- **跨平台兼容**: 支持Windows和Linux
- **零依赖**: 仅使用CMake标准功能

## 🚀 扩展版本系统

如需添加更多信息，可以：

1. **在 VersionConfig.cmake 中添加新变量**
2. **在 version.h.in 中添加对应常量**
3. **更新 get_version_info() 函数**

### 示例：添加编译器信息

```cmake
# VersionConfig.cmake
if(CMAKE_CXX_COMPILER_ID)
    set(PACPRISM_COMPILER_ID "${CMAKE_CXX_COMPILER_ID}")
    set(PACPRISM_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
endif()
```

```cpp
// version.h.in
constexpr const char* COMPILER_ID = "@PACPRISM_COMPILER_ID@";
constexpr const char* COMPILER_VERSION = "@PACPRISM_COMPILER_VERSION@";
```

## 📊 输出示例

```
pacPrism - Semi-decentralized Package Distribution System
Version 0.1.0
Build: 0.1.0 (Release, 2025-11-30)
Git: c2f0ec0-dirty [c2f0ec0]
Starting HTTP server...
Server started, listening on port 8080
```

这个版本系统为pacPrism提供了专业级的版本管理功能，支持自动构建信息生成和Git集成。