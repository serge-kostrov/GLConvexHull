#include "qquickhull.h"
#include "stdheaders.h"
#include "geometrytools.h"


bool CQuickHull::algo(CMesh& mesh)
{
    if (!initialSimplex(mesh))
        return false;

    double3 ptEye{ 0, 0, 0 };
    ItFace fcVisible = mesh.nullFc();
    while (getPtEye(ptEye, fcVisible, mesh))
    {
        std::list<ItEdge> aRidge;
        std::list<ItFace> aVisible;
        getHorizon(aRidge, aVisible, ptEye, fcVisible);
        mesh.addVxFan(ptEye, aRidge, aVisible);
    }

    return true;
}

void CQuickHull::addNextVx(CMesh &mesh)
{
    double3 ptEye{ 0, 0, 0 };
    ItFace fcVisible = mesh.nullFc();
    if(getPtEye(ptEye, fcVisible, mesh))
    {
        std::list<ItEdge> aRidge;
        std::list<ItFace> aVisible;
        getHorizon(aRidge, aVisible, ptEye, fcVisible);
        mesh.addVxFan(ptEye, aRidge, aVisible);
    }
}

void CQuickHull::addPt(CMesh &mesh, const double3& pt)
{
    auto& aFc = mesh.aFc();
    for(auto fc = aFc.begin(); fc != aFc.end(); ++fc)
    {
        if(checkVisible(pt, fc))
        {
            mesh.addVx(pt);
            fc->aVx().push_back(pt);
        }
    }
}

bool CQuickHull::initialSimplex(CMesh& mesh)
{
    mesh.clear();

    // searching for extreme points
    const int nExtreme = 6;
    std::array<double3, nExtreme> aExtreme;
    double3 maxPt{-cts::big, -cts::big, -cts::big}, minPt{cts::big, cts::big, cts::big};
    for (const auto& pt : m_aPt)
    {
        for (int j = 0; j < double3::dim; ++j)
        {
            if (pt[j] < minPt[j])
            {
                minPt[j] = pt[j];
                aExtreme[2*j] = pt;
            }
            if (pt[j] > maxPt[j])
            {
                maxPt[j] = pt[j];
                aExtreme[2*j + 1] = pt;
            }
        }
    }

    std::list<CVertex>& aVx = mesh.aVx();
    std::list<CFace>& aFc = mesh.aFc();

    // searching for the two most distant points
    {
        double distMax = 0;
        double3 ptLeft{ 0, 0, 0 }, ptRight{ 0, 0, 0 };
        for (int i = 0; i < nExtreme; ++i)
        {
            for (int j = i + 1; j < nExtreme; ++j)
            {
                const double dist = (aExtreme[i] - aExtreme[j]).norm();
                if (distMax < dist)
                {
                    distMax = dist;
                    ptLeft = aExtreme[i];
                    ptRight = aExtreme[j];
                }
            }
        }

        if (distMax < cts::eps)
            return false;
        else
        {
            ItVertex vxLeft = mesh.addVx();
            ItVertex vxRight = mesh.addVx();
            vxLeft->pos() = ptLeft;
            vxRight->pos() = ptRight;
            mesh.addEd(std::pair<ItVertex, ItVertex>(vxLeft, vxRight));
        }
    }

    // searching for the point most distant from the line
    {
        const double3& orgLn = aVx.back().pos();
        const double3 dirLn = (aVx.front().pos() - aVx.back().pos()).normalized();

        const double3 pt = *std::max_element(aExtreme.begin(), aExtreme.end(), [orgLn, dirLn](const double3& lhs, const double3& rhs) { return distPtLn(lhs, orgLn, dirLn) < distPtLn(rhs, orgLn, dirLn); });
        if (distPtLn(pt,orgLn, dirLn) < cts::eps)
            return false;
        else
        {
            ItVertex vx1 = aVx.begin();
            ItVertex vx2 = prev(aVx.end());
            ItVertex vx = mesh.addVx();
            vx->pos() = pt;

            ItEdge ed1 = vx1->ed();
            ItEdge ed2 = vx2->ed();
            ItEdge ed3 = mesh.addEd();
            ItEdge ed4 = mesh.addEd();
            ItEdge ed5 = mesh.addEd();
            ItEdge ed6 = mesh.addEd();

            ItFace fc1 = mesh.addFc();
            ItFace fc2 = mesh.addFc();

            ed1->prev() = ed3;
            ed1->next() = ed5;
            ed1->face() = fc1;

            ed2->prev() = ed6;
            ed2->next() = ed4;
            ed2->face() = fc2;

            ed3->twin() = ed4;
            ed3->prev() = ed5;
            ed3->next() = ed1;
            ed3->vx() = vx;
            ed3->face() = fc1;

            ed4->twin() = ed3;
            ed4->prev() = ed2;
            ed4->next() = ed6;
            ed4->vx() = vx1;
            ed4->face() = fc2;

            ed5->twin() = ed6;
            ed5->prev() = ed1;
            ed5->next() = ed3;
            ed5->vx() = vx2;
            ed5->face() = fc1;

            ed6->twin() = ed5;
            ed6->prev() = ed4;
            ed6->next() = ed2;
            ed6->vx() = vx;
            ed6->face() = fc2;

            fc1->ed() = ed5;
            fc2->ed() = ed5->twin();

            // normals and conflict lists
            {
                const double3& pt = vx->pos();
                const double3& pt1 = vx1->pos();
                const double3& pt2 = vx2->pos();
                const double3 nm = ((pt1 - pt)%(pt2 - pt)).normalized();
                fc1->nm() = nm;
                fc2->nm() = -nm;

                for (const auto& i : m_aPt)
                {
                    const double dist = distPtPln(i, pt, nm);
                    if (dist > cts::eps)
                        fc1->aVx().push_back(i);
                    else if (dist < -cts::eps)
                        fc2->aVx().push_back(i);
                }
            }
        }
    }

    // searching for the point most distant from the plane
    {
        const double3& orgPln = next(aVx.begin())->pos();
        const double3& ptU = aVx.front().pos();
        const double3& ptV = aVx.back().pos();
        const double3 nmPln = ((ptU - orgPln)%(ptV - orgPln)).normalized();

        const double3 pt = *std::max_element(aExtreme.begin(), aExtreme.end(), [orgPln, nmPln](const double3& lhs, const double3& rhs) { return distPtPlnU(lhs, orgPln, nmPln) < distPtPln(rhs, orgPln, nmPln); });
        if (distPtPlnU(pt, orgPln, nmPln) < cts::eps)
            return false;
        else
        {
            std::list<ItFace> aVisible;
            std::list<ItEdge> aRidge;

            const double3& nm = aFc.front().nm();
            const double3& org = aFc.front().ed()->vx()->pos();
            const double dist = distPtPln(pt, org, nm);
            ItFace fc = mesh.nullFc();
            if (dist > 0)
                fc = aFc.begin();
            else
                fc = prev(aFc.end());

            aVisible.push_back(fc);
            aRidge.push_back(fc->ed());
            aRidge.push_back(fc->ed()->next());
            aRidge.push_back(fc->ed()->prev());

            mesh.addVxFan(pt, aRidge, aVisible);
        }
    }

    return true;
}

