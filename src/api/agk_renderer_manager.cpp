#include "agk_renderer_manager.hpp"

gpu::program agk_fx_manager::fx_model;

void agk_fx_manager::init() {
    gpu::create_program(agk_fx_manager::fx_model, "data/fx/fx_model.vsh", "data/fx/fx_model.fsh");
}