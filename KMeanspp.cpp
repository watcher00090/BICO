#include <random>
#include <cmath>
#include "Matrix.h"
#include <tuple>
#include <cmath>
#include "KMeanspp.h"
#include <iostream>

#define MAX_ITERATIONS 200000

#define KMEANSDEBUG false


float cumulativecentroidshift(const Vec<float>* oldCentroids, const Vec<float>* newCentroids, uint k) {
    float ret=0;
    for (int i=0; i<k; ++i)
        ret += Vec<float>::dist(oldCentroids[i],newCentroids[i]);
    return ret;
}

/**
 * The k-means++ algorithm initializes the centroids (prior to calling kmeans) as follows:
 *      1. Randomly select the first centroid from the data points.
 *      2. For each data point compute its distance from the nearest, previously choosen centroid.
 *      3. Select the next centroid from the data points such that the probability of choosing a 
 *         point as centroid is directly proportional to its distance from the nearest, previously 
 *         chosen centroid. (i.e. the point having maximum distance from the nearest centroid 
 *         is most likely to be selected next as a centroid).
 *      4. Repeat steps 2 and 3 until k centroids have been sampled.
 */
void kmeansppinit(Vec<float>* data, int numpoints, int k, Vec<float>* centroids, std::vector<float>* w) {
    std::vector<float> distances = std::vector<float>(numpoints,0);
    size_t dim = data[0].size();

    std::vector<float> ones_vec = std::vector<float>(numpoints,1);
    if (w == nullptr) { 
        w = &ones_vec;
    }

    // choose the first centroid uniformly from the data
    int j=0; 
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0,numpoints-1);
    int index = distribution(generator);
     
    centroids[j].copyFrom(data[index]);

    ++j;
    
    for (;j<k;++j) { // chosen the remaining k-1 centroids (j is the number of centroids which have already been chosen)
        distances.clear();
        if (KMEANSDEBUG) std::cout << "j = " << j << std::endl;
        for (int i=0; i<numpoints; ++i) {
            float mindistance = INFINITY;
            for (int t=0; t<j; ++t) { 
                float x = Vec<float>::dist(data[i],centroids[t]);
                float dist = (*w)[i]*x*x;
                if (dist < mindistance) mindistance = dist;
            }
            distances.push_back(mindistance);
            if (KMEANSDEBUG) std::cout << "distances[" << i << "] = " << distances[i] << std::endl;
        }

        unsigned seed0 = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine gen(seed0);
        std::discrete_distribution<int> dist(distances.begin(),distances.end());
        uint index = dist(gen);

        centroids[j].copyFrom(data[index]);
    }

    /*
    uint iterations=1000;
    std::vector<float> disthist = std::vector<float>(numpoints,0);
    for (int l=0;l<iterations;++l) {
        int val = dist(gen);
        disthist[val]++;
    }
    */
    //for (int l=0;l<numpoints;++l) std::cout << l << " generated " << disthist[l] << " times" << std::endl;

}

float kmeansclusterscore(Vec<float>* data, int numpoints, int k, Vec<float>* centroids, Vec<uint> c, std::vector<float>* w) {
    float score = 0;
    std::vector<float> ones_vec = std::vector<float>(numpoints,1);
    if (w == nullptr) { 
        w = &ones_vec;
    }
    for (int i=0; i<numpoints; ++i) {
        float x = Vec<float>::dist(centroids[c[i]], data[i]);
        score += (*w)[i]*x*x;
    }    
    return score;
}

float kmeansclusterscore(CoresetPair* data, int numpoints, int k, Vec<float>* centroids, Vec<uint> c) {
    float score = 0;
    for (int i=0; i<numpoints; ++i) {
        float x = Vec<float>::dist(centroids[c[i]], data[i].p_);
        score += ((data[i].w_)*x*x);
    }    
    return score;
}

