#include "NetworkScanner.h"
std::mutex output_mutex;
bool isHostAlive(const std::string& ip) {
    std::string command = "ping -c 1 -W 1 " + ip + " > /dev/null 2>&1"; 
    int result = system(command.c_str());
    
    if (result == 0) {
        return true;
    } else {
        return false;
    }
}
std::vector<std::string> NetworkScanner::getNetworkIPs(){
    std::vector<std::string> ip_list;

    size_t pos = network.find('/');
    if (pos == std::string::npos) {
        std::cout << "##Error##: Invalid network format. Expected x.x.x.x/x." << std::endl;
        return ip_list;
    }

    std::string ip = network.substr(0, pos);
    int mask = std::stoi(network.substr(pos + 1));

    struct sockaddr_in sa;
    inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));

    uint32_t ipAddr = ntohl(sa.sin_addr.s_addr);
    uint32_t subnetMask = ~((1 << (32 - mask)) - 1);

    uint32_t networkAddress = ipAddr & subnetMask;
    uint32_t broadcastAddress = networkAddress | ~subnetMask;

    uint32_t firstIP = networkAddress + 1;
    uint32_t lastIP = broadcastAddress - 1;

    for (uint32_t ipAddr = firstIP; ipAddr <= lastIP; ++ipAddr) {
        struct in_addr ipStruct;
        ipStruct.s_addr = htonl(ipAddr);
        ip_list.push_back(inet_ntoa(ipStruct));
    }

    return ip_list;
}


void NetworkScanner::scanTCP(const std::string& ip, int port) const{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        std::cout<<"##Error##: Coult not create socket: "<<errno<<std::endl;
        return;
    }
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == 0) {
        std::cout << "\t[+] TCP Port "<< port << " is OPEN" <<std::endl;
    }
    close(sock);    
}

void NetworkScanner::scanUDP(const std::string& ip, int port) const{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        std::cout << "##Error##: Could not create socket for UDP" << std::endl;
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    char buffer[1] = {0};
    sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    timeval timeout{};
    timeout.tv_sec = 1;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socklen_t addr_len = sizeof(server_addr);
    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len) >= 0) {
        std::cout << "\t[+] UDP Port " << port << " is OPEN" << std::endl;
    }

    close(sock);

}

void NetworkScanner::scanPort(const std::string& ip, int port, Mode mode) const {
    if (mode == TCP || mode == FULL) {
        scanTCP(ip, port);
    }
    if (mode == UDP || mode == FULL) {
        scanUDP(ip, port);
    }
}

void NetworkScanner::runScan(Mode mode) {
    std::vector<std::string> ip_list = getNetworkIPs();
    std::vector<std::thread> threads;
    size_t maxThreads = 100;

    for (const auto& ip : ip_list) {
        {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "[*] Starting scan on IP: " << ip << std::endl;
        }
        if (!isHostAlive(ip)) {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "\t[-] No device found at " << ip << std::endl;
            continue;
        }

        for (int port = start_port; port <= end_port; ++port) {
            threads.push_back(std::thread([=]() {
                scanPort(ip, port, mode);
            }));

            if (threads.size() >= maxThreads) {
                for (auto& t : threads) {
                    t.join();
                }
                threads.clear();
            }
        }
    }
}


NetworkScanner::NetworkScanner(std::string network, int start, int end){
    this->network = network;
    this->start_port = start;
    this->end_port = end;
}

