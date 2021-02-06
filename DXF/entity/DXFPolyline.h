#ifndef __DXF_POLYLINE_H__
#define __DXF_POLYLINE_H__
#pragma once

#ifndef __DXF_3DPOINT_H__
    #include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "../base/DXFLineWeight.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

#include "vector"

class CDXFGeBulgePoint3D
{
// Construction/Destruction
public:
    CDXFGeBulgePoint3D() { }
    CDXFGeBulgePoint3D(double x, double y, double z, double dBulge) : m_dBulge(dBulge) { m_point.SetValues(x, y, z); }
    CDXFGeBulgePoint3D(const CDXF3DPoint &pt, double dBulge) : m_dBulge(dBulge) { m_point = pt; }
    CDXFGeBulgePoint3D(const CDXFGeBulgePoint3D &rhs) { *this = rhs; }

// Operators
public:
    CDXFGeBulgePoint3D &operator=(const CDXFGeBulgePoint3D &rhs) {
        m_point  = rhs.m_point;
        m_dBulge = rhs.m_dBulge;
        return (*this);
    }

// Attributes
public:
    CDXF3DPoint m_point;
    double m_dBulge {0.};
};

class CDXFPolyline : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFPolyline() : CDXFEntity("LWPOLYLINE") { }
    CDXFPolyline(const std::vector<CDXFGeBulgePoint3D> &vertices, bool bClosed)
        : CDXFEntity("LWPOLYLINE"), m_bClosed(bClosed), m_vertices(vertices) { }
    CDXFPolyline(const CDXFPolyline &polyline)
        : CDXFEntity(polyline, "LWPOLYLINE"), 
          m_eLineWeight(polyline.m_eLineWeight), 
          m_dConstantWidth(polyline.m_dConstantWidth),
          m_bClosed(polyline.m_bClosed),
          m_vertices(polyline.m_vertices) { }
    CDXFPolyline(double x1, double y1, double x2, double y2, const CDXFObjectID &layerID) 
        : CDXFEntity("LWPOLYLINE") { 
        this->AddVertex(x1, y1, 0.0, 0.0);
        this->AddVertex(x2, y2, 0.0, 0.0);
        this->SetLayerID(layerID);
    }
    virtual ~CDXFPolyline() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFPolyline(*this); }

// Operations
public:
    void AddVertex(const CDXFGeBulgePoint3D &vertex) { m_vertices.push_back(vertex); }
    void AddVertex(const CDXF3DPoint &vertex) { m_vertices.push_back(CDXFGeBulgePoint3D(vertex, 0.)); }
    void AddVertex(double x, double y, double z, double bulge) {
        AddVertex(CDXFGeBulgePoint3D(x, y, z, bulge));
    }
    bool IsClosed() const { return m_bClosed; }
    void SetClosed(bool bClosed) { m_bClosed = bClosed; }

    double GetConstantWidth() const { return m_dConstantWidth; }
    void SetConstantWidth(double dWidth) { m_dConstantWidth = dWidth; }
    
    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }
    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }

    std::vector<CDXFGeBulgePoint3D> &GetVertices() { return m_vertices; }

// Attributes
public:
    std::vector<CDXFGeBulgePoint3D> m_vertices;

private:
    bool m_bClosed {false};
    double m_dConstantWidth {0.0};
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif
