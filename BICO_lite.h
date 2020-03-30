#pragma once

#include <vector>
#include "Matrix.h"

class CoresetPair { // (vector,weight) pair
    public: 
        CoresetPair(Vec<float> p, float w) : p_(p), w_(w) {}
        Vec<float> p_; 
        float w_;
};

class ClusteringFeature {
    public:
        ClusteringFeature(Vec<float> r, bool isroot) : r_(r), n_(), P_(), s_(), level_(), isroot_(isroot) {};
        ClusteringFeature(Vec<float> r) : r_(r), n_(), P_(), s_(), level_() {};
        ClusteringFeature() : r_(), n_(), P_(), s_(), level_() {};
        Vec<float> r_;
        unsigned n_; // number of points represented
        Vec<float> P_; // sum of represented points
        float s_; // sum of squared lengths of represented points
        float c_;
        unsigned level_; 

        std::vector<ClusteringFeature*> children;
        static ClusteringFeature* nearest(Vec<float> x, std::vector<ClusteringFeature*> F);
        void insert(Vec<float> x);
        void toStandardOutput();
        bool isRoot() { return isroot_; };

     private:
        bool isroot_ = 0;
};

class BICO_lite {
    public:
        BICO_lite() : root(new ClusteringFeature(Vec<float>(0, nullptr),1)) {};
        BICO_lite(unsigned d) : root(new ClusteringFeature(Vec<float>(d, new float[d]),1)) {};
        BICO_lite(unsigned d, unsigned n_max) : n_max_(n_max), root(new ClusteringFeature(Vec<float>(d, new float[d]),1)) {};
        BICO_lite(unsigned d, float T) : T_(T), root(new ClusteringFeature(Vec<float>(d, new float[d]),1)) {};
        BICO_lite(unsigned d, float T, unsigned n_max) : n_max_(n_max), T_(T), root(new ClusteringFeature(Vec<float>(d, new float[d]),1)) {};
        void update(Vec<float> x);
        std::vector<CoresetPair> queryCoreset();
        void rebuild();
        void cf_update(Vec<float> x, ClusteringFeature cf);
        void toStandardOutput();

        /*
        level1filter[i][0] = elements in level 1 that have their projection length onto u[i] within [0,R_1)
        level1filter[i][1] = elements in level 1 that have their projection length onto u[i] within [R_1, 2*R_1)
        level1filter[i][2] = elements in level 1 that have their projection length onto u[i] within [2*R_1, 3*R_1)
        ..............
        */
        std::vector<std::vector<Vec<float>>>* level1filter; 
        Vec<float>* u; // unit vectors generated randomly after a construtor call or rebuild step

        ClusteringFeature* root; 
    private:
        float T_ = 500;
        unsigned n_max_;
        unsigned nFeatures_ = 0;
};

std::ostream& operator<<(std::ostream& os, CoresetPair pair);
std::ostream& operator<<(std::ostream& os, BICO_lite tree);
std::ostream& operator<<(std::ostream& os, ClusteringFeature feature);
