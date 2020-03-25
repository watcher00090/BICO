#include <vector>
#include "Matrix.h"

namespace xtreml {
class CoresetPair { // (vector,weight) pair
    public: 
        CoresetPair(Vec<float> p, float w) : p_(p), w_(w) {}
        Vec<float> p_; 
        float w_;
};

class ClusteringFeature {
    public:
        ClusteringFeature(Vec<float> r) { r_ = r; };
        ClusteringFeature() {};
        Vec<float> r_;
        unsigned n_; // number of points represented
        Vec<float> P_; // sum of represented points
        float s_; // sum of squared lengths of represented points
        unsigned level; 

        std::vector<ClusteringFeature*> children;
        static ClusteringFeature* nearest(Vec<float> x, std::vector<ClusteringFeature*> F);
        void insert(Vec<float> x);
};

class BICO_lite {
    public:
        BICO_lite();
        BICO_lite(unsigned n_max); // n_max is the largest number of clustering features 
                                   // this object is able to support 
                                   // (intended to be useful on resource-constrained machines)
        void update(xtreml::Vec<float> x);
        std::vector<CoresetPair> queryCoreset();
        void rebuild();
        void cf_update(xtreml::Vec<float> x, ClusteringFeature cf);

        /*
        level1filter[i][0] = elements in level 1 that have their projection length onto u[i] within [0,R_1)
        level1filter[i][1] = elements in level 1 that have their projection length onto u[i] within [R_1, 2*R_1)
        level1filter[i][2] = elements in level 1 that have their projection length onto u[i] within [2*R_1, 3*R_1)
        ..............
        */
        std::vector<std::vector<xtreml::Vec<float>>>* level1filter; 
        Vec<float>* u; // unit vectors generated randomly after a construtor call or rebuild step

        ClusteringFeature* root; 
    private:
        float T_ = 500;
        unsigned n_max_;
        unsigned nFeatures_ = 0;
};
}

std::ostream& operator<<(std::ostream& os, xtreml::CoresetPair pair);
std::ostream& operator<<(std::ostream& os, xtreml::BICO_lite tree);
std::ostream& operator<<(std::ostream& os, xtreml::ClusteringFeature feature);
