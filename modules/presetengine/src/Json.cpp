#include "gnpreset/Json.h"
#include <cctype>

namespace gnpreset {

namespace {

struct Cursor {
    const std::string& text;
    size_t pos{0};

    explicit Cursor(const std::string& t) : text(t) {}

    char peek() const { return pos < text.size() ? text[pos] : '\0'; }
    char get() { return pos < text.size() ? text[pos++] : '\0'; }
    void skipWhitespace() {
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos;
    }
};

bool parseValue(Cursor& cur, JsonValue& out);

bool parseLiteral(Cursor& cur, const char* literal) {
    for (size_t i = 0; literal[i]; ++i) {
        if (cur.get() != literal[i]) return false;
    }
    return true;
}

bool parseString(Cursor& cur, std::string& out) {
    if (cur.get() != '"') return false;
    out.clear();
    while (true) {
        char c = cur.get();
        if (c == '\0') return false;
        if (c == '"') break;
        if (c == '\\') {
            char esc = cur.get();
            switch (esc) {
            case '"': out.push_back('"'); break;
            case '\\': out.push_back('\\'); break;
            case '/': out.push_back('/'); break;
            case 'b': out.push_back('\b'); break;
            case 'f': out.push_back('\f'); break;
            case 'n': out.push_back('\n'); break;
            case 'r': out.push_back('\r'); break;
            case 't': out.push_back('\t'); break;
            default: return false;
            }
        } else {
            out.push_back(c);
        }
    }
    return true;
}

bool parseNumber(Cursor& cur, double& out) {
    size_t start = cur.pos;
    while (std::isdigit(static_cast<unsigned char>(cur.peek())) || cur.peek() == '-' || cur.peek() == '+' || cur.peek() == '.' || cur.peek() == 'e' || cur.peek() == 'E') {
        cur.get();
    }
    const std::string substr = cur.text.substr(start, cur.pos - start);
    try {
        out = std::stod(substr);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseArray(Cursor& cur, JsonArray& out) {
    if (cur.get() != '[') return false;
    cur.skipWhitespace();
    out.clear();
    if (cur.peek() == ']') { cur.get(); return true; }
    while (true) {
        JsonValue v;
        if (!parseValue(cur, v)) return false;
        out.push_back(v);
        cur.skipWhitespace();
        char c = cur.get();
        if (c == ']') break;
        if (c != ',') return false;
        cur.skipWhitespace();
    }
    return true;
}

bool parseObject(Cursor& cur, JsonObject& out) {
    if (cur.get() != '{') return false;
    cur.skipWhitespace();
    out.clear();
    if (cur.peek() == '}') { cur.get(); return true; }
    while (true) {
        std::string key;
        if (!parseString(cur, key)) return false;
        cur.skipWhitespace();
        if (cur.get() != ':') return false;
        cur.skipWhitespace();
        JsonValue val;
        if (!parseValue(cur, val)) return false;
        out.emplace(std::move(key), std::move(val));
        cur.skipWhitespace();
        char c = cur.get();
        if (c == '}') break;
        if (c != ',') return false;
        cur.skipWhitespace();
    }
    return true;
}

bool parseValue(Cursor& cur, JsonValue& out) {
    cur.skipWhitespace();
    char c = cur.peek();
    if (c == '"') {
        std::string s;
        if (!parseString(cur, s)) return false;
        out.type = JsonValue::Type::String;
        out.data = std::move(s);
        return true;
    }
    if (c == '{') {
        JsonObject obj;
        if (!parseObject(cur, obj)) return false;
        out.type = JsonValue::Type::Object;
        out.data = std::move(obj);
        return true;
    }
    if (c == '[') {
        JsonArray arr;
        if (!parseArray(cur, arr)) return false;
        out.type = JsonValue::Type::Array;
        out.data = std::move(arr);
        return true;
    }
    if (std::isdigit(static_cast<unsigned char>(c)) || c == '-') {
        double num{0.0};
        if (!parseNumber(cur, num)) return false;
        out.type = JsonValue::Type::Number;
        out.data = num;
        return true;
    }
    if (c == 't') {
        if (!parseLiteral(cur, "true")) return false;
        out.type = JsonValue::Type::Bool;
        out.data = true;
        return true;
    }
    if (c == 'f') {
        if (!parseLiteral(cur, "false")) return false;
        out.type = JsonValue::Type::Bool;
        out.data = false;
        return true;
    }
    if (c == 'n') {
        if (!parseLiteral(cur, "null")) return false;
        out.type = JsonValue::Type::Null;
        out.data = std::monostate{};
        return true;
    }
    return false;
}

} // namespace

bool parseJson(const std::string& text, JsonValue& outValue) {
    Cursor cur{text};
    const bool ok = parseValue(cur, outValue);
    return ok;
}

} // namespace gnpreset
