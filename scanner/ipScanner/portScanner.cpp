
#include "PortScanner.h"

PortScanner::PortScanner(std::string ip, int start, int end){
    this->target_ip = ip;
    this->start_port = start;
    this->end_port = end;
}

void PortScanner::scanTCP(int port){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1){
            cout<<"##Error##: Coult not create socket: "<<errno<<endl;
            return;
        }
        sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        inet_pton(AF_INET, this->target_ip.c_str(), &server_address.sin_addr);

        if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == 0) {
            cout << "\t[+] TCP Port "<< port << " is OPEN" <<endl;
        }
        close(sock);
}

void PortScanner::scanUDP(int port){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        cout << "##Error##: Could not create socket for UDP" << endl;
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, target_ip.c_str(), &server_addr.sin_addr);

    char buffer[1] = {0};
    sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    timeval timeout{};
    timeout.tv_sec = 1;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socklen_t addr_len = sizeof(server_addr);
    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len) >= 0) {
        cout << "\t[+] UDP Port " << port << " is OPEN" << endl;
    }

    close(sock);
}

void PortScanner::runScan(Mode mode) {
    vector<thread> threads;

    for (int port = start_port; port <= end_port; ++port) {
        switch(mode){
            case TCP:
                threads.emplace_back(&PortScanner::scanTCP, this, port);
                break;
            case UDP:
                threads.emplace_back(&PortScanner::scanUDP, this, port);
                break;
            default:
                threads.emplace_back(&PortScanner::scanTCP, this, port);
                threads.emplace_back(&PortScanner::scanUDP, this, port);
                break;
        }

        if (threads.size() >= 1000) {
            for (auto& t : threads) {
                t.join();
            }
            threads.clear();
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}