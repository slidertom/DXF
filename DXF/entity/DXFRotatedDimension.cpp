#include "stdafx.h"
#include "DXFRotatedDimension.h"

#include "entity/DXFSolid.h"
#include "entity/DXFLine.h"
#include "entity/DXFText.h"
#include "entity/DXFInsert.h"
#include "DXFAlignedDimension.h"

#include "Utils/DXFMath.h"
#include "Utils/DXFTextUtils.h"
#include "DXFEntityVisitor.h"

namespace internal
{
    static CDXF3DPoint CalcTextMiddlePoint(const CDXF3DPoint &defPoint, const CDXF3DPoint &defPoint1, const CDXF3DPoint &defPoint2, 
                                           const CDXFDimStyleTableRecord *pDimStyle, bool bSwitch)
    {
        CDXF3DPoint middlept;
        if ( bSwitch ) {
            middlept.x = defPoint.x - pDimStyle->m_dDimGap * pDimStyle->m_dDimScale;
            middlept.y = (defPoint1.y + defPoint2.y) * 0.5;
        }
        else {
            middlept.x = (defPoint1.x + defPoint2.x) * 0.5;
            middlept.y = defPoint.y + pDimStyle->m_dDimGap * pDimStyle->m_dDimScale;
        }

        return middlept;
    }
};

CDXFRotatedDimension::CDXFRotatedDimension(double dRotation, 
                                           const CDXF3DPoint &extensionLine1StartPoint,
                                           const CDXF3DPoint &extensionLine2StartPoint,
                                           const CDXF3DPoint &dimensionLinePoint,
                                           const char *sDimText)
{
    m_dRotation = dRotation;
    m_defPoint1 = extensionLine1StartPoint;
    m_defPoint2 = extensionLine2StartPoint;
    m_defPoint  = dimensionLinePoint;

    if (sDimText) {
        // If text passed, then using instead of real dimension
        SetOverrideDimText(true);
        SetDimText(sDimText);
    }
}

CDXFRotatedDimension::CDXFRotatedDimension(double dRotation, 
                                           double x1, double y1, double x2, double y2, double dimx, double dimy,
                                           const char *sDimText)
: m_dRotation(dRotation),
  m_defPoint1(x1, y1, 0.),
  m_defPoint2(x2, y2, 0.)
{
    m_defPoint.SetValues(dimx, dimy, 0.);

    if (sDimText) {
        // If text passed, then using instead of real dimension
        SetOverrideDimText(true);
        SetDimText(sDimText);
    }
}

CDXFRotatedDimension::CDXFRotatedDimension(const CDXFRotatedDimension &dimension) 
: CDXFDimension(dimension)
{
    m_defPoint1 = dimension.m_defPoint1;
    m_defPoint2 = dimension.m_defPoint2;
    m_dRotation = dimension.m_dRotation;
}

