#include "graph.h"
#include "utils.h"
#include "pool.h"

#include <vector>
#include <mutex>
#include <iostream>
#include <string>

template <class G>
size_t fastAlpha(const G& g, std::vector<char>* used, std::vector<size_t>* order)
{
    order->resize(g.verticesCount());
    for (size_t i = 0; i < order->size(); ++i)
        (*order)[i] = i;
    std::random_shuffle(order->begin(), order->end());
    used->assign(g.verticesCount(), false);
    size_t alpha = 0;
    for (size_t v: *order) {
        bool canBeAdded = true;
        for (size_t w: g.list(v)) {
            if ((*used)[w]) {
                canBeAdded = false;
                break;
            }
        }
        if (canBeAdded) {
            ++alpha;
            (*used)[v] = true;
        }
    }
    return alpha;
}

template <class G>
std::pair<size_t, size_t> maxAlpha(const G& g, size_t iterations)
{
    ThreadPool pool;
    std::mutex alphaMutex;
    size_t maxAlpha = 0;
    pool.mapChunks(iterations, [&g, &maxAlpha, &alphaMutex]
            (size_t from, size_t to) {
        size_t localMaxAlpha = 0;
        std::vector <size_t> order;
        std::vector <char> used;
        for (; from < to; ++from) {
            localMaxAlpha = std::max(
                localMaxAlpha,
                fastAlpha(g, &used, &order));
            if (g.verticesCount() == localMaxAlpha)
                break;
        }

        std::lock_guard<std::mutex> lock(alphaMutex);
        maxAlpha = std::max(maxAlpha, localMaxAlpha);
    });
    return {maxAlpha, estimateChi(g.verticesCount(), maxAlpha)};
}

int main(int argc, char** argv)
{
    if (argc < 5 || argc > 7) {
        std::cerr << "Usage: " << argv[0] <<
            " graph-type n k t [kpos] [iterations]" << std::endl;
        return 1;
    }
    std::string graphType(argv[1]);
    int n, k, t, kPos;
    size_t iterations = 10;
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    t = atoi(argv[4]);
    if (graphType == "t") {
        kPos = atoi(argv[5]);
        if (argc > 6)
            iterations = atoi(argv[6]);
    } else {
        if (argc > 5)
            iterations = atoi(argv[5]);
    }

    std::pair<size_t, size_t> alphaChi;
    if (graphType == "b") {
        alphaChi = maxAlpha(B<Storage::Lists>(n, k, t), iterations);
    } else if (graphType == "th") {
        alphaChi = maxAlpha(Th<Storage::Lists>(n, k, t), iterations);
    } else if (graphType == "t") {
        alphaChi = maxAlpha(
                T<Storage::Lists>(n, kPos, k - kPos, t),
                iterations);
    }

    std::cout << alphaChi.first << " " << alphaChi.second << std::endl;
}
