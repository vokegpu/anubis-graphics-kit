#ifndef AGK_WORLD_PBR_MODEL_H
#define AGK_WORLD_PBR_MODEL_H

#include "gpu/tools.hpp"
#include "core/imodule.hpp"
#include <functional>
#include "util/math.hpp"

class model : public imodule {
protected:
	mesh::data mesh_data {};
	bool compiled {};
	std::function<void(buffering&, mesh::data&)> mixin {};
public:
	buffering buffer {};
	bool static_buffers {true};
	util::aabb aabb {};

	void load(std::string_view path, const std::function<void(buffering&, mesh::data&)> &injection_mixin = {});
	void recompile();
	bool is_compiled();

	void on_destroy() override;
	void on_render() override;
};

#endif