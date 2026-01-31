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
    core/flow_node.cpp \
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
    modules/test/testdemo.cpp


HEADERS += \
    core/flow_connection.h \
    core/flow_context.h \
    core/flow_executor.h \
    core/flow_graph.h \
    core/flow_node.h \
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
    modules/test/testdemo.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
