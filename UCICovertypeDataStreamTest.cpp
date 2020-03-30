#include "UCICovertypeDataStream.h"
#include <iostream>

class UCICovertypeDataStreamTest {
public:
    static void test1() {
        UCICovertypeDataStream ds = UCICovertypeDataStream("data/covtype.data");
        int* tmp; // has 53 attributes
        int label;
        while (!ds.done()) {
            tmp = (int*) ds.currPoint();
            label = ds.currLabel();
            for (int i=0;i<53;++i) std::cout << tmp[i] << " ";
            std::cout << "# " << label << std::endl;
            ds.next();
        }
    }

};

int main() {
    UCICovertypeDataStreamTest::test1();
}
