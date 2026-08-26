#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <io.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace Utils {

    // ANSI Color Constants
    namespace Color {
        const std::string RESET   = "\033[0m";
        const std::string BOLD    = "\033[1m";
        const std::string DIM     = "\033[2m";
        const std::string RED     = "\033[31m";
        const std::string GREEN   = "\033[32m";
        const std::string YELLOW  = "\033[33m";
        const std::string BLUE    = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN    = "\033[36m";
        const std::string WHITE   = "\033[37m";
        const std::string BOLD_CYAN   = "\033[1;36m";
        const std::string BOLD_GREEN  = "\033[1;32m";
        const std::string BOLD_YELLOW = "\033[1;33m";
        const std::string BOLD_RED    = "\033[1;31m";
        const std::string BG_BLUE     = "\033[44m";
        const std::string BG_CYAN     = "\033[46m";
    }

    inline void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[1;1H";
#endif
    }

    inline void pauseScreen() {
        std::cout << "\n" << Color::DIM << "Press Enter to continue..." << Color::RESET;
#ifdef _WIN32
        if (!_isatty(0)) {
            return;
        }
#endif
        std::cin.ignore(10000, '\n');
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    inline std::string toUpper(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        return str;
    }

    inline std::string getMaskedPassword(const std::string& prompt = "Enter Password: ") {
        std::cout << prompt;
        std::string password = "";
#ifdef _WIN32
        if (!_isatty(0)) {
            std::getline(std::cin, password);
            return trim(password);
        }
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') { // Backspace
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            } else if (ch == 3) { // Ctrl+C
                exit(0);
            } else if (ch >= 32 && ch <= 126) {
                password.push_back(ch);
                std::cout << '*';
            }
        }
        std::cout << "\n";
#else
        if (!isatty(STDIN_FILENO)) {
            std::getline(std::cin, password);
            return trim(password);
        }
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        std::getline(std::cin, password);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        std::cout << "\n";
#endif
        return trim(password);
    }

    inline std::string getCurrentTimestamp() {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    inline std::string generateBookingId() {
        static int counter = 1000;
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        std::ostringstream oss;
        oss << "BK-" << (1900 + t->tm_year) << "-" << (++counter);
        return oss.str();
    }

    inline std::string formatCurrency(double amount) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << amount << " BDT";
        return oss.str();
    }

    inline size_t simpleHash(const std::string& str) {
        // Polynomial rolling hash with salt
        const size_t p = 31;
        const size_t m = 1e9 + 9;
        size_t hash_val = 0;
        size_t p_pow = 1;
        std::string salted = "TMS_SALT_2026_" + str;
        for (char c : salted) {
            hash_val = (hash_val + (static_cast<unsigned char>(c) + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        return hash_val;
    }

    inline std::string repeatString(const std::string& s, int count) {
        std::string res = "";
        for (int i = 0; i < count; ++i) res += s;
        return res;
    }

    inline void printHeader(const std::string& title) {
        int width = 72;
        int padding = (width - static_cast<int>(title.length())) / 2;
        if (padding < 0) padding = 0;

        std::cout << "\n" << Color::CYAN << "╔" << repeatString("═", width) << "╗" << Color::RESET << "\n";
        std::cout << Color::CYAN << "║" << Color::BOLD_YELLOW 
                  << std::string(padding, ' ') << title 
                  << std::string(std::max(0, width - padding - static_cast<int>(title.length())), ' ') 
                  << Color::CYAN << "║" << Color::RESET << "\n";
        std::cout << Color::CYAN << "╚" << repeatString("═", width) << "╝" << Color::RESET << "\n\n";
    }

    inline void printDivider(int width = 74) {
        std::cout << Color::DIM << std::string(width, '-') << Color::RESET << "\n";
    }
}

#endif // UTILS_H
