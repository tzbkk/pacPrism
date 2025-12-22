# pacPrism 项目架构

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
├── src/                        # 主应用程序源代码
│   ├── CMakeLists.txt          # 可执行文件配置
│   └── main.cpp                # 应用程序入口点
│
├── lib/                        # 库组件
│   ├── CMakeLists.txt          # 库配置
│   ├── network/                # 网络层实现
│   │   ├── CMakeLists.txt      # 网络库配置
│   │   └── transmission/       # HTTP 传输层
│   │       ├── CMakeLists.txt  # 传输库配置
│   │       ├── transmission.cpp # HTTP 服务器实现
│   │       └── transmission.h  # HTTP 传输接口
│   └── node/                   # 节点功能
│       └── dht/                # 分布式哈希表
│           ├── CMakeLists.txt  # DHT 库配置
│           ├── dht_operation.cpp # DHT 核心操作
│           ├── dht_operation.h  # DHT 操作接口
│           └── dht_types.h      # DHT 数据结构
│
├── include/                    # 头文件
│   ├── network/                # 网络相关头文件
│   │   └── transmission/       # HTTP 传输头文件
│   │       └── transmission.h  # HTTP 传输接口声明
│   └── node/                   # 节点相关头文件
│       ├── dht/                # DHT 头文件
│       │   ├── dht_operation.h  # DHT 操作接口声明
│       │   └── dht_types.h      # DHT 数据结构定义
│       └── sharding/           # 分片相关
│           └── sharding_types.h # 分片数据结构
│
├── docs/                       # 项目文档
│   ├── PROJECT_STRUCTURE.md    # 项目架构说明 (本文件)
│   ├── CURRENT_STATUS.md       # 当前开发状态
│   ├── dht_operation.md        # DHT 操作文档
│   └── VERSION_SYSTEM.md       # 版本管理系统文档
│
├── devlog_zh/                  # 中文开发日志
│   ├── README_DEVLOG.md        # 开发日志索引
│   └── devlog_*.md            # 每日开发记录
│
├── build/                      # 构建输出目录 (自动生成)
│   ├── bin/                    # 可执行文件和库
│   │   ├── pacprism            # 主可执行文件
│   │   ├── libnetwork_transmission.dll # 网络库
│   │   └── libnode_dht.dll     # DHT 库
│   └── include/                # 生成的头文件
│       └── pacPrism/
│           └── version.h       # 自动生成的版本信息
│
└── vcpkg/                      # vcpkg 包管理器 (Git submodule)
    ├── vcpkg.json             # vcpkg 依赖配置
    └── ...                    # vcpkg 相关文件
```

## 核心组件说明

### 1. 应用程序层 (`src/`)

**main.cpp**: 应用程序入口点
- 负责初始化各个子系统
- 启动 HTTP 服务器
- 版本信息显示

### 2. 网络层 (`lib/network/`)

**transmission.cpp**: HTTP 服务器实现
- `ServerTrans` 类：HTTP 服务器核心
- 异步 I/O 处理，支持并发连接
- HTTP/1.1 协议支持，兼容 APT 客户端
- 请求解析和响应生成

### 3. 分布式哈希表 (`lib/node/dht/`)

**dht_operation.cpp**: DHT 核心操作
- `store_entry()`: 存储节点条目，支持 TTL 管理
- `query_entry()`: 按节点 IP 或分片查询条目
- `remove_entry()`: 移除指定节点的条目
- `clean_by_ttl()`: 清理过期的条目

**dht_types.h**: 核心数据结构
- `dht_entry`: 节点条目结构 (IP、分片信息、时间戳、TTL)
- `sharding`: 分片信息结构 (ID、包列表)

### 4. 构建系统 (`CMakeLists.txt`, `cmake/`)

**模块化 CMake 配置**:
- `VersionConfig.cmake`: 版本管理和 Git 集成
- `LibraryConfig.cmake`: 库目标配置和依赖管理
- `BuildConfig.cmake`: 构建选项和编译器配置

**vcpkg 集成**:
- `vcpkg.json`: 项目依赖配置 (仅依赖 boost-beast)
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
1. **克隆仓库**: `git clone --recurse-submodules` (vcpkg 是 submodule)
2. **配置预设**: `cmake --preset debug` 或 `cmake --preset release`
3. **构建项目**: `cmake --build --preset debug`
4. **版本注入**: Git 信息和构建时间戳自动注入

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
- **包管理**: vcpkg (Git submodule + manifest 模式)
- **版本控制**: Git (集成版本信息)

---

*最后更新: 2025-12-22*