#ifndef PRINT_MODULE_H
#define PRINT_MODULE_H
// =======================================================
// modules/test/print_module.h
// =======================================================
#pragma once


#include <QDebug>
#include "modules/base_module.h"


class PrintModule : public BaseModule
{
public:
    bool execute() override
    {
        int value = input("in").toInt();
        qDebug() << "[PrintModule] value =" << value;
        return true;
    }
};
#endif // PRINT_MODULE_H
