#include <iostream>
#include "scanner/PortScanner.h"

int main(int argc, const char* argv[]) {
    ArgumentParser parser(argc, argv);
    
    if (!parser.parseArguments()) {
        return 1;
    }

    PortScanner scanner(parser.getTargetIP(), parser.getStartPort(), parser.getEndPort());

    cout << "##Scanning " << parser.getTargetIP() << " from port " << parser.getStartPort() 
         << " to " << parser.getEndPort() << " in mode: ";

    Mode mode = parser.getScanMode();
    if (mode == TCP) {
        cout << "TCP" << endl;
    } else if (mode == UDP) {
        cout << "UDP" << endl;
    } else {
        cout << "Both TCP and UDP" << endl;
    }

    scanner.runScan(mode);

    return 0;
}
