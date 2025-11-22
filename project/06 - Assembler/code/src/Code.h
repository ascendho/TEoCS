#ifndef CODE_H
#define CODE_H

#include <string>
#include <unordered_map>

class Code {
private:
    std::unordered_map<std::string, std::string> destMap;
    std::unordered_map<std::string, std::string> compMap;
    std::unordered_map<std::string, std::string> jumpMap;

public:
    Code() {
        // dest 助记符
        destMap[""] = "000";
        destMap["M"] = "001";
        destMap["D"] = "010";
        destMap["MD"] = "011";
        destMap["A"] = "100";
        destMap["AM"] = "101";
        destMap["AD"] = "110";
        destMap["AMD"] = "111";

        // comp 助记符 (a=0)
        compMap["0"] = "0101010";
        compMap["1"] = "0111111";
        compMap["-1"] = "0111010";
        compMap["D"] = "0001100";
        compMap["A"] = "0110000";
        compMap["!D"] = "0001101";
        compMap["!A"] = "0110001";
        compMap["-D"] = "0001111";
        compMap["-A"] = "0110011";
        compMap["D+1"] = "0011111";
        compMap["A+1"] = "0110111";
        compMap["D-1"] = "0001110";
        compMap["A-1"] = "0110010";
        compMap["D+A"] = "0000010";
        compMap["D-A"] = "0010011";
        compMap["A-D"] = "0000111";
        compMap["D&A"] = "0000000";
        compMap["D|A"] = "0010101";

        // comp 助记符 (a=1, 用 M 代替 A)
        compMap["M"] = "1110000";
        compMap["!M"] = "1110001";
        compMap["-M"] = "1110011";
        compMap["M+1"] = "1110111";
        compMap["M-1"] = "1110010";
        compMap["D+M"] = "1000010";
        compMap["D-M"] = "1010011";
        compMap["M-D"] = "1000111";
        compMap["D&M"] = "1000000";
        compMap["D|M"] = "1010101";

        // jump 助记符
        jumpMap[""] = "000";
        jumpMap["JGT"] = "001";
        jumpMap["JEQ"] = "010";
        jumpMap["JGE"] = "011";
        jumpMap["JLT"] = "100";
        jumpMap["JNE"] = "101";
        jumpMap["JLE"] = "110";
        jumpMap["JMP"] = "111";
    }

    std::string dest(const std::string& mnemonic) const {
        if (destMap.find(mnemonic) != destMap.end()) {
            return destMap.at(mnemonic);
        }
        return "000";
    }

    std::string comp(const std::string& mnemonic) const {
        if (compMap.find(mnemonic) != compMap.end()) {
            return compMap.at(mnemonic);
        }
        return "0000000";
    }

    std::string jump(const std::string& mnemonic) const {
        if (jumpMap.find(mnemonic) != jumpMap.end()) {
            return jumpMap.at(mnemonic);
        }
        return "000";
    }
};

#endif
