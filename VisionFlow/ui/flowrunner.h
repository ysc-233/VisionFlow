#ifndef FLOWRUNNER_H
#define FLOWRUNNER_H
#pragma once

#include <QtNodes/DataFlowGraphModel>

class FlowRunner
{
public:
    static void run(QtNodes::DataFlowGraphModel& model);
};

#endif // FLOWRUNNER_H
