#include <iostream>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/impl.h>
#include <nlohmann/json.hpp>
#include <inja.hpp>

using json = nlohmann::json;
using namespace inja;

nlohmann::json YAMLtoJSON(const YAML::Node &node) {
    int i = 0;
    nlohmann::json data;

    switch (node.Type()) {
        case YAML::NodeType::Null: // ...
            data = nullptr;
            break;
        case YAML::NodeType::Scalar: // ...
            try {
                data = node.as<bool>();
            }
            catch (YAML::BadConversion &x) { // YAML::TypedBadConversion<bool>
                try {
                    data = node.as<int>();
                }
                catch (YAML::BadConversion &x) { // YAML::TypedBadConversion<bool>
                    try {
                        data = node.as<double>();
                    }
                    catch (YAML::BadConversion &x) { // YAML::TypedBadConversion<bool>
                        data = node.as<std::string>();
                    }
                }
            }
        std::cout << "Value: " << data << std::endl;
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

    return data;
}

int main() {

    YAML::Node yamlSchema = YAML::LoadFile("../migration01.yaml");
    assert(yamlSchema.IsDefined()); // TODO: add proper error handling
    assert(yamlSchema.IsMap()); // TODO: add proper error handling
    nlohmann::json schema = YAMLtoJSON(yamlSchema);

    YAML::Node yamlMap = YAML::LoadFile("../map.yaml");
    assert(yamlMap.IsDefined()); // TODO: add proper error handling
    assert(yamlMap.IsMap()); // TODO: add proper error handling
    schema["map"] = YAMLtoJSON(yamlMap);

    std::cout << "schema:" << schema.dump(4) << std::endl;

    Environment env = Environment("../");

    env.add_callback("map", 2, [&env](Parsed::Arguments args, json x) {
        std::string map = env.get_argument<std::string>(args, 0, x);
        std::string key = env.get_argument<std::string>(args, 1, x);

        std::string r = "Map key not found.";
        try {
            r = x.at("map").at(map).at(key).get<std::string>();
        }
        catch (...) { ; // do nothing
        }
        return r;
    });

    std::string result = env.render_file("template01.inja", schema);
    std::cout << result << std::endl;

    return 0;
}