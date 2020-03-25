#include "BICO_lite.h"
#include <iterator>
#include "iostream"
#include <vector>
#include <stack>
#include <cmath>

using namespace xtreml;

BICO_lite::BICO_lite() {
    std::cout << "constructor being called..." << std::endl;
}

std::ostream& operator<<(std::ostream& os, CoresetPair pair) {
    return os << "(" << pair.p_ << ", " << pair.w_ << ")";
}

std::ostream& operator<<(std::ostream& os, ClusteringFeature f) {
    return os << "[" << "reference point = " << f.r_ << ", n = " << f.n_ << ", P = " << f.P_ << ", s = " << f.s_ << ", level = " << f.level << "]";
}

std::ostream& operator<<(std::ostream& os, BICO_lite tree) {
    std::stack<ClusteringFeature*> S;
    S.push(tree.root);

    while (!S.empty()) {
        ClusteringFeature* v = S.top();
        S.pop();

        //perform operation
        for (int k=0; k<4*v->level; ++k) {
            os << " ";
        }
        os << *v << std::endl;

        int i=0;
        for (auto it = v->children.begin(); it != v->children.end(); ++it) { // traverse all children of v
            S.push(*it);
        }
    }
    return os;
}

// assumes the feature has been initialized
void ClusteringFeature::insert(Vec<float> x) {
    if (r_.v_ == nullptr) { // r_ has not been initialized
        r_ = x; // set the reference point to x
    } 
    P_ += x;
    float l = Vec<float>::length(x);
    s_ += (l*l);
    ++n_;
}

// x may be used after calling
void BICO_lite::update(Vec<float> x) {
    ClusteringFeature* rFeat = root;
    Vec<float> r = rFeat->r_;

    std::vector<ClusteringFeature*> F = rFeat->children;
    unsigned i=1;

    float* xArrayCopy = new float[x.size()];
    Vec<float> xCopy = Vec<float>(x.size(), xArrayCopy);
    xCopy.copyFrom(x);

    while (true)
    if (F.empty() || Vec<float>::dist(x, ClusteringFeature::nearest(x,F)->r_) > sqrt(T_ / pow(2,i+4))) {

        // open a new feature on level i as child of cf(r)
        ClusteringFeature newFeature = ClusteringFeature(xCopy);
        newFeature.n_ = 1;

        // create P buffer for new feature
        float* newFeaturePbuf = new float[x.size()];
        for (int i=0; i<x.size(); ++i) newFeaturePbuf[i] = 0;
        Vec<float> P = Vec<float>(x.size(), newFeaturePbuf);
        newFeature.P_ = P;

        float l = Vec<float>::length(x);
        newFeature.s_ = l*l;
        newFeature.level = 0;

        rFeat->children.push_back(&newFeature);
        ++nFeatures_;
        break;

    } else {
        ClusteringFeature* yFeat = ClusteringFeature::nearest(x, F);
        Vec<float> y = yFeat->r_;
        float d = Vec<float>::dist(x,y);
        if (yFeat->level + d*d <= T_) {
            yFeat->insert(x);
            break;
        } else {
            F = yFeat->children;
            rFeat = yFeat; r = y;
            ++i;
        }
    }

    if (nFeatures_ > n_max_) rebuild();
}

void BICO_lite::rebuild() {

}

std::vector<CoresetPair> BICO_lite::queryCoreset() {
    std::vector<CoresetPair> coreset;
    std::stack<ClusteringFeature*> S;
    S.push(root);

    while (!S.empty()) {
        ClusteringFeature* v = S.top();
        S.pop();

        assert(v->n_ != 0);

        Vec<float> p(v->r_.n_, new float[v->r_.n_]);  //allocate memory for the coreset point
        p /= (float) v->n_;

        coreset.push_back(CoresetPair(p, (float) v->n_));

        for (auto it = v->children.begin(); it != v->children.end(); ++it) { // traverse all children of v
            S.push(*it);
        }
    }
    return coreset;
}

ClusteringFeature* ClusteringFeature::nearest(Vec<float> x, std::vector<ClusteringFeature*> F) {
    ClusteringFeature* nearest;
    float mindistance = INFINITY;
    for (auto it = F.begin(); it != F.end(); ++it) {
        float distance = Vec<float>::dist(x, (*it)->r_);
        if (distance < mindistance) {
            mindistance = distance;
            nearest = *it;
        }
    }
    return nearest;
}
