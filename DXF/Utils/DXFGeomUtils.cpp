#include "stdafx.h"
#include "DXFGeomUtils.h"

#include "../Utils/DXFEntityBoundingBoxVisitor.h"

CDXFBoundingBox dxf_geom_utils::GetBlockBoundingBox(CDXFBlockTableRecord &block)
{
    return CDXFEntityBoundingBoxVisitor::GetBoundingBox(block);
}