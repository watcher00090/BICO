#include "BICO_lite.h"
#include <iterator>
#include <iostream>
#include <vector>
#include <stack>
#include <cmath>

std::ostream& operator<<(std::ostream& os, CoresetPair pair) {
    return os << "(" << pair.p_ << ", " << pair.w_ << ")";
}

std::ostream& operator<<(std::ostream& os, ClusteringFeature f) {
    return os << "[" << "reference point = " << f.r_ << ", n = " << f.n_ << ", P = " << f.P_ << ", s = " << f.s_ << ", level = " << f.level_ << "]";
}

std::ostream& operator<<(std::ostream& os, BICO_lite tree) {
    std::stack<ClusteringFeature*> S;
    S.push(tree.root);

    while (!S.empty()) {
        ClusteringFeature* v = S.top();
        S.pop();

        //perform operation
        for (int k=0; k<4*v->level_; ++k) {
            os << " ";
        }
        os << *v << std::endl;

        for (auto it = v->children.begin(); it != v->children.end(); ++it) { // traverse all children of v
            S.push(*it);
        }
    }
    return os;
}

// used for debugging with lldb
void BICO_lite::toStandardOutput() {
    std::cout << *this << std::endl;
}

void ClusteringFeature::toStandardOutput() {
    std::cout << *this << std::endl;
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

void BICO_lite::CF-update(ClusteringFeature* xFeat) {
    ClusteringFeature* rFeat = root;
    Vec<float> r = rFeat->r_;
    Vec<float> x = xFeat->r_;

    std::vector<ClusteringFeature*> F = rFeat->children;
    unsigned i=1;

    while (true)
    if (F.empty() || Vec<float>::dist(x, ClusteringFeature::nearest(x,F)->r_) > sqrt(T_ / pow(2,i+4))) {

        
        rFeat->children.push_back(xFeat);
        ++nFeatures_;

        break;

    } else {
        ClusteringFeature* yFeat = ClusteringFeature::nearest(x, F);
        Vec<float> y = yFeat->r_;
        xFeat->P_ /= xFeat->n_;
        float len = Vec<float>::length(xFeat->P_);
        float d   = Vec<float>::dist(y,xFeat->P_);
        float cprime = xFeat->n_ * (l*l + d*d);
        xFeat->P_ *= xFeat->n_;

        if (yFeat->level_ + d*d <= T_) {


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
    while (nFeatures_ >= n_max_) {
        T_ = 2*T_;
        std::queue<ClusteringFeature*> Q;
        while (!root->children.empty()) {
            Q.push(root->children.pop_back());
            --nFeatures;
        }

        while (!Q.empty()) {
            ClusteringFeature* cf_x = Q.front();
            Vec<float> x = feat->P_;
            while (!cf_x->children.empty()) {
                Q.push(cf_x->children.pop_back());
                --nFeatures;
            }
            CF-update(cf_x);
            Q.pop(); // remove cf_x from Q
        }
    }
}

// x may be used after calling
void BICO_lite::update(Vec<float> x) {
    ClusteringFeature* rFeat = root;
    Vec<float> r = rFeat->r_;

    std::vector<ClusteringFeature*> F = rFeat->children;
    unsigned i=1;

    float* xArrayCopy = new float[x.size()];
    Vec<float>* xCopy = new Vec<float>(x.size(), xArrayCopy);
    xCopy->copyFrom(x);

    while (true)
    if (F.empty() || Vec<float>::dist(x, ClusteringFeature::nearest(x,F)->r_) > sqrt(T_ / pow(2,i+4))) {

        // open a new feature on level i as child of cf(r)
        ClusteringFeature* newFeature = new ClusteringFeature(*xCopy);
        newFeature->n_ = 1;

        // create P buffer for new feature
        float* newFeaturePbuf = new float[x.size()];
        Vec<float>* P = new Vec<float>(x.size(), newFeaturePbuf);
        P->copyFrom(x);
        newFeature->P_ = *P;

        float l = Vec<float>::length(x);
        newFeature->s_ = l*l;
        newFeature->level_ = i;

        rFeat->children.push_back(newFeature);
        ++nFeatures_;

        break;

    } else {
        ClusteringFeature* yFeat = ClusteringFeature::nearest(x, F);
        Vec<float> y = yFeat->r_;
        float d = Vec<float>::dist(x,y);
        if (yFeat->level_ + d*d <= T_) {
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

std::vector<CoresetPair> BICO_lite::queryCoreset() {
    std::vector<CoresetPair> coreset;
    std::stack<ClusteringFeature*> S;
    S.push(root);

    while (!S.empty()) {
        ClusteringFeature* v = S.top();
        S.pop();
        
        if (!(v->isRoot())) {
            assert(v->n_ != 0);

            Vec<float>* p = new Vec<float>(v->r_.n_, new float[v->r_.n_]);  //allocate memory for the coreset point
            p->copyFrom(v->P_); // sum of all represented points
            *p /= (float) v->n_; // divide by number of points

            CoresetPair* pair = new CoresetPair(*p, (float) v->n_);
            coreset.push_back(*pair);
        }

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