// runs K-means on the given coreset
void kmeans(Vec<float>* data, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, float epsilon, std::vector<float>* w) {

    std::vector<float> ones_vec = std::vector<float>(numpoints,1);
    if (w == nullptr) { 
        w = &ones_vec;
    }

    Vec<float>* oldCentroids = new Vec<float>[k];
    size_t dim = centroids[0].size();
    for (int i=0;i<k;++i) {
        oldCentroids[i] = Vec<float>(dim,new float[dim]);
        oldCentroids[i].copyFrom(centroids[i]);
        if (KMEANSDEBUG) std::cout << "oldCentroids[" << i << "] = " << oldCentroids[i] << std::endl;
    }

    int iterations = 0;
    c.reset();

    while ((!(cumulativecentroidshift(centroids,oldCentroids,k)<epsilon) || iterations==0) && iterations<MAX_ITERATIONS) {
        // update old centroids
        if (KMEANSDEBUG) std::cout << "starting iteration..." << std::endl;
        if (KMEANSDEBUG) std::cout << "printing centroids..." << std::endl;
        for (int i=0; i<k; ++i) {
            if (KMEANSDEBUG) std::cout << "centroids[" << i << "] = " << centroids[i] << ", ";
            if (KMEANSDEBUG) std::cout << "oldCentroids[" << i << "] = " << oldCentroids[i] << std::endl;
            oldCentroids[i].copyFrom(centroids[i]);
        }
        if (KMEANSDEBUG) std::cout << "printing c vector..." << std::endl;
        if (KMEANSDEBUG) std::cout << "c = " << c << std::endl;
        // assign each point to a cluster 
        for (int i=0;i<numpoints;++i) {
            float min_distance = INFINITY;
            for (int j=0; j<k; ++j) {
                float x = Vec<float>::dist(data[i],centroids[j]);
                float dist = x*x;
                if (dist < min_distance) {
                    min_distance = dist;
                    c[i] = j;
                }
            }
        }
        // compute new cluster centers
        for (int j=0;j<k;++j) {
            bool updatingCentroidj = false;
            float weightsum = 0;
            for (int i=0; i<numpoints; ++i) {
                if (c[i]==j) { 
                    if (!updatingCentroidj) {
                        updatingCentroidj = true;
                        centroids[j].reset();
                    }
                    data[i] *= (*w)[i];
                    centroids[j] += data[i];
                    weightsum += (*w)[i];
                    if ((*w)[i] == 0) {
                        std::cout << "w->size() = " << w->size() << std::endl;
                        std::cout << "i = " << i << std::endl;
                    }
                    data[i] /= (*w)[i];
                }
            }
            // if the cluster doesn't contain any points, the previous centroid is used as the current centroid
            if (updatingCentroidj) centroids[j] /= weightsum;
        }
        iterations++;
        if (KMEANSDEBUG) std::cout << std::endl;
        if (KMEANSDEBUG) std::cout << "current cluster score = " << kmeansclusterscore(data,numpoints,k,centroids,c) << std::endl;
    }
    if (KMEANSDEBUG) std::cout << "kmeans call completing, numiterations run: " << iterations << std::endl;
}

