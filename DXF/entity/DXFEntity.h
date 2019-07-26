#ifndef __DXF_ENTITY_H__
#define __DXF_ENTITY_H__
#pragma once

#ifndef __DXF_OBJECT_H__
	#include "../base/DXFObject.h"
#endif

class CDXFEntityVisitor;

class CDXFEntity : public CDXFObject
{
// Construction/Destruction
public:
    CDXFEntity(const CDXFEntity &entity) = delete;
    CDXFEntity(const char *sObjectName) : CDXFObject(sObjectName) { }
    CDXFEntity(const CDXFEntity &entity, const char *sObjectName) 
        : CDXFObject(sObjectName), m_bInModelSpace(entity.m_bInModelSpace) { }
    virtual ~CDXFEntity() { }

// Operations
public:
    const CDXFObjectID &GetLayerID() const { return m_layerID; }
    void SetLayerID(const CDXFObjectID &layerID) { m_layerID = layerID; }

    void SetInModelSpace(bool bInModelSpace) { m_bInModelSpace = bInModelSpace; };
    bool GetInModelSpace() const { return m_bInModelSpace; };

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) = 0;
    virtual void Accept(CDXFObjectVisitorAbs &vis) { vis.VisitDXFEntity(*this); }

// Attributes
private:
    bool m_bInModelSpace {true};
    CDXFObjectID m_layerID;
};

#endif
