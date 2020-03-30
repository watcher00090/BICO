#pragma once 

#include <fstream>

class UCICovertypeDataStream {

public:
    UCICovertypeDataStream(const char* fname);

    virtual void next();
    virtual bool done() const;
    virtual void rewind();
    virtual void* currPoint() const;
    virtual int currLabel() const;

protected:
    std::ifstream _file; // file stream
    int _currdatapoint[53];
    int _currlabel;
};
