#include "ArgumentParser.h"

#ifndef SCANNER_H
#define SCANNER_H

class Scanner {
public:
    virtual void runScan(Mode mode = FULL) = 0;
    virtual ~Scanner() {}
};

#endif
