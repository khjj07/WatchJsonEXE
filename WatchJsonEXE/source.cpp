
#include <iostream>
#include <fstream>
#include <string>
#include <direct.h> 
#include <sstream>
#include <vector>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <windows.h>
#include "nlohmann/json.hpp"

std::vector<std::string> Split(std::string line, char delimeter)
{
    std::vector<std::string> words;
    std::stringstream sstream(line);
    std::string word;
    while (getline(sstream, word, delimeter))
    {
        words.push_back(word);
    }
    return words;
}

int main()
{
    bool spaceInput = false;
    bool changed = false;
    std::string filePath = "setting.ini";
    // read File
    std::ifstream settingFile(filePath.data());
    if (settingFile.is_open()) {
        std::string line;
        while (getline(settingFile, line)) {
            std::cout << line << std::endl;
            std::vector<std::string> words;
            words = Split(line, '=');
            //std::cout << "Current path is " << std::filesystem::current_path() << '\n';
            std::filesystem::current_path(words[1]);
            //std::cout << "Current path is " << std::filesystem::current_path() << '\n';


            std::filesystem::directory_iterator itr(std::filesystem::current_path());
            while (itr != std::filesystem::end(itr)) {
                const std::filesystem::directory_entry& entry = *itr;
                auto words = Split(entry.path().filename().string(), '.');
                if (words[1] == std::string("json"))
                {
                    std::cout << entry.path().filename() << std::endl;
                }
                itr++;
            }
        }
        settingFile.close();

        std::cout << "파일을 선택하십시오 : " << std::endl;

        std::string select;
        std::cin >> select;

        using json = nlohmann::json;
        //json 출력
        std::ifstream jsonFile(select.c_str());
      
        json predata = json::parse(jsonFile);
        std::cout << predata.dump(4);

        std::filesystem::directory_entry currentEntry;
        std::filesystem::directory_iterator itr(std::filesystem::current_path());
        while (itr != std::filesystem::end(itr)) {
            const std::filesystem::directory_entry& entry = *itr;
            if (entry.path().filename().string() == select)
            {
                currentEntry = entry;
                break;
            }
            itr++;
        }

        auto lastWriteTime = std::filesystem::last_write_time(currentEntry);

        //json 감지
        while (1)
        {
            if (lastWriteTime != std::filesystem::last_write_time(currentEntry))
            {
                if (!changed)
                {
                    Sleep(100);
                    std::ifstream jsonFile1(select.c_str());
                    json currentData = json::parse(jsonFile1);
                    json patch = json::diff(predata, currentData);
                    std::cout << patch;
                    std::cout << std::endl;
                    std::cout << std::endl;
                    for (auto data : patch)
                    {
                        if (to_string(data["op"]).compare("\"add\"") == 0)
                        {
                            std::cout << "added --->";
                            std::cout << data["path"] << " : " << data["value"] << std::endl;
                        }
                        else if (to_string(data["op"]).compare("\"remove\"") == 0)
                        {
                            std::cout << "removed --->";
                            std::cout << data["path"] << std::endl;
                        }
                        else if (to_string(data["op"]).compare("\"replace\"") == 0)
                        {
                            std::cout << "replaced --->";
                            std::cout << data["path"] << " : " << data["value"] << std::endl;
                        }
                        
                    }
                    

                    std::cout << std::endl << select << " 파일이 수정되었습니다.  스페이스를 누르면 다시 로드합니다" << std::endl;
                    changed = true;
                }

                if (GetAsyncKeyState(VK_SPACE))
                {
                    spaceInput = true;
                }

                if (spaceInput)
                {
                    std::ifstream jsonFile1(select.c_str());
                    json currentData = json::parse(jsonFile1);
                    std::cout << currentData.dump(4);
                    lastWriteTime = std::filesystem::last_write_time(currentEntry);
                    spaceInput = false;
                }
            }
            else
                changed = false;
        }
    }


    return 0;
}