#include "stream.hpp"

std::map<std::string, std::map<std::string, std::string>> &stream::serializer::get_metadata() {
    return this->metadata;
}

void stream::serializer::set_serialiazer(const stream::serialiazer &_serialiazer) {
    this->serialiazer = _serialiazer;
}

stream::serializer &stream::mtl::get_serializer() {
    return this->serializer;
}