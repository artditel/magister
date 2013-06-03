#pragma once

#include "pool.h"

#include <vector>
#include <algorithm>
#include <iostream>

typedef std::vector<int> Vertex;
typedef std::vector<Vertex> Vertices;

class GraphBase
{
public:
    GraphBase(int n, int t): n_(n), t_(t)
    {}

    GraphBase(int n, int t, const Vertices& vertices):
        n_(n), t_(t), vertices_(vertices)
    {}

    int n() const { return n_; }
    int t() const { return t_; }

    size_t verticesCount() const
    {
        return vertices_.size();
    }

    const Vertex& vertex(size_t id) const
    {
        return vertices_[id];
    }

    int intersection(size_t x, size_t y) const
    {
        int res = 0;
        const auto& vx = vertices_[x];
        const auto& vy = vertices_[y];
        for (size_t k_ = 0; k_ < n_; ++k_)
            res += vx[k_] * vy[k_];
        return res;
    }

    bool hasEdge(size_t x, size_t y) const
    {
        return intersection(x, y) == t_;
    }

protected:
    int n_;
    int t_;
    Vertices vertices_;
};

enum class Storage {
    Matrix,
    Lists
};

template <Storage S>
class Graph;
/*class Graph: public GraphBase{
protected:
    void generateData()
    {}
};
*/

template <>
class Graph<Storage::Matrix>: public GraphBase
{
public:
    Graph(int n, int t): GraphBase(n, t)
    {}

    Graph(int n, int t, const Vertices& vertices): GraphBase(n, t, vertices)
    {}

    size_t degree(size_t v = 0) const
    {
        size_t degree = 0;
        for (auto x : matrix_[v])
            degree += x;
        return degree;
    }

    bool isRegular() const
    {
        size_t degree0 = degree();

        for (size_t v = 1; v < verticesCount(); ++v) {
            if (degree(v) != degree0)
                return false;
        }
        return true;
    }

    // interface for ARPACK
    size_t nrows() const { return vertices_.size(); }
    size_t ncols() const { return vertices_.size(); }

    template<class ART>
    void MultMv(ART* v, ART* w)
    {
        pool_.mapChunks(
            ncols(),
            [v, w, &matrix_] (size_t down, size_t up) {
                for (size_t i = down; i < up; ++i)
                    w[i] = masked_dot(
                        matrix_[i].begin(), matrix_[i].end(), v, ART(0.0));
            });
    }

protected:
    void generateData()
    {
        matrix_.resize(ncols());
        for (int i = 0; i < ncols(); ++i) {
            matrix_[i].resize(ncols());
        }

        pool_.mapChunks(
            ncols(),
            [&matrix_, &vertices_, this] (size_t down, size_t up) {
                for (size_t i = down; i < up; ++i)
                    for (size_t j = i; j < ncols(); ++j)
                        matrix_[i][j] = matrix_[j][i] = hasEdge(i, j);
            });
    }

private:
    std::vector<std::vector<bool> > matrix_;
    ThreadPool pool_;
};


template <>
class Graph<Storage::Lists>: public GraphBase
{
public:
    typedef std::vector<size_t> List;

    Graph(int n, int t): GraphBase(n, t)
    {}

    Graph(int n, int t, const Vertices& vertices): GraphBase(n, t, vertices)
    {}

    const List& list(int v) const
    {
        return lists_[v];
    }

    bool addEdges(int intersection)
    {
        bool added = false;
        for (size_t i = 0; i < verticesCount(); ++i) {
            for (size_t j = i + 1; j < verticesCount(); ++j) {
                if (this->intersection(i, j) == intersection) {
                    added = true;
                    addEdge(i, j);
                }
            }
        }
        if (!added)
            return false;
        for (auto& list: lists_)
            std::sort(list.begin(), list.end());
        return true;
    }

protected:
    void generateData()
    {
        lists_.resize(verticesCount());
        addEdges(t_);
    }

private:
    void addEdge(int x, int y)
    {
        lists_[x].push_back(y);
        lists_[y].push_back(x);
    }

    std::vector <List> lists_;
};

template <Storage S>
class B: public Graph<S> {
public:
    B(int n, int k, int t): Graph<S>(n, t), k_(k)
    {
        generateVertices(std::vector<int>(), 0);
        Graph<S>::generateData();
    }

    B(const B& other, const Vertices& vertices):
        Graph<S>(other.n_, other.t_, vertices),
        k_(other.k_)
    {
        Graph<S>::generateData();
    }

protected:
    int k_;

private:
    void generateVertices(std::vector<int> v, int onesUsed)
    {
        if (onesUsed > k_)
            return;
        if (v.size() == Graph<S>::n_) {
            if (onesUsed == k_)
                Graph<S>::vertices_.push_back(v);
            return;
        }
        v.push_back(0);
        generateVertices(v, onesUsed);
        v.back() = 1;
        generateVertices(v, onesUsed + 1);
    }
};

template <Storage S>
class T: public Graph<S>
{
public:
    T(int n, int kp, int kn, int t): Graph<S>(n,t), kp_(kp), kn_(kn)
    {
        generateVertices(std::vector<int>(), 0, 0);
        Graph<S>::generateData();
    }

    T(const T<S>& other, const Vertices& vertices):
        Graph<S>(other.n_, other.t_, vertices),
        kp_(other.kp_), kn_(other.kn_)
    {
        Graph<S>::generateData();
    }

protected:
    int kp_, kn_;

private:
    void generateVertices(std::vector<int> v, int pUsed, int nUsed)
    {
        if (pUsed > kp_ || nUsed > kn_)
            return;
        if (v.size() == Graph<S>::n_) {
            if (pUsed == kp_ && nUsed == kn_)
                Graph<S>::vertices_.push_back(v);
            return;
        }
        v.push_back(0);
        generateVertices(v, pUsed, nUsed);
        v.back() = -1;
        generateVertices(v, pUsed, nUsed + 1);
        v.back() = 1;
        generateVertices(v, pUsed + 1, nUsed);
    }
};

template <Storage S>
class Th: public Graph<S>
{
public:
    Th(int n, int k, int t): Graph<S>(n, t), k_(k)
    {
        generateVertices(std::vector<int>(), 0);
        Graph<S>::generateData();
    }

    Th(const Th<S>& other, const Vertices& vertices):
        Graph<S>(other.n_, other.t_, vertices),
        k_(other.k_)
    {
        Graph<S>::generateData();
    }

protected:
    int k_;

private:
    void generateVertices(std::vector<int> v, int onesUsed)
    {
        if (onesUsed > k_)
            return;
        if (v.size() == Graph<S>::n_) {
            if (onesUsed == k_)
                Graph<S>::vertices_.push_back(v);
            return;
        }
        v.push_back(0);
        generateVertices(v, onesUsed);
        if (onesUsed != 0) {
            v.back() = -1;
            generateVertices(v, onesUsed + 1);
        }
        v.back() = 1;
        generateVertices(v, onesUsed + 1);
    }
};
