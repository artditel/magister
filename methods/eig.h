#pragma once
#include "graph.h"

#include "arssym.h"

#include <iostream>
#include <cmath>
#include <string>

template<class G>
float lowestEig(G* g)
{
    ARSymStdEig<float, G> dprob(
            g->ncols(),
            1L,
            g,
            &G::MultMv,
            const_cast<char*>("SA"));
    dprob.ChangeTol(0.001);
    dprob.ChangeMaxit(1000);
    dprob.FindEigenvalues();
    float eig = dprob.Eigenvalue(0);
    return eig;
}
