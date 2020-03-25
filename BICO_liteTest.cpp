#include "BICO_lite.h" 
#include "Types.h"
#include "UCICovertypeDataStream.h"
#include "KMeanspp.h"

void test1() {
    xtreml::BICO_lite kmeans = xtreml::BICO_lite();
}

void test2() {
    float* v = new float[5];
    uint dim = 5;
    xtreml::Vec<float> p = xtreml::Vec<float>(dim,v);
    std::cout << "p = " << p << std::endl;
    xtreml::CoresetPair pair = xtreml::CoresetPair(p, 15.0);
    std::cout << "printing the pair..." << std::endl;
    std::cout << pair << std::endl;
}

void test3() {
    float* v = new float[5];
    uint dim = 5;
    xtreml::Vec<float> p = xtreml::Vec<float>(dim,v);
    std::cout << "p = " << p << std::endl;
    xtreml::CoresetPair pair = xtreml::CoresetPair(p, 15.0);
    std::cout << "printing the pair..." << std::endl;
    std::cout << pair << std::endl;
}

void test4() {
    std::cout << __FUNCTION__ << std::endl;

    float v0_arr[2] = {5.0,6.5};
    xtreml::Vec<float> v0(2,v0_arr);
    float v1_arr[2] = {4.0,-6.5};
    xtreml::Vec<float> v1(2,v1_arr);
    float v2_arr[2] = {2.5,15.5};
    xtreml::Vec<float> v2(2,v2_arr);

    float x_arr[2] = {18.0,4.5};
    xtreml::Vec<float> x(2,x_arr);

    xtreml::ClusteringFeature f0 = xtreml::ClusteringFeature(v0);
    xtreml::ClusteringFeature f1 = xtreml::ClusteringFeature(v1);
    xtreml::ClusteringFeature f2 = xtreml::ClusteringFeature(v2);

    std::vector<xtreml::ClusteringFeature*> featureVector = std::vector<xtreml::ClusteringFeature*>();
    featureVector.push_back(&f0);
    featureVector.push_back(&f1);
    featureVector.push_back(&f2);

    float df0 = xtreml::Vec<float>::dist(f0.r_,x);
    float df1 = xtreml::Vec<float>::dist(f1.r_,x);
    float df2 = xtreml::Vec<float>::dist(f2.r_,x);

    std::cout << "v0 = " << v0 << std::endl;
    std::cout << "v1 = " << v1 << std::endl;
    std::cout << "v2 = " << v2 << std::endl;
    std::cout << "x = " << x << std::endl;

    std::cout << "d(x,f0.ref) = " << df0 << ", d(x,f1.ref) = " << df1 << ", d(x,f2.ref) = " << df2 << std::endl;

    xtreml::ClusteringFeature* nearestFeature = xtreml::ClusteringFeature::nearest(x, featureVector);
    xtreml::Vec<float> nearestPoint = nearestFeature->r_;
    std::cout << nearestPoint << std::endl;
}

void test5() {
    std::cout << __FUNCTION__ << std::endl;
    std::cout << "testing << operator for BICO_lite objects..." << std::endl;
    
    float v0_arr[2] = {5.0,6.5};
    xtreml::Vec<float> v0(2,v0_arr);
    float v1_arr[2] = {4.0,-6.5};
    xtreml::Vec<float> v1(2,v1_arr);
    float v2_arr[2] = {2.5,15.5};
    xtreml::Vec<float> v2(2,v2_arr);
    float v3_arr[2] = {-7.0,25.5};
    xtreml::Vec<float> v3(2,v3_arr);
    float v4_arr[2] = {14.0,-.5};
    xtreml::Vec<float> v4(2,v4_arr);
    float v5_arr[2] = {0.07070707,10.5};
    xtreml::Vec<float> v5(2,v5_arr);
    float v6_arr[2] = {0.77738327,10000.5};
    xtreml::Vec<float> v6(2,v6_arr);
    float v7_arr[2] = {77777.77777,.0005};
    xtreml::Vec<float> v7(2,v7_arr);
    float v8_arr[2] = {99981,8885};
    xtreml::Vec<float> v8(2,v8_arr);
    float v9_arr[2] = {389383,0.0};
    xtreml::Vec<float> v9(2,v9_arr);

    xtreml::ClusteringFeature f0 = xtreml::ClusteringFeature(v0);
    xtreml::ClusteringFeature f1 = xtreml::ClusteringFeature(v1);
    xtreml::ClusteringFeature f2 = xtreml::ClusteringFeature(v2);
    xtreml::ClusteringFeature f3 = xtreml::ClusteringFeature(v3);
    xtreml::ClusteringFeature f4 = xtreml::ClusteringFeature(v4);
    xtreml::ClusteringFeature f5 = xtreml::ClusteringFeature(v5);
    xtreml::ClusteringFeature f6 = xtreml::ClusteringFeature(v6);
    xtreml::ClusteringFeature f7 = xtreml::ClusteringFeature(v7);
    xtreml::ClusteringFeature f8 = xtreml::ClusteringFeature(v8);
    xtreml::ClusteringFeature f9 = xtreml::ClusteringFeature(v9);

    /************************************
     *
     * We have the following tree structure: 
     * f0
     *      f1
     *          f2
     *          f3
     *          f4
     *      f5
     *          f6
     *              f7
     *              f8
     *          f9
     *
     * *********************************/
    f0.level=0;
    f1.level=1;
    f2.level=2;
    f3.level=2;
    f4.level=2;
    f5.level=1;
    f6.level=2;
    f7.level=3;
    f8.level=3;
    f9.level=2;

    f0.children.push_back(&f1);
    f0.children.push_back(&f5);
    f1.children.push_back(&f2);
    f1.children.push_back(&f3);
    f1.children.push_back(&f4);
    f5.children.push_back(&f6);
    f5.children.push_back(&f9);
    f6.children.push_back(&f7);
    f6.children.push_back(&f8);

    xtreml::BICO_lite feature_tree = xtreml::BICO_lite();
    feature_tree.root = &f0;

    std::cout << feature_tree << std::endl;
}

void test6() {
    std::cout <<  __FUNCTION__ << std::endl;

    xtreml::BICO_lite clusteringMachine = xtreml::BICO_lite(5000); //n_max = 5000

    xtreml::UCICovertypeDataStream covertypeDataStream = xtreml::UCICovertypeDataStream;
    xtreml::UCICovertypeDataStream ds = xtreml::UCICovertypeDataStream("../data/covtype.data");
    int* tmp; // has 53 attributes
    int label;

    float* dataVec_buf = new float[53];
    Vec<float> dataVec = Vec<float>(53,dataVec_buf);

    while (!ds.done()) {
        tmp = (int*) ds.currPoint();
        label = static_cast<int>(reinterpret_cast<intptr_t>(ds.currLabel()));
        for (int i=0;i<53;++i) std::cout << tmp[i] << " ";
        std::cout << "# " << label << std::endl;

        for (int i=0;i<53;++i) dataVec[i] = (float) tmp[i];

        clusteringMachine.update(dataVec);

        std::cout << "num points processed = " << i << ", clustering score = " << xtreml::kmeanspp(clusteringMachine.queryCoreset(), 7) << std::end; // 7 forest cover types

        ds.next();
    }

    delete[] dataVec_buf;
}

int main() {
    //test1();
    //test2();
    //test4();
    //test5();
    test6();
}
