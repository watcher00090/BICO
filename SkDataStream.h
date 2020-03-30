#pragma once 

#include <fstream>

class SkDataStream {

public:
    SkDataStream(const char* data_ifstream, const char* labels_ifstream, uint k);

    virtual void next();
    virtual bool done() const;
    virtual void rewind();
    virtual void* currPoint() const;
    virtual int currLabel() const;

protected:
    std::ifstream _data_ifstream; // file stream
    std::ifstream _labels_ifstream; // file stream
    int _currdatapoint[2];
    int _currlabel;
    int _k;
};
