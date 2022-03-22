#include <cstddef>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <vector>
#include <fstream>
#include "OpenXLSX.hpp"

//Вектор, хранящий замену для всех статических маркеров
std::vector<std::string> staticMarks = {"test"};

//Вектор, хранящий замену для всех динамических маркеров
std::vector<std::vector<std::string>> dynamicMarks;

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
                output << staticMarks[number];
            } else if (current == 'D') {
                output << dynamicMarks[count][number];
            } else return 2;
            in >> current;
            if (current != ']') return 2;
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
        return 1;
    input = &finput;
    *input >> std::noskipws;

    std::ofstream foutput("../resources/result.html", std::ios::out);
    if (!foutput.good()) 
        return 1;
    output = &foutput;

    OpenXLSX::XLDocument doc("../resources/test.xlsx");
    if (doc.name() == "")
        return 1;
    auto wk = doc.workbook().worksheet("1");
    size_t latter = 0;
    while(wk.cell(latter + 1, 1).value().type() != OpenXLSX::XLValueType::Empty) {
        dynamicMarks.push_back(std::vector<std::string>());
        std::string current;
        int number = 1;
        while (wk.cell(latter + 1, number).value().type() != OpenXLSX::XLValueType::Empty) {
            std::cerr << number << '\n';
            std::string current = wk.cell(latter + 1, number).value().get<std::string>();
            dynamicMarks[latter].push_back(std::move(current));
            ++number;
        }
        ++latter;
    }
    doc.close();


    std::string file((std::istreambuf_iterator<char>(*input)), std::istreambuf_iterator<char>());

    for (count = 0; count < dynamicMarks.size(); ++count) {
        auto j = std::stringstream(file);
        j << std::noskipws;
        if (replace(j, *output)) return 2;
    }

    return 0;
};