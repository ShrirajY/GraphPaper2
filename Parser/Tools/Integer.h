#ifndef INTEGER_H
#define INTEGER_H

#include <iostream>
#include <string>
#include <unordered_map>

class Integer {
public:
    Integer(int value) : value_(value) {}
    int getValue() const { return value_; }
private:
    int value_;
};

class IntegerSymbolTables{
    public:

        Integer getInteger(const std::string& name) const {
            auto it = integerMap.find(name);
            if (it != integerMap.end()) {
                return it->second;
            }
            throw std::invalid_argument("Integer not found");
        }

        void addInteger(const std::string& name, const Integer& integer) {
            this->integerMap.emplace(name, integer);
        }
        void clear() {
            this->integerMap.clear();
        }
    private:
        std::unordered_map<std::string, Integer> integerMap;
};

IntegerSymbolTables globalIntegerSymbolTable;

#endif // INTEGER_H