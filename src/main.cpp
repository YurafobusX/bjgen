#include <cstddef>
#include <ios>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <vector>
#include <fstream>
#include <map>
#include "OpenXLSX.hpp"

#define LOG(__str__) if (isLogEnable) {std::clog << __str__;}

//Вектор, хранящий замену для всех динамических маркеров
std::vector<std::vector<std::string>> dynamicMarks;

//Индекс замены для динамических маркеров
size_t count = 0;

//Включение логов
bool isLogEnable = true;

//Функция считывания данных из конфига
int config(const std::map<std::string, std::string&>& map, const std::string& configPath) {
    LOG("Чтение конфигурационного файла\n")
    std::ifstream config(configPath, std::ios::in);
    if (!config.good()) {
        std::cerr << "Не удалось открыть файл \"" << configPath << "\"\n"; 
        return 1;
    }
    std::string key;
    while (std::getline(config, key, '=')) {
        if (key[0] == '#') {
            std::getline(config, key);
            LOG("Пропуск строки\n")
            continue;
        }
        try {
            std::string& value = map.at(key);
            std::getline(config, value);
            LOG("Запись \"" + value + "\" в \"" + key + "\"\n")
        } catch (std::out_of_range) {
            std::cerr << "Введён неверный параметр конфигурации \"" << key << "\"\n";
            return 1; 
        };
    }
    LOG("Конец чтения конфигурационного файла\n")
    return 0;
};


//Принимает на вход строку и пишет её в выводной поток, заменяя все маркеры (\[[S,D]number\]) на их значение
int replace(std::istream& in, std::ostream& output = std::cout) {
    LOG("Начало " + std::to_string(count + 1) + " прохода\n")
    std::string current;
    while(std::getline(in, current, '[')) {
        if (in.eof())
            continue;
        LOG("Найдена \"[\"...")
        std::getline(in, current, ']');
        if (in.eof()) {
            std::cerr << "\nНе найдена закрывающая \"]\" до конца файла\n";
            return 1;
        }
        LOG("Найдена \"]\"...")
        try {
            int number = std::stoi(current);
            LOG("Проверка на число \"" + current + "\"...")
            output << dynamicMarks[count][number];
            LOG("Запись динамической марки \"" + dynamicMarks[count][number] + "\"\n")
        } catch (std::invalid_argument) {
            std::cerr << "[" << current << "]" << " не подходит под формат метки";
            return 1;
        }
    };
    //in.seekg(0);
    return 0;
};

int main(int argc, char* argv[]) {
    std::string inputPath, outputPath, docPath, isLogEnableStr, configPath;
    std::map<std::string, std::string&> map = {
        {"input", inputPath},
        {"output", outputPath},
        {"xlsxData", docPath},
        {"log", isLogEnableStr}
    };
    if (isLogEnableStr == "false") {isLogEnable = false;}
    if (argc < 2) {
        std::cout << "Введите название конфигурационного файла:\n";
        std::cin >> configPath;
    } else {
        configPath = argv[1];
    }

    config(map, configPath);

    std::istream* input = &std::cin;
    std::ostream* output = &std::cout; 

    std::ifstream finput(inputPath, std::ios::in);
    if (!finput.good()) 
        return 1;
    input = &finput;
    *input >> std::noskipws;

    std::ofstream foutput(outputPath, std::ios::out);
    if (!foutput.good()) 
        return 1;
    output = &foutput;

    OpenXLSX::XLDocument doc(docPath);
    if (doc.name() == "")
        return 1;
    auto wk = doc.workbook().worksheet("1");
    size_t latter = 0;
    while(wk.cell(latter + 1, 1).value().type() != OpenXLSX::XLValueType::Empty) {
        dynamicMarks.push_back(std::vector<std::string>());
        std::string current;
        int number = 1;
        while (wk.cell(latter + 1, number).value().type() != OpenXLSX::XLValueType::Empty) {
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