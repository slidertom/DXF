#ifndef __DXF_POINT_H__
#define __DXF_POINT_H__
#pragma once

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "base/DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

class CDXFPoint : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFPoint() : CDXFEntity("POINT") { }
    CDXFPoint(CDXF3DPoint &point) : CDXFEntity("POINT"), m_point(point) { }
    CDXFPoint(const CDXFPoint &entity) : CDXFEntity(entity, "POINT"), m_point(entity.m_point) { }
    virtual ~CDXFPoint() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFPoint(*this); }

// Attributes
public:
    CDXF3DPoint m_point;
};

#endif
