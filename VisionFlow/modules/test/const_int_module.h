#ifndef CONST_INT_MODULE_H
#define CONST_INT_MODULE_H

// =======================================================
// modules/test/const_int_module.h
// =======================================================
#pragma once
#include <QDebug>

#include "modules/base_module.h"


class ConstIntModule : public BaseModule
{
public:
    bool execute() override
    {
        int value = parameter("value").toInt();
         qDebug() << "[ConstInt] emit" << value;
        setOutput("out", value);
        return true;
    }
};

#endif // CONST_INT_MODULE_H
