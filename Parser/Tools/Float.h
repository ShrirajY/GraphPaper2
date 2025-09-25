#ifndef FLOAT_H
#define FLOAT_H

#include <iostream>
#include <string>
#include <unordered_map>

class Float {
public:
    Float(float value) : value_(value) {}
    float getValue() const { return value_; }
private:
    float value_;
};

class FloatSymbolTables{
    public:

        Float getFloat(const std::string& name) const {
            auto it = floatMap.find(name);
            if (it != floatMap.end()) {
                return it->second;
            }
            throw std::invalid_argument("Float not found");
        }

        void addFloat(const std::string& name, const Float& floatVal) {
            this->floatMap.emplace(name, floatVal);
        }

        void clear() {
            this->floatMap.clear();
        }

    private:
        std::unordered_map<std::string, Float> floatMap;
};

FloatSymbolTables globalFloatSymbolTable;

#endif // FLOAT_H