#pragma once

#include "graph.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <cmath>

template<typename A, typename B, typename T>
inline T masked_dot(A maskBegin, A maskEnd, B begin, T res)
{
    for (;maskBegin != maskEnd; ++maskBegin, ++begin) {
        if (*maskBegin)
            res += *begin;
    }
    return res;
}

template <class G>
int estimateChi(const G& g, float alphaUp)
{
    return ceil(float(g.verticesCount()) / alphaUp);
}

size_t estimateChi(size_t verticesCount, size_t alphaUp)
{
    if (verticesCount % alphaUp == 0)
        return verticesCount/alphaUp;
    return verticesCount/alphaUp + 1;
}

std::string vertexToString(const Vertex& v)
{
    std::ostringstream s;
    std::ostream_iterator<int> it(s, "\t");
    std::copy(v.begin(), v.end(), it);
    return s.str();
}

template <class G>
G removeVertices(const G& g, std::vector<bool>& mask)
{
    Vertices vertices;
    for (size_t i = 0; i < g.verticesCount(); ++i) {
        if (mask[i])
            vertices.push_back(g.vertex(i));
    }
    return G(g, vertices);
}

template <class G>
G fixAndRemoveVertex(const G& g, size_t id)
{
    std::vector<bool> mask(g.verticesCount(), true);
    mask[id] = false;
    for (auto v: g.list(id))
        mask[v] = false;
    return removeVertices(g, mask);
}


template <class G>
G removeVertices(const G& g, std::vector<size_t> ids)
{
    std::vector <bool> mask(g.verticesCount(), true);
    for (auto id : ids)
        mask[id] = false;
    return removeVertices(g, mask);
}

template <class G>
G fixAndRemoveTwoVertices(const G& g, int intersection)
{
    for (int i = 0; i < g.verticesCount(); ++i)
        if (g.intersection(0, i) == intersection) {
            return fixAndRemoveVertex(fixAndRemoveVertex(g, i), 0);
        }
    throw 1;
}
