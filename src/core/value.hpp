#ifndef AGK_CORE_VALUE_H
#define AGK_CORE_VALUE_H

template<typename t>
class value {
protected:
    t value {};
    t min {};
    t max {};
public:
    inline void set_value(const t &val) {
        this->value = val;
    }

    inline t get_value() {
        return this->value;
    }

    inline void set_min(const t &val) {
        this->min = val;
    }

    inline t get_min() {
        return this->min;
    }

    inline void set_max(const t &val) {
        this->max = val;
    }

    inline t get_max() {
        return this->max;
    }
};

#endif

#ifndef AGK_CORE_VALUE_GLOBAL_H
#define AGK_CORE_VALUE_GLOBAL_H

class valueglobal {
public:
    /* Post-processing. */
    value<bool> enable_post_processing {};
    value<bool> enable_hdr {};
    value<float> hdr_exposure {};

    value<bool> enable_motion_blur {};
    value<float> motion_blur_intensity {};

    /* World rendering. */
    value<glm::vec2> fog_bounding {};
    value<glm::vec3> fog_color {};
    value<int32_t> maximum_buffers {};

    /* World update. */
    value<int32_t> chunk_generation_interval {};
    value<int32_t> chunk_dimension {};
    value<int32_t> chunk_generation_distance {};
    value<int32_t> chunk_terrain_height {};

    value<float> chunk_fbm_frequency {};
    value<float> chunk_fbm_amplitude {};
    value<float> chunk_fbm_persistence {};
    value<float> chunk_fbm_lacunarity {};
    value<int32_t> chunk_fbm_octaves {};

    void init() {
        this->fog_bounding.set_value({0.0f, 512.0f});
        this->fog_color.set_value({0.0f, 0.0f, 0.0f});
        this->chunk_terrain_height.set_value(256);
        this->maximum_buffers.set_value(10000);
    }
};

#endif