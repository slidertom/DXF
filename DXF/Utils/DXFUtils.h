#ifndef __DXF_UTILS_H__
#define __DXF_UTILS_H__
#pragma once

#include "vector"

class CDXF2DLineSegment final
{
// Attributes
public:
    double m_x1; // Start
    double m_y1;
    double m_x2; // End
    double m_y2;
};

class CDXF2DLineVector final: public std::vector<CDXF2DLineSegment>
{
// Construction/Destruction
public:
    CDXF2DLineVector() { }
    virtual ~CDXF2DLineVector() { }
};

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

class CDXFArc;

class DXFDLLAPI CArcGenerator
{
public:
    // dStartAngle and dEndAngle - in degrees
    static void ConvertArcToLines(CDXF2DLineVector &lines, double dCenterX, double dCenterY, double dStartAngle, double dEndAngle,
                                  double dRadius, double dAngleOffset = 5.0);

    // Creates an arc from its start point, end point and bulge.
	static void CreateArcFrom2PBulge(double startPointX, double startPointY,
						             double endPointX, double endPointY,
						             double dBulge,
						             CDXFArc &arc);
};

#endif