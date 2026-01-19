#pragma once

namespace fm {

enum class DestroyMode : int {
    Default = 0, // 相邻的6个面
    Cube    = 1  // 3x3x3=27
};

struct DispatcherConfig {
    int globalBlockLimitPerTick{256}; // 全局每tick方块上限
    int maxResumeTasksPerTick{16};    // 每tick最大恢复任务数
};

} // namespace fm