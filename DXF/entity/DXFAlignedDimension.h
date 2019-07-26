#ifndef __DXF_ALIGNEDDIMENSION_H__
#define __DXF_ALIGNEDDIMENSION_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_DIMENSION_H__
	#include "DXFDimension.h"
#endif

#ifndef __DXF_3DPOINT_H__
	#include "../base/DXF3DPoint.h"
#endif

class DXFDLLAPI CDXFAlignedDimension : public CDXFDimension
{
// Construction/Destruction
public:
    CDXFAlignedDimension() { SetFlags(33); }
	CDXFAlignedDimension(const CDXF3DPoint &defPoint1, const CDXF3DPoint &defPoint2, double dHeight, bool bNoDimLines) {
	    SetFlags(33);	// 1 = Aligned, 32 = Indicates that the block reference (group code 2)
					    // is referenced by this dimension only
	    m_defPoint1   = defPoint1;
	    m_defPoint2   = defPoint2;
	    m_dHeight     = dHeight;
        m_bNoDimLines = bNoDimLines;
    }
	CDXFAlignedDimension(const CDXFAlignedDimension &dimension) : CDXFDimension(dimension) {
	    m_defPoint1     = dimension.m_defPoint1;
	    m_defPoint2     = dimension.m_defPoint2;
	    m_dHeight       = dimension.m_dHeight;
        m_bNoDimLines   = dimension.m_bNoDimLines;
    }
    virtual ~CDXFAlignedDimension() { }

// Operations
public:
    void SetFirstPoint(const CDXF3DPoint &point)  { m_defPoint1 = point; }
    void SetSecondPoint(const CDXF3DPoint &point) { m_defPoint2 = point; }
    const CDXF3DPoint &GetFirstPoint() const  { return m_defPoint1; }
    const CDXF3DPoint &GetSecondPoint() const { return m_defPoint2; }
    CDXFBlockTableRecord *GetBlock();

// Overrides
public:
    virtual void AcceptDim(CDXFDimensionVisitor &vis) override { vis.VisitDXFAlignedDimension(*this); };

// Static operations
public:
    static bool Init(CDXFAlignedDimension *pDimension, CDXFDatabase *pDB);
    static bool InitNoDimLines(CDXFAlignedDimension *pDimension, CDXFDatabase *pDB);
    static bool InitWithDimLines(CDXFAlignedDimension *pDimension, CDXFDatabase *pDB);

// Attributes
private:
    CDXF3DPoint m_defPoint1;
    CDXF3DPoint m_defPoint2;
    double m_dHeight     {0.};
    bool m_bNoDimLines   {false};
};

#endif