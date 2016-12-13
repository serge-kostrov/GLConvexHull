#pragma once
#include "base.h"


inline double dToR(double degrees)
{
	using namespace cts;
	return degrees*(rPi/dPi);
}

inline double rToD(double radians)
{
	using namespace cts;
	return radians*(dPi/rPi);
}

inline void getRightHandOrts(double3& ox, double3& oy, const double3& oz)
{
	ox = double3(-oz.y(), oz.x(), 0.).normalized();
    oy = (ox%oz).normalized();
}

inline double distPtLn(const double2& pt, const double2& org, const double2& dir)
{
    const double2 nmRight{dir.y(), -dir.x()};
    return nmRight*(pt - org);
}

inline double distPtLnU(const double2& pt, const double2& org, const double2& dir)
{
    return abs(distPtLn(pt, org, dir));
}

inline double distPtLn(const double3& pt, const double3& org, const double3& dir)
{
    return ((pt - org)%dir).norm()/dir.norm();
}

inline double distPtPln(const double3& pt, const double3& org, const double3& nm)
{
    return nm*(pt - org);
}

inline double distPtPlnU(const double3& pt, const double3& org, const double3& nm)
{
    return abs(distPtPln(pt, org, nm));
}

inline double projPtPln(double3& proj, const double3& pt, const double3& org, const double3& nm)
{
    const double dist = distPtPln(pt, org, nm);
    proj = pt - nm*dist;
    return dist;
}

double projPtPln(double2& uv, double3* proj, const double3& pt, const double3& org, const double3& u, const double3& v);
int intersectPtPln(double* t, double3& ptIntersect, const double3& orgLn, const double3& dirLn, const double3& orgPln, const double3& nmPln);
