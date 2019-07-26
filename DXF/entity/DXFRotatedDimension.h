#ifndef __DXF_ROTATEDDIMENSION_H__
#define __DXF_ROTATEDDIMENSION_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_DIMENSION_H__
	#include "DXFDimension.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor"
#endif

class DXFDLLAPI CDXFRotatedDimension : public CDXFDimension  
{
// Construction/Destruction
public:
    CDXFRotatedDimension() { }
	CDXFRotatedDimension(double dRotation, 
		                 const CDXF3DPoint &extensionLine1StartPoint,
		                 const CDXF3DPoint &extensionLine2StartPoint,
		                 const CDXF3DPoint &dimensionLinePoint,
		                 const char *sDimText = nullptr);
	CDXFRotatedDimension(const CDXFRotatedDimension &dimension);
    virtual ~CDXFRotatedDimension() { }

// Overrides
public:
    virtual void AcceptDim(CDXFDimensionVisitor &vis) override { vis.VisitDXFRotatedDimension(*this); };

// Static operations
public:
    static bool Init(CDXFRotatedDimension *pDim, CDXFDatabase *pDB);

// Operations
public:
    CDXFBlockTableRecord *GetBlock();
    void SetRotation(double dRotation) { m_dRotation = dRotation; }
    double GetRotation() const { return m_dRotation; }

// Attributes
public:
	CDXF3DPoint m_defPoint1;
	CDXF3DPoint m_defPoint2;

private:
    double m_dRotation   {0.}; // In radians
};

#endif