void kmeans(CoresetPair* data, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, float epsilon) {

    Vec<float>* oldCentroids = new Vec<float>[k];
    size_t dim = centroids[0].size();
    for (int i=0;i<k;++i) {
        oldCentroids[i] = Vec<float>(dim,new float[dim]);
        oldCentroids[i].copyFrom(centroids[i]);
        if (KMEANSDEBUG) std::cout << "oldCentroids[" << i << "] = " << oldCentroids[i] << std::endl;
    }

    int iterations = 0;
    c.reset();

    while ((!(cumulativecentroidshift(centroids,oldCentroids,k)<epsilon) || iterations==0) && iterations<MAX_ITERATIONS) {
        // update old centroids
        if (KMEANSDEBUG) std::cout << "starting iteration..." << std::endl;
        if (KMEANSDEBUG) std::cout << "printing centroids..." << std::endl;
        for (int i=0; i<k; ++i) {
            if (KMEANSDEBUG) std::cout << "centroids[" << i << "] = " << centroids[i] << ", ";
            if (KMEANSDEBUG) std::cout << "oldCentroids[" << i << "] = " << oldCentroids[i] << std::endl;
            oldCentroids[i].copyFrom(centroids[i]);
        }
        if (KMEANSDEBUG) std::cout << "printing c vector..." << std::endl;
        if (KMEANSDEBUG) std::cout << "c = " << c << std::endl;
        // assign each point to a cluster 
        for (int i=0;i<numpoints;++i) {
            float min_distance = INFINITY;
            for (int j=0; j<k; ++j) {
                float x = Vec<float>::dist(data[i].p_,centroids[j]);
                float dist = data[i].w_*x*x;
                if (dist < min_distance) {
                    min_distance = dist;
                    c[i] = j;
                }
            }
        }
        // compute new cluster centers
        for (int j=0;j<k;++j) {
            bool updatingCentroidj = false;
            float weightsum = 0;
            for (int i=0; i<numpoints; ++i) {
                if (c[i]==j) { 
                    if (!updatingCentroidj) {
                        updatingCentroidj = true;
                        centroids[j].reset();
                    }
                    data[i].p_ *= data[i].w_;
                    centroids[j] += data[i].p_;
                    weightsum += data[i].w_;
                    data[i].p_ /= data[i].w_;
                }
            }
            // if the cluster doesn't contain any points, the previous centroid is used as the current centroid
            if (updatingCentroidj) centroids[j] /= weightsum;
        }
        iterations++;
        if (KMEANSDEBUG) std::cout << std::endl;
        if (KMEANSDEBUG) std::cout << "current cluster score = " << kmeansclusterscore(data,numpoints,k,centroids,c) << std::endl;
    }
    if (KMEANSDEBUG) std::cout << "kmeans call completing, numiterations run: " << iterations << std::endl;
}


float kmeanspp(std::vector<CoresetPair> pointsAndWeightsData, uint k, Vec<float>* centroids, Vec<uint> c) {
    CoresetPair* coresetPairs = &pointsAndWeightsData.at(0);

    uint dim = pointsAndWeightsData.at(0).p_.size();
    std::vector<Vec<float>> pointsVec;
    std::vector<float> wVec;
    for (int i=0; i<pointsAndWeightsData.size(); ++i) {
        pointsVec.push_back(pointsAndWeightsData.at(i).p_);
        wVec.push_back(pointsAndWeightsData.at(i).w_);
    }

    kmeansppinit(&pointsVec.at(0),pointsVec.size(),k,centroids,&wVec);
    kmeans(&pointsVec.at(0),pointsVec.size(),k,centroids,c,1E-8,&wVec);
    return kmeansclusterscore(coresetPairs, pointsAndWeightsData.size(), k, centroids, c);
}

float kmeanspp(Vec<float>* data, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c) {
    kmeansppinit(data,numpoints,k,centroids);
    kmeans(data,numpoints,k,centroids,c);
    return kmeansclusterscore(data,numpoints,k,centroids,c);
}

// Use the metric that all points whose computed centroid is closest to the ground truth centroid for
// cluster k (as opposed to any other ground truth) centroid belong to ground truth cluster k.
float accuracy(Vec<float>* data, uint* labels, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, Vec<float>* groundTruthCentroids, Matrix<float> confusionMatrix) {
    float overallaccuracy = 0;
    for (int i=0; i<numpoints; ++i) {
        float mindistance = INFINITY;
        Vec<float> distVec(k,new float[k]); // stores the distances between the point's computed centroid and all the ground truth centroids
        for (int j=0;j<k;++j) distVec[j] = Vec<float>::dist(centroids[c[i]],groundTruthCentroids[j]);
        uint computedgroundtruthcluster = distVec.argMin();
        uint actualgroundtruthcluster = labels[i]-1;

        confusionMatrix[computedgroundtruthcluster][actualgroundtruthcluster]++;

        overallaccuracy += (computedgroundtruthcluster == actualgroundtruthcluster ? 1 : 0);
        distVec.clear();
    }
    overallaccuracy /= numpoints;

    // normalize each column by the sum of its elements
    for (int j=0; j<confusionMatrix.cols(); ++j) {
        float colsum = 0; 
        for (int i=0; i<confusionMatrix.rows(); ++i) {
            colsum += confusionMatrix[i][j];
        }
        for (int i=0; i<confusionMatrix.rows(); ++i) {
            confusionMatrix[i][j] /= colsum;
        }
    }

    return overallaccuracy;
}




