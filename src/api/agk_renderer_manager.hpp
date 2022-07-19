#pragma once

#ifndef AGK_RESOURCE_MANAGER_H
#define AGK_RESOURCE_MANAGER_H

#include "agk_gpu.hpp"

/**
 * The effects manager.
 **/
struct agk_fx_manager {
   static gpu::program fx_model;
   static void init();
};

#endif