bool CQuickHull::checkVisible(const double3& ptEye, const ItFace& fc, double eps)
{
    const double dist = distPtPln(ptEye, fc->ed()->vx()->pos(), fc->nm());
    return dist > eps;
}

bool CQuickHull::getPtEye(double3& ptEye, ItFace& fcVisible, CMesh& mesh) const
{
    std::list<CFace>& aFc = mesh.aFc();

    bool bFound = false;
    double distMax = 0.;
    ptEye = { 0., 0., 0. };
    fcVisible = mesh.nullFc();
    for (auto fc = aFc.begin(); fc != aFc.end(); ++fc)
    {
        std::list<double3>& aVx = fc->aVx();
        for (const auto& pt : aVx)
        {
            const double dist = distPtPln(pt, fc->ed()->vx()->pos(), fc->nm());
            if (dist > distMax + cts::eps)
            {
                distMax = dist;
                ptEye = pt;
                fcVisible = fc;
                bFound = true;
            }
        }
    }

    return bFound;
}

void CQuickHull::getHorizon(std::list<ItEdge>& aRidge, std::list<ItFace>& aVisible, const double3& ptEye, const ItFace& fcVisible) const
{
    std::stack<ItFace> st;
    st.push(fcVisible);

    std::list<ItEdge> aRidgeTmp;

    // DFS traversal
    while (!st.empty())
    {
        const ItFace fc = st.top();
        st.pop();
        if (!fc->visited())
        {
            fc->visited() = true;
            if (checkVisible(ptEye, fc))
                aVisible.push_back(fc);

            ItEdge ed = fc->ed();
            do
            {
                const ItFace& fcNb = ed->twin()->face();
                if (!fcNb->visited())
                {
                    if (!checkVisible(ptEye, fcNb))
                        aRidgeTmp.push_back(ed);
                    else
                        st.push(fcNb);
                }
                ed = ed->next();
            } while (ed != fc->ed());
        }
    }

    // Reordering ridge edges
    aRidge.push_back(aRidgeTmp.front());
    aRidgeTmp.pop_front();
    while (!aRidgeTmp.empty())
    {
        const ItEdge& ed = aRidge.back();
        bool bFound = false;
        for (auto i = aRidgeTmp.begin(); (i != aRidgeTmp.end()) && !bFound;)
        {
            const ItEdge edTmp = *i;
            if (edTmp->vx() == ed->twin()->vx())
            {
                aRidge.push_back(edTmp);
                i = aRidgeTmp.erase(i);
                bFound = true;
            }
            else
                ++i;
        }
        assert(bFound);
    }
}
