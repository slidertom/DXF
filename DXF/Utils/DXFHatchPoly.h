#ifndef __DXF_HATCH_POLY_H__
#define __DXF_HATCH_POLY_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

#include "vector"

class CDXFHatch;

namespace dxf_hatch_poly
{
    class CHatchPoint   { public: double x; double y; };
    class CHatchSegment { public: CHatchPoint first; CHatchPoint second; };
    typedef std::vector<CHatchSegment> CHatchSegments;
    typedef std::vector<CHatchPoint>   CHatchPoly;

    void GetPoly(const CDXFHatch &entity, CHatchPoly &points);
    void DXFDLLAPI GetHatch(const CDXFHatch &entity, CHatchSegments &result);
};

#endif