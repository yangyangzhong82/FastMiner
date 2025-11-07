#include "MinerPermitAwaiter.h"
#include "core/MinerDispatcher.h"
#include "core/MinerTask.h"

namespace fm {


MinerPermitAwaiter::MinerPermitAwaiter(MinerTask* task, MinerDispatcher& dispatcher)
: task(task),
  dispatcher(dispatcher) {}

void MinerPermitAwaiter::await_suspend(std::coroutine_handle<> h) {
    // 将任务和协程句柄交给调度器排队
    dispatcher.enqueue(task, h, exec);
}


} // namespace fm