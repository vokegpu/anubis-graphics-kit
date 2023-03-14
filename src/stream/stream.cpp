#include "stream.hpp"

std::map<std::string, std::map<std::string, std::string>> &stream::serializer::get_metadata() {
    return this->metadata;
}

void stream::mtl::set_serializer(const stream::serializer &_serializer) {
    this->serializer = _serializer;
}

stream::serializer &stream::mtl::get_serializer() {
    return this->serializer;
}