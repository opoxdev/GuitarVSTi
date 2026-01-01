#include "gnpreset/PresetEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: presetgen <template_dir> <out_file> [count]\n";
        return 0;
    }

    const fs::path templateDir{argv[1]};
    const fs::path outFile{argv[2]};
    const int count = (argc > 3) ? std::atoi(argv[3]) : 16;

    std::vector<std::string> templates;
    for (const auto& entry : fs::directory_iterator(templateDir)) {
        if (entry.path().extension() == ".json") {
            std::ifstream in(entry.path());
            templates.emplace_back(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
        }
    }
    if (templates.empty()) {
        std::cerr << "No templates found\n";
        return 1;
    }

    std::mt19937 rng{1337u};
    std::uniform_int_distribution<size_t> pick(0, templates.size() - 1);

    std::ofstream out(outFile);
    out << "{\n  \"presets\": [\n";
    for (int i = 0; i < count; ++i) {
        std::string presetText = templates[pick(rng)];
        if (i > 0) out << ",\n";
        out << "    " << presetText;
    }
    out << "\n  ]\n}\n";
    std::cout << "Generated " << count << " presets into " << outFile << "\n";
    return 0;
}
