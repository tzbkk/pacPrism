# pacPrism

![pacPrism Banner](assets/prism-banner.png)

> **系统软件包的分布式缓存层** - 高性能包感知透明代理

[English Version](README.md)

## 愿景

pacPrism 致力于成为**系统软件包的分布式缓存层**，通过透明代理模式提升现有包管理器的性能。

### 目标架构（设计愿景）

1. **透明代理** - 修改 `sources.list` 即可拦截 APT/Pacman 请求
2. **语义分片** - 将相关包分组，保证依赖完整性和空间局部性
3. **混合分发** - 中心网关 + 去中心化 P2P 数据层
4. **智能路由** - 查询 DHT 寻找最优节点获取内容

### 当前现实

**这是早期原型代码。** 以上特性大部分是设计目标，非已实现功能。

## 实现状态

### ✅ 能工作（已测试验证）
- **HTTP/1.1 服务器** - 基于 Boost.Beast，监听 9001 端口，能返回基本响应
- **DHT 内存索引** - 内存哈希表存储，基础 CRUD 操作可用
- **构建系统** - CMake Presets，跨平台（Windows/Linux），vcpkg 集成

### 🔄 部分实现（有代码但未完成）
- **HTTP 请求路由器** - 架构存在，大部分函数是空壳
- **自定义 HTTP 头** - 能检测 "Operation" 头，没有实际路由逻辑
- **响应构建器** - 基本字符串响应，variant 处理框架

### 📋 设计阶段（未写代码）
- **P2P 协议** - 0% 实现
- **真正的分布式 DHT** - 当前 DHT 只是单进程内存结构
- **语义分片** - 定义了数据结构，没有算法
- **包感知路由** - 无法处理真实 APT 请求
- **缓存回退** - 没有官方源集成

## 技术栈

- **核心系统**: **C++23** - 高性能、低延迟网络处理
- **网络库**: **Boost.Beast 1.89.0** - HTTP/1.1异步服务器
- **构建系统**: **CMake 3.14+** - 跨平台模块化构建
- **依赖管理**: **vcpkg** - 自动化依赖安装

## 快速开始

### 环境要求
- **C++23 兼容编译器** (GCC 13+, Clang 14+, MSVC 19.36+)
- **CMake 3.19+** (支持 Presets 功能)
- **Visual Studio Build Tools** (仅Windows, MSVC 19.36+)

### 构建与运行（推荐）

**使用 CMake Presets**（跨平台）：
```bash
# 克隆仓库（包含 vcpkg 子模块）
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism

# 配置并构建（Debug）
cmake --preset debug
cmake --build --preset debug

# 或配置并构建（Release）
cmake --preset release
cmake --build --preset release

# 运行应用程序
./build/bin/pacprism
```

### 构建与运行（传统方式）

**Windows (PowerShell):**
```powershell
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism
.\scripts\build.ps1
.\build\bin\pacprism.exe
```

**Linux/macOS (Bash):**
```bash
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism
chmod +x scripts/build.sh
./scripts/build.sh
./build/bin/pacprism
```

## 文档

- [📋 当前开发状态](docs/CURRENT_STATUS.md) - 详细实现进度和功能验证
- [🏗️ 项目架构](docs/PROJECT_STRUCTURE.md) - 完整架构设计说明
- [📝 开发日志](devlog_zh/README_DEVLOG.md) - 每日开发记录

## 许可证

GPLv3