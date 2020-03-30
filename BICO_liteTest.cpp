#include <iostream>
#include "BICO_lite.h" 
#include "UCICovertypeDataStream.h"
#include "SkDataStream.h"
#include "KMeanspp.h"

void test1() {
    BICO_lite kmeans = BICO_lite();
}

void test2() {
    float* v = new float[5];
    uint dim = 5;
    Vec<float> p = Vec<float>(dim,v);
    std::cout << "p = " << p << std::endl;
    CoresetPair pair = CoresetPair(p, 15.0);
    std::cout << "printing the pair..." << std::endl;
    std::cout << pair << std::endl;
}

void test3() {
    float* v = new float[5];
    uint dim = 5;
    Vec<float> p = Vec<float>(dim,v);
    std::cout << "p = " << p << std::endl;
    CoresetPair pair = CoresetPair(p, 15.0);
    std::cout << "printing the pair..." << std::endl;
    std::cout << pair << std::endl;
}

void test4() {
    std::cout << __FUNCTION__ << std::endl;

    float v0_arr[2] = {5.0,6.5};
    Vec<float> v0(2,v0_arr);
    float v1_arr[2] = {4.0,-6.5};
    Vec<float> v1(2,v1_arr);
    float v2_arr[2] = {2.5,15.5};
    Vec<float> v2(2,v2_arr);

    float x_arr[2] = {18.0,4.5};
    Vec<float> x(2,x_arr);

    ClusteringFeature f0 = ClusteringFeature(v0);
    ClusteringFeature f1 = ClusteringFeature(v1);
    ClusteringFeature f2 = ClusteringFeature(v2);

    std::vector<ClusteringFeature*> featureVector = std::vector<ClusteringFeature*>();
    featureVector.push_back(&f0);
    featureVector.push_back(&f1);
    featureVector.push_back(&f2);

    float df0 = Vec<float>::dist(f0.r_,x);
    float df1 = Vec<float>::dist(f1.r_,x);
    float df2 = Vec<float>::dist(f2.r_,x);

    std::cout << "v0 = " << v0 << std::endl;
    std::cout << "v1 = " << v1 << std::endl;
    std::cout << "v2 = " << v2 << std::endl;
    std::cout << "x = " << x << std::endl;

    std::cout << "d(x,f0.ref) = " << df0 << ", d(x,f1.ref) = " << df1 << ", d(x,f2.ref) = " << df2 << std::endl;

    ClusteringFeature* nearestFeature = ClusteringFeature::nearest(x, featureVector);
    Vec<float> nearestPoint = nearestFeature->r_;
    std::cout << nearestPoint << std::endl;
}

void test5() {
    std::cout << __FUNCTION__ << std::endl;
    std::cout << "testing << operator for BICO_lite objects..." << std::endl;
    
    float v0_arr[2] = {5.0,6.5};
    Vec<float> v0(2,v0_arr);
    float v1_arr[2] = {4.0,-6.5};
    Vec<float> v1(2,v1_arr);
    float v2_arr[2] = {2.5,15.5};
    Vec<float> v2(2,v2_arr);
    float v3_arr[2] = {-7.0,25.5};
    Vec<float> v3(2,v3_arr);
    float v4_arr[2] = {14.0,-.5};
    Vec<float> v4(2,v4_arr);
    float v5_arr[2] = {0.07070707,10.5};
    Vec<float> v5(2,v5_arr);
    float v6_arr[2] = {0.77738327,10000.5};
    Vec<float> v6(2,v6_arr);
    float v7_arr[2] = {77777.77777,.0005};
    Vec<float> v7(2,v7_arr);
    float v8_arr[2] = {99981,8885};
    Vec<float> v8(2,v8_arr);
    float v9_arr[2] = {389383,0.0};
    Vec<float> v9(2,v9_arr);

    ClusteringFeature f0 = ClusteringFeature(v0);
    ClusteringFeature f1 = ClusteringFeature(v1);
    ClusteringFeature f2 = ClusteringFeature(v2);
    ClusteringFeature f3 = ClusteringFeature(v3);
    ClusteringFeature f4 = ClusteringFeature(v4);
    ClusteringFeature f5 = ClusteringFeature(v5);
    ClusteringFeature f6 = ClusteringFeature(v6);
    ClusteringFeature f7 = ClusteringFeature(v7);
    ClusteringFeature f8 = ClusteringFeature(v8);
    ClusteringFeature f9 = ClusteringFeature(v9);

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

    BICO_lite feature_tree = BICO_lite();
    feature_tree.root = &f0;

    std::cout << feature_tree << std::endl;
}

