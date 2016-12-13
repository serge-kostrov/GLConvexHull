#pragma once
#include "halfedge.h"
#include "constants.h"
#include "stdheaders.h"
#include "geometrytools.h"

class CQuickHull
{
public:
    CQuickHull(const std::vector<double3>& aPt) : m_aPt(aPt) {}
    ~CQuickHull() {}

    bool initialSimplex(CMesh& mesh);
    void addNextVx(CMesh& mesh);
    void addPt(CMesh& mesh, const double3& pt);
    bool algo(CMesh& mesh);

private:
    static bool checkVisible(const double3& ptEye, const ItFace& fc, double eps = 0.);
    bool getPtEye(double3& ptEye, ItFace& fcVisible, CMesh& mesh) const;
    void getHorizon(std::list<ItEdge>& aRidge, std::list<ItFace>& aVisible, const double3& ptEye, const ItFace& fcVisible) const;

private:
    const std::vector<double3>& m_aPt;
};
