#pragma once
#include "ll/api/base/Concepts.h"
#include "ll/api/coro/Executor.h"

#include <coroutine>

namespace fm {


struct MinerTask;
class MinerDispatcher;

class MinerPermitAwaiter {
    ll::coro::ExecutorRef exec;
    MinerTask*            task;
    MinerDispatcher&      dispatcher;

public:
    explicit MinerPermitAwaiter(MinerTask* task, MinerDispatcher& dispatcher);

    void setExecutor(ll::coro::ExecutorRef exec) { this->exec = exec; }

    // 永远不就绪, 由全局的 MinerDispatcher 调度分配许可
    constexpr bool await_ready() noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h);

    void await_resume() noexcept {}
};

static_assert(ll::concepts::Awaitable<MinerPermitAwaiter>);


} // namespace fm