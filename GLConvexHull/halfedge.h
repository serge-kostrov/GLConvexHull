#pragma once
#include "base.h"
#include "stdheaders.h"
#include "idnumber.h"


class CVertex;
class CEdge;
class CFace;

using ItVertex = list<CVertex>::iterator;
using ItEdge = list<CEdge>::iterator;
using ItFace = list<CFace>::iterator;

class CVertex
{
public:
    CVertex(const ItEdge& ed) : m_pos(0., 0., 0.), m_ed(ed), m_id(CIdVxManager::instance()->next()) {}
	CVertex(double x, double y, double z, const ItEdge& ed) : m_pos(x, y, z), m_ed(ed), m_id(CIdVxManager::instance()->next()) {}
	CVertex(const double3& pos, const ItEdge& ed) : m_pos(pos), m_ed(ed), m_id(CIdVxManager::instance()->next()) {}
	CVertex(const CVertex& other) : m_pos(other.m_pos), m_ed(other.m_ed), m_id(other.m_id) {}
    ~CVertex() {}

    // Access
    const ItEdge& ed() const { return m_ed; }
    ItEdge& ed() { return m_ed; }
    const double3& pos() const { return m_pos; }
    double3& pos() { return m_pos; }

    double& operator[](int index) { return m_pos[index]; }
    double operator[](int index) const { return m_pos[index]; }

    double& x() { return m_pos.x(); }
	double x() const { return m_pos.x(); }
	double& y() { return m_pos.y(); }
	double y() const { return m_pos.y(); }
	double& z() { return m_pos.z(); }
	double z() const { return m_pos.z(); }
	bool visited() const { return m_bVisited; }
	bool& visited() { return m_bVisited; }
	const SIdVx& id() const { return m_id; }

    // Assignment operators
    void operator=(const CVertex& other) { m_ed = other.m_ed; m_pos = other.m_pos; }

private:
    ItEdge m_ed;        // outgoing edge
    double3 m_pos;      // vertex coordinates
	bool m_bVisited;
	SIdVx m_id;
};

class CEdge
{
public:
    CEdge(const ItVertex& vx, const ItEdge& edTwin, const ItEdge& edPrev, const ItEdge& edNext, const ItFace& fc) : m_vx(vx), m_edTwin(edTwin), m_edPrev(edPrev), m_edNext(edNext), m_fc(fc) {}
    CEdge(const CEdge& other) : m_vx(other.m_vx), m_edTwin(other.m_edTwin), m_edPrev(other.m_edPrev), m_edNext(other.m_edNext), m_fc(other.m_fc) {}
    ~CEdge() {}

    // Assignment operators
    void operator=(const CEdge& other) { m_vx = other.m_vx; m_edTwin = other.m_edTwin; m_edPrev = other.m_edPrev; m_edNext = other.m_edNext; m_fc = other.m_fc; }

    // Access
    const ItVertex& vx() const { return m_vx; }
    ItVertex& vx() { return m_vx; }
    const ItEdge& twin() const { return m_edTwin; }
    ItEdge& twin() { return m_edTwin; }
    const ItEdge& prev() const { return m_edPrev; }
    ItEdge& prev() { return m_edPrev; }
    const ItEdge& next() const { return m_edNext; }
    ItEdge& next() { return m_edNext; }
    const ItFace& face() const { return m_fc; }
    ItFace& face() { return m_fc; }
	bool visited() const { return m_bVisited; }
	bool& visited() { return m_bVisited; }

private:
    ItVertex m_vx;			// tail vertex
    ItEdge m_edTwin;		// twin edge
    ItEdge m_edPrev;		// previous edge
    ItEdge m_edNext;		// next edge
    ItFace m_fc;			// incident face
	bool m_bVisited;
};

class CFace
{
public:
    CFace(const ItEdge& ed) : m_nm(0., 0., 0.), m_ed(ed), m_bVisited(false) {}
    CFace(const CFace& other) : m_nm(other.m_nm), m_ed(other.m_ed), m_aVx(other.m_aVx), m_bVisited(other.m_bVisited) {}
    ~CFace() {}

    // Assignment operators
    void operator=(const CFace& other)
    {
        m_nm = other.m_nm;
        m_ed = other.m_ed;
        m_aVx = other.m_aVx;
    }

    // Access
    const double3& nm() const { return m_nm; }
    double3& nm() { return m_nm; }
    const ItEdge& ed() const { return m_ed; }
    ItEdge& ed() { return m_ed; }
    const list<double3>& aVx() const { return m_aVx; }
    list<double3>& aVx() { return m_aVx; }
    bool visited() const { return m_bVisited; }
    bool& visited() { return m_bVisited; }

    // Geometry tools
    bool isIntersectedBy(const double3& orgRay, const double3& dirRay) const;
    double3 calcCM() const;

private:
    double3 m_nm;               // face normal;
    //double3 m_cm;               // center of mass
    ItEdge m_ed;                // incident edge
    list<double3> m_aVx;	    // conflict list
    bool m_bVisited;            // flag showing whether this face was visited in the DFS process
};

class CMesh
{
public:
    CMesh() {}
    ~CMesh() {}

    void clear() { m_aVx.clear(); m_aEd.clear(); m_aFc.clear(); }

    // Access
    const list<CVertex>& aVx() const { return m_aVx; }
    list<CVertex>& aVx() { return m_aVx; }

    const list<CEdge>& aEd() const { return m_aEd; }
    list<CEdge>& aEd() { return m_aEd; }

    const list<CFace>& aFc() const { return m_aFc; }
    list<CFace>& aFc() { return m_aFc; }

    double scale() const;

    // Edit
    ItVertex addVx();
    ItVertex addVx(const double3& pos);
    ItVertex addVx(const CVertex& vx);
    void eraseVx(const ItVertex& vx);

    std::pair<ItEdge, ItEdge> addEd(const std::pair<ItVertex, ItVertex>& vxPair);

    ItEdge addEd();
    ItEdge addEd(const CEdge& ed);
    void eraseEd(const ItEdge& ed);

    ItFace addFc();
    ItFace addFc(const CFace& fc);
    void eraseFc(const ItFace& fc);

    void addVxFan(const double3& pt, const list<ItEdge>& aRidge, const list<ItFace>& aVisible);

    // "Null" iterators
    ItVertex nullVx() { return m_aVx.end(); }
    ItEdge nullEd() { return m_aEd.end(); }
    ItFace nullFc() { return m_aFc.end(); }

	// Visit flags
	void clearVxVisit();
	void clearEdVisit();
	void clearFcVisit();

private:
    CMesh(const CMesh& other);
    void operator=(const CMesh& other) {}

private:
    list<CVertex> m_aVx;
    list<CEdge> m_aEd;
    list<CFace> m_aFc;
};
