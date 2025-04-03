#ifndef NETWORKSCANNER_H
#define NETWORKSCANNER_H

#include "scanner.h"
#include <vector>
#include <unistd.h>
#include <thread>
#include <string>
#include <fcntl.h>
#include <netinet/ip_icmp.h>
#include<mutex>
#include <iostream>

class NetworkScanner : public Scanner {
private:
    std::string network;
    int start_port;
    int end_port;

    std::vector<std::string> getNetworkIPs();
    void scanTCP(const std::string& ip, int port) const;
    void scanUDP(const std::string& ip, int port) const;
    void scanPort(const std::string& ip, int port, Mode mode) const;
    
public:
    NetworkScanner(std::string network, int start, int end);
    void runScan(Mode mode = FULL) override;
};

#endif
