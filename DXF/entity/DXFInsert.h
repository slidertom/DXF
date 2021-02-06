#ifndef __DXF_INSERT_H__
#define __DXF_INSERT_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_MATRIX3D_H__
    #include "../base/DXFMatrix3D.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_BLOCKTABLERECORD_H__
    #include "../tables/DXFBlockTableRecord.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "DXFLineWeight.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

class DXFDLLAPI CDXFInsert : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFInsert() : CDXFEntity("INSERT") { }
    virtual ~CDXFInsert() { }  // m_attributes will be deleted in database object

private:
    CDXFInsert(const CDXFInsert &insert) 
        : CDXFEntity(insert, "INSERT"), m_pointIns(insert.m_pointIns), m_dAngle(insert.m_dAngle), m_scale(insert.m_scale), 
          m_eLineWeight(insert.m_eLineWeight) { }

// Static operations
public:
    static CDXFInsert *CDXFInsert::CreateInsert(const CDXFObjectID &recordID);
    static CDXFInsert *CDXFInsert::CreateInsert(const CDXFInsert &src, const CDXFObjectID &recordID);

// Operations
public:
    void SetBlockTableRecordID(const CDXFObjectID &recordID);
    bool IsNullBlockID() const { return m_blockID.IsNull(); }
    CDXFBlockTableRecord *GetBlock() const { return (CDXFBlockTableRecord *)m_blockID.GetObject(); }

    CDXF3DPoint GetInsertPoint() const { return m_pointIns; }
    void SetInsertPoint(const CDXF3DPoint &insPoint) { m_pointIns = insPoint; }

    void SetScale(double scale) { m_scale.SetValues(scale, scale, scale); }
    void SetScale(double x, double y, double z) { m_scale.SetValues(x, y, z); }
    CDXF3DPoint GetScale() const { return m_scale; }

    double GetAngle() const { return m_dAngle ; }
    void SetAngle(double dAngle) { m_dAngle = dAngle; }

    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }
    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }
   
    CDXFMatrix3D GetOCSToWCSMatrix() const;

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFInsert(*this); }

// Attributes
private:
    CDXF3DPoint m_pointIns;
    CDXF3DPoint m_scale {1., 1., 1.};
    double m_dAngle {0.}; // In radians
    CDXFObjectID m_blockID;
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif