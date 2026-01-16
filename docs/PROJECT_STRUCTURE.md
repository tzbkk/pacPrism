# pacPrism 项目架构

> **Status**: Early prototype. Most components are incomplete or design-only.

## 项目目录结构

```
pacPrism/
├── CMakeLists.txt              # 根 CMake 配置文件
├── CMakePresets.json           # CMake Presets 配置 (推荐构建方式)
├── vcpkg.json                  # vcpkg 依赖管理文件
├── README.md                   # 英文项目说明文档
├── README_zh.md               # 中文项目说明文档
├── CLAUDE.md                   # Claude Code 项目指导文档
│
├── scripts/                    # 构建和配置脚本 (传统方式)
│   ├── build.ps1              # Windows PowerShell 构建脚本
│   └── build.sh               # Linux/macOS Bash 构建脚本
│
├── cmake/                      # CMake 配置模块
│   ├── VersionConfig.cmake     # 版本管理配置
│   ├── LibraryConfig.cmake     # 库目标配置
│   ├── BuildConfig.cmake       # 构建系统配置
│   └── version.hpp.in          # 版本头文件模板
│
├── cxxopts/                    # 命令行参数解析库 (header-only)
│   ├── include/cxxopts.hpp     # 单头文件库
│   ├── CMakeLists.txt          # cxxopts 自身的构建文件
│   └── README.md               # cxxopts 说明文档
│
├── config/                     # 配置文件
│   └── pacprism.conf           # 主配置文件
│
├── src/                        # 主应用程序源代码
│   ├── CMakeLists.txt          # 可执行文件配置
│   ├── main.cpp                # 应用程序入口点
│   ├── console/                # 控制台层实现
│   │   ├── banner/             # 启动横幅显示
│   │   │   ├── banner.cpp
│   │   │   └── banner.hpp
│   │   ├── parser/             # 命令行参数解析
│   │   │   ├── parser.cpp
│   │   │   └── parser.hpp
│   │   └── io/                 # 控制台 I/O
│   │       ├── io.cpp
│   │       └── io.hpp
│   ├── network/                # 网络层实现
│   │   ├── transmission/       # HTTP 传输层
│   │   │   ├── transmission.cpp
│   │   │   └── transmission.hpp
│   │   └── router/             # HTTP 请求路由层
│   │       ├── router.cpp
│   │       └── router.hpp
│   └── node/                   # 节点功能
│       ├── dht/                # 分布式哈希表
│       │   ├── dht_operation.cpp
│       │   ├── dht_operation.hpp
│       │   └── dht_types.hpp
│       └── validator/          # 请求验证器
│           ├── validator.cpp
│           └── validator.hpp
│
├── include/                    # 头文件
│   ├── console/                # 控制台相关头文件
│   │   ├── banner/banner.hpp
│   │   ├── parser/parser.hpp
│   │   └── io/io.hpp
│   ├── network/                # 网络相关头文件
│   │   ├── transmission/transmission.hpp
│   │   └── router/router.hpp
│   └── node/                   # 节点相关头文件
│       ├── dht/dht_operation.hpp
│       ├── dht/dht_types.hpp
│       ├── validator/validator.hpp
│       └── sharding/sharding_types.h
│
├── docs/                       # 项目文档
│   ├── PROJECT_STRUCTURE.md    # 项目架构说明 (本文件)
│   ├── CURRENT_STATUS.md       # 当前开发状态
│   ├── ROADMAP.md              # 发展路线图
│   ├── dht_operation.md        # DHT 操作文档
│   └── VERSION_SYSTEM.md       # 版本管理系统文档
│
├── devlog_zh/                  # 中文开发日志
│   ├── README_DEVLOG.md        # 开发日志索引
│   └── devlog_*.md            # 每日开发记录
│
├── build/                      # 构建输出目录 (自动生成)
│   ├── bin/                    # 可执行文件和库
│   │   ├── pacprism.exe        # 主可执行文件 (Windows)
│   │   ├── libconsole_banner.dll
│   │   ├── libconsole_io.dll
│   │   ├── libconsole_parser.dll
│   │   ├── libnetwork_router.dll
│   │   ├── libnetwork_transmission.dll
│   │   ├── libnode_dht.dll
│   │   └── libnode_validator.dll
│   ├── config/                 # 配置文件输出
│   │   └── pacprism.conf
│   └── include/                # 生成的头文件
│       └── pacPrism/
│           └── version.h       # 自动生成的版本信息
```

## 核心组件说明

### 1. 应用程序层 (`src/`)

**main.cpp**: 应用程序入口点
- 负责初始化各个子系统
- 启动 HTTP 服务器
- 版本信息显示

**状态**: ✅ Working

### 1.1 控制台层 (`src/console/`)

**banner.cpp**: 启动横幅显示
- 彩色版本信息输出
- 编译时间和 Git 信息显示
- 跨平台 ANSI 颜色支持

**状态**: ✅ Working

**parser.cpp**: 命令行参数解析
- 使用 cxxopts 库进行参数解析
- 支持配置文件路径 (`--config`)
- 支持端口配置 (`--port`)
- 帮助信息显示 (`--help`)

**状态**: ✅ Working

**io.cpp**: 控制台 I/O 操作
- 控制台输入输出管理
- 日志级别控制
- 错误信息格式化

**状态**: ✅ Working

**cxxopts 依赖**:
- Header-only C++ library
- 包含在项目的 `cxxopts/` 目录
- 需要在 CMakeLists.txt 中添加 include 路径：
  ```cmake
  target_include_directories(console_parser PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../cxxopts/include
  )
  ```

### 2. 网络层 (`src/network/`)

**transmission.cpp**: HTTP 服务器实现
- `ServerTrans` 类：HTTP 服务器核心
- 异步 I/O 处理，支持并发连接
- HTTP/1.1 协议支持
- 请求解析和响应生成

**状态**: ✅ Working (basic responses), ❌ APT compatibility not implemented

**router/**: HTTP 请求路由
- Router 类架构存在
- DHT 集成框架
- 大部分函数是空壳

**状态**: 🔄 Partial (architecture exists, no logic)

### 3. 分布式哈希表 (`lib/node/dht/`)

**dht_operation.cpp**: DHT 核心操作
- `store_entry()`: 存储节点条目，支持 TTL 管理
- `query_node_ids_by_shard_id()`: 按分片查询节点
- `verify_entry()`: 节点存在性验证
- `clean_by_expiry_time()`: 清理过期条目

**dht_types.hpp**: 核心数据结构
- `dht_entry`: 节点条目结构 (node_id, IP, 分片信息, 时间戳)
- `shard`: 分片信息结构 (ID、包列表)

**状态**: ✅ Working (in-memory only), ❌ Not distributed, ❌ No persistence

### 4. 构建系统 (`CMakeLists.txt`, `cmake/`)

**模块化 CMake 配置**:
- `VersionConfig.cmake`: 版本管理和 Git 集成
- `LibraryConfig.cmake`: 库目标配置和依赖管理
- `BuildConfig.cmake`: 构建选项和编译器配置

**vcpkg 集成**:
- `vcpkg.json`: 项目依赖配置 (仅依赖 boost-beast)
- 需要设置 VCPKG_ROOT 环境变量指向本地 vcpkg 安装
- 自动依赖下载和安装
- 跨平台兼容性

### 5. 构建脚本 (`scripts/`)

**build.ps1** / **build.sh**: 传统构建脚本 (Legacy)
- Visual Studio Build Tools 检测
- vcpkg 状态检查和初始化
- 自动化构建流程
- **注意**: 推荐使用 CMake Presets 代替

### 6. CMake Presets (`CMakePresets.json`)

**CMakePresets.json**: 推荐的构建配置方式
- `debug` preset: Debug 构建配置
- `release` preset: Release 构建配置
- 自动 vcpkg toolchain 集成
- 生成 compile_commands.json 用于 IDE 支持

**使用方法**:
```bash
# 查看可用 presets
cmake --list-presets

# 配置并构建 (Debug)
cmake --preset debug
cmake --build --preset debug

# 配置并构建 (Release)
cmake --preset release
cmake --build --preset release
```

## 依赖关系

```
src/main.cpp
    ↓
lib/network/transmission (network_transmission.dll)
    ↓
lib/node/dht (node_dht.dll)
    ↓
boost-beast (第三方依赖，通过 vcpkg 安装)
```

## 构建流程

### 推荐方式 (CMake Presets)
1. **安装 vcpkg**:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git C:/vcpkg  # 或 ~/vcpkg
   cd C:/vcpkg
   .\bootstrap-vcpkg.bat  # Windows: bootstrap-vcpkg.sh on Linux/macOS
   export VCPKG_ROOT=C:/vcpkg  # 设置环境变量
   ```
2. **克隆仓库**: `git clone https://github.com/tzbkk/pacPrism.git`
3. **配置预设**: `cmake --preset debug` 或 `cmake --preset release`
4. **构建项目**: `cmake --build --preset debug`
5. **版本注入**: Git 信息和构建时间戳自动注入

### 传统方式 (构建脚本)
1. **环境检测**: 检查 Visual Studio Build Tools (Windows) 或 C++ 编译器
2. **vcpkg 初始化**: 自动下载和构建 boost-beast 依赖
3. **CMake 配置**: 生成构建文件，配置 toolchain
4. **编译构建**: 构建所有库和可执行文件
5. **版本注入**: Git 信息和构建时间戳自动注入

## 技术栈

- **编程语言**: C++23
- **网络库**: Boost.Beast 1.89.0 (包含 Boost.Asio)
- **构建系统**: CMake 3.19+ (Presets) + vcpkg
- **包管理**: vcpkg (本地安装 + VCPKG_ROOT 环境变量 + manifest 模式)
- **版本控制**: Git (集成版本信息)

---

*最后更新: 2026-01-17*