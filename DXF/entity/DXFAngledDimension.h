#ifndef __DXF_ANGLED_DIMENSION_H__
#define __DXF_ANGLED_DIMENSION_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_DIMENSION_H__
    #include "DXFDimension.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "DXF3DPoint.h"
#endif

class DXFDLLAPI CDXFAngledDimension : public CDXFDimension
{
// Construction/Destruction
public:
    CDXFAngledDimension(const CDXF3DPoint &ptCenter, const CDXF3DPoint &pt1, const CDXF3DPoint &pt2);
    CDXFAngledDimension(const CDXFAngledDimension &dim);
    virtual ~CDXFAngledDimension() { }

// Overrides
public:
    virtual void AcceptDim(CDXFDimensionVisitor &vis) override { vis.VisitDXFAngledDimension(*this); }

// Static operations
public:
    static bool Init(CDXFAngledDimension *pDim, CDXFDatabase *pDB, double dRadius);
    
public:
    const CDXF3DPoint &GetCenterPoint() const { return m_ptCenter; }
    const CDXF3DPoint &GetFirstPoint() const  { return m_pt1; }
    const CDXF3DPoint &GetSecondPoint() const { return m_pt2; }
    const CDXF3DPoint &GetThirdPoint() const  { return m_pt3; }

// Attributes
private:
    CDXF3DPoint m_ptCenter;
    CDXF3DPoint m_pt1;
    CDXF3DPoint m_pt2;
    CDXF3DPoint m_pt3;
};

#endif