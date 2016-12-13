#include "geometrytools.h"


double projPtPln(double2& uv, double3* proj, const double3& pt, const double3& org, const double3& u, const double3& v)
{
    const double3 nm = (u%v).normalized();
    double3 rVec = (pt - org);
    const double dist = distPtPln(pt, org, nm);
    rVec -= nm*dist;
    if(proj)
        *proj = org + rVec;
    uv = double2{ rVec*u, rVec*v };

    return dist;
}

int intersectPtPln(double* pT, double3& ptIntersect, const double3& orgLn, const double3& dirLn, const double3& orgPln, const double3& nmPln)
{
    const double prod = dirLn*nmPln;

    // a line doesn't intersect a plane in one point
    if(abs(prod) < cts::eps)
    {
        // a line lies within a plane
        if(distPtPlnU(orgLn, orgPln, nmPln) < cts::eps)
            return 2;
        // a line is parallel to a plane
        else
            return 0;
    }
    // a line intersects a plane
    else
    {
        const double t = -nmPln*(orgLn + orgPln)/prod;
        if(pT)
            *pT = t;
        ptIntersect = orgLn + dirLn*t;
        return 1;
    }
}
