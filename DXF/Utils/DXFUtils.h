#ifndef __DXF_UTILS_H__
#define __DXF_UTILS_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#include "vector"

class CDXF2DLineSegment
{
// Attributes
public:
	double m_x1; // Start
	double m_y1;
	double m_x2; // End
	double m_y2;
};

class CDXF2DLineVector : public std::vector<CDXF2DLineSegment>
{
public:
    CDXF2DLineVector() { }
    ~CDXF2DLineVector() { }
};

class DXFDLLAPI CArcGenerator
{
public:
	// dStartAngle and dEndAngle - in degrees
	static void ConvertArcToLines(CDXF2DLineVector &lines, double dCenterX, double dCenterY, double dStartAngle, double dEndAngle,
								  double dRadius, double dAngleOffset = 5.0);
};

#endif