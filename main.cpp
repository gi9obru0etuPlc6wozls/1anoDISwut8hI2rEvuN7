#include <iostream>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/impl.h>
#include <nlohmann/json.hpp>
#include <inja.hpp>

using json = nlohmann::json;
using namespace inja;


std::string snakeToCamel(const std::string &snake, const bool initCap = false) {
    std::string r;

    bool underscore = false;
    for (int i = 0; i < snake.length(); ++i) {

        char c = snake[i];

        if (i == 0 && initCap)
            c = toupper(c);

        if (underscore)
            c = toupper(c);

        underscore = false;

        if (c == '_')
            underscore = true;
        else
            r += c;
    }
    return r;
}

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
            catch (YAML::BadConversion &x) {
                try {
                    data = node.as<int>();
                }
                catch (YAML::BadConversion &x) {
                    try {
                        data = node.as<double>();
                    }
                    catch (YAML::BadConversion &x) {
                        data = node.as<std::string>();
                    }
                }
            }
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

    std::string s = "this_is_snake_case_";
    std::cout << "snakeToCamel:" << snakeToCamel(s) << std::endl;
    std::cout << "SnakeToCamel:" << snakeToCamel(s, true) << std::endl;


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

        std::string r = key;
        try {
            r = x.at("map").at(map).at(key).get<std::string>();
        }
        catch (...) { ; // do nothing
        }
        return r;
    });

    env.add_callback("lCamel", 1, [&env](Parsed::Arguments args, json x) {
        return snakeToCamel(env.get_argument<std::string>(args, 0, x));
    });

    std::string result = env.render_file("template01.inja", schema);
    std::cout << result << std::endl;

    return 0;
}