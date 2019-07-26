#include "stdafx.h"
#include "DXFUtils.h"

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
