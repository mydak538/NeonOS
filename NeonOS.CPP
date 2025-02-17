#include <iostream>
#include <string>
#include <windows.h>
#include <shellapi.h>    // Для ShellExecuteA
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

const string SKYOS_DESKTOP = "C:\\Neon\\Desktop";

// Функция для создания структуры папок рабочего стола SkyOS
void setupSkyOSDesktop() {
    if (!fs::exists(SKYOS_DESKTOP))
        fs::create_directories(SKYOS_DESKTOP);
}

// Функция для добавления программы в автозагрузку
void addToAutostart() {
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) {
        cout << "Error: Unable to get module file name." << endl;
        return;
    }

    string programPath = path;
    if (programPath.find(" ") != string::npos) {
        programPath = "\"" + programPath + "\"";
    }

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);

    if (result == ERROR_SUCCESS) {
        const char* programName = "SkyOS";
        result = RegSetValueExA(hKey, programName, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(programPath.c_str()),
            (DWORD)(programPath.size() + 1));
        RegCloseKey(hKey);
        cout << "Program added to autostart successfully!" << endl;
    }
    else {
        cout << "Error: Unable to open registry key." << endl;
    }
}

// Функция для перезагрузки системы
void rebootSystem() {
    cout << "System is restarting..." << endl;
    system("shutdown /r /f /t 0");  // Перезагружаем систему немедленно
}

// Функция для выполнения команд с правами администратора
void runAsRoot(const string& command) {
    string commandToRun = "powershell -Command \"Start-Process cmd -ArgumentList '/c " + command + "' -Verb RunAs\"";
    system(commandToRun.c_str());
}

// Функция для создания файла на рабочем столе
void createFile(const string& filename) {
    string filepath = SKYOS_DESKTOP + "\\" + filename;
    ofstream file(filepath);
    if (file.is_open()) {
        file << "This is a new file." << endl;  // Пример содержимого файла
        file.close();
        cout << "File created: " << filename << endl;
    }
    else {
        cout << "Error: Unable to create file." << endl;
    }
}

// Функция для удаления файла с рабочего стола
void deleteFile(const string& filename) {
    string filepath = SKYOS_DESKTOP + "\\" + filename;
    if (fs::exists(filepath)) {
        fs::remove(filepath);
        cout << "File deleted: " << filename << endl;
    }
    else {
        cout << "Error: File does not exist." << endl;
    }
}

// Функция для открытия файла с рабочего стола в Notepad
void openFile(const string& filename) {
    string filepath = SKYOS_DESKTOP + "\\" + filename;
    if (fs::exists(filepath)) {
        string command = "start notepad " + filepath;
        system(command.c_str());
    }
    else {
        cout << "Error: File does not exist." << endl;
    }
}

// Функция для вывода списка файлов на рабочем столе
void listFiles() {
    cout << "Files on desktop:" << endl;
    for (const auto& entry : fs::directory_iterator(SKYOS_DESKTOP)) {
        cout << entry.path().filename() << endl;
    }
}

// Функция для открытия сайта в браузере с помощью ShellExecuteA
void openWebsite(string url) {
    // Если URL не содержит "http://" или "https://", добавляем префикс "http://"
    if (url.find("http://") == string::npos && url.find("https://") == string::npos) {
        url = "http://" + url;
    }
    // Попытка открыть URL
    HINSTANCE result = ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if ((int)result <= 32) {
        cout << "Error: Unable to open URL: " << url << " (error code " << (int)result << ")" << endl;
    }
}

// Функция для обработки команд
void executeCommand(const string& command) {
    if (command == "exit") {
        cout << "Exiting SkyOS..." << endl;
        exit(0);
    }
    else if (command == "help") {
        cout << "Available commands:" << endl;
        cout << "1. open <url> - Open a website" << endl;
        cout << "2. desktop - Go to desktop" << endl;
        cout << "3. help - Show available commands" << endl;
        cout << "4. exit - Exit the program" << endl;
        cout << "5. root <command> - Execute a command with root privileges" << endl;
        cout << "6. reboot - Restart the system" << endl;
    }
    else if (command.substr(0, 5) == "open ") {
        string url = command.substr(5);
        openWebsite(url);
    }
    else if (command == "desktop") {
        cout << "Desktop commands:" << endl;
        cout << "1. createfile <filename> - Create a new file" << endl;
        cout << "2. deletefile <filename> - Delete a file" << endl;
        cout << "3. openfile <filename> - Open a file" << endl;
        cout << "4. dir - List files on the desktop" << endl;
    }
    else if (command.substr(0, 11) == "createfile ") {
        string filename = command.substr(11);
        createFile(filename);
    }
    else if (command.substr(0, 12) == "deletefile ") {
        string filename = command.substr(12);
        deleteFile(filename);
    }
    else if (command.substr(0, 10) == "openfile ") {
        string filename = command.substr(10);
        openFile(filename);
    }
    else if (command == "dir") {
        listFiles();
    }
    else if (command == "reboot") {
        rebootSystem();
    }
    else {
        cout << "Unknown command." << endl;
    }
}

int main() {
    setupSkyOSDesktop();
    addToAutostart();

    string command;
    cout << "Welcome to Neon CLI. Type 'help' for commands." << endl;

    while (true) {
        cout << "NeonOS> ";
        getline(cin, command);
        executeCommand(command);
    }

    return 0;
}
