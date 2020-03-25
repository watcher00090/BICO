#include "UCICovertypeDataStream.h"
#include <fstream>
#include <iostream>

using namespace xtreml;

UCICovertypeDataStream::UCICovertypeDataStream(const char* fname) : 
    _file(fname)
{
    if (_file.fail()) { // check file
        std::cerr << "file open failed: " << fname << std::endl;
        return;
    }
    next();
}

void UCICovertypeDataStream::next() {
    // each line contains 54 attributes (in addition to the label)
    for (int pos=0; pos<54; ++pos) {  // parse all but the last value (the label)
        _file >> _currdatapoint[pos];
        _file.get(); // parse the comma
    }
    _file >> _currlabel;
    _file.get(); // remove the newline
}

bool UCICovertypeDataStream::done() const {
    return _file.eof();
}

void UCICovertypeDataStream::rewind() {
    _file.clear();
    _file.seekg(0);
    next();
}

void* UCICovertypeDataStream::currPoint() const {
    return (void*)_currdatapoint;
}

void* UCICovertypeDataStream::currLabel() const {
    return (void*)_currlabel;
}
