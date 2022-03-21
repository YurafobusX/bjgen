#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <fstream>

//Вектор, хранящий замену для всех статических маркеров
std::vector<std::string> staticMarks = {"test"};

//Принимает на вход строку и пишет её в выводной поток, заменяя все маркеры (\[[S,D]number\]) на их значение
int replace(std::stringstream& in, std::ostream& output = std::cout) {
    char current;

    while(in >> current) {
        if (current =='[') {
            in >> current;
            if (current == 'S') {
                int number;
                //TODO Может закончится файл
                in >> number;
                output << staticMarks[number - 1];
            } else {
                //TODO Динамические маркеры
            }
            in >> current;
            if (current != ']') throw std::runtime_error("");
        } else output << current;
    };
    return 0;
};

int main(int argc, char* argv[]) {
    std::istream* input = &std::cin;
    std::ostream* output = &std::cout; 

    std::ifstream finput("../resources/example.html", std::ios::in);
    if (!finput.good()) 
        return -1;
    input = &finput;

    std::ofstream foutput("../resources/result.html", std::ios::out);
    output = &foutput;

    std::stringstream file(std::string((std::istreambuf_iterator<char>(*input)), std::istreambuf_iterator<char>()));

    replace(file, *output);

    return 0;
};