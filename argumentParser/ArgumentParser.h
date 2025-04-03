#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>

enum Mode {
    TCP,
    UDP,
    FULL
};

class ArgumentParser {
private:
    std::string target_ip;
    int start_port;
    int end_port;
    Mode scan_mode;

    bool isValidIP(const char* ip);
    bool isValidPort(const char* portStr);
    Mode parseMode(const char* modeStr);

public:
    ArgumentParser(int argc, const char* argv[]);
    bool parseArguments();
    std::string getTargetIP() const;
    int getStartPort() const;
    int getEndPort() const;
    Mode getScanMode() const;
};

#endif
