#include "stdafx.h"
#include "DXFAngledDimension.h"

#include "DXFEntityVisitor.h"

CDXFAngledDimension::CDXFAngledDimension(const CDXF3DPoint &ptCenter, const CDXF3DPoint &pt1, const CDXF3DPoint &pt2)
{
    SetFlags(2|128); // 2 = Angular;
                     // 128 = This is a bit value (bit 8) added to the other group 70 values if the dimension text
                     // has been positioned at a user-defined location rather than at the default location


    m_ptCenter = ptCenter;
    m_pt1      = pt1;
    m_pt2      = pt2;

    EnableTextPosOverride(true);
    EnableBlockRef(false);
}

CDXFAngledDimension::CDXFAngledDimension(const CDXFAngledDimension &dim) 
: CDXFDimension(dim)
{
    m_ptCenter = dim.m_ptCenter;
    m_pt1      = dim.m_pt1;
    m_pt2      = dim.m_pt2;
    m_pt3      = dim.m_pt3;
}

bool CDXFAngledDimension::Init(CDXFAngledDimension *pDim, CDXFDatabase *pDB, double dRadius)
{
    if (!CDXFDimension::Init(pDim, pDB)) {
        return false;
    }

    pDim->m_defPoint = pDim->m_pt1;
    
    CDXF3DPoint pt1_(pDim->m_ptCenter);
    CDXF3DPoint pt2_(pDim->m_ptCenter);

    CDXF3DPoint::MovePointOnLine(pDim->m_ptCenter, pDim->m_pt1, pt1_, dRadius);
    CDXF3DPoint::MovePointOnLine(pDim->m_ptCenter, pDim->m_pt2, pt2_, dRadius);
        
    CDXF3DPoint ptTmp;
    ptTmp.x = (pt1_.x+pt2_.x)*0.5;
    ptTmp.y = (pt1_.y+pt2_.y)*0.5;

    pDim->m_pt3 = pDim->m_ptCenter;

    CDXF3DPoint::MovePointOnLine(pDim->m_ptCenter, ptTmp, pDim->m_pt3, dRadius);

    CDXF3DPoint ptText = pDim->m_ptCenter;
    CDXF3DPoint::MovePointOnLine(pDim->m_ptCenter, ptTmp, ptText, dRadius*1.5);

    pDim->SetTextMiddlePoint(ptText);

    return true;
}