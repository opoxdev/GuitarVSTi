#pragma once

#include "Json.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace gnpreset {

struct ParamSnapshot {
    std::map<std::string, float> values;
};

struct PresetMeta {
    std::string name;
    std::vector<std::string> tags;
};

struct PresetDefinition {
    PresetMeta meta;
    ParamSnapshot snapshot;
};

class PresetDatabase {
public:
    bool loadBankFromString(const std::string& jsonText);
    const std::vector<PresetDefinition>& getPresets() const { return presets; }
    std::optional<PresetDefinition> findByName(const std::string& name) const;
    ParamSnapshot makeSnapshot(size_t index) const;

private:
    std::vector<PresetDefinition> presets;
};

} // namespace gnpreset
