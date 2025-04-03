#include "ArgumentParser.h"
using namespace std;

bool ArgumentParser::isValidIP(const char* ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}

bool ArgumentParser::isValidPort(const char* portStr) {
    for (int i = 0; portStr[i] != '\0'; i++) {
        if (!isdigit(portStr[i])) return false;
    }
    int port = atoi(portStr);
    return port >= 1 && port <= 65535;
}

Mode ArgumentParser::parseMode(const char* modeStr) {
    if (strcmp(modeStr, "tcp") == 0) {
        return TCP;
    } else if (strcmp(modeStr, "udp") == 0) {
        return UDP;
    } else if (strcmp(modeStr, "both") == 0) {
        return FULL;
    } else {
        cout << "##Error##: Invalid mode! Use 'tcp', 'udp', or 'both'." << endl;
        exit(1);
    }
}

ArgumentParser::ArgumentParser(int argc, const char* argv[]) {
    if (argc < 4 || argc > 5) {
        cout << "##Error##: Use the app: " << argv[0] << " <target-ip> <start-port> <end-port> [tcp/udp/both]" << endl;
        exit(1);
    }

    target_ip = argv[1];
    start_port = atoi(argv[2]);
    end_port = atoi(argv[3]);
    scan_mode = (argc == 5) ? parseMode(argv[4]) : FULL;
}

bool ArgumentParser::parseArguments() {
    if (!isValidIP(target_ip.c_str())) {
        cout << "##Error##: Invalid IP address format!" << endl;
        return false;
    }

    if (!isValidPort(to_string(start_port).c_str()) || !isValidPort(to_string(end_port).c_str())) {
        cout << "##Error##: Ports must be between 1 and 65535!" << endl;
        return false;
    }

    if (start_port > end_port) {
        swap(start_port, end_port);
        cout << "##Info##: Swapped start and end ports as start_port > end_port." << endl;
    }

    return true;
}

string ArgumentParser::getTargetIP() const {
    return target_ip;
}

int ArgumentParser::getStartPort() const {
    return start_port;
}

int ArgumentParser::getEndPort() const {
    return end_port;
}

Mode ArgumentParser::getScanMode() const {
    return scan_mode;
}
