#include "stdafx.h"
#include "DXFAlignedDimension.h"

#include "DXFSolid.h"
#include "DXFLine.h"
#include "DXFText.h"
#include "DXFInsert.h"

#include "../base/DXFVector.h"

#include "DXFEntityVisitor.h"
#include "Utils/DXFMath.h"
#include "Utils/DXFTextUtils.h"


namespace internal
{
	// checks if vector is more vertical than horizontal
	static inline bool IsVectVert(double cx, double cy)
	{
		return (dxf_math::fabsx(cx) < dxf_math::fabsx(cy) - dxf_math::sqeps);
	}

	// returns true if vector direction does not match nearest axis direction
	static inline bool IsVectReversed(double cx, double cy)
	{
		if ( IsVectVert(cx, cy) ) {
			return dxf_math::more(cy, 0);
		}
		return dxf_math::less(cx, 0);
	}

    static bool IsDimReversed(const CDXF3DPoint &defPoint1, const CDXF3DPoint &defPoint2)
    {
        const double cx = defPoint1.x - defPoint2.x;
	    const double cy = defPoint1.y - defPoint2.y;
	    const bool bReversed = internal::IsVectReversed(cx, cy);
        return bReversed;
    }

    static bool IsDimVert(const CDXF3DPoint &defPoint1, const CDXF3DPoint &defPoint2)
    {
        const double cx = defPoint1.x - defPoint2.x;
	    const double cy = defPoint1.y - defPoint2.y;
        return internal::IsVectVert(cx, cy);
    }

    static double CalcLineAngle(const CDXF3DPoint &defPoint1, const CDXF3DPoint &defPoint2)
    {
        const bool bReversed = internal::IsDimReversed(defPoint1, defPoint2);
	    CDXF3DPoint dimPoint1Angle = !bReversed ? defPoint1 : defPoint2;
        CDXF3DPoint dimPoint2Angle = !bReversed ? defPoint2 : defPoint1;
	    const double dAngleLine = ::atan2(dimPoint2Angle.y - dimPoint1Angle.y,  dimPoint2Angle.x - dimPoint1Angle.x);
        return dAngleLine;
    }

    static CDXF3DPoint CalcTextMiddlePoint(const CDXF3DPoint &defPoint1, const CDXF3DPoint &defPoint2, 
                                           const double dHeight, const CDXFDimStyleTableRecord *pDimStyle)
    {
        const double dAngleLine = internal::CalcLineAngle(defPoint1, defPoint2);
        const bool bReversed = IsDimReversed(defPoint1, defPoint2);

        CDXF3DPoint textMiddlePoint;
        textMiddlePoint.x = (defPoint2.x + defPoint1.x) * 0.5;
	    textMiddlePoint.y = (defPoint2.y + defPoint1.y) * 0.5;

	    const double dDist = (dHeight*0.5) + (pDimStyle->m_dDimGap * pDimStyle->m_dDimScale);

	    double dAngle = dAngleLine;
	    if ( bReversed ) {
		    dAngle -= DXF_PI*0.5;
	    }
	    else {
		    dAngle += DXF_PI*0.5;
	    }

	    textMiddlePoint.x = textMiddlePoint.x + ::cos(dAngle)*dDist;
	    textMiddlePoint.y = textMiddlePoint.y + ::sin(dAngle)*dDist;

        return textMiddlePoint;
    }
};

bool CDXFAlignedDimension::InitNoDimLines(CDXFAlignedDimension *pDim, CDXFDatabase *pDB)
{
    if (!pDim->m_bNoDimLines) {
        ASSERT(false);
        return false;
    }

    if (!CDXFDimension::Init(pDim, pDB)) {
		return false;
    }

	ASSERT(!pDim->m_dimStyleID.IsNull());

    CDXFBlockTableRecord *pDimBlock    = (CDXFBlockTableRecord *)pDim->m_dimBlockID.GetObject();
	CDXFDimStyleTableRecord *pDimStyle = (CDXFDimStyleTableRecord *)pDim->m_dimStyleID.GetObject();
    
    if (!pDimBlock || !pDimStyle) {
		return false;
    }

    //Calc text position related to line center
    pDim->SetTextMiddlePoint(internal::CalcTextMiddlePoint(pDim->m_defPoint1, pDim->m_defPoint2, pDim->m_dHeight, pDimStyle));

    const double dLength = CDXFVector(pDim->m_defPoint1 - pDim->m_defPoint2).Length();
	pDim->SetDimText(dxf_text_utils::FormatDouble(dLength, pDimStyle->m_nDimDec).c_str());

    double dTextAngle = internal::CalcLineAngle(pDim->m_defPoint1, pDim->m_defPoint2);
	if ( !internal::IsDimVert(pDim->m_defPoint1, pDim->m_defPoint2) ) {
		dTextAngle = dTextAngle - DXF_PI;
	}

	CDXFText *pDimText = new CDXFText;
    pDimText->SetRotationAngle(dTextAngle);
	pDimText->SetTextAlign(eCentre, eVertAlignMiddle);
    pDim->SetDimTextEntity(pDimText);
	pDimBlock->AddEntity(pDimText, pDB);

    return true;
}

