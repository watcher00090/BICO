#pragma once

#include <cassert>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

/**
* Basic implementation of vectors as a boxed pointer
* to externally managed storage.
*/
template <class T> class Vec {
public:
    /**
    * default constructor
    */
    Vec() : n_(0), v_(NULL) {}

    /**
    * construct a vector from a given set of values.
    */
    Vec(uint n, T* v) : n_(n), v_(v) {}

    /**
     * construct a constant vector
     */
    Vec(uint n, T* v, T c) {
        Vec(n,v);
        for (uint i=0;i<n_;++i) v_[i] = c;
    }

    /**
    * copy constructor
    */
    Vec(const Vec<T>& v) : n_(v.n_), v_(v.v_) {}

    void uniform_init(T range) {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distribution(-range,range);
        for (uint i=0; i<n_; ++i) v_[i] = distribution(generator);
    }

    void normal_init() {
        uint seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::normal_distribution<double> distribution(0.0,1.0);
        for (uint i = 0; i<n_; ++i) v_[i] = distribution(generator);
    }

    /**
    * Return the i-th value (R-value)
    */
    T operator[](uint i) const { return v_[i]; }

    /**
    * Return the i-th value (L-value)
    */
    T& operator[](uint i) { return v_[i]; }

    Vec<T>& operator+=(T c) {
        for (uint i=0; i<n_; ++i) v_[i] += c;
        return *this;
    }

    Vec<T>& operator+=(const Vec<T> c) {
        assert(n_==c.size());
        for (uint i=0; i<n_; ++i) v_[i] += c[i];
        return *this;
    }

    Vec<float>& operator*=(float c) {
        for (uint i=0; i<n_; ++i) v_[i] *= c;
        return *this;
    }

    Vec<float>& operator/=(float c) {
        assert(c != 0);
        for (uint i=0; i<n_; ++i) v_[i] /= c;
        return *this;
    }

    // gradient descent step
    void gd(T alpha, T lambda, const Vec<T>& dv) {
        uint n = dv.size();
        assert(n_==n);
        T* v = dv.data();
        for (uint i=0; i<n_; ++i)
            v_[i] -= (alpha*v[i] + lambda*v_[i]);
    }

    /**
    * Output to stream
    */
    friend std::ostream& operator<<(std::ostream& out, const Vec<T>& v) {
        out << "[ ";
        for (uint i = 0; i < v.size(); ++i)
            out << std::setw(10) << std::setprecision(3) << v[i];
        return out << " ]";
    }

    /**
    * explicit copy from a given vector.
    */
    void copyFrom(Vec<T> z) {
        n_ = z.size();
        memcpy(v_, z.v_, n_*sizeof(T));
    }

    /**
    * explicit copy from a given array.
    */
    void copyFrom(uint size, T* z) {
        n_ = size;
        memcpy(v_, z, n_*sizeof(T));
    }

    void copyFrom(uint size, const T* z) {
        n_ = size;
        memcpy(v_, z, n_*sizeof(T));
    }

    /**
    * Return the underlying array of this Vec
    */
    T* getArray() const { return v_; }
    T* data() { return v_; }
    T* data() const { return v_; }

    /**
    * Return Vec size
    */
    uint size() const { return n_; }

    /**
    * Set size
    */
    void setSize(uint n) { n_ = n; }

    /**
    * predicate: check if array is NULL.
    */
    bool isNULL() { return v_==NULL; }

    /**
    * vec.vec multiply
    */
    static float dot(const Vec<T> A, const Vec<T> B) {
        assert(A.n_==B.n_);
        float sum = 0;
        for (uint k = 0; k < A.n_; ++k)
            sum += A[k] * B[k];
        return sum;
    }

    /*
     * Euclidean distance
     */
    static float dist(const Vec<float> A, const Vec<float> B) {
        assert(A.size() == B.size());
        float ret = 0;
        for (int i=0; i<A.size(); ++i) 
            ret += (A[i]-B[i])*(A[i]-B[i]);
        return sqrt(ret);
    }

    /*
     * Euclidean norm
     */
    static float length(const Vec<float> x) {
        float ret = 0;
        for (int i=0; i<x.size(); ++i) 
            ret += x[i]*x[i];
        return sqrt(ret);
    }

    /**
    * vec ⦿ vec, Hadamard product
    */
    static void hmul(const Vec<T> A, const Vec<T> B, Vec<T>& C) {
        assert(A.n_==B.n_);
        assert(A.n_==C.n_);
        for (uint k = 0; k < A.n_; ++k) {
            C[k] = A[k] * B[k];
        }
    }

    /**
    * replace v[i] by (v[i] - μ) / σ 
    */
    void gaussianNormalize() {
        T sum = std::accumulate(v_, v_+n_, 0);
        T mu = sum / n_;
        T var = 0;
        for (uint i=0; i<n_; ++i) {
            v_[i] -= mu;
            var += v_[i]*v_[i];
        }
        T sigma = std::sqrt(var);
        for (uint i=0; i<n_; ++i) v_[i] /= sigma;
    }

    /**
    * return index of largest value
    */
    uint argMax() const {
        uint maxIndex = 0;
        T maxValue = -1e38; //-std::numeric_limits<T>::max();
        for (uint i=0; i<n_; ++i) {
            if (v_[i] > maxValue) {
                maxIndex = i;
                maxValue = v_[i];
            }
        }
        return maxIndex;
    }

    /**
    * return index of largest value
    */
    uint argMin() const {
        uint minIndex = 0;
        T minValue = 1e38; //std::numeric_limits<T>::max();
        for (uint i=0; i<n_; ++i) {
            if (v_[i] < minValue) {
                minIndex = i;
                minValue = v_[i];
            }
        }
        return minIndex;
    }

    // iterator interface
    T* begin() { return v_; }
    T* end() { return v_+n_; }

    // clear - delete underlying array
    void clear() { delete[] v_; v_ = NULL; }

    // reset - reset all entries to 0
    void reset() { memset(v_, 0, n_*sizeof(T)); }

public:
    uint n_;
    T* v_;
};

