# pacPrism

![pacPrism Banner](assets/prism-banner.png)

> **系统软件包的分布式缓存层** - 高性能包感知透明代理

**当前版本: v0.1.0** | [发布说明](CHANGELOG.md)

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

### ✅ 生产就绪功能（已完全实现并测试）

**HTTP 透明代理系统**:
- 真正的透明 HTTP 代理（非重定向）- 从上游获取文件并直接返回
- FileCache 上游获取、本地磁盘缓存、SHA256 校验
- Range 请求支持（HTTP 206 Partial Content）- APT 断点续传关键功能
- 条件请求支持（If-Modified-Since, If-None-Match → HTTP 304）
- 错误处理与自动重试（3 次，指数退避：1s、2s、4s）
- 可配置超时（连接: 10s，读取: 30s）

**DHT 核心操作**:
- 9 维索引系统完整实现
- 节点验证、智能存储（冲突解决）、基于分片查询
- 自动过期清理
- O(1) 核心操作，多维度索引

**HTTP 路由器（三重依赖注入）**:
- Validator 集成，请求类型分类（PlainClient vs Node vs Invalid）
- 完整的 DHT HTTP API（5 个 JSON 端点：verify、store、query、clean/expiry、clean/liveness）
- 文件代理支持 Range/条件请求（通过 FileCache）
- 生产就绪，非占位符

**Validator 与 SHA256**:
- 基于自定义头的请求类型验证
- 跨平台 SHA256 计算（Windows: bcrypt，Linux: OpenSSL）
- 文件完整性 SHA256 验证

**Debian 包解析器**:
- 二进制包解析：`name_version_arch.extension`
- 源码包解析：`.orig.tar.gz/xz`、`.dsc`、`.tar.gz/xz`
- 组件提取（main/contrib/non-free）

**构建系统**:
- CMake Presets（debug/release）跨平台支持
- 自动化 vcpkg 依赖管理
- 已在 Windows MSYS2（GCC 15.2.0）测试通过

### 📝 待办/存根实现（仅 3 项）
- `ClientTrans::start_connecting()` - 空存根（客户端连接逻辑）
- `DHT_operation::clean_by_liveness()` - 空存根（基于健康度的节点清理）
- `Validator::verify_node_identity()` - 演示模式，始终返回 true（Ed25519 待实现）

### 📋 设计阶段（尚未实现）
- **P2P 协议** - 节点间通信协议
- **真正的分布式 DHT** - 节点间网络通信
- **语义分片算法** - 智能包分组策略
- **节点认证** - Ed25519 签名验证（当前为演示模式）

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
- **vcpkg** 包管理器（需要本地安装）

### 安装 vcpkg

**首次安装 vcpkg**（如果尚未安装）：
```bash
# 将 vcpkg 克隆到您选择的目录（不要在这个项目内部）
git clone https://github.com/Microsoft/vcpkg.git C:/vcpkg  # Linux/macOS 上使用 ~/vcpkg
cd C:/vcpkg  # 或 cd ~/vcpkg
.\bootstrap-vcpkg.bat  # Windows 上
# ./bootstrap-vcpkg.sh  # Linux/macOS 上

# 设置 VCPKG_ROOT 环境变量
# Windows (PowerShell):
$env:VCPKG_ROOT="C:/vcpkg"
# 若要永久生效，添加到系统环境变量

# Linux/macOS (bash/zsh):
export VCPKG_ROOT=~/vcpkg
# 添加到 ~/.bashrc 或 ~/.zshrc 以持久化
```

### 构建与运行（推荐）

**使用 CMake Presets**（跨平台）：
```bash
# 克隆仓库（不需要 --recurse-submodules）
git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism

# 确保 VCPKG_ROOT 环境变量已设置
# Windows PowerShell: echo $env:VCPKG_ROOT
# Linux/macOS: echo $VCPKG_ROOT

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
# 首先确保设置了 VCPKG_ROOT
$env:VCPKG_ROOT="C:/vcpkg"  # 调整为你的 vcpkg 位置

git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism
.\scripts\build.ps1
.\build\bin\pacprism.exe
```

**Linux/macOS (Bash):**
```bash
# 首先确保设置了 VCPKG_ROOT
export VCPKG_ROOT=~/vcpkg  # 调整为你的 vcpkg 位置

git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism
chmod +x scripts/build.sh
./scripts/build.sh
./build/bin/pacprism
```

## 文档

- [🗺️ 发展路线图](docs/ROADMAP.md) - 三阶段演进策略：从校园网到公网 P2P
- [📋 当前开发状态](docs/CURRENT_STATUS.md) - 详细实现进度和功能验证
- [🏗️ 项目架构](docs/PROJECT_STRUCTURE.md) - 完整架构设计说明
- [📝 开发日志](devlog_zh/README_DEVLOG.md) - 每日开发记录

## 许可证

GPLv3