bool CDXFAlignedDimension::InitWithDimLines(CDXFAlignedDimension *pDim, CDXFDatabase *pDB)
{
    if (pDim->m_bNoDimLines) {
        ASSERT(false);
        return false;
    }

    if (!CDXFDimension::Init(pDim, pDB)) {
		return false;
    }

	ASSERT(!pDim->m_dimStyleID.IsNull());

    CDXFBlockTableRecord *pDimBlock    = (CDXFBlockTableRecord *)pDim->m_dimBlockID.GetObject();
	CDXFDimStyleTableRecord *pDimStyle = (CDXFDimStyleTableRecord *)pDim->m_dimStyleID.GetObject();
    
    if (!pDimBlock || !pDimStyle) {
		return false;
    }

    CDXF3DPoint dimPoint1(pDim->m_defPoint1), dimPoint2(pDim->m_defPoint2);
	CDXFVector vector = pDim->m_defPoint2 - pDim->m_defPoint1;
	double dTextAngle;

	vector.Normalize();

	double temp = vector.x;
	vector.x = -vector.y;
	vector.y = temp;

	pDim->m_defPoint = vector * pDim->m_dHeight;
	pDim->m_defPoint += pDim->m_defPoint1;

	const double dAngle = ::atan2(pDim->m_defPoint2.y - pDim->m_defPoint1.y,  pDim->m_defPoint2.x - pDim->m_defPoint1.x);

	// Add extension lines
	if (pDim->m_dHeight <= -DXF_EPSILON || pDim->m_dHeight >= DXF_EPSILON)
	{
		pDimBlock->AddEntity(new CDXFLine(pDim->m_defPoint1, pDim->m_defPoint), pDB);
		pDimBlock->AddEntity(new CDXFLine(pDim->m_defPoint2, pDim->m_defPoint2 + (CDXF3DPoint)(vector * pDim->m_dHeight)), pDB);
	}

	// Add dimension line
	pDimBlock->AddEntity(new CDXFLine(pDim->m_defPoint, pDim->m_defPoint2 + (CDXF3DPoint)(vector * pDim->m_dHeight)), pDB);

    CDXF3DPoint midpoint;
    midpoint.x = (pDim->m_defPoint2.x + pDim->m_defPoint1.x) / 2.0;
    midpoint.y = (pDim->m_defPoint2.y + pDim->m_defPoint1.y) / 2.0;

	if (dAngle <= DXF_PI / 2.0 && dAngle >= -DXF_PI / 2.0)
	{
        midpoint.x += vector.x * (pDim->m_dHeight + pDimStyle->m_dDimGap * pDimStyle->m_dDimScale);
        midpoint.y += vector.y * (pDim->m_dHeight + pDimStyle->m_dDimGap * pDimStyle->m_dDimScale);

		dTextAngle = dAngle;
	}
	else
	{
        midpoint.x += vector.x * (pDim->m_dHeight - pDimStyle->m_dDimGap * pDimStyle->m_dDimScale);
        midpoint.y += vector.y * (pDim->m_dHeight - pDimStyle->m_dDimGap * pDimStyle->m_dDimScale);

		dTextAngle = dAngle + DXF_PI;
	}

    pDim->SetTextMiddlePoint(midpoint);

	if (::strlen(pDim->GetDimText()) == 0) {
        const double dLength = CDXFVector(pDim->m_defPoint1 - pDim->m_defPoint2).Length();
		pDim->SetDimText(dxf_text_utils::FormatDouble(dLength, pDimStyle->m_nDimDec).c_str());
	}

	if (::strcmp(pDim->GetDimText(), ".") != 0) {
		// Add dimension text
		if (pDim->GetDrawText()) {
			CDXFText *pDimText = new CDXFText;
            pDimText->SetRotationAngle(dTextAngle);
			pDimText->SetTextAlign(eCentre, eBaseline);
            pDim->SetDimTextEntity(pDimText);
			pDimBlock->AddEntity(pDimText, pDB);
		}
	}

	// Add arrowheads or ticks
	CDXFObjectID arrowBlock1, arrowBlock2;
    CDXFDimension::GetArrowBlocks(pDim, pDB, arrowBlock1, arrowBlock2);

	// ArrowBlockID is NULL, then that means, that closed solid arrowhead is applied
	if (!arrowBlock1.IsNull()) {
		CDXFDimension::AddArrowBlock(pDim, pDimBlock, dimPoint1, arrowBlock1, dAngle - DXF_PI, pDB);
	}
	else {
        CDXFDimension::AddDefaultArrowBlock(pDim, pDimBlock, pDB, dimPoint1, dAngle);
	}

	if (!arrowBlock2.IsNull()) {
		CDXFDimension::AddArrowBlock(pDim, pDimBlock, dimPoint2, arrowBlock2, dAngle, pDB);
	}
	else {
        CDXFDimension::AddDefaultArrowBlock(pDim, pDimBlock, pDB, dimPoint1, dAngle + DXF_PI);
	}

    return true;
}

bool CDXFAlignedDimension::Init(CDXFAlignedDimension *pDim, CDXFDatabase *pDB)
{
    if (pDim->m_bNoDimLines) {
        return InitNoDimLines(pDim, pDB);
    }
    return InitWithDimLines(pDim, pDB);
}

CDXFBlockTableRecord *CDXFAlignedDimension::GetBlock()
{ 
	if (m_dimBlockID.IsNull()) {
		return nullptr;
	}
	return (CDXFBlockTableRecord *)m_dimBlockID.GetObject();
}