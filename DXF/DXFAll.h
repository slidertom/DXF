#ifndef __DXF_ALL_H__
#define __DXF_ALL_H__
#pragma once

#ifdef _DXF_EXPORT_DLL
    
#else
    #ifdef _DEBUG
        #define _DXFDLL_LIB "MxDXF20D.lib"
    #else
        #define _DXFDLL_LIB "MxDXF20.lib"
    #endif

    #pragma message("MxDXF20 will automatically link with " _DXFDLL_LIB)
    #pragma comment(lib, _DXFDLL_LIB)
#endif

//////////////////////////////////////////////////////////////////////////
//Selected MINIMAL list of includes comprising FULL dxf library interface
//////////////////////////////////////////////////////////////////////////

//Dll interface includes
#ifndef __DXF_ALIGNEDDIMENSION_H__
    #include "entity/DXFAlignedDimension.h"
#endif

#ifndef __DXF_ANGLED_DIMENSION_H__
    #include "entity/DXFAngledDimension.h"
#endif

#ifndef __DXF_ARC_H__
    #include "entity/DXFArc.h"
#endif

#ifndef __DXF_CIRCLE_H__
    #include "entity/DXFCircle.h"
#endif

#ifndef __DXF_DATABASEEX_H__
    #include "DXFDatabaseEx.h"
#endif

#ifndef __DXF_DATABASE_H__
    #include "DXFDatabase.h"
#endif

#ifndef __DXF_DIM_STYLE_TABLE_RECORD_H__
    #include "DXFDimStyleTableRecord.h"
#endif

#ifndef __DXF_HATCH_H__
    #include "entity/DXFHatch.h"
#endif

#ifndef __DXF_LAYER_TABLE_RECORD_H__
    #include "tables/DXFLayerTableRecord.h"
#endif

#ifndef __DXF_LINE_H__
    #include "entity/DXFLine.h"
#endif

#ifndef __DXF_MLINESTYLE_H__
    #include "base/DXFMlineStyle.h"
#endif

#ifndef __DXF_POINT_H__
    #include "entity/DXFPoint.h"
#endif

#ifndef __DXF_POLYLINE_H__
    #include "entity/DXFPolyline.h"
#endif

#ifndef __DXF_ROTATEDDIMENSION_H__
    #include "entity/DXFRotatedDimension.h"
#endif

#ifndef __DXF_SOLID_H__
    #include "entity/DXFSolid.h"
#endif

#ifndef __DXF_VIEW_TABLE_RECORD_H__
    #include "tables/DXFViewTableRecord.h"
#endif

#ifndef __DXF_SCALE_VISITOR_H__
    #include "Utils/DXFScaleVisitor.h"
#endif

#ifndef __DXF_HATCH_FINDER_VISITOR_H__
    #include "Utils/DXFHatchFinderVisitor.h"
#endif

#ifndef __DXF_TEXT_FINDER_VISITOR_H__
    #include "Utils/DXFTextFinderVisitor.h"
#endif

#ifndef __DXF_UTILS_H__
    #include "Utils/DXFUtils.h"
#endif

#endif
