#pragma once

#include "Matrix.h"
#include "BICO_lite.h"
#include <vector>

float cumulativecentroidshift(const Vec<float>* oldCentroids, const Vec<float>* newCentroids, uint k);

void kmeansppinit(Vec<float>* data, int numpoints, int k, Vec<float>* centroids, std::vector<float>* w=0);
float kmeansclusterscore(Vec<float>* data, int numpoints, int k, Vec<float>* centroids, Vec<uint> c, std::vector<float>* w = 0);
float kmeansclusterscore(CoresetPair* data, int numpoints, int k, Vec<float>* centroids, Vec<uint> c);

void kmeans(Vec<float>* data, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, float epsilon=1E-8, std::vector<float>* w=0);
void kmeans(CoresetPair* pointsAndWeights, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, float epsilon=1E-8);

float kmeanspp(std::vector<CoresetPair> pointsAndWeightsData, unsigned k, Vec<float>* centroids, Vec<uint> c);
float kmeanspp(Vec<float>* data, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c);

float accuracy(Vec<float>* data, uint* labels, uint numpoints, uint k, Vec<float>* centroids, Vec<uint> c, Vec<float>* groundTruthCentroids, Matrix<float> confusionMatrix);
