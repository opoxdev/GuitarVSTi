#include "gnpreset/PresetEngine.h"
#include <fstream>
#include <sstream>

namespace gnpreset {

namespace {
float getNumberOr(const JsonObject& obj, const std::string& key, float def) {
    auto it = obj.find(key);
    if (it == obj.end()) return def;
    if (it->second.isNumber()) return static_cast<float>(it->second.asNumber());
    return def;
}

std::vector<std::string> getStringArray(const JsonObject& obj, const std::string& key) {
    std::vector<std::string> result;
    auto it = obj.find(key);
    if (it == obj.end() || !it->second.isArray()) return result;
    for (const auto& v : it->second.asArray()) {
        if (v.isString()) result.push_back(v.asString());
    }
    return result;
}

} // namespace

bool PresetDatabase::loadBankFromString(const std::string& jsonText) {
    JsonValue root;
    if (!parseJson(jsonText, root) || !root.isObject()) return false;
    const auto& obj = root.asObject();
    auto it = obj.find("presets");
    if (it == obj.end() || !it->second.isArray()) return false;

    presets.clear();
    for (const auto& presetVal : it->second.asArray()) {
        if (!presetVal.isObject()) continue;
        const auto& pobj = presetVal.asObject();
        PresetDefinition def;
        auto nameIt = pobj.find("name");
        if (nameIt != pobj.end() && nameIt->second.isString()) {
            def.meta.name = nameIt->second.asString();
        }
        def.meta.tags = getStringArray(pobj, "tags");
        auto paramIt = pobj.find("parameters");
        if (paramIt != pobj.end() && paramIt->second.isObject()) {
            for (const auto& kv : paramIt->second.asObject()) {
                if (kv.second.isNumber()) {
                    def.snapshot.values[kv.first] = static_cast<float>(kv.second.asNumber());
                }
            }
        }
        presets.push_back(def);
    }
    return true;
}

std::optional<PresetDefinition> PresetDatabase::findByName(const std::string& name) const {
    for (const auto& p : presets) {
        if (p.meta.name == name) return p;
    }
    return std::nullopt;
}

ParamSnapshot PresetDatabase::makeSnapshot(size_t index) const {
    if (index >= presets.size()) return {};
    return presets[index].snapshot;
}

} // namespace gnpreset
