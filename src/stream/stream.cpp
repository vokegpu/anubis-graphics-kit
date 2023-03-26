#include "stream.hpp"

std::string stream::i(int32_t value) {
    return std::to_string(value);
}

std::string stream::f(float value) {
    return std::to_string(value);
}

std::string stream::vec(float x, float y, float z, float w) {
    std::string output {std::to_string(x)};
    output += ' ';
    output += std::to_string(y);
    output += ' ';
    output += std::to_string(z);
    output += ' ';
    output += std::to_string(w);
    return output;
}

std::string stream::vec(float x, float y, float z) {
    std::string output {std::to_string(x)};
    output += ' ';
    output += std::to_string(y);
    output += ' ';
    output += std::to_string(z);
    return output;
}

std::string stream::vec(float x, float y) {
    std::string output {std::to_string(x)};
    output += ' ';
    output += std::to_string(y);
    return output;
}

std::map<std::string, std::map<std::string, std::string>> &stream::serializer::get_metadata() {
    return this->metadata;
}

void stream::mtl::set_serializer(const stream::serializer &_serializer) {
    this->serializer = _serializer;
}

stream::serializer &stream::mtl::get_serializer() {
    return this->serializer;
}