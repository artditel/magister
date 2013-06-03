#include "proof.h"
#include "graph.h"
#include "utils.h"
#include <thread>
#include <atomic>
#include <sstream>

enum class FastCheck {
    No, Yes
};

template <class G>
bool checkAlpha(const G& g, size_t expected, FastCheck fastCheck = FastCheck::Yes)
{
    static ThreadPool pool;

    if (fastCheck == FastCheck::No) {
        bool stopCondition = false;
        AlphaSetSearcher<G> searcher(g, stopCondition, expected);
        return searcher.search();
    }

    std::vector <size_t> tasks;
    for (size_t i = 0; i + 2 < g.verticesCount(); ++i)
        tasks.push_back(i);

    return pool.searchAny(
        tasks,
        [&g, expected] (size_t v, bool& stopCondition) -> bool {
            std::vector <size_t> skippedVertices(v);
            for (size_t i = 0; i < v; ++i)
                skippedVertices.push_back(i);
            G reduced = removeVertices(g, skippedVertices);
            reduced = fixAndRemoveVertex(reduced, 0);
            if (reduced.verticesCount() < expected-1)
                return false;
            AlphaSetSearcher<G> searcher(reduced, stopCondition, expected - 1);
            if (searcher.search())
                stopCondition = true;
        });
}

template <class G>
int findIntersection(const G& g, size_t expected)
{
    int bestIntersection = -1;
    size_t bestVerticesCount = 0;
    for (int intersection = 0; intersection < g.n() / 2; ++intersection) {
        if (intersection == g.t())
            continue;
        G reduced = g;
        if (!reduced.addEdges(intersection))
            continue;
        if (!checkAlpha(reduced, expected - 1, FastCheck::No)) {
            size_t verticesCount =
                fixAndRemoveTwoVertices(g, intersection).verticesCount();
            if (verticesCount > bestVerticesCount) {
                bestIntersection = intersection;
                bestVerticesCount = verticesCount;
            }
        }
    }
    return bestIntersection;
}

template <class G>
void testGraph(G g)
{
    int intersection = -1;
    size_t addendAlpha = 0;

    size_t intialVerticesCount = g.verticesCount();
    std::cerr << "Graph with " << g.verticesCount() <<
        " vertices" << std::endl;
    for (size_t expectedAlpha = 2; ; ++expectedAlpha)
    {
        std::cerr << expectedAlpha << std::endl;
        if (expectedAlpha >=g.n() &&
                expectedAlpha <= 2 * g.n() &&
                intersection == -1) {
            intersection = findIntersection(g, expectedAlpha);
            if (intersection >= 0) {
                std::cerr << "Intersection found: "
                    << intersection << std::endl;
                g = fixAndRemoveTwoVertices(g, intersection);
                std::cerr << "Graph with " <<
                    g.verticesCount() << " vertices" << std::endl;
                addendAlpha = 2;
            }
        }
        if (checkAlpha(g, expectedAlpha - addendAlpha)) {
            std::cerr << "Set found: " << expectedAlpha << std::endl;
        } else {
            std::cerr << "Set not found: " << expectedAlpha << std::endl;
            std::cout << expectedAlpha - 1 << " " <<
                estimateChi(intialVerticesCount, expectedAlpha - 1) <<
                std::endl;
            return;
        }
    }
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
        testGraph(B<Storage::Lists>(n, k, t));
    } else if (graphType == "th") {
        testGraph(Th<Storage::Lists>(n, k, t));
    } else if (graphType == "t") {
        testGraph(T<Storage::Lists>(n, kPos, k - kPos, t));
    }
}
