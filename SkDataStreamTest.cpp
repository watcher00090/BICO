#include <iostream>
#include "SkDataStream.h" 

int main() {
    SkDataStream ds = SkDataStream("s1.txt", "s-originals/s1-label.pa", 1);

    int* tmp; // has 2 attributes
    int label;
    while (!ds.done()) {
        tmp = (int*) ds.currPoint();
        label = ds.currLabel();
        for (int i=0;i<2;++i) std::cout << tmp[i] << " ";
        std::cout << "# " << label << std::endl;
        ds.next();
    }

}
