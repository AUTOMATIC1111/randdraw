#include "Program.h"

#include <map>
#include <algorithm>
#include <cctype>
#include <locale>

std::map<std::string, const ShapeInfo *> shapesMap;

std::vector<std::string> split(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos) {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    strings.push_back(str.substr(prev));

    return strings;
}

static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));

    return s;
}

static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

Program::Program(std::string line) {
    currentInstruction = 0;
    currentIteration = 0;
    totalIterationsCount = 0;

    if (shapesMap.empty()) {
        for (const ShapeInfo &i: shapes) {
            shapesMap[i.name] = &i;
        }
    }

    for (std::string command: split(line, ",")) {
        std::vector<std::string> args = split(trim(command), ":");
        if (args.size() < 2) continue;

        std::string shapeName = args[0];
        std::string iterationsText = args[1];

        Instruction instruction;

        trim(shapeName);
        int iterations = std::stol(trim(iterationsText));

        auto iter = shapesMap.find(shapeName);
        if (iter == shapesMap.end()) continue;

        const ShapeInfo *info = iter->second;

        instruction.shape = info;
        instruction.iterations = iterations;
        instruction.scale = 1.0;

        for(int i=2;i<args.size();i++){
            std::vector<std::string> kv = split(trim(args[i]), "=");
            if (kv.size() != 2) continue;

            std::string k = trim(kv[0]);
            std::string v = trim(kv[1]);

            if(k=="scale"){
                instruction.scale = atof(v.c_str());
            }
        }

        instructions.push_back(instruction);
        totalIterationsCount += iterations;
    }
}

void Program::get(const Instruction *&instruction, int &iterations) {
    if (currentInstruction == instructions.size()) {
        iterations = 0;
        return;
    }

    instruction = &instructions[currentInstruction];
    iterations = instructions[currentInstruction].iterations - currentIteration;
}

void Program::advance(int iterations) {
    if (currentInstruction == instructions.size()) {
        return;
    }

    int remaining = instructions[currentInstruction].iterations - currentIteration;
    if (remaining > iterations) {
        currentIteration += iterations;
        return;
    }

    currentInstruction++;
    currentIteration = 0;

    advance(iterations - remaining);
}

int Program::totalIterations() {
    return totalIterationsCount;
}
