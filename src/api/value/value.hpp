#ifndef AGK_API_VALUE_H
#define AGK_API_VALUE_H

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

#ifndef AGK_API_VALUE_GLOBAL_H
#define AGK_API_VALUE_GLOBAL_H

class value_global {
};

#endif