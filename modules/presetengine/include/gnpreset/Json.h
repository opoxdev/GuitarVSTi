#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace gnpreset {

struct JsonValue;
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

struct JsonValue {
    enum class Type { Null, Bool, Number, String, Object, Array };

    Type type{Type::Null};
    std::variant<std::monostate, bool, double, std::string, JsonObject, JsonArray> data{};

    bool isObject() const { return type == Type::Object; }
    bool isArray() const { return type == Type::Array; }
    bool isString() const { return type == Type::String; }
    bool isNumber() const { return type == Type::Number; }

    const JsonObject& asObject() const { return std::get<JsonObject>(data); }
    const JsonArray& asArray() const { return std::get<JsonArray>(data); }
    const std::string& asString() const { return std::get<std::string>(data); }
    double asNumber() const { return std::get<double>(data); }
};

bool parseJson(const std::string& text, JsonValue& outValue);

} // namespace gnpreset
