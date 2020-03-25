#include <random>
#include <cmath>
#include "Matrix.h"
#include <tuple>
#include <cmath>
#include "KMeanspp.h"

#define MAX_ITERATIONS 2000

#define KMEANSDEBUG false

namespace xtreml {


float cumulativecentroidshift(const Vec<float>* oldCentroids, const Vec<float>* newCentroids, uint k) {
    float ret=0;
    for (int i=0; i<k; ++i)
        ret += Vec<float>::dist(oldCentroids[i],newCentroids[i]);
    return ret;
}

/**
 * The k-means++ algorithm initializes the centroids prior to calling kmeans as follows:
 *      1. Randomly select the first centroid from the data points.
 *      2. For each data point compute its distance from the nearest, previously choosen centroid.
 *      3. Select the next centroid from the data points such that the probability of choosing a 
 *         point as centroid is directly proportional to its distance from the nearest, previously 
 *         chosen centroid. (i.e. the point having maximum distance from the nearest centroid 
 *         is most likely to be selected next as a centroid)
 *      4. Repeat steps 2 and 3 until k centroids have been sampled
 */
void kmeansppinit(Vec<float>* data, int numpoints, int k, Vec<float>* centroids, std::vector<float>* w) {
    std::vector<float> distances = std::vector<float>(numpoints,0);
    size_t dim = data[0].size();

    if (w == nullptr) { 
        std::vector<float> ones_vec = std::vector<float>(numpoints,1);
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
    if (w == nullptr) { 
        std::vector<float> ones_vec = std::vector<float>(numpoints,1);
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
void kmeans(CoresetPair* pointsAndWeights, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, float epsilon=1E-8) {

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
                float dist = (*w)[i]*x*x;
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
                    centroids[j] += (*w)[i]*data[i]; 
                    weightsum += (*w)[i];
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
                    centroids[j] += (data[i].w_*data[i].p_); 
                    weightsum += data[i].w_;
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


float kmeans(std::vector<CoresetPair> pointsAndWeightsData, unsigned k) {
    xtreml::CoresetPair* coresetPairs = &pointsAndWeightsData.at(0);
    Vec<float>* centroids = new Vec<float>[k];
    Vec<float>* c = new Vec<float>[k];
    kmeans(coresetPairs, coresetPairs.size(), k, centroids, c);
    return kmeansclusterscore(coresetPairs, coresetPairs.size(), k, centroids, c);
    delete[] centroids;
    delete[] c;
}

}
