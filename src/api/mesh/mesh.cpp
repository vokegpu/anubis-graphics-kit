#include "mesh.hpp"

void mesh::data::append(mesh::type type, const glm::vec4 &vec) {
    switch (type) {
        case mesh::type::vertex: {
            this->v_list.push_back(vec.x);
            this->v_list.push_back(vec.y);
            this->v_list.push_back(vec.z);
            this->v_list.push_back(vec.w);
            this->v_len = 4;
            break;
        }

        case mesh::type::textcoord: {
            this->t_list.push_back(vec.x);
            this->t_list.push_back(vec.y);
            this->t_list.push_back(vec.z);
            this->t_list.push_back(vec.w);
            this->t_len = 4;
            break;
        }

        case mesh::type::normal: {
            this->n_list.push_back(vec.x);
            this->n_list.push_back(vec.y);
            this->n_list.push_back(vec.z);
            this->n_list.push_back(vec.w);
            this->n_len = 4;
            break;
        }

        case mesh::type::color: {
            this->c_list.push_back(vec.x);
            this->c_list.push_back(vec.y);
            this->c_list.push_back(vec.z);
            this->c_list.push_back(vec.w);
            break;
        }
    }
}

void mesh::data::append(mesh::type type, const glm::vec3 &vec) {
    switch (type) {
        case mesh::type::vertex: {
            this->v_list.push_back(vec.x);
            this->v_list.push_back(vec.y);
            this->v_list.push_back(vec.z);
            this->v_len = 3;
            break;
        }

        case mesh::type::textcoord: {
            this->t_list.push_back(vec.x);
            this->t_list.push_back(vec.y);
            this->t_list.push_back(vec.z);
            this->t_len = 3;
            break;
        }

        case mesh::type::normal: {
            this->n_list.push_back(vec.x);
            this->n_list.push_back(vec.y);
            this->n_list.push_back(vec.z);
            this->n_len = 3;
            break;
        }

        case mesh::type::color: {
            this->c_list.push_back(vec.x);
            this->c_list.push_back(vec.y);
            this->c_list.push_back(vec.z);
            break;
        }
    }
}

void mesh::data::append(mesh::type type, const glm::vec2 &vec) {
    switch (type) {
        case mesh::type::vertex: {
            this->v_list.push_back(vec.x);
            this->v_list.push_back(vec.y);
            this->v_len = 2;
            break;
        }

        case mesh::type::textcoord: {
            this->t_list.push_back(vec.x);
            this->t_list.push_back(vec.y);
            this->t_len = 2;
            break;
        }

        case mesh::type::normal: {
            this->n_list.push_back(vec.x);
            this->n_list.push_back(vec.y);
            this->n_len = 2;
            break;
        }

        case mesh::type::color: {
            this->c_list.push_back(vec.x);
            this->c_list.push_back(vec.y);
            break;
        }
    }
}

void mesh::data::append(mesh::type type, float value) {
    switch (type) {
        case mesh::type::vertex: {
            this->v_list.push_back(value);
            this->v_len = 1;
            break;
        }

        case mesh::type::textcoord: {
            this->t_list.push_back(value);
            this->t_len = 1;
            break;
        }

        case mesh::type::normal: {
            this->n_list.push_back(value);
            this->n_len = 1;
            break;
        }

        case mesh::type::color: {
            this->c_list.push_back(value);
            break;
        }
    }
}

void mesh::data::append(mesh::type type, uint32_t index) {
    switch (type) {
        case mesh::type::vertex: {
            this->iv_list.push_back(index);
            break;
        }

        case mesh::type::textcoord: {
            this->it_list.push_back(index);
            break;
        }

        case mesh::type::normal: {
            this->in_list.push_back(index);
            break;
        }

        case mesh::type::color: {
            this->ic_list.push_back(index);
            break;
        }
    }
}

std::vector<uint32_t> &mesh::data::get_uint_list(mesh::type type) {
    switch (type) {
        case mesh::type::vertex: {
            return this->iv_list;
        }

        case mesh::type::textcoord: {
            return this->it_list;
        }

        case mesh::type::color: {
            return this->ic_list;
        }

        default: {
            break;
        }
    }

    return this->in_list;
}

std::vector<float> &mesh::data::get_float_list(mesh::type type) {
    switch (type) {
        case mesh::type::vertex: {
            return this->v_list;
        }

        case mesh::type::textcoord: {
            return this->t_list;
        }

        case mesh::type::color: {
            return this->c_list;
        }

        default: {
            break;
        }
    }

    return this->n_list;
}

bool mesh::data::contains(mesh::type type, bool indexing) {
    switch (type) {
        case mesh::type::vertex: {
            return indexing ? !this->iv_list.empty() : !this->v_list.empty();
        }

        case mesh::type::textcoord: {
            indexing ? !this->it_list.empty() : !this->t_list.empty();
        }

        case mesh::type::color: {
            return indexing ? !this->ic_list.empty() : !this->c_list.empty();
        }

        default: {
            break;
        }
    }

    return indexing ? !this->in_list.empty() : !this->n_list.empty();
}
