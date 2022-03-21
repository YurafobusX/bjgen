#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <vector>
#include <stdexcept>

//Вектор, хранящий замену для всех статических маркеров
std::vector<std::string> staticMarks = {"test"};

//Принимает на вход строку и возращает копию, заменяя все маркеры (\[[S,D]number\]) на их значение
std::string replace(std::stringstream& in) {
    std::stringstream res;

    char current;

    while(in >> current) {
        if (current =='[') {
            in >> current;
            if (current == 'S') {
                int number;
                //TODO Может закончится файл
                in >> number;
                res << staticMarks[number - 1];
            } else {
                //TODO Динамические маркеры
            }
            in >> current;
            if (current != ']') throw std::runtime_error("");
        } else res << current;
    };
    return res.str();
};

int main(int argc, char* argv[]) {

    std::stringstream file = std::stringstream(std::string((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>()));

    std::cout << replace(file);

    return 0;
};