
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
        perror("##Error##: Could not create socket for UDP");
        return;
    }
    
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, target_ip.c_str(), &server_addr.sin_addr);

    char buffer[1] = {0};
    ssize_t bytes_sent = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bytes_sent == -1) {
        perror("##Error##: Could not send data");
        close(sock);
        return;
    }

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);

    timeval timeout{};
    timeout.tv_sec = 1;

    int select_result = select(sock + 1, &read_fds, nullptr, nullptr, &timeout);

    if (select_result > 0) {
        sockaddr_in response_addr{};
        socklen_t addr_len = sizeof(response_addr);
        char recv_buffer[1024];

        ssize_t bytes_received = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&response_addr, &addr_len);
        if (bytes_received >= 0) {
            struct ip *ip_header = (struct ip *)recv_buffer;
            struct icmphdr *icmp_header = (struct icmphdr *)(recv_buffer + (ip_header->ip_hl << 2));
            if (icmp_header->type == ICMP_DEST_UNREACH) {
                std::cout << "\t[-] UDP Port " << port << " is CLOSED (ICMP response)" << std::endl;
            }
        }
    } else {
        std::cout << "\t[+] UDP Port " << port << " is OPEN (no ICMP response)" << std::endl;
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

        if (threads.size() >= 100) {
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