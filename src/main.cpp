#include <cstddef>
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

//Вектор, хранящий замену для всех динамических маркеров
std::vector<std::vector<std::string>> dynamicMarks = {
    {"1"},
    {"2"}
};

//Индекс замены для динамических маркеров
size_t count = 0;

//Принимает на вход строку и пишет её в выводной поток, заменяя все маркеры (\[[S,D]number\]) на их значение
int replace(std::istream& in, std::ostream& output = std::cout) {
    char current;

    while(in >> current) {
        if (current =='[') {
            in >> current;
            //TODO Может закончится файл
            size_t number;
            in >> number;
            if (current == 'S') {
                output << staticMarks[number - 1];
            } else if (current == 'D') {
                output << dynamicMarks[count][number - 1];
            } else return -1;
            in >> current;
            if (current != ']') return -1;
        } else output << current;
    };
    //in.seekg(0);
    return 0;
};

int main(int argc, char* argv[]) {
    std::istream* input = &std::cin;
    std::ostream* output = &std::cout; 

    std::ifstream finput("../resources/example.html", std::ios::in);
    if (!finput.good()) 
        return -1;
    input = &finput;
    *input >> std::noskipws;

    std::ofstream foutput("../resources/result.html", std::ios::out);
    if (!foutput.good()) 
        return -1;
    output = &foutput;

    std::string file((std::istreambuf_iterator<char>(*input)), std::istreambuf_iterator<char>());

    for (count = 0; count < dynamicMarks.size(); ++count) {
        auto j = std::stringstream(file);
        j << std::noskipws;
        if (replace(j, *output)) return -1;
    }

    return 0;
};