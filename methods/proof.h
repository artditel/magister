#pragma once

#include "graph.h"

#include <algorithm>
#include <vector>


template <class G>
class AlphaSetSearcher
{
public:
    AlphaSetSearcher(const G& graph,
            const bool& stopCondition,
            size_t expectedAlpha = 0):
        graph_(graph),
        stopCondition_(stopCondition),
        expectedAlpha_(expectedAlpha),
        curAlpha_(0),
        maxAlpha_(0),
        curVertex_(0),
        verticesCount_(graph.verticesCount()),
        potentialAlpha_(verticesCount_),
        mask_(verticesCount_)
    {
        for (size_t i = 0; i < verticesCount_; ++i) {
            listsStart_.push_back(
                    std::upper_bound(
                        graph.list(i).begin(),
                        graph.list(i).end(),
                        i) -
                    graph.list(i).begin());
        }
    }

    bool search()
    {
        if (stopCondition_ || potentialAlpha_ + curAlpha_ < expectedAlpha_) {
            return false;
        }
        if (curAlpha_ > maxAlpha_) {
            maxAlpha_ = curAlpha_;
            bestMask_ = mask_;
        }
        if (curAlpha_ >= expectedAlpha_) {
            return true;
        }

        bool found = false;
        // Skeep excluded vertices for recursion optimization
        size_t skipCount = 0;
        while (curVertex_ < verticesCount_ && mask_[curVertex_] != 0) {
            ++skipCount;
            ++curVertex_;
        }
        if (curVertex_ == verticesCount_) {
            curVertex_ -= skipCount;
            return false;
        }

        size_t potentialDecrease = 1;
        --potentialAlpha_;

        // try to exclude the vertex
        mask_[curVertex_] = -1;
        found = found || search();

        // try to take the vertex
        ++curAlpha_;
        mask_[curVertex_] = 1;

        const auto& list = graph_.list(curVertex_);
        size_t listsSize_ = list.size();
        for (size_t i = listsStart_[curVertex_]; i < listsSize_; ++i) {
            if (mask_[list[i]] == 0) {
                --potentialAlpha_;
                ++potentialDecrease;
            }
            --mask_[list[i]];
        }
        found = found || search();
        // revert made changes
        --curAlpha_;
        for (size_t i = listsStart_[curVertex_]; i < listsSize_; ++i) {
            ++mask_[list[i]];
        }
        mask_[curVertex_] = 0;
        curVertex_ -= skipCount;
        potentialAlpha_ += potentialDecrease;
        return found;
    }

    int bestAlpha() const
    {
        return maxAlpha_;
    }

    std::vector<size_t> alphaSet() const
    {
        std::vector<size_t> set;
        for (size_t i = 0; i < verticesCount_; ++i) {
            if (bestMask_[i] == i)
                set.push_back(i);
        }
        return set;
    }

private:
    const G& graph_;
    const bool& stopCondition_;
    size_t expectedAlpha_;
    size_t curAlpha_;
    size_t maxAlpha_;
    size_t curVertex_;
    size_t verticesCount_;
    size_t potentialAlpha_;
    std::vector <int> mask_; // 0 - unknown, 1 - taken, < 0 - excluded
    std::vector <int> bestMask_;
    std::vector <size_t> listsStart_;
};
