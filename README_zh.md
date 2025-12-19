# pacPrism

![pacPrism Banner](assets/prism-banner.png)

> **系统软件包的分布式缓存层** - 高性能包感知透明代理

[English Version](README.md)

## 核心价值主张

pacPrism 是一个针对系统软件包的**分布式缓存层**，通过透明代理模式提升现有包管理器的性能和可靠性。它作为本地拦截器，与 APT/Pacman 等工具无缝集成，无需任何额外的客户端软件。

### 技术特性

1. **包感知透明代理**
   - 通过修改 `sources.list` 即可启用，对包管理器完全透明
   - 零配置启动，保持与现有工作流的完全兼容性
   - 自动识别包类型、依赖关系和版本信息

2. **语义分片机制**
   - 将相关包分组到逻辑单元（Shards）中
   - 确保依赖完整性和空间局部性
   - 减少跨节点依赖解析的网络往返次数

3. **混合分发架构**
   - **集中化访问层（网关）**：提供统一入口，兼容现有镜像协议
   - **去中心化数据层（P2P）**：节点间共享包缓存，减轻中心服务器负载
   - 有效缓解 P2P 网络的节点波动问题

4. **延迟隐藏流水线**
   - 重叠计算和传输过程
   - 预测性预取热门包及其依赖
   - 并行化多个分片的获取过程

## 架构设计

### 核心组件

#### 1. HTTP 透明代理层
```
sources.list: http://pacprism.local:8080/debian
                ↓
           pacPrism Gateway
                ↓
        [P2P 查询] 或 [官方源回退]
```

#### 2. 语义分片系统
- **Shard ID**: `core-utils`, `web-server`, `development-tools`
- **包映射**: 将逻辑相关的包组织到同一分片中
- **空间局部性**: 同一分片的包通常在同一节点缓存

#### 3. DHT 索引层
- **Key**: `package_name` 或 `shard_id`
- **Value**: `node_ip:port` 列表，按节点健康度排序
- **查询路由**: 智能选择最优节点进行内容获取

#### 4. 缓存回退机制
```
请求 → 本地缓存 → P2P 节点 → 官方镜像
       ← 命中       ← 未命中   ← 最终回退
```

## 实现状态

### ✅ 已完成
- **HTTP 透明代理基础框架**: 基于 Boost.Beast 的 HTTP/1.1 服务器
- **DHT 内存索引**: 基础的分布式哈希表实现 (O(1) 查询性能)
- **跨平台构建系统**: CMake + vcpkg 自动依赖管理

### 🔄 进行中
- **包感知路由逻辑**: 根据 HTTP 头中的包名进行智能分发
- **语义分片算法**: 依赖关系分析和分组策略实现

### 🔬 研究中
- **P2P 内容分发协议**: 节点间文件传输和缓存同步机制
- **缓存预取策略**: 基于使用模式的智能预取算法

## 技术栈

- **核心系统**: **C++23** - 高性能、低延迟网络处理
- **网络库**: **Boost.Beast 1.89.0** - HTTP/1.1异步服务器
- **构建系统**: **CMake 3.14+** - 跨平台模块化构建
- **依赖管理**: **vcpkg** - 自动化依赖安装

## 快速开始

### 环境要求
- **C++23 兼容编译器** (GCC 13+, Clang 14+, MSVC 19.36+)
- **CMake 3.14+**
- **Visual Studio Build Tools** (仅Windows)

### 构建与运行

**Windows:**
```powershell
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism
.\scripts\build.ps1
.\build\bin\pacprism.exe
```

**Linux/macOS:**
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