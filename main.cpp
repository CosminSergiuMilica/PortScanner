#include <iostream>
#include "scanner/interface/scanner.h"
#include "scanner/networkScanner/NetworkScanner.h"
#include "scanner/ipScanner/PortScanner.h"

int main(int argc, const char* argv[]) {
    ArgumentParser parser(argc, argv);

    Scanner * scanner;
    if(parser.getIsNetwork()){
        cout<<parser.getTarget()<<endl;
        scanner = new NetworkScanner(parser.getTarget(), parser.getStartPort(), parser.getEndPort());
    }
    else{
        scanner = new PortScanner(parser.getTarget(), parser.getStartPort(), parser.getEndPort());
    }

    std::cout << "##Scanning " << parser.getTarget() << " from port " << parser.getStartPort() 
         << " to " << parser.getEndPort() << " in mode: ";

    Mode mode = parser.getScanMode();
    if (mode == TCP) {
        std::cout << "TCP" << std::endl;
    } else if (mode == UDP) {
        std::cout << "UDP" << std::endl;
    } else {
        std::cout << "Both TCP and UDP" << std::endl;
    }

    scanner->runScan(mode);

    delete scanner;
    scanner = nullptr;
    return 0;
}
