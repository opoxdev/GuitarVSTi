#include "gnpreset/PresetEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: bankpacker <input_json> <output_bin>\n";
        return 0;
    }
    std::ifstream in(argv[1]);
    if (!in.good()) {
        std::cerr << "Unable to open input" << std::endl;
        return 1;
    }
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    gnpreset::PresetDatabase db;
    if (!db.loadBankFromString(text)) {
        std::cerr << "Failed to parse bank" << std::endl;
        return 1;
    }

    std::ofstream out(argv[2], std::ios::binary);
    uint32_t count = static_cast<uint32_t>(db.getPresets().size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (const auto& preset : db.getPresets()) {
        uint32_t nameLen = static_cast<uint32_t>(preset.meta.name.size());
        out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        out.write(preset.meta.name.data(), nameLen);
    }
    std::cout << "Packed " << count << " presets into " << argv[2] << "\n";
    return 0;
}
