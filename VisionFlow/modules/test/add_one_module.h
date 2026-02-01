#ifndef ADD_ONE_MODULE_H
#define ADD_ONE_MODULE_H
// =======================================================
// modules/test/add_one_module.h
// =======================================================
#pragma once


#include "modules/base_module.h"

#include <QDebug>
class AddOneModule : public BaseModule
{
public:
    bool execute() override
    {
        int in = input("in").toInt();
        qDebug() << "[AddOne] in =" << in;
        setOutput("out", in + 2);
        return true;
    }
};
#endif // ADD_ONE_MODULE_H
