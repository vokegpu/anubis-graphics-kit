#include "agk_render.hpp"
#include "agk.hpp"
#include "agk_util.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <fstream>

gpu::program agk_batch3d::fx_shape3d;

std::vector<float> &agk_mesh_stream::access_data() {
    return this->data;
}

void agk_mesh_stream::dispatch(float x, float y, float z, float u, float v) {
    this->data.push_back(x);
    this->data.push_back(y);
    this->data.push_back(z);
    this->data.push_back(u);
    this->data.push_back(v);
    this->vertex_amount++;
}

void agk_mesh_stream::dispatch(uint32_t sizeof_list, float* list) {
    for (uint32_t i = 0; i < sizeof_list; i++) {
        this->data.push_back(list[i]);
    }

    this->vertex_amount += sizeof_list / 5;
}

GLuint agk_mesh_stream::size() {
    return this->data.size();
}

void agk_mesh_stream::clear() {
    this->data.clear();
    this->data_normal.clear();
}

uint32_t agk_mesh_stream::get_vertex_amount() {
    return this->vertex_amount;
}

void agk_mesh_stream::dispatch_normal(float x, float y, float z) {
    this->data_normal.push_back(x);
    this->data_normal.push_back(y);
    this->data_normal.push_back(z);
}

std::vector<float> &agk_mesh_stream::access_data_normals() {
    return this->data_normal;
}

void agk_batch3d::init() {
    gpu::create_program(agk_batch3d::fx_shape3d, "data/effects/DiffuseLighting.vs", "data/effects/DiffuseLighting.fs");
}

void agk_batch3d::invoke() {
    this->mesh.clear();
}

void agk_batch3d::revoke() {
    if (!this->should_not_create_buffers) {
        glGenVertexArrays(1, &this->vao_all_data);
        glGenBuffers(1, &this->vbo_data1);
        glGenBuffers(1, &this->vbo_data2);
        this->should_not_create_buffers = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_data1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->mesh.size(), &this->mesh.access_data()[0], GL_STATIC_DRAW);

    glBindVertexArray(this->vao_all_data);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 3));

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_data2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->mesh.access_data_normals().size(), &this->mesh.access_data_normals()[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void agk_batch3d::draw(const glm::vec3 &pos, const glm::vec4 &color) {
    if (!this->should_not_create_buffers) {
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    glm::mat3 normal_matrix = glm::mat3(model);
    normal_matrix = glm::inverseTranspose(normal_matrix);

    glm::vec3 light_pos = agk::core->camera->position;

    agk_batch3d::fx_shape3d.use();
    agk::push_back_camera(agk_batch3d::fx_shape3d);

    agk_batch3d::fx_shape3d.setm4f("ModelViewMatrix", &model[0][0]);
    agk_batch3d::fx_shape3d.setm3f("NormalMatrix", &normal_matrix[0][0]);
    agk_batch3d::fx_shape3d.set3f("Kd", &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
    agk_batch3d::fx_shape3d.set3f("Ld", &color[0]);
    agk_batch3d::fx_shape3d.set4f("LightSpot", &light_pos[0]);
    agk_batch3d::fx_shape3d.set3f("Camera", &agk::core->camera->position[0]);

    glBindVertexArray(this->vao_all_data);
    glDrawArrays(GL_TRIANGLES, 0, (int32_t) this->mesh.get_vertex_amount());
    glBindVertexArray(0);
    glUseProgram(0);
}

void agk_batch3d::dispatch(agk_mesh_stream &m_stream) {
    this->mesh += m_stream;
}

bool model::load(agk_mesh_stream &mesh, const std::string &path, model::type type) {
    std::ifstream ifs(path.c_str());
    std::string string_builder;
    bool flag = false;

    if ((flag = ifs.is_open())) {
        std::string string_buffer;
        int32_t len = 0;

        int32_t index1 = 0;
        int32_t index2 = 0;
        int32_t index3 = 0;

        float x = 0;
        float y = 0;
        float z = 0;

        std::string find;

        while (getline(ifs, string_buffer)) {
            len = (int32_t) string_buffer.size();

            if (len < 1) {
                continue;
            }

            find = string_buffer.substr(0, 2);

            if (find == "v ") {
                index1 = (int32_t) string_buffer.find(' ') + 1;
                index2 = (int32_t) string_buffer.find(' ', index1 + 1);
                index3 = (int32_t) string_buffer.find(' ', index2 + 2);

                x = static_cast<float>(std::stof(string_buffer.substr(index1, index2)));
                y = static_cast<float>(std::stof(string_buffer.substr(index2, index3)));
                z = static_cast<float>(std::stof(string_buffer.substr(index3, len)));

                mesh.dispatch(x, y, z, 1.0, 1.0);
            }

            if (find == "vn") {
                index1 = (int32_t) string_buffer.find(' ') + 1;
                index2 = (int32_t) string_buffer.find(' ', index1 + 1);
                index3 = (int32_t) string_buffer.find(' ', index2 + 2);

                x = static_cast<float>(std::stof(string_buffer.substr(index1, index2)));
                y = static_cast<float>(std::stof(string_buffer.substr(index2, index3)));
                z = static_cast<float>(std::stof(string_buffer.substr(index3, len)));

                mesh.dispatch_normal(x, y, z);
            }
        }

        ifs.close();
    }

    return flag;
}
