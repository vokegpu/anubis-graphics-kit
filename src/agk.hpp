#pragma once
#ifndef AGK_H
#define AGK_H

#include "impl/agk_client.hpp"
#include "api/agk_gpu.hpp"

namespace agk {
    extern agk_client* const core;

    void push_back_camera(gpu::program &program);
};

#endif