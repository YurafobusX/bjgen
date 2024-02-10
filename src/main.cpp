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
//#include <format> Терпим до 23 стандарта
//#include <filesystem>
//Надо протестировать насколько хорошо она с кириллицей работает
#include "OpenXLSX.hpp"

//Убрать, ужасно, сделать inline функцией
#define LOG(__str__) if (isLogEnable) {std::wclog << __str__;}

//Вектор, хранящий замену для всех динамических маркеров
std::vector<std::vector<std::string>> dynamicMarks;

//Индекс замены для динамических маркеров
size_t count = 0;

//Включение логов
bool isLogEnable = true;

//Функция считывания данных из конфига
int config(const std::map<std::string, std::string&>& map, const std::string& configPath) {
    LOG(L"Чтение конфигурационного файла\n")
    std::ifstream config(configPath, std::ios::in);
    if (!config.good()) {
        std::wcerr << L"Не удалось открыть файл \"";
        std::cerr << configPath << "\"\n"; 
        return 1;
    }
    std::string key;
    while (std::getline(config, key, '=')) {
        if (key[0] == '#') {
            std::getline(config, key);
            LOG(L"Пропуск строки\n")
            continue;
        }
        try {
            std::string& value = map.at(key);
            std::getline(config, value);
            //LOG(L"Запись \"") LOG(std::to_wstring(value.begin(), value.end()))//  LOG(L"\" в \"")  LOG(std::to_wstring(key.begin(), key.end())) LOG(L"\"\n")
        } catch (std::out_of_range) {
            std::wcerr << L"Введён неверный параметр конфигурации \"";
            std::cerr << key << "\"\n";
            return 1; 
        };
    }
    LOG(L"Конец чтения конфигурационного файла\n")
    return 0;
};

//Принимает на вход строку и пишет её в выводной поток, заменяя все маркеры (\[[S,D]number\]) на их значение
int replace(std::istream& in, std::ostream& output = std::cout) {
    LOG(L"Начало ") LOG(std::to_wstring(count + 1)) LOG(L" прохода\n")
    std::string current;
    while(std::getline(in, current, '[')) {
        output << current;
        if (in.eof())
            continue;
        LOG(L"Найдена \"[\"...")
        std::getline(in, current, ']');
        if (in.eof()) {
            std::wcerr << L"\nНе найдена закрывающая \"]\" до конца файла\n";
            return 1;
        }
        LOG(L"Найдена \"]\"...")
        try {
            int number = std::stoi(current);
            //LOG(L"Проверка на число \"" << current << L"\"...")
            output << dynamicMarks[count][number];
            //LOG(L"Запись динамической марки \"" << dynamicMarks[count][number] << L"\"...\n")
        } catch (std::invalid_argument) {
            //std::wcerr << L"[" << current << L"]" << L" не подходит под формат метки";
            return 1;
        }
    };
    LOG(L"Конец ") LOG(std::to_wstring(count + 1))  LOG(L" прохода\n")
    //in.seekg(0);
    return 0;
};

int main(int argc, char* argv[]) {
    std::locale::global (std::locale (""));
    std::string inputPath, outputPath, docPath, isLogEnableStr, configPath, sheet;
    std::map<std::string, std::string&> map = {
        {"input", inputPath},
        {"output", outputPath},
        {"xlsxData", docPath},
        {"log", isLogEnableStr},
        {"sheet", sheet}
    };
    if (argc < 2 ) {
        std::wcout << L"Введите название конфигурационного файла:\n";
        std::cin >> configPath;
    } else {
        configPath = argv[1];
    }

    config(map, configPath);
    if (isLogEnableStr == "false") {isLogEnable = false;}

    std::istream* input = &std::cin;
    std::ostream* output = &std::cout; 

    std::ifstream finput;
    if (inputPath.size() != 0) {
        finput.open(inputPath, std::ios::in);
        if (!finput.good()) {
            std::cerr << L"Не удалось открыть файл " << inputPath << '\n';
            return 1;
        }
        input = &finput;
        LOG(L"Окрыт файл на ввод \"") LOG(std::wstring(inputPath.begin(), inputPath.end())) LOG("\"\n")
    } else { std::cout << L"Введите текст для обработки:\n";}
    *input >> std::noskipws;

    std::string file((std::istreambuf_iterator<char>(*input)), std::istreambuf_iterator<char>());

    std::ofstream foutput;
    if (outputPath != "") {
        foutput.open(outputPath, std::ios::out);
        if (!foutput.good()) {
            std::cerr << L"Не удалось открыть файл " << outputPath << '\n';
            return 1;
        }
        output = &foutput;
        LOG(L"Окрыт файл на вывод \"") LOG(std::wstring(outputPath.begin(), outputPath.end())) LOG("\"\n")
    } else {LOG(L"Вывод будет направлен в cout")}

    OpenXLSX::XLDocument doc;
    try {
        doc.open(docPath);
    } catch (std::runtime_error) {
        std::cerr << L"Не удалось открыть файл " << docPath << '\n';
        return 1;
    }
    LOG(L"Данные будут браться из файла \"") LOG(std::wstring(docPath.begin(), docPath.end())) LOG("\"\n")

    OpenXLSX::XLWorksheet wk;
    try {
        wk = doc.workbook().worksheet(sheet);
    } catch (std::runtime_error) {
        std::cerr << L"В книге отсутсвует страница \"" << sheet << "\"\n";  
        return 1;
    }
    LOG(L"Открыта страница \"") LOG(std::wstring(sheet.begin(), sheet.end())) LOG("\"\n")

    size_t latter = 0;
    LOG(L"Чтение из xlsx документа...\n")
    while(wk.cell(latter + 1, 1).value().type() != OpenXLSX::XLValueType::Empty) {
        dynamicMarks.push_back(std::vector<std::string>());
        std::string current;
        int number = 1;
        while (wk.cell(latter + 1, number).value().type() != OpenXLSX::XLValueType::Empty) {
            LOG(L"Считывание из ячейки \"") LOG(std::to_wstring(latter + 1)) LOG(L",") LOG(std::to_wstring(number)) LOG("\"\n")
            std::string current = wk.cell(latter + 1, number).value().get<std::string>();
            dynamicMarks[latter].push_back(std::move(current));
            ++number;
        }
        ++latter;
    }
    doc.close();
    LOG(L"Чтение завершено\n")

    for (count = 0; count < dynamicMarks.size(); ++count) {
        auto j = std::stringstream(file);
        j << std::noskipws;
        if (replace(j, *output)) return 2;
    }

    std::cout << "Программа завершилась успешно\n";

    return 0;
};
