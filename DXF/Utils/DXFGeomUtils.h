#ifndef __DXF_GEOM_UTILS_H__
#define __DXF_GEOM_UTILS_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_BOUNDINGBOX_H__
    #include "../base/DXFBoundingBox.h"
#endif

class CDXFBlockTableRecord;

namespace dxf_geom_utils
{
    DXFDLLAPI CDXFBoundingBox GetBlockBoundingBox(CDXFBlockTableRecord &block);
};

#endif
