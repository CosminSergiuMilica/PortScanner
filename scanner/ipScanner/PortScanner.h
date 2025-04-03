#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <vector>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <fcntl.h>
#include "scanner.h"

using namespace std;

class PortScanner: public Scanner {
private:
    std::string target_ip;
    int start_port;
    int end_port;
protected:
    void scanTCP(int port);
    void scanUDP(int port);

public:
    PortScanner(std::string ip, int start, int end);
    void runScan(Mode mode = FULL) override;
};

#endif
