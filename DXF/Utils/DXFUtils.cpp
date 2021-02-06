#include "stdafx.h"
#include "DXFUtils.h"

#include "../entity/DXFArc.h"
#include "DXFMath.h"

void CArcGenerator::ConvertArcToLines(CDXF2DLineVector &lines, double dCenterX, double dCenterY, double dStartAngle, double dEndAngle, double dRadius, double dAngleOffset)
{
    const double dToRadians = DXF_PI / 180.0;

    if ( dxf_math::more(dStartAngle, dEndAngle)) {
        dEndAngle += 360.0;
    }

    double dCurrentAngle(dStartAngle);

    CDXF2DLineSegment segment;
    segment.m_x1 = dCenterX + dRadius * ::cos(dCurrentAngle * dToRadians);
    segment.m_y1 = dCenterY + dRadius * ::sin(dCurrentAngle * dToRadians);

    bool bBreak = false;
    while (true)
    {
        dCurrentAngle += dAngleOffset;

        if (dxf_math::more(dCurrentAngle, dEndAngle)) {
            dCurrentAngle = dEndAngle;
            bBreak = true;
        }

        segment.m_x2 = dCenterX + dRadius * ::cos(dCurrentAngle * dToRadians);
        segment.m_y2 = dCenterY + dRadius * ::sin(dCurrentAngle * dToRadians);
        
        lines.push_back(segment);

        if (bBreak) {
            break;
        }

        segment.m_x1 = segment.m_x2;
        segment.m_y1 = segment.m_y2;
    }
}

// Implementation from QCad sources. Dainius.
void CArcGenerator::CreateArcFrom2PBulge(double startPointX, double startPointY,
						                 double endPointX, double endPointY,
				                         double dBulge,
						                 CDXFArc &arc)
{
	CDXF3DPoint startPoint(startPointX, startPointY, 0.0);
	CDXF3DPoint endPoint(endPointX, endPointY, 0.0);
		
	const double dAlpha = ::atan(dBulge) * 4.0;

	CDXF3DPoint middlePoint = startPoint;
	middlePoint += endPoint;
		
	middlePoint /= 2.0;

	const double dDist = CDXF3DPoint::DistanceTo(startPoint, endPoint) / 2.0;

	// alpha can't be 0.0 at this point
	const double dRadius = ::fabs(dDist / ::sin(dAlpha / 2.0));

	const double dWu = ::fabs(::pow(dRadius, 2.0) - ::pow(dDist, 2.0));
	double dH = ::sqrt(dWu);
	double dAngle = CDXF3DPoint::AngleTo(startPoint, endPoint);

	if (dxf_math::more(dBulge, 0.0)) {
		dAngle += DXF_PI / 2.0;
	} 
	else {
		dAngle -= DXF_PI / 2.0;
	}

	if (dxf_math::more(fabs(dAlpha), DXF_PI)) {
		dH *= -1.0;
	}

	CDXF3DPoint centerPoint;
	CDXF3DPoint::SetPolar(centerPoint, dH, dAngle);

	centerPoint += middlePoint;
		
	double dAngle1 = CDXF3DPoint::AngleTo(centerPoint, startPoint);
	double dAngle2 = CDXF3DPoint::AngleTo(centerPoint, endPoint);

	// To degrees
	dAngle1 = dAngle1 * (180.0 / DXF_PI);
	dAngle2 = dAngle2 * (180.0 / DXF_PI);
		
	arc.m_ptCenter = centerPoint; 
	arc.m_dRadius  = dRadius;

	if (dxf_math::less(dBulge, 0.0)) {
		arc.m_dStartAngle	= dAngle2;
		arc.m_dEndAngle		= dAngle1;
	}
	else {
		arc.m_dStartAngle	= dAngle1;
		arc.m_dEndAngle		= dAngle2;
	}
}