void test6() {
    std::cout <<  __FUNCTION__ << std::endl;

    SkDataStream ds = SkDataStream("s1.txt", "s-originals/s1-label.pa", 1);
    //UCICovertypeDataStream ds = UCICovertypeDataStream("data/covtype.data");

    //uint dim = 53;
    //uint k = 7;
    uint dim = 2;
    uint k = 15;

    std::vector<Vec<float>> data;
    std::vector<uint> labels;
    uint groundTruthClusterSize[k]; // groundTruthCentroids[k] = centroid of points belonging to ground truth cluster k

    for (int i=0;i<k;++i) groundTruthClusterSize[i] = 0;

    Vec<float> groundTruthCentroids[k];
    for (int i=0;i<k;++i) {
        groundTruthCentroids[i] = Vec<float>(dim,new float[dim]);
        groundTruthCentroids[i].reset();
    }

    int* tmp;
    int label;
    while (!ds.done()) {
        tmp = (int*) ds.currPoint();
        label = ds.currLabel();


        float* databuf = new float[dim];
        for (int j=0;j<dim;++j) databuf[j] = (float)tmp[j];

        data.push_back(Vec<float>(dim,databuf));
        labels.push_back((float)label);
        groundTruthCentroids[label-1] += Vec<float>(dim,databuf);
        groundTruthClusterSize[label-1]++;

        ds.next();
    }

    assert(data.size() == labels.size());
    uint numpoints = data.size();

    for (int i=0;i<k;++i) {
        assert(groundTruthClusterSize[i] > 0);
        groundTruthCentroids[i] /= (float)groundTruthClusterSize[i];
    }

    uint* c_buf = new uint[data.size()];
    Vec<uint> c(data.size(),c_buf);

    Vec<float>* centroids = new Vec<float>[k];
    for (int i=0;i<k;++i) {
        centroids[i] = Vec<float>(dim,new float[dim]);
        centroids[i].reset();
    }

    Matrix<float> confusionMatrix(k,k,new float[k*k]);
    confusionMatrix.reset();

    float score = kmeanspp(&data.at(0),data.size(),k,centroids,c);

    std::cout << "score = " << score << std::endl;

    // confusion matrix: element in (row,col) = (i,j) = percent of items classified into class i but actually in class j
    float overallAccuracy = accuracy(&data.at(0),&labels.at(0),data.size(),k,centroids,c,groundTruthCentroids,confusionMatrix);

    std::cout << "overall accuracy = " << overallAccuracy << std::endl;

    std::cout << "printing confusion matrix..." << std::endl;
    std::cout << confusionMatrix << std::endl;










    // test the BICO data structure
    ds.rewind();
    std::cout << "now testing the same dataset using BICO..." << std::endl;

    BICO_lite clusteringMachine = BICO_lite(dim, 500.0, 1400); //n_max = 200k = 1400

    float* dataVec_buf = new float[dim];
    Vec<float> dataVec = Vec<float>(dim,dataVec_buf);

    std::vector<CoresetPair> coreset;

    //for (int i=0;i<k;++i) centroids[i].reset(); 
    //cprime.reset();

    int i = 0;
    while (!ds.done()) {
        tmp = (int*) ds.currPoint();
        label = ds.currLabel();

        //for (int i=0;i<dim;++i) std::cout << tmp[i] << " ";
        //std::cout << "# " << label << std::endl;

        for (int i=0;i<dim;++i) dataVec[i] = (float) tmp[i];

        clusteringMachine.update(dataVec);

        ds.next();
        
        if (i%200 == 0) std::cout << "numpoints processed = " << i << std::endl;

        //if (i%200 == 0 && !ds.done()) { 
        //    coreset = clusteringMachine.queryCoreset();
        //
        //    float score = kmeanspp(coreset, k,  centroids, cprime);
        //    float newOverallAccuracy = accuracy(&data.at(0), &labels.at(0), numpoints, k, centroids, cprime, groundTruthCentroids, confusionMatrix);
        //
        //    std::cout << "num points processed = " << i << ", clustering score = " << score << std::endl; // 7 forest cover types
        //    std::cout << "overall accuracy on the coreset= " << newOverallAccuracy << std::endl;
        //
        //    confusionMatrix.reset();
        //    for (int j=0;j<k;++j) centroids[j].reset(); 
        //    cprime.reset();
        //
        //}

        ++i;

    }


    confusionMatrix.reset();
    for (int i=0;i<k;++i) centroids[i].reset();

    coreset = clusteringMachine.queryCoreset();
    std::cout << "after inserting all the points, BICO generated a coreset of size = " << coreset.size() << std::endl;

    Vec<uint> cprime(coreset.size(),new uint[coreset.size()]);
    cprime.reset();

    float newScore = kmeanspp(coreset,k,centroids,cprime);

    std::cout << "the clustering score of the coreset is " << newScore << std::endl;

    c.reset();
    for (int i=0; i<numpoints; ++i) {
        float mindistance = INFINITY;
        Vec<float> distVec(k,new float[k]); // stores the distances of the point to it's BICO-computed centroid 
        for (int j=0;j<k;++j) distVec[j] = Vec<float>::dist(data.at(i),centroids[j]);
        c[i] = distVec.argMin();
        distVec.clear();
    }

    float overallScore = kmeansclusterscore(&data.at(0),numpoints,k,centroids,c);
    float newOverallAccuracy = accuracy(&data.at(0), &labels.at(0), numpoints, k, centroids, c, groundTruthCentroids, confusionMatrix);

    std::cout << "using BICO-computed centroids as cluster centers, overall classification accuracy = .... " << overallAccuracy << std::endl;
    std::cout << "printing the new confusion matrix..." << std::endl;
    std::cout << confusionMatrix << std::endl;



    delete[] dataVec_buf;
}

int main() {
    //test1();
    //test2();
    //test4();
    //test5();
    test6();
}
