#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <fstream>
#include <thread>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include "ThreadPool.h"
#include <tchar.h>
#include <iostream>
#include <string>

#define BUF_SIZE 4096
std::vector<std::string> lines;
std::vector<std::vector<std::string>> blocks;
LPCSTR pBuf;

bool ReadFile()
{
    const std::string filename = "input.txt";
    OFSTRUCT fileInfo;
    HFILE file = OpenFile(filename.c_str(), &fileInfo, OF_READWRITE);
    DWORD fileSize = GetFileSize((HANDLE)file, nullptr);
    HANDLE hMapFile = CreateFileMappingA((HANDLE)file, nullptr, PAGE_READWRITE, 0, fileSize, nullptr);
    if (hMapFile == nullptr)
    {
        std::cout << "Can't create file mapping" << std::endl;
        return false;
    }
    pBuf = static_cast<LPCSTR>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, fileSize));
    if (pBuf == nullptr)
    {
        std::cout << "Can't get mapped file" << std::endl;
        return false;
    }
    auto fileContent = static_cast<char*>(calloc(fileSize + 1, sizeof(char)));
    if (fileContent == nullptr)
    {
        std::cout << "Can't allocate memory for file content" << std::endl;
        return false;
    }
    strncpy(fileContent, pBuf, fileSize);
    fileContent[fileSize] = '\0';

    char* token = strtok(fileContent, "\r\n");
    while (token != nullptr)
    {
        lines.emplace_back(token);
        token = strtok(nullptr, "\r\n");
    }

    CloseHandle((HANDLE)file);
    CloseHandle(hMapFile);

    return true;
}

bool WriteToFile(const std::string& filename, const std::string& content)
{
    HANDLE file = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE)
    {
        std::cout << "Can't create file" << std::endl;
        return false;
    }
    DWORD bytesWritten;
    if (!WriteFile(file, content.c_str(), content.length(), &bytesWritten, nullptr))
    {
        std::cout << "Can't write to file" << std::endl;
        return false;
    }
    CloseHandle(file);
    return true;
}

short GetThreadCount()
{
    short threadsCount;
    std::cout << "Number of threads: ";
    std::cin >> threadsCount;
    std::cout << "\n";
    if (threadsCount > lines.size() || threadsCount <= 0)
    {
        std::cout << "Incorrect number of threads" << std::endl;
        return -1;
    }
    return threadsCount;
}

void SortLine(void* line)
{
    auto obj = static_cast<std::vector<std::string>*>(line);
    std::sort(obj->begin(), obj->end());
}


std::vector<std::string> Merge(const std::vector<std::vector<std::string>>& test)
{
    std::vector<std::string> result;

    std::merge(test[0].begin(), test[0].end(), test[1].begin(), test[1].end(), std::back_inserter(result));
    for (size_t i = 2; i < test.size(); i++)
    {
        std::vector<std::string> temp;
        std::merge(result.begin(), result.end(), test[i].begin(), test[i].end(), std::back_inserter(temp));
        std::swap(result, temp);
    }
    return result;
}

std::string CorrectSpaces(std::string str)
{
    const std::string delimiter = " ";
    size_t pos;
    std::string token;

    std::string finalStr;
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        if (!token.empty())
        {
            finalStr += (token + " ");
        }
        str.erase(0, pos + delimiter.length());
    }
    finalStr += str;
    return finalStr;
}

void CreateTasks(std::vector<std::string> lines, short threadsCount, TaskQueue* taskQueue)
{
    bool isEven = lines.size() % threadsCount == 0;
    int standardStringCapacity = lines.size() / threadsCount;
    if (!isEven)
        standardStringCapacity = lines.size() / threadsCount + 1;
    int globalCounter = 0;
    for (int i = 0; i < threadsCount; i++)
    {
        blocks[i].clear();
        int tmpCounter = 0;
        while (standardStringCapacity != tmpCounter)
        {
            if (globalCounter == lines.size())
                break;
            std::string str = CorrectSpaces(lines[globalCounter]);
            if (!str.empty())
            {
                blocks[i].push_back(str);
            }
            globalCounter++;
            tmpCounter++;
        }
        auto newTask = new Task(&SortLine, static_cast<void*>(&blocks[i]));
        taskQueue->Enqueue(newTask);
    }
}

int main()
{
    if (!ReadFile())
    {
        system("pause");
        return -1;
    }

    const short threadsCount = GetThreadCount();
    if (threadsCount == -1)
    {
        std::cout << "Invalid thread count value." << std::endl;
        system("pause");
        return -1;
    }

    blocks.resize(threadsCount);
    auto taskQueue = new TaskQueue();
    CreateTasks(lines, threadsCount, taskQueue);
    lines.clear();

    const ThreadPool* threads = new ThreadPool(threadsCount, taskQueue);
    threads->WaitAll();
    delete(threads);
    delete(taskQueue);

    lines = Merge(blocks);

    std::string joinedStr = "";
    const int dataCount = lines.size();
    for (int i = 0; i < dataCount; i++)
    {
        if (i == dataCount - 1)
        {
            joinedStr += lines[i];
            break;
        }
        joinedStr += lines[i] + "\n";
    }

    WriteToFile("output.txt", joinedStr);

    lines.clear();
    UnmapViewOfFile(pBuf);
    system("pause");
    return 0;
}
