#include "ArgumentParser.h"
#include<regex>
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

bool ArgumentParser::isValidNetwork(const char* network) {
    std::regex network_regex("^([0-9]{1,3}\\.){3}[0-9]{1,3}/([0-9]|[1-2][0-9]|3[0-2])$");
    if (std::regex_match(network, network_regex)) {

        const char* slashPos = strchr(network, '/');
        if (slashPos != nullptr) {
            std::string ip(network, slashPos - network);
            std::string mask(slashPos + 1);            

            if (isValidIP(ip.c_str()) && atoi(mask.c_str()) >= 0 && atoi(mask.c_str()) <= 32) {
                return true;
            }
        }
    }
    return false;
}

void ArgumentParser::processArguments(int argc, const char* argv[]) {
    bool ipSpecified = false;
    bool netSpecified = false;

    if(argc < 3 || argc > 8) {
        std::cout<<"####################################################################"<<endl;
        std::cout << "##Error##: No correct arguments provided!" << std::endl;
        std::cout<<"usage pscan -ip <target_ip>"<<endl;
        std::cout<<"\t\t-net <target_ip_network/mask>"<<endl;
        std::cout<<"\t\t-sp <start_port>(*optional): default 1"<<endl;
        std::cout<<"\t\t-ep <end_port>(*optional): default 65535"<<endl;
        std::cout<<"\t\t-mode <[tcp/udp/both]>(*optional): default both"<<endl;
        std::cout<<"####################################################################"<<endl;
        exit(1);
    }
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "-ip") == 0 && isValidIP(argv[++i])) {
            if (netSpecified) {
                std::cout << "##Error##: You cannot specify both -ip and -net at the same time!" << std::endl;
                exit(1);
            }
            target = argv[i];
            isNetwork = false;
            ipSpecified = true;
        } else if(strcmp(argv[i], "-net") == 0 && isValidNetwork(argv[++i])) {
            if (ipSpecified) {
                std::cout << "##Error##: You cannot specify both -ip and -net at the same time!" << std::endl;
                exit(1);
            }
            target = argv[i];
            isNetwork = true;
            netSpecified = true;
        } else if(strcmp(argv[i], "-sp") == 0 && isValidPort(argv[++i])) {
            start_port = atoi(argv[i]);
        } else if(strcmp(argv[i], "-ep") == 0 && isValidPort(argv[++i])) {
            end_port = atoi(argv[i]);
        } else if(strcmp(argv[i], "-mode") == 0) {
            scan_mode = parseMode(argv[++i]);
        } else {
            std::cout << "##Error##: Unknown argument " << argv[i] << std::endl;
            exit(1);
        }
    }

    if (!ipSpecified && !netSpecified) {
        std::cout<< "##Error##: You must specify either an IP address (-ip) or a network (-net)." << std::endl;
        exit(1);
    }
}
ArgumentParser::ArgumentParser(int argc, const char* argv[]) {
    start_port = 1;
    end_port = 65535;
    scan_mode = FULL;
    
    processArguments(argc, argv);
}

string ArgumentParser::getTarget() const {
    return target;
}

bool ArgumentParser::getIsNetwork() const{
    return isNetwork;
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
