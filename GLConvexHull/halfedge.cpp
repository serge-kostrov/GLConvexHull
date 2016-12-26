#include "halfedge.h"
#include "geometrytools.h"


double3 CFace::calcCM() const
{
    int nVx = 0;
    double3 cm{0.};
    ItEdge ed = m_ed;
    do
    {
        ++nVx;
        cm += ed->vx()->pos();
        ed = ed->next();
    } while(ed != m_ed);
    cm /= nVx;

    return cm;
}

bool CFace::isIntersectedBy(const double3& orgRay, const double3& dirRay) const
{
    double3 cm = calcCM();

    double3 ptIntersect{0.};
    int nIntersect = intersectPtPln(nullptr, ptIntersect, orgRay, dirRay, cm, m_nm);
    if(nIntersect != 1)
        return false;

    double3 ox{0.}, oy{0.};
    getRightHandOrts(ox, oy, m_nm);

    double2 uvIntersect{0.};
    projPtPln(uvIntersect, nullptr, ptIntersect, cm, ox, oy);

    double2 uv{0.}, uvPrev{0.};
    ItEdge ed = m_ed;
    projPtPln(uvPrev, nullptr, ed->prev()->vx()->pos(), cm, ox, oy);
    do
    {
        projPtPln(uv, nullptr, ed->vx()->pos(), cm, ox, oy);

        // "in hull" check
        const double2 dir = (uv - uvPrev).normalized();
        const double2 nmOut{ dir.y(), -dir.x() };
        const double dist = distPtLn(uvIntersect, uv, nmOut);
        if(dist > 0)
            return false;

        ed = ed->next();
        uvPrev = uv;
    } while(ed != m_ed);

    return true;
}

double CMesh::scale() const
{
    double distMax = 0.;
    for(auto i = m_aVx.begin(); i != m_aVx.end(); ++i)
    {
        for(auto j = std::next(i); j != m_aVx.end(); ++j)
        {
            const double dist = (i->pos() - j->pos()).norm();
            if(dist > distMax)
                distMax = dist;
        }
    }

    return distMax;
}

ItVertex CMesh::addVx()
{
    m_aVx.push_back(CVertex{ nullEd() });
    return std::prev(m_aVx.end());
}

ItVertex CMesh::addVx(const double3& pos)
{
    m_aVx.push_back(CVertex{pos, nullEd()});
    return std::prev(m_aVx.end());
}

ItVertex CMesh::addVx(const CVertex& vx)
{
    m_aVx.push_back(vx);
    return std::prev(m_aVx.end());
}

void CMesh::eraseVx(const ItVertex& vx)
{
    m_aVx.erase(vx);
}

std::pair<ItEdge, ItEdge> CMesh::addEd(const std::pair<ItVertex, ItVertex>& vxPair)
{
    std::pair<ItEdge, ItEdge> edPair;

    edPair.first = addEd();
    edPair.second = addEd();

    edPair.first->vx() = vxPair.first;
    edPair.first->twin() = edPair.second;
    if (vxPair.first->ed() == nullEd())
        vxPair.first->ed() = edPair.first;

    edPair.second->vx() = vxPair.second;
    edPair.second->twin() = edPair.first;
    if (vxPair.second->ed() == nullEd())
        vxPair.second->ed() = edPair.second;

    return edPair;
}

ItEdge CMesh::addEd()
{
    m_aEd.push_back(CEdge{ nullVx(), nullEd(), nullEd(), nullEd(), nullFc() });
    return prev(m_aEd.end());
}

ItEdge CMesh::addEd(const CEdge& ed)
{
    m_aEd.push_back(ed);
    return prev(m_aEd.end());
}

void CMesh::eraseEd(const ItEdge& ed)
{
    m_aEd.erase(ed);
}

ItFace CMesh::addFc()
{
    m_aFc.push_back(CFace{ nullEd() });
    return prev(m_aFc.end());
}

ItFace CMesh::addFc(const CFace& fc)
{
    m_aFc.push_back(fc);
    return prev(m_aFc.end());
}

void CMesh::eraseFc(const ItFace& fc)
{
    m_aFc.erase(fc);
}

void CMesh::addVxFan(const double3& pt, const std::list<ItEdge>& aRidge, const std::list<ItFace>& aVisible)
{
    ItVertex vx = addVx();
    vx->pos() = pt;
    ItFace firstFc = nullFc();
    ItFace lastFc = nullFc();
    std::list<ItFace> aNewFc;
    for (auto& i : aRidge)
    {
        ItFace newFc = addFc();
        aNewFc.push_back(newFc);

        ItEdge ed = addEd(*i);
        ItEdge edPrev = addEd();
        ItEdge edNext = addEd();

        // link previous edge
        {
            edPrev->face() = newFc;
            edPrev->prev() = edNext;
            edPrev->next() = ed;
            edPrev->vx() = vx;
            if (lastFc != nullFc())
            {
                const ItEdge& twin = lastFc->ed()->next();
                edPrev->twin() = twin;
                twin->twin() = edPrev;
            }
        }

        // link next edge
        {
            edNext->face() = newFc;
            edNext->prev() = ed;
            edNext->next() = edPrev;
            edNext->vx() = ed->next()->vx();
            if (firstFc != nullFc())
            {
                const ItEdge& twin = firstFc->ed()->prev();
                edNext->twin() = twin;
                twin->twin() = edNext;

            }
            else
                firstFc = newFc;
        }

        // update links for current edge
        {
            ed->face() = newFc;
            ed->prev() = edPrev;
            ed->next() = edNext;
            ed->twin()->twin() = ed;
        }

        // link new face
        {
            newFc->ed() = ed;
            const double3& pt = vx->pos();
            const double3& ptNext = ed->vx()->pos();
            const double3& ptPrev = edNext->vx()->pos();
            newFc->nm() = ((ptNext - pt)%(ptPrev - pt)).normalized();
        }

        lastFc = newFc;
    }


    std::list<double3> aConflict;
    for (auto i = aVisible.begin(); i != aVisible.end(); ++i)
    {
        const std::list<double3>& aVx = (*i)->aVx();
        for (const auto& j : aVx)
            aConflict.push_back(j);
    }

    for (const auto& fc : aNewFc)
    {
        for (auto i = aConflict.begin(); i != aConflict.end();)
        {
            double3 pt = *i;
            const double3& org = fc->ed()->vx()->pos();
            const double3& nm = fc->nm();
            const double dist = distPtPln(pt, org, nm);
            if (dist > 0)
            {
                fc->aVx().push_back(pt);
                i = aConflict.erase(i);
            }
            else
                ++i;
        }
    }

    for (const auto& fc : aVisible)
    {
        std::vector<ItEdge> aEd;
        ItEdge ed = fc->ed();

        do
        {
            aEd.push_back(ed);
            ed = ed->next();
        } while (ed != fc->ed());

        for (const auto& ed : aEd)
            eraseEd(ed);

        eraseFc(fc);
    }
}

void CMesh::clearVxVisit()
{
	for (auto& vx : m_aVx)
		vx.visited() = false;
}

void CMesh::clearEdVisit()
{
	for (auto& ed : m_aEd)
		ed.visited() = false;
}

void CMesh::clearFcVisit()
{
	for (auto& fc : m_aFc)
		fc.visited() = false;
}
