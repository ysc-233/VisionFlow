# VisionFlow

基于 Qt + QtNodes + OpenCV 的可视化图像处理流程编辑与执行工具

## 目录

- [简介](#简介)
- [功能特性](#功能特性)
- [系统架构](#系统架构)
- [节点列表](#节点列表)
- [环境要求](#环境要求)
- [使用指南](#使用指南)
- [开发指南](#开发指南)
- [项目结构](#项目结构)

---

## 简介

VisionFlow 是一个工业视觉流程快速构建工具，通过节点拖拽和连线的方式搭建图像处理 Pipeline，无需编写代码即可完成复杂的视觉检测任务。它提供可视化界面用于构建图像处理流程，支持实时执行和结果可视化。

---

## 功能特性

### 流程编辑
- 节点拖拽创建（右键菜单/左侧面板）
- 端口自动连接与数据类型检查
- 多 Tab 工作区，支持并行编辑多个流程
- 流程保存/加载（JSON 格式）

### 执行控制
- Run/Stop 按钮控制流程执行
- 节点执行状态实时显示（Idle/Running/Done/Timeout）
- 执行耗时统计（总时间 + 单节点时间）
- 全局超时保护（默认 3000ms）

### 数据处理
- 双通道数据输出：
  - **QtNodes 数据流**：`outData()` 用于图像/结构化数据
  - **FlowExecutor 执行流**：`getOutput()` 用于 QVariant 数据
- 支持数据类型：`MatPtr`、`QMap<QString, QVariant>`、`int`

### 可视化
- 实时图像显示（支持多图拼接）
- 匹配结果标注（框 + 中心点 + 坐标）
- 日志系统（分级显示 + 关键词过滤）

---

## 系统架构

```
+-------------------------------------------------------------+
|                      UI 层 (Qt Widgets)                       |
|  +---------------+  +----------------+  +-----------------+  |
|  |VisionFlowView |->|FlowEditorWidget|->| FlowView + Scene|  |
|  +---------------+  +----------------+  +-----------------+  |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                   图模型层 (QtNodes)                          |
|  +---------------------------------------------------------+ |
|  |              DataFlowGraphModel                          | |
|  |  - 节点管理           - 连接管理                           | |
|  |  - 数据传播           - 保存/加载                          | |
|  +---------------------------------------------------------+ |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                      节点层                                  |
|  +------------+ +------------+ +------------+ +-----------+  |
|  |ImageLoad   | |TemplateMatch| |ImageGray   | |ImageSave  |  |
|  |NodeDelegate| |NodeDelegate | |NodeDelegate| |NodeDelegate| |
|  |+ FlowNode  | |+ FlowNode   | |+ FlowNode  | |+ FlowNode |  |
|  +------------+ +------------+ +------------+ +-----------+  |
+-------------------------------------------------------------+
                              |
                              v
+-------------------------------------------------------------+
|                    执行层                                    |
|  +-------------+  +------------------+  +----------------+   |
|  | FlowRunner  |->|FlowExecutorWorker|->|FlowGraphExporter|  |
|  | (单例)      |  | (拓扑排序)        |  | (图导出)        |  |
|  +-------------+  +------------------+  +----------------+   |
+-------------------------------------------------------------+
```

### 数据流架构

```
+--------------------------------------------------------------+
|                    双通道数据输出                              |
+--------------------------------------------------------------+

  Node::outData(PortIndex)
     |
     v
  std::shared_ptr<NodeData>
     |
     v
  QtNodes 图形数据流 -> 下游节点 setInData()

  -------------------------------------------------------------

  Node::getOutput(int port)
     |
     v
  QVariant
     |
     v
  FlowExecutor 执行流 -> 下游节点 setInput()

+--------------------------------------------------------------+
```

---

## 节点列表

### 输入/输出节点

| 节点名称 | 输入端口 | 输出端口 | 功能描述 |
|----------|----------|----------|----------|
| ImageLoad | 0 | 1 (Image) | 从文件加载图像 |
| ImageSave | 1 (Image) | 0 | 保存图像到文件 |
| IntOutput | 0 | 1 (int) | 输出整数常量 |

### 图像处理节点

| 节点名称 | 输入端口 | 输出端口 | 功能描述 |
|----------|----------|----------|----------|
| ImageGray | 1 (Image) | 1 (Image) | 彩色转灰度 |
| ImageMerge | 2 (Image) | 1 (Image) | 图像拼接（横/纵） |
| CenterCrop | 1 (Image) + 1 (int) | 1 (Image) | 中心裁剪 |

### 算法节点

| 节点名称 | 输入端口 | 输出端口 | 功能描述 |
|----------|----------|----------|----------|
| TemplateMatch | 1 (Image) | 1 (Image) + 1 (Map) | 模板匹配检测 |

### TemplateMatch 输出格式

```cpp
// 端口 1: QMap<QString, QVariant>
{
    "pixelX": float,      // 匹配中心 X 坐标
    "pixelY": float,      // 匹配中心 Y 坐标
    "angle": float,       // 匹配角度
    "score": float,       // 匹配置信度 (0-1)
    "width": int,         // 模板宽度
    "height": int         // 模板高度
}
```

---

## 环境要求

### 软件
- Qt: 5.12 或更高版本
- OpenCV: 4.5 或更高版本
- CMake: 3.16 或更高版本

### 编译器
- Windows: MSVC 2017 或更高版本
- Linux: GCC 9 或更高版本
- macOS: Clang 12 或更高版本

---


## 使用指南

### 基本流程

1. 启动应用程序
2. 从左侧面板拖拽节点到工作区
3. 通过从输出端口拖拽到输入端口连接节点
4. 配置节点参数（如文件路径、阈值等）
5. 点击 Run 按钮执行流程
6. 在显示面板和日志窗口查看结果

### 保存/加载流程

- **保存**: 文件 -> 保存 (或 Ctrl+S)
- **另存为**: 文件 -> 另存为
- **打开**: 文件 -> 打开 (或 Ctrl+O)

### 模板匹配使用

1. 添加 TemplateMatch 节点到工作区
2. 连接输入图像（来自 ImageLoad 或上游节点）
3. 点击节点上的"创建模板"按钮
4. 加载参考图像并选择模板区域
5. 点击"保存模板"存储模板
6. 调整阈值参数 (0.0-1.0)
7. 运行流程检测匹配

---

## 开发指南

### 创建新节点

```cpp
// 1. 继承 NodeDelegateModel 和 FlowNode
class MyNode : public QtNodes::NodeDelegateModel, public FlowNode
{
    Q_OBJECT
public:
    // 必需接口
    unsigned int nPorts(QtNodes::PortType type) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port) override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
    
    // FlowNode 接口
    QVariant getOutput(int port) override;
    void setInput(int port, const QVariant &data) override;
    void compute() override;
    
    // 可选：持久化
    QJsonObject saveData() const override;
    void loadData(const QJsonObject &obj) override;
};

// 2. 注册节点
REGISTER_NODE(MyNode, "Category")
```

### 节点接口参考

| 方法 | 用途 | 调用时机 |
|------|------|----------|
| `setInData()` | 接收 QtNodes 数据流 | 上游节点数据更新时 |
| `outData()` | 输出 QtNodes 数据流 | 下游节点请求数据时 |
| `setInput()` | 接收 FlowExecutor 数据 | 执行引擎传递数据时 |
| `getOutput()` | 输出 FlowExecutor 数据 | 执行引擎获取结果时 |
| `compute()` | 核心计算逻辑 | 执行引擎调度时 |
| `outputImages()` | 返回可视化图像 | UI 刷新显示时 |

### 数据类型注册

```cpp
// main.cpp
int main(int argc, char *argv[])
{
    qRegisterMetaType<MatPtr>("MatPtr");
    qRegisterMetaType<QMap<QString, QVariant>>("QMap<QString,QVariant>");
    qRegisterMetaType<LogLevel>("LogLevel");
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
```

### 日志使用

```cpp
// 使用日志宏
LOG_DEBUG("调试信息");
LOG_INFO("信息");
LOG_WARNING("警告信息");
LOG_ERROR("错误信息");
LOG_FATAL("致命错误");
```

---

## 项目结构

```
VisionFlow/
├── CMakeLists.txt              # 构建配置
├── flow/                       # 执行引擎
│   ├── executor/               # 执行器
│   │   ├── FlowRunner          # 执行控制器（单例）
│   │   ├── FlowExecutorWorker  # 工作线程（拓扑排序）
│   │   ├── FlowTimeoutController # 超时保护
│   │   └── FlowExecutionContext  # 执行上下文
│   ├── exporter/               # 图导出器
│   │   └── FlowGraphExporter   # QtNodes 转 FlowGraph
│   ├── graph/                  # 图数据结构
│   │   ├── FlowGraph           # 执行图
│   │   ├── FlowConnection      # 连接关系
│   │   └── FlowNode            # 节点基类
│   └── flowtypes.h             # 数据类型定义
├── nodes/                      # 节点实现
│   ├── imageloadnode.*         # 图像加载
│   ├── imagesavenode.*         # 图像保存
│   ├── imagegraynode.*         # 灰度转换
│   ├── imagemergenode.*        # 图像拼接
│   ├── centercropnode.*        # 中心裁剪
│   ├── templatmatchnode.*      # 模板匹配
│   └── flownodeautoregister.*  # 自动注册
├── ui/                         # 用户界面
│   ├── view/
│   │   ├── VisionFlowView      # 主视图
│   │   └── visionflowview.ui   # UI 设计
│   └── component/
│       ├── WorkSpace/          # 工作区组件
│       │   ├── FlowEditorWidget # 流程编辑器
│       │   ├── FlowScene       # 场景
│       │   ├── FlowView        # 视图
│       │   └── CustomTabWidget # 多 Tab 支持
│       └── NodeLibrary/        # 节点库
│           ├── NodeLibraryWidget
│           └── NodeTreeWidget
└── utils/                      # 工具类
    ├── imageconvert.*          # Mat 转 QImage 转换
    └── logmanager.*            # 日志系统
```

---

## 执行流程

```
用户点击 Run
     |
     v
FlowRunner::run()
     |
     v
FlowExecutorWorker (新线程)
     |
     v
拓扑排序 -> 确定执行顺序
     |
     v
遍历节点 -> 调用 compute()
     |
     v
节点状态：Idle -> Running -> Done
     |
     v
getOutput() -> setInput() 传递数据
     |
     v
emit nodeUpdated() -> UI 刷新
     |
     v
全部完成 -> emit finished()
```

