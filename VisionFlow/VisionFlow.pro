QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/flow_connection.cpp \
    core/flow_context.cpp \
    core/flow_executor.cpp \
    core/flow_graph.cpp \
    core/flow_graph_validator.cpp \
    core/flow_node.cpp \
    core/flow_node_factory.cpp \
    core/flow_port.cpp \
    core/flow_types.cpp \
    core/module_port_binding.cpp \
    main.cpp \
    mainwindow.cpp \
    modules/base_module.cpp \
    modules/module_definition.cpp \
    modules/module_registry.cpp \
    modules/test/add_one_module.cpp \
    modules/test/const_int_module.cpp \
    modules/test/print_module.cpp \
    modules/test/register_test_modules.cpp \
    modules/test/testdemo.cpp \
    ui/flow_workspace/connection_graphics_item.cpp \
    ui/flow_workspace/flow_scene.cpp \
    ui/flow_workspace/flow_view.cpp \
    ui/flow_workspace/flow_workspace_widget.cpp \
    ui/flow_workspace/node_graphics_item.cpp \
    ui/log_output/log_output_widget.cpp \
    ui/main_editor/main_editor_widget.cpp \
    ui/models/flow_graph_model.cpp \
    ui/module_library/module_library_widget.cpp \
    ui/module_library/module_tree_widget.cpp \
    ui/property_editor/property_editor_widget.cpp \
    ui/toolbar/main_toolbar.cpp


HEADERS += \
    core/flow_connection.h \
    core/flow_context.h \
    core/flow_executor.h \
    core/flow_graph.h \
    core/flow_graph_validator.h \
    core/flow_node.h \
    core/flow_node_factory.h \
    core/flow_port.h \
    core/flow_types.h \
    core/module_port_binding.h \
    mainwindow.h \
    modules/base_module.h \
    modules/module_definition.h \
    modules/module_registry.h \
    modules/test/add_one_module.h \
    modules/test/const_int_module.h \
    modules/test/print_module.h \
    modules/test/register_test_modules.h \
    modules/test/testdemo.h \
    ui/flow_workspace/connection_graphics_item.h \
    ui/flow_workspace/flow_scene.h \
    ui/flow_workspace/flow_view.h \
    ui/flow_workspace/flow_workspace_widget.h \
    ui/flow_workspace/node_graphics_item.h \
    ui/log_output/log_output_widget.h \
    ui/main_editor/main_editor_widget.h \
    ui/models/flow_graph_model.h \
    ui/module_library/module_library_widget.h \
    ui/module_library/module_tree_widget.h \
    ui/property_editor/property_editor_widget.h \
    ui/toolbar/main_toolbar.h


FORMS += \
    mainwindow.ui \
    ui/main_editor/main_editor_widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
