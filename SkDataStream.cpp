#include "SkDataStream.h"
#include <fstream>
#include <iostream>

SkDataStream::SkDataStream(const char* datafilename, const char* labelsfilename, uint k) :
    _data_ifstream(datafilename), _labels_ifstream(labelsfilename), _k(k)
{
    if (_data_ifstream.fail()) { // check file
        std::cerr << "data file open failed: " << datafilename << std::endl;
        return;
    }
    if (_labels_ifstream.fail()) { // check file
        std::cerr << "labels file open failed: " << labelsfilename << std::endl;
        return;
    }
    next();
}

void SkDataStream::next() {
    if (!done()) {
        _data_ifstream >> _currdatapoint[0];
        _data_ifstream >> _currdatapoint[1];
        _labels_ifstream >> _currlabel;
    }
}

bool SkDataStream::done() const {
    return _data_ifstream.eof();
}

void SkDataStream::rewind() {
    _data_ifstream.clear();
    _data_ifstream.seekg(0);
    _labels_ifstream.clear();
    _labels_ifstream.seekg(0);
    next();
}

void* SkDataStream::currPoint() const {
    return (void*)_currdatapoint;
}

int SkDataStream::currLabel() const {
    return _currlabel;
}
