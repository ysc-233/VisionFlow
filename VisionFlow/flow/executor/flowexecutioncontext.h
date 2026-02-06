#ifndef FLOWEXECUTIONCONTEXT_H
#define FLOWEXECUTIONCONTEXT_H
#include <atomic>

class FlowExecutionContext {
public:
    static std::atomic<bool> running;
};
#endif // FLOWEXECUTIONCONTEXT_H
