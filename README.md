# VisionFlow 当前框架与实现概览

> 本文档用于总结当前 VisionFlow 项目的**整体架构、已实现功能与关键设计决策**，并给出**下一阶段的推荐演进路线**。

---

## 1. 项目目标（当前阶段）

VisionFlow 是一个 **基于节点（Node）/端口（Port）/连线（Connection）的数据流执行引擎**，目标是：

- 支持模块化的数据处理流程
- 支持清晰、可扩展的 Flow Graph 描述
- 提供稳定、可验证的执行引擎（Execution Engine）
- 为后续 UI 编辑器 / 序列化 / 图像处理模块打下核心基础

当前阶段的目标已经聚焦在：

> **“一个最小但完整、可执行、数据正确流动的 Flow 引擎内核”**

---

## 2. 核心架构总览

整体架构可以抽象为五个核心层次：

```
ModuleDefinition / ModuleRegistry
            ↓
         FlowNode
            ↓
FlowGraph (Nodes + Connections)
            ↓
      FlowExecutor
            ↓
        FlowContext
```

---

## 3. Core 数据模型

### 3.1 FlowNode

**职责**：
- 表示 FlowGraph 中的一个节点实例
- 绑定一个 moduleType（对应可执行模块）
- 持有端口（FlowPort）与参数（parameters）

**核心属性**：
- NodeId（全局唯一）
- moduleType（字符串）
- ports（输入 / 输出端口）
- parameters（运行期参数）

**设计要点**：
- FlowNode 本身不负责创建端口
- 不在构造期依赖 ModuleRegistry
- 是一个“图数据节点”，而不是模块工厂

---

### 3.2 FlowPort

**职责**：
- 表示节点上的一个输入 / 输出端口
- 提供数据流在 FlowContext 中的唯一 Key

**关键设计**：
- FlowPort 同时持有：
  - portId（如 `in` / `out`）
  - ownerNodeId（所属节点）
- 通过 `contextKey()` 统一生成数据通道 Key：

```
<nodeId>.<portId>
```

这是当前数据流正确运行的**关键约定**。

---

### 3.3 FlowConnection

**职责**：
- 描述一个端口到端口的连接关系

**当前模型**：
- fromPort：上游端口（contextKey）
- toPort：下游端口（contextKey）

**设计取舍**：
- Connection 是“端口级”的，不直接依赖 Node
- 连接语义在执行期由 FlowExecutor 解析

---

## 4. 执行系统

### 4.1 FlowGraph

**职责**：
- 管理所有 FlowNode
- 管理 FlowConnection
- 提供拓扑排序（topologicalSort）

**当前能力**：
- Node 增删
- Connection 增删
- DAG 校验基础能力

---

### 4.2 FlowExecutor

**职责**：
- 根据 FlowGraph 的拓扑顺序执行节点
- 为每个节点创建模块实例
- 构建 ModulePortBinding
- 调度模块执行

**执行流程（简化）**：

```
clear context
for node in topo order:
    create module
    build input/output binding
    bind context & parameters
    module.execute()
```

**已验证能力**：
- 执行顺序正确
- stop 机制可扩展
- 多模块数据流正确传播

---

### 4.3 FlowContext

**职责**：
- 保存运行期所有端口的数据
- 通过 contextKey 进行读写

**当前特点**：
- 使用 QVariant 存储数据
- 不存在 Key 时返回默认值（当前阶段设计）

---

## 5. 模块系统

### 5.1 BaseModule

**职责**：
- 所有模块的基类
- 定义统一的生命周期接口

**核心能力**：
- bind(binding, parameters, context)
- input(name)
- setOutput(name, value)
- execute()

模块不关心 Graph，只关心：
> **“我从哪里读数据、往哪里写数据”**

---

### 5.2 ModuleRegistry / ModuleDefinition

**ModuleDefinition**：
- 描述模块的结构性信息（ports / 默认参数）

**ModuleRegistry**：
- 管理 moduleType → definition / creator 的映射
- 支持动态注册模块

**当前状态**：
- 模块系统稳定
- 支持最小 demo 模块注册

---

## 6. 最小可执行 Demo（已完成）

当前已实现并验证的最小 Flow：

```
ConstInt(value=1) → AddOne → Print
```

**运行日志验证**：
- ConstInt 正确输出 1
- AddOne 正确接收并输出 2
- PrintModule 正确打印结果

这标志着：

> **VisionFlow 的“数据流执行内核”已经完整成立**

---

## 7. 当前阶段的设计边界（刻意未做）

以下内容是**刻意未实现**，属于下一阶段：

- Node 自动从 ModuleDefinition 构建
- 执行前强校验（未连接端口 / 类型不匹配）
- UI / NodeEditor
- Graph 序列化 / 反序列化
- 多线程 / 并行执行