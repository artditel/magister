#include "eig.h"
#include "graph.h"
#include "utils.h"

#include <iostream>
#include <cstdlib>

template<class G>
float estimateAlpha(const G& g, float eig)
{
    return g.verticesCount() * fabs(eig) / (g.degree() + std::abs(eig));
}

template<class G>
void eigMethod(G* g)
{
    if (!g->isRegular())
        return;
    float alphaUp = estimateAlpha(*g, lowestEig(g));
    std::cout << static_cast<int>(ceil(alphaUp)) << " "
        << estimateChi(*g, alphaUp) << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 5 && argc != 6) {
        std::cerr << "Usage: " << argv[0] <<
            " graph-type n k t [kpos] " << std::endl;
        return 1;
    }
    std::string graphType(argv[1]);
    int n, k, t, kPos;
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    t = atoi(argv[4]);
    if (graphType == "t")
        kPos = atoi(argv[5]);
    if (graphType == "b") {
        B<Storage::Matrix> g(n, k, t);
        eigMethod(&g);
    } else if (graphType == "th") {
        Th<Storage::Matrix> g(n, k, t);
        eigMethod(&g);
    } else if (graphType == "t") {
        T<Storage::Matrix> g(n, kPos, k - kPos, t);
        eigMethod(&g);
    }
}