//
// nxm generic matrix
//
template <class T> class Matrix {
public: 
    /**
    * default constructor : empty matrix
    */
    Matrix() : n_(0), m_(0), sz_(0), v_(NULL) { }

    /**
    * copy constructor : share underlying array
    */
    Matrix(const Matrix<T>& M) : n_(M.n_), m_(M.m_), sz_(M.sz_), v_(M.v_) { }

    /**
    * construct matrix with given dimensions and storage
    * @param n  The row dimension.
    * @param m  The column dimension.
    * @param v  The array - externally allocated.
    */
    Matrix(uint n, uint m, T* v) : n_(n), m_(m), sz_(n*m), v_(v) { }

    void uniform_init(T range) {
        uint seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<T> distribution(-range,range);
        for (uint i=0; i<sz_; ++i) v_[i] = distribution(generator);
    }

    void normal_init() {
        uint seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::normal_distribution<double> distribution(0.0,1.0);
        for (uint i = 0; i<sz_; ++i) v_[i] = distribution(generator);
    }

   /**
    * return (i,j)-th R-value
    */
    T operator()(int i, int j) const {
        return v_[m_*i+j];
    }

    /**
    * return (i,j)-th L-value
    */
    T& operator()(int i, int j) { 
        return v_[m_*i+j];
    }

    /**
    * return the i-th row, then mat[i][j] will work
    */
    T* operator[](uint i) { return v_ + m_*i; }

    /**
    * return the i-th row, then mat[i][j] will work
    */
    const T* operator[](uint i) const { return v_ + m_*i; }

    /**
    * scalar multiply increment operator
    */
    Matrix<T>& operator*=(T c) {
        for (uint i=0; i<sz_; ++i) v_[i] *= c;
        return *this;
    }

    // copy from array
    void copyFrom(const T* w, uint n) {
        memcpy(v_, w, n*sizeof(T));
    }

    /**
    * return the i-th row as a vector
    */
    Vec<T> row(uint i) { return Vec<T>(m_, (*this)[i]); }
    //const Vec<T> row(uint i) const { return Vec<T>(m_, (*this)[i]); }

    /**
    * return underlying array
    */
    T* getArray() const { return v_; }

    /**
    * predicate: check if array is NULL.
    */
    bool isNULL() { return v_==NULL; }

    /**
    * zero out all the entries.
    */
    void reset() {
        if (m_*n_==0) return;
        memset(v_, 0, m_*n_*sizeof(T));
    }

    /**
    * Release storage.
    */
    void clear() { delete[] v_; v_ = NULL; }

    /**
    * Allocate square matrix
    */
    void alloc(uint d) {
        n_ = m_ = d;
        sz_ = n_*m_;
        v_ = new T[sz_];
    }

    /**
    * matrix.matrix multiply
    * A  The first input matrix
    * B  The second input matrix
    * C  The output matrix
    */
    static void mul(const Matrix<T> A, const Matrix<T> B, Matrix<T> C) {
        assert(A.m_==B.n_);
        assert(B.m_==C.n_);
    
        for (uint i=0; i < A.n_; ++i) {
            for (uint j=0; j < B.m_; ++j) {
                float sum = 0;
                for (uint k = 0; k < A.m_; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

    /**
    * simple matrix·vec multiply
    * @param A  The input matrix
    * @param B  The input vec
    * @param C  The output vec
    */
    static void mul(const Matrix<T> A, const Vec<T> B, Vec<T> C) {
        assert(A.m_==B.n_);
    
        for (uint i=0; i < A.n_; ++i) {
            float sum = 0;
            for (uint k = 0; k < A.m_; ++k) {
                sum += A[i][k] * B[k];
            }
            C[i] = sum;
        }
    }

    /**
    * affine matrix·vec multiply
    * @param A  The input matrix, last column = biases
    * @param B  The input vec
    * @param C  The output vec
    */
    static void amul(const Matrix<T> A, const Vec<T> B, Vec<T> C) {
        assert(A.m_==B.n_+1);    // +1 for bias
        assert(A.n_==C.n_);
    
        for (uint i=0; i < A.n_; ++i) {
            float sum = 0;
            for (uint k = 0; k < A.m_-1; ++k) {
                sum += A[i][k]*B[k];
            }
            sum += A[i][A.m_-1];    // bias
            C[i] = sum;
        }
    }

    /**
    * Outer product of two vectors
    */
    static void omul(const Vec<T> A, const Vec<T> B, Matrix<T> C)
    {
        assert(A.size()==C.rowDim());
        assert(B.size()==C.colDim());

        for (uint i=0; i<A.size(); ++i) {
            for (uint j=0; j<B.size(); ++j)
                C[i][j] = A[i]*B[j];
        }
    }

    uint rowDim() const { return n_; }
    uint colDim() const { return m_; }

    uint rows() const { return n_; }
    uint cols() const { return m_; }
    uint& rows() { return n_; }
    uint& cols() { return m_; }

    uint size() const { return sz_; }
    bool square(uint n) const { return (n==n_ && n==m_); }
    bool square() const { return (n_==m_); }

    T* data() { return v_; }
    T* data() const { return v_; }
    T*& getData() { return v_; }

    /**
    * Output to stream
    */
    friend std::ostream& operator<<(std::ostream& out, const Matrix<T>& m) {
        for (uint i=0; i<m.rowDim(); ++i) {
            out << "[ ";
            for (uint j = 0; j < m.colDim(); ++j)
                out << std::setw(10) << std::setprecision(4) << m[i][j];
            out << "]\n";
        }
        return out;
    }

    Matrix<T>& operator+=(const Matrix<T>& Z) {
        T* p = v_;
        T* q = Z.data();
        T* o = v_ + sz_;
        assert(sz_ == Z.size());
        while (p<o) *p++ += *q++;
        return *this;
    }

public: 
    uint n_;    // row dimension
    uint m_;    // column dimension
    uint sz_;   // n_*m_, computed once and for all
    T* v_;      // array storage

};

