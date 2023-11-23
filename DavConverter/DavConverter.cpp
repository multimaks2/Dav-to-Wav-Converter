#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <vector>


bool ConvertDavToWav(const std::string& davFilePath) {
    try {
        // Открываем .dav файл для чтения в бинарном режиме
        std::ifstream inputFile(davFilePath, std::ios::binary);
        if (!inputFile.is_open()) {
            fprintf(stderr, "Error: File does not exist or cannot be opened.\n");
            return false;
        }

        // Получаем путь и имя файла без расширения
        size_t lastDotIndex = davFilePath.find_last_of(".");
        if (lastDotIndex == std::string::npos) {
            fprintf(stderr, "Error: Invalid file name.\n");
            return false;
        }

        std::string outputPath = davFilePath.substr(0, lastDotIndex) + ".wav";

        // Читаем содержимое файла в байтовый вектор
        std::vector<char> buffer((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

        // Применяем преобразование XOR к каждому байту
        for (char& byte : buffer) {
            byte ^= 123;
        }

        // Закрываем входной файл
        inputFile.close();

        // Записываем преобразованные данные обратно в файл (.wav)
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            fprintf(stderr, "Error: Could not create or open output file.\n");
            return false;
        }

        outputFile.write(buffer.data(), buffer.size());

        // Закрываем выходной файл
        outputFile.close();

        printf("Файл сохранен: %s\n\n", outputPath.c_str());
        return true;
    }
    catch (const std::exception& ex) {
        fprintf(stderr, "Conversion failed. Reason: %s\n", ex.what());
        return false;
    }
}

int findDavFiles(const std::string& directory) {
    int count = 0;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((directory + "\\*").c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Пропускаем . и ..
                if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                    // Рекурсивно вызываем для подпапок
                    count += findDavFiles(directory + "\\" + findFileData.cFileName);
                }
            }
            else {
                if (strstr(findFileData.cFileName, ".dav") != nullptr) {
                    //printf("Найден файл: %s\n", (directory + "\\" + findFileData.cFileName).c_str());
                    if (ConvertDavToWav(directory + "\\" + findFileData.cFileName)) {
                        printf("Файл конвертируется в wav: %s\n", (directory + "\\" + findFileData.cFileName).c_str());
                    }
                    count++;
                }
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    return count;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    // Проверяем, передан ли аргумент командной строки
    if (argc > 1) {
        // Печатаем путь к папке
        

        // Вызываем функцию поиска файлов
        int fileCount = findDavFiles(argv[1]);

        // Печатаем суммарное количество найденных файлов
        printf("\n\nСуммарное количество файлов .dav: %d\n", fileCount);
        printf("Путь к папке: %s\n", argv[1]);
    }
    else {
        // Если аргумента нет, сообщаем об этом
        printf("Не указан путь к папке.\n");
        return 1; // Возвращаем код ошибки
    }

    // Ожидаем ввода для того, чтобы консоль не закрылась сразу
    printf("Нажмите Enter для завершения...\n");
    std::string userInput;
    std::getline(std::cin, userInput);

    return 0; // Возвращаем успешный код завершения
}
