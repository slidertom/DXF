#ifndef __DXF_BOUND_POINTS_VISITOR_H__
#define __DXF_BOUND_POINTS_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "entity/DXFEntityVisitor.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "../entity/DXFEntity.h"
#endif

#ifndef __DXF_MATH_H__
    #include "DXFMath.h"
#endif

#include "vector"

class CDXF2DPointSegment
{
// Construction/Destruction
public:
	CDXF2DPointSegment() { }
	CDXF2DPointSegment(const CDXF2DPointSegment &x) : m_x(x.m_x), m_y(x.m_y) { }

// Operators
public:
	CDXF2DPointSegment &operator=(const CDXF2DPointSegment &src) {
	    m_x = src.m_x;
	    m_y = src.m_y;
        return *this;
    }

	bool operator==(const CDXF2DPointSegment &src) const {
        return dxf_math::eqx(m_x, src.m_x) && dxf_math::eqx(m_y, src.m_y);
    }

// Attributes
public:
    double m_x {0.};
    double m_y {0.};
};

class CDXF2DPointVector : public std::vector<CDXF2DPointSegment>
{
// Construction/Destruction
public:
	CDXF2DPointVector() { }
	virtual ~CDXF2DPointVector() { }
};

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

class DXFDLLAPI CDXFBoundPointsVisitor : public CDXFEntityVisitor
{
public:
	CDXFBoundPointsVisitor(CDXF2DPointVector *pPoints) : m_pPoints(pPoints) { }
    ~CDXFBoundPointsVisitor() { }

protected:
	virtual void VisitDXFArc(CDXFArc &entity)             override;
	virtual void VisitDXFCircle(CDXFCircle &entity)       override;
	virtual void VisitDXFDimension(CDXFDimension &entity) override;
	virtual void VisitDXFHatch(CDXFHatch &entity)         override;
	virtual void VisitDXFLine(CDXFLine &entity)           override;
	virtual void VisitDXFMText(CDXFMText &entity)         override;
	virtual void VisitDXFPoint(CDXFPoint &entity)         override;
	virtual void VisitDXFPolyline(CDXFPolyline &entity)   override;
	virtual void VisitDXFSolid(CDXFSolid &entity)         override;
	virtual void VisitDXFText(CDXFText &entity)           override;
	virtual void VisitDXFInsert(CDXFInsert &entity)       override; 

    virtual void VisitDXFEllipse(CDXFEllipse &entity) override { }
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) override { }

// Attributes
private:
	CDXF2DPointVector *m_pPoints;
};

#endif
