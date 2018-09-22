#include <iostream>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <inja.hpp>

static int level = 0;

void debugYAML(YAML::Node node) {
    level++;
    std::cout << std::string(level, '-') << "debugYAML..." << std::endl;

    switch (node.Type()) {
        case YAML::NodeType::Null: // ...
            std::cout << std::string(level, '-') << "Null" << std::endl;
            break;
        case YAML::NodeType::Scalar: // ...
            std::cout << std::string(level, '-') << "Scalar Value: " << node.as<std::string>() << ":" << std::endl;
            break;
        case YAML::NodeType::Sequence: // ...
            std::cout << std::string(level, '-') << "Sequence Start" << std::endl;
            for (YAML::iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                debugYAML(*n_it);
            }
            std::cout << std::string(level, '-') << "Sequence End" << std::endl;
            break;
        case YAML::NodeType::Map: // ...
            std::cout << std::string(level, '-') << "Map Start" << std::endl;
            for (YAML::iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                std::cout << "First: " << n_it->first.as<std::string>() << std::endl;
                debugYAML(n_it->second);
            }
            std::cout << std::string(level, '-') << "Map End" << std::endl;
            break;
        case YAML::NodeType::Undefined: // ...
            std::cout << std::string(level, '-') << "Undefined" << std::endl;
            break;
    }
    level--;
}

void YAMLtoJSON(YAML::Node node) {
    nlohmann::json data;

    level++;
    std::cout << std::string(level, '-') << "YAMLtoJSON..." << std::endl;

    switch (node.Type()) {
        case YAML::NodeType::Null: // ...
            std::cout << std::string(level, '-') << "Null" << std::endl;
            break;
        case YAML::NodeType::Scalar: // ...
            std::cout << std::string(level, '-') << "Scalar Value: " << node.as<std::string>() << ":" << std::endl;
            break;
        case YAML::NodeType::Sequence: // ...
            std::cout << std::string(level, '-') << "Sequence Start" << std::endl;
            for (YAML::iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                debugYAML(*n_it);
            }
            std::cout << std::string(level, '-') << "Sequence End" << std::endl;
            break;
        case YAML::NodeType::Map: // ...
            std::cout << std::string(level, '-') << "Map Start" << std::endl;
            for (YAML::iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                std::cout << "First: " << n_it->first.as<std::string>() << std::endl;
                debugYAML(n_it->second);
            }
            std::cout << std::string(level, '-') << "Map End" << std::endl;
            break;
        case YAML::NodeType::Undefined: // ...
            std::cout << std::string(level, '-') << "Undefined" << std::endl;
            break;
    }
    level--;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    YAML::Node schemaNodes = YAML::LoadFile("../migration01.yaml");
    assert(schemaNodes.IsDefined()); // TODO: add proper error handling
    assert(schemaNodes.IsMap()); // TODO: add proper error handling

    YAMLtoJSON(schemaNodes);



    return 0;
}