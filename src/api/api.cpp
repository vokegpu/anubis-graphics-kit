#include "api.hpp"
#include "api/util/env.hpp"
#include <GL/glew.h>
#include <amogpu/amogpu.hpp>

core api::app {};

void api::mainloop(feature* initial_scene) {
	util::log("initialising");

	if (SDL_Init(SDL_INIT_VIDEO)) {
		util::log("failed to init SDL");
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	api::app.root = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, api::app.screen_width, api::app.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(api::app.root)};
    util::log("window released");

    amogpu::init();
    dynamic_batching batch {};
    font_renderer f_renderer {};
    f_renderer.load("./data/fonts/impact.ttf", 36);
    amogpu::gl_version = "#version 450 core";
    f_renderer.from(amogpu::invoked);

	glewExperimental = true;
    glewInit();
	SDL_GL_SetSwapInterval(1);
	util::log("OpenGL 4 context created");

	util::timing reduce_cpu_ticks_timing {};
	util::timing counter_fps_timing {};

	uint64_t cpu_ticks_interval {api::app.fps};
	uint64_t fps_interval {1000};
	uint64_t ticked_frames {};
	SDL_Event sdl_event {};

    glm::vec3 prev_camera {};

	api::app.mainloop = true;
	api::scene::load(initial_scene);
    api::gc::create(&api::app.world_render_manager);
    api::gc::create(&api::app.world_client);
    api::gc::create(&api::app.world_camera3d);

	while (api::app.mainloop) {
		if (util::resetifreach(reduce_cpu_ticks_timing, cpu_ticks_interval)) {
			while (SDL_PollEvent(&sdl_event)) {
				switch (sdl_event.type) {
					case SDL_QUIT: {
						api::app.mainloop = false;
						break;
					}

					default: {
						if (api::app.current_scene != nullptr) api::app.current_scene->on_event(sdl_event);
                        api::app.world_camera3d.on_event(sdl_event);
                        api::app.world_client.on_event(sdl_event);
                        api::app.world_render_manager.on_event(sdl_event);
						break;
					}
				}
			}

			if (util::resetifreach(reduce_cpu_ticks_timing, fps_interval)) {
				api::app.display_fps = ticked_frames;
				ticked_frames = 0;
			}

			if (api::app.current_scene != nullptr) {
				api::app.current_scene->on_update();
			}

            api::app.world_client.on_update();
            api::app.world_render_manager.on_update();
			api::app.garbage_collector.do_update();

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

            api::app.world_render_manager.on_render();

			if (api::app.current_scene != nullptr) {
				api::app.current_scene->on_render();
			}

            if (prev_camera.x != api::app.world_camera3d.yaw || prev_camera.y != api::app.world_camera3d.yaw) {
                prev_camera.x = api::app.world_camera3d.yaw;
                prev_camera.y = api::app.world_camera3d.pitch;

                batch.invoke();
                f_renderer.render("Yaw Pitch [" + std::to_string(prev_camera.x) + ", " + std::to_string(prev_camera.y) + "]", 10, 10, {1.0f, 1.0f, 1.0f, 1.0f});
                batch.revoke();
            }

            batch.draw();

			ticked_frames++;
			SDL_GL_SwapWindow(api::app.root);
		}
	}

    api::app.shader_manger.quit();
}

void api::scene::load(feature* scene) {
	auto &current_scene {api::scene::current()};

	// if you think it is bad, you should code more.

	if (scene == nullptr && current_scene != nullptr) {
		api::gc::destroy(current_scene);
		api::app.current_scene = nullptr;
		return;
	}

	if (scene != nullptr && current_scene != nullptr && current_scene == scene) {
		api::gc::destroy(scene);
		return;
	}

	if (scene != nullptr && current_scene != nullptr && current_scene != scene) {
		api::gc::destroy(current_scene);
		api::gc::create(scene);
		api::app.current_scene = scene;
		return;
	}

	if (scene != nullptr && current_scene == nullptr) {
		api::gc::create(scene);
		api::app.current_scene = scene;
	}
}

feature* &api::scene::current() {
	return api::app.current_scene;
}

void api::gc::destroy(feature* target) {
	api::app.garbage_collector.destroy(target);
}

void api::gc::create(feature* target) {
	api::app.garbage_collector.create(target);
}

bool api::shading::createprogram(std::string_view name, ::shading::program &program, const std::vector<::shading::resource> &resources) {
    return api::app.shader_manger.create_shading_program(name, program, resources);
}

camera3d &api::world::camera3d() {
    return api::app.world_camera3d;
}

::world &api::world::current() {
    return api::app.world_client;
}

world_render &api::world::render() {
    return api::app.world_render_manager;
}

void api::world::create(object* target) {
    api::app.world_client.loaded_object_list.push_back(target);
    api::app.world_client.loaded_object_map[target->id] = target;
}

void api::world::destroy(object* target) {
    // todo destroy/kill object/entity from the world client
}

bool api::mesh::load(::mesh::data &data, std::string_view path) {
    return api::app.mesh3d_loader.load_object(data, path);
}

void api::mesh::compile(::mesh::data &data, buffer_builder* model) {
    model->invoke();
    model->primitive = GL_TRIANGLES;
    model->vert_amount = data.vert_amount;
    model->mode = buffer_builder_mode::normal;

    model->bind();
    model->send_data((int32_t) data.vertices.size(), data.vertices.data(), GL_STATIC_DRAW);
    model->shader(0, 3, 0, 0);

    model->bind();
    model->send_data((int32_t) data.normals.size(), data.normals.data(), GL_STATIC_DRAW);
    model->shader(1, 3, 0, 0);
    model->revoke();
}
