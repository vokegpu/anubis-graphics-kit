#include "model.hpp"
#include "agk.hpp"

model::model(stream::mesh &_mesh) {
    this->mesh = _mesh;
}

model::~model() {
    this->buffer.delete_buffers();
}

void model::load(stream::mesh &_mesh) {
    this->compiled = true;
    this->mesh = _mesh;
    this->recompile();
}

void model::recompile() {
    if (this->compiled) {
        return;
    }

    int32_t buffers_driver_read_mode {this->static_buffers ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW};
    this->buffer.invoke();
    bool tbn_flag[4] {};

    if (this->mesh.contains(stream::type::vertex)) {
        auto &list {this->mesh.get_float_list(stream::type::vertex)};
        this->buffer.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.attach(0, 3);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = static_cast<int32_t>(list.size()) / 3;
        util::generateaabb(this->aabb, this->mesh);
        tbn_flag[0] = true;
    }

    if (this->mesh.contains(stream::type::texcoord)) {
        auto &list {this->mesh.get_float_list(stream::type::texcoord)};
        this->buffer.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.attach(1, 2);
        tbn_flag[1] = true;
    }

    if (this->mesh.contains(stream::type::normal)) {
        auto &list {this->mesh.get_float_list(stream::type::normal)};
        this->buffer.bind(2, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.attach(2, 3);
        tbn_flag[2] = true;
    }

    if (this->mesh.contains(stream::type::index)) {
        auto &list {this->mesh.get_uint_list(stream::type::index)};
        this->buffer.bind(3, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        this->buffer.send<uint32_t>(sizeof(uint32_t)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = list.size();
        tbn_flag[3] = true;
    }

    if (tbn_flag[0] && tbn_flag[1] && tbn_flag[2]) {
        auto &v_list {this->mesh.get_float_list(stream::type::vertex)};
        auto &t_list {this->mesh.get_float_list(stream::type::texcoord)};
        auto &n_list {this->mesh.get_float_list(stream::type::normal)};

        std::vector<float> tangent_list {};
        bool generate_tangents_from_indices {tbn_flag[3]};

        glm::vec3 vec_list_v[3] {};
        glm::vec2 vec_list_t[3] {};
        glm::vec3 tangent {};
        glm::mat3 mat3x3tbn {};

        /*
         * (triangle face = 3 vertex) == 3 tangents
         */
        if (generate_tangents_from_indices) {
            auto &i_list {this->mesh.get_uint_list(stream::type::index)};
            tangent_list.resize(this->buffer.stride[1]);
            int32_t index {};

            for (uint64_t it {}; it < i_list.size(); it++) {
                index = i_list.at(it);
                float *p_v {&v_list.at(index * 3)};
                float *p_t {&t_list.at(index * 2)};
                float *p_n {&n_list.at(index * 3)};

                vec_list_v[0].x = p_v[0];
                vec_list_v[0].y = p_v[1];
                vec_list_v[0].z = p_v[2];
                vec_list_v[1].x = p_v[3];
                vec_list_v[1].y = p_v[4];
                vec_list_v[1].z = p_v[5];
                vec_list_v[2].x = p_v[6];
                vec_list_v[2].y = p_v[7];
                vec_list_v[2].z = p_v[8];

                vec_list_t[0].x = p_t[0];
                vec_list_t[0].y = p_t[1];
                vec_list_t[1].x = p_t[2];
                vec_list_t[1].y = p_t[3];
                vec_list_t[2].x = p_t[4];
                vec_list_t[2].y = p_t[5];

                tangent = util::generatetangent(vec_list_v, vec_list_t);

                vec_list_v[0].x = p_n[0];
                vec_list_v[0].y = p_n[1];
                vec_list_v[0].z = p_n[2];
                vec_list_v[1].x = p_n[3];
                vec_list_v[1].y = p_n[4];
                vec_list_v[1].z = p_n[5];
                vec_list_v[2].x = p_n[6];
                vec_list_v[2].y = p_n[7];
                vec_list_v[2].z = p_n[8];

                tangent_list.push_back(tangent.x);
                tangent_list.push_back(tangent.y);
                tangent_list.push_back(tangent.z);

                /* Bitangent one */
                vec_list_v[0] = glm::cross(vec_list_v[0], tangent);
                tangent_list.push_back(vec_list_v[0].x);
                tangent_list.push_back(vec_list_v[0].y);
                tangent_list.push_back(vec_list_v[0].z);

                tangent_list.push_back(tangent.x);
                tangent_list.push_back(tangent.y);
                tangent_list.push_back(tangent.z);

                /* Bitangent two */
                vec_list_v[1] = glm::cross(vec_list_v[1], tangent);
                tangent_list.push_back(vec_list_v[1].x);
                tangent_list.push_back(vec_list_v[1].y);
                tangent_list.push_back(vec_list_v[1].z);

                tangent_list.push_back(tangent.x);
                tangent_list.push_back(tangent.y);
                tangent_list.push_back(tangent.z);

                /* Bitangent two */
                vec_list_v[2] = glm::cross(vec_list_v[2], tangent);
                tangent_list.push_back(vec_list_v[2].x);
                tangent_list.push_back(vec_list_v[2].y);
                tangent_list.push_back(vec_list_v[2].z);
            }
        } else {
            tangent_list.resize(this->buffer.stride[1] * 3);

            // 2 faces == 6 vertex == 18 vectors == 18*3 float components
            for (uint64_t it {}; it < this->buffer.stride[1] / 3; it += 3) {
                float *p_v {&v_list.at(it * 3)};
                float *p_t {&t_list.at(it * 2)};
                float *p_n {&n_list.at(it * 3)};

                vec_list_v[0].x = p_v[0];
                vec_list_v[0].y = p_v[1];
                vec_list_v[0].z = p_v[2];
                
                vec_list_v[1].x = p_v[3];
                vec_list_v[1].y = p_v[4];
                vec_list_v[1].z = p_v[5];
                
                vec_list_v[2].x = p_v[6];
                vec_list_v[2].y = p_v[7];
                vec_list_v[2].z = p_v[8];

                vec_list_t[0].x = p_t[0];
                vec_list_t[0].y = p_t[1];
                
                vec_list_t[1].x = p_t[2];
                vec_list_t[1].y = p_t[3];

                vec_list_t[2].x = p_t[4];
                vec_list_t[2].y = p_t[5];

                tangent = util::generatetangent(vec_list_v, vec_list_t);

                vec_list_v[0].x = p_n[0];
                vec_list_v[0].y = p_n[1];
                vec_list_v[0].z = p_n[2];
                vec_list_v[1].x = p_n[3];
                vec_list_v[1].y = p_n[4];
                vec_list_v[1].z = p_n[5];
                vec_list_v[2].x = p_n[6];
                vec_list_v[2].y = p_n[7];
                vec_list_v[2].z = p_n[8];

                tangent_list.push_back(tangent.x);
                tangent_list.push_back(tangent.y);
                tangent_list.push_back(tangent.z);

                /* Bitangent one */
                vec_list_v[0] = glm::cross(vec_list_v[0], tangent);
                tangent_list.push_back(vec_list_v[0].x);
                tangent_list.push_back(vec_list_v[0].y);
                tangent_list.push_back(vec_list_v[0].z);

                tangent_list.push_back(tangent.x);
                tangent_list.push_back(tangent.y);
                tangent_list.push_back(tangent.z);

                /* Bitangent two */
                vec_list_v[1] = glm::cross(vec_list_v[1], tangent);
                tangent_list.push_back(vec_list_v[1].x);
                tangent_list.push_back(vec_list_v[1].y);
                tangent_list.push_back(vec_list_v[1].z);

                tangent_list.push_back(tangent.x);
                tangent_list.push_back(tangent.y);
                tangent_list.push_back(tangent.z);

                /* Bitangent two */
                vec_list_v[2] = glm::cross(vec_list_v[2], tangent);
                tangent_list.push_back(vec_list_v[2].x);
                tangent_list.push_back(vec_list_v[2].y);
                tangent_list.push_back(vec_list_v[2].z);
            }

            this->buffer.bind(4, {GL_ARRAY_BUFFER, GL_FLOAT});
            this->buffer.send<float>(sizeof(float)*tangent_list.size(), tangent_list.data(), buffers_driver_read_mode);
            this->buffer.attach(4, 3, {sizeof(float)*6, 0});
            this->buffer.attach(5, 3, {sizeof(float)*6, sizeof(float)*3});
            this->buffer.unbind();
        }
    }

    this->buffer.revoke();
    this->compiled = true;
    util::log("Compiled model buffers: " + std::to_string(this->mesh.faces) + " faces");
}

bool model::is_compiled() {
    return this->compiled;
}