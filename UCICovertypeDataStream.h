#pragma once 

#include <fstream>

namespace xtreml {

class UCICovertypeDataStream {

public:
    UCICovertypeDataStream(const char* fname);

    virtual void next();
    virtual bool done() const;
    virtual void rewind();
    virtual void* currPoint() const;
    virtual void* currLabel() const;

protected:
    std::ifstream _file; // file stream
    int _currdatapoint[53];
    int _currlabel;
};

}