bool CDXFRotatedDimension::Init(CDXFRotatedDimension *pDim, CDXFDatabase *pDB)
{
    if (!CDXFDimension::Init(pDim, pDB)) {
        return false;
    }

    ASSERT(!pDim->m_dimStyleID.IsNull());

    CDXFBlockTableRecord *pDimBlock = (CDXFBlockTableRecord *)pDim->m_dimBlockID.GetObject();
    
    if (pDimBlock == NULL || pDim->m_pOverrideStyle == NULL) {
        return false;
    }

    CDXF3DPoint dimPoint1, dimPoint2;
    double dAngle;
    double dTextAngle;
    
    if (pDim->m_dRotation >= -DXF_EPSILON && pDim->m_dRotation <= DXF_EPSILON)
    {
        // Horizontal dimension
        dimPoint1.x = pDim->m_defPoint1.x;
        dimPoint1.y = pDim->m_defPoint.y;

        dimPoint2.x = pDim->m_defPoint2.x;
        dimPoint2.y = pDim->m_defPoint.y;

        pDim->SetTextMiddlePoint(internal::CalcTextMiddlePoint(pDim->m_defPoint, pDim->m_defPoint1, pDim->m_defPoint2, pDim->m_pOverrideStyle, false));

        if (0 == ::strlen(pDim->GetDimText()))
        {
            const double dDimLineLength = ::fabs(pDim->m_defPoint1.x - pDim->m_defPoint2.x);
            pDim->SetDimText(dxf_text_utils::FormatDouble(dDimLineLength, pDim->m_pOverrideStyle->m_nDimDec).c_str());
        }

        if (pDim->m_defPoint2.x - pDim->m_defPoint1.x >= 0.0) {
            dAngle = 0.0;
        }
        else {
            dAngle = DXF_PI;
        }

        dTextAngle = 0.0;
    }
    else if (pDim->m_dRotation >= DXF_PI / 2.0 - DXF_EPSILON && pDim->m_dRotation <= DXF_PI / 2.0 + DXF_EPSILON)
    {
        // Vertical dimension
        dimPoint1.x = pDim->m_defPoint.x;
        dimPoint1.y = pDim->m_defPoint1.y;

        dimPoint2.x = pDim->m_defPoint.x;
        dimPoint2.y = pDim->m_defPoint2.y;

        pDim->SetTextMiddlePoint(internal::CalcTextMiddlePoint(pDim->m_defPoint, pDim->m_defPoint1, pDim->m_defPoint2, pDim->m_pOverrideStyle, true));

        if (::strlen(pDim->GetDimText()) == 0)
        {
            const double dDimLineLength = ::fabs(pDim->m_defPoint1.y - pDim->m_defPoint2.y);
            pDim->SetDimText(dxf_text_utils::FormatDouble(dDimLineLength, pDim->m_pOverrideStyle->m_nDimDec).c_str());
        }

        if (pDim->m_defPoint2.y - pDim->m_defPoint1.y >= 0.0) {
            dAngle = DXF_PI * 0.5;
        }
        else {
            dAngle = DXF_PI * 1.5;
        }

        dTextAngle = DXF_PI * 0.5;
    }
    else {
        return false;
    }

    pDimBlock->AddEntity(new CDXFLine(dimPoint1, dimPoint2), pDB); // Add dimension line
    
    // Add dimension text
    if (0 != ::strcmp(pDim->GetDimText(), ".")) {
        if (pDim->GetDrawText()) {   
            CDXFText *pDimText = new CDXFText;
            pDimText->SetRotationAngle(dTextAngle);
            pDimText->SetTextAlign(eCentre, eBaseline);
            pDim->SetDimTextEntity(pDimText);
            pDimBlock->AddEntity(pDimText, pDB);
        }
    }

    // Add arrowheads and/or ticks
    CDXFObjectID arrowBlock1, arrowBlock2;
    CDXFDimension::GetArrowBlocks(pDim, pDB, arrowBlock1, arrowBlock2);

    // ArrowBlockID is nullptr, then that means, that closed solid arrowhead is applied
    if (!arrowBlock1.IsNull()) {
        CDXFDimension::AddArrowBlock(pDim, pDimBlock, dimPoint1, arrowBlock1, dAngle - DXF_PI, pDB);
    }
    else {
        pDimBlock->AddEntity(new CDXFLine(pDim->m_defPoint1, dimPoint1), pDB);
        CDXFDimension::AddDefaultArrowBlock(pDim, pDimBlock, pDB, dimPoint1, dAngle);
    }

    if (!arrowBlock2.IsNull()) {
        CDXFDimension::AddArrowBlock(pDim, pDimBlock, dimPoint2, arrowBlock2, dAngle, pDB);
    }
    else {
        pDimBlock->AddEntity(new CDXFLine(pDim->m_defPoint2, dimPoint2), pDB);
        CDXFDimension::AddDefaultArrowBlock(pDim, pDimBlock, pDB, dimPoint1, dAngle + DXF_PI);
    }

    return true;
}

CDXFBlockTableRecord *CDXFRotatedDimension::GetBlock()
{ 
    if (m_dimBlockID.IsNull()) {
        return nullptr;
    }

    return (CDXFBlockTableRecord *)m_dimBlockID.GetObject();
}

