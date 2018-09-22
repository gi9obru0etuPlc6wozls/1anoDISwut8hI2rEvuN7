#include <iostream>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <inja.hpp>

using json = nlohmann::json;
using namespace inja;

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

nlohmann::json YAMLtoJSON(const YAML::Node &node) {
    int i = 0;
    nlohmann::json data;

    switch (node.Type()) {
        case YAML::NodeType::Null: // ...
            data = nullptr;
            break;
        case YAML::NodeType::Scalar: // ...
            data = node.as<std::string>();
            break;
        case YAML::NodeType::Sequence: // ...
            for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                data[i] = YAMLtoJSON(*n_it);
                ++i;
            }
            break;
        case YAML::NodeType::Map: // ...
            for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                data[n_it->first.as<std::string>()] = YAMLtoJSON(n_it->second);
            }
            break;
        case YAML::NodeType::Undefined: // ...
            break;
    }
    level--;
    return data;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::string result;

    YAML::Node schemaNodes = YAML::LoadFile("../migration01.yaml");
    assert(schemaNodes.IsDefined()); // TODO: add proper error handling
    assert(schemaNodes.IsMap()); // TODO: add proper error handling

    nlohmann::json data;
    data = YAMLtoJSON(schemaNodes);

    std::cout << data.dump(4) << std::endl;

    Environment env = Environment("../");
    Template temp = env.parse_template("../template01.inja");

    result = env.render_file("template01.inja", data);
    std::cout << result << std::endl;

    return 0;
}