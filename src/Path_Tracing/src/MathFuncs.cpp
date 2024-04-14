#include "MathFuncs.h"

std::string arrayElementName(const char* arrayName, int index, const char* keyName) {
    return std::string(arrayName).append("[").append(std::to_string(index)).append("].").append(keyName);
}
