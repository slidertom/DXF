#ifndef __DXF_HATCH_H__
#define __DXF_HATCH_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "base/DXF3DPoint.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "base/DXFLineWeight.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

#include "vector"

class CDXFGePoint2D
{
// Construction/Destruction
public:
    CDXFGePoint2D() : m_x(0.), m_y(0.) { }
    CDXFGePoint2D(const CDXFGePoint2D &point) : m_x(point.m_x), m_y(point.m_y) { }
    CDXFGePoint2D(double x, double y) : m_x(x), m_y(y) { }
    ~CDXFGePoint2D() { }

// Operators
public:
    double operator[](int32_t nIndex) const {
        ASSERT(nIndex >= 0 && nIndex < 1);
        return m_coords[nIndex];
    }
    double &operator[](int32_t nIndex) {
        ASSERT(nIndex >= 0 && nIndex < 1);
        return m_coords[nIndex];
    }

// Attributes
public:
    union {
        double m_coords[2];
        struct {
            double m_x;
            double m_y;
        };
    };
};

class CDXFGeBulgePoint2D
{
// Construction/Destruction
public:
    CDXFGeBulgePoint2D() { }
    CDXFGeBulgePoint2D(const CDXFGePoint2D &pt, double dBulge) : m_dBulge(dBulge) { m_point = pt; };
    CDXFGeBulgePoint2D(const CDXFGeBulgePoint2D &rhs) { *this = rhs; }
    ~CDXFGeBulgePoint2D() { }

// Operators
public:
    CDXFGeBulgePoint2D &operator=(const CDXFGeBulgePoint2D &rhs) {
        m_point  = rhs.m_point;
        m_dBulge = rhs.m_dBulge;
        return (*this);
    }

// Attributes
public:
    CDXFGePoint2D m_point;
    double m_dBulge {0.};
};

class CDXFGeLineSeg2D
{
// Construction/Destruction
public:
    CDXFGeLineSeg2D() { }
    CDXFGeLineSeg2D(const CDXFGeLineSeg2D &seg) : m_start(seg.m_start), m_end(seg.m_end) { }
    CDXFGeLineSeg2D(const CDXFGePoint2D &start, const CDXFGePoint2D &end) : m_start(start), m_end(end) { }
    ~CDXFGeLineSeg2D() { }

// Operators
public:
    CDXFGeLineSeg2D &operator=(const CDXFGeLineSeg2D &seg) { m_start = seg.m_start; m_end = seg.m_end; return *this; }

// Attributes
public:
    CDXFGePoint2D m_start;
    CDXFGePoint2D m_end;
};

enum EDXFHatchBoundaryType
{
    eBoundaryTypeDefault   = 0,
    eBoundaryTypeExternal  = 1,
    eBoundaryTypePolyline  = 2,
    eBoundaryTypeDerived   = 4,
    eBoundaryTypeTextBox   = 8,
    eBoundaryTypeOutermost = 16,
};

enum EDXFHatchPatternType
{
    ePatternUserDefined = 0,
    ePatternPredefined  = 1,
    ePatternCustom      = 2,
};

enum EDXFHatchStyle
{
    eNormalStyle = 0,
    eOuterStyle  = 1,
    eIgnore      = 2,
};

class CDXFSplineControlPoint
{
public:
    CDXFSplineControlPoint() { }
    ~CDXFSplineControlPoint() { }

public:
    CDXFGePoint2D m_point;
    double m_dWeight {1.0};
};

class CDXFBoundaryEdgeVis;
class CDXFBoundaryEdgeBase
{
// Construction/Destruction
public:
    CDXFBoundaryEdgeBase() { }
    virtual ~CDXFBoundaryEdgeBase() { }

// Overrides
public:
    virtual void Accept(CDXFBoundaryEdgeVis &vis) = 0;
};

class CDXFBoundaryEdgeArc;
class CDXFBoundaryEdgeLine;
class CDXFBoundaryEdgeEllipse;
class CDXFBoundaryEdgeSpline;

class CDXFBoundaryEdgeVis
{
// Overrides
public:
    virtual void OnVisitEdgeArc(CDXFBoundaryEdgeArc         &edge) = 0;
    virtual void OnVisitEdgeLine(CDXFBoundaryEdgeLine       &edge) = 0;
    virtual void OnVisitEdgeEllipse(CDXFBoundaryEdgeEllipse &edge) = 0;
    virtual void OnVisitEdgeSpline(CDXFBoundaryEdgeSpline   &edge) = 0;
};

class CDXFNonPolylineBoundary : public std::vector<CDXFBoundaryEdgeBase *>
{
// Construction/Destruction
public:
    CDXFNonPolylineBoundary() { }
    ~CDXFNonPolylineBoundary(){ Clear(); }

// Operations
public:
    void Clear() {
        for (auto *pItem : *this) {
            delete pItem;
        }
        clear();
    }
};

class CDXFBoundaryEdgeArc: public CDXFBoundaryEdgeBase
{
// Construction/Destruction
public:
    CDXFBoundaryEdgeArc() { }
    CDXFBoundaryEdgeArc(const CDXFBoundaryEdgeArc &rhs);
    virtual ~CDXFBoundaryEdgeArc() { }

// Attributes
public:
    CDXFGePoint2D m_centerPoint;
    double m_dStartAngle {0.};
    double m_dEndAngle   {0.};
    double m_dRadius     {0.};
    int32_t m_nCounterclockwise {0};

// Overrides CDXFBoundaryEdgeBase
public:
    virtual void Accept(CDXFBoundaryEdgeVis &vis) override { vis.OnVisitEdgeArc(*this); }
};

class CDXFBoundaryEdgeLine: public CDXFBoundaryEdgeBase
{
// Construction/Destruction
public:
    CDXFBoundaryEdgeLine(){ }
    CDXFBoundaryEdgeLine(const CDXFBoundaryEdgeLine &rhs) { m_startPoint = rhs.m_startPoint; m_endPoint = rhs.m_endPoint; }
    virtual ~CDXFBoundaryEdgeLine() { }

// Overrides
public:
    virtual void Accept(CDXFBoundaryEdgeVis &vis) override { vis.OnVisitEdgeLine(*this); }

// Attributes
public:
    CDXFGePoint2D m_startPoint;
    CDXFGePoint2D m_endPoint;
};

class CDXFBoundaryEdgeEllipse: public CDXFBoundaryEdgeBase
{
// Construction/Destruction
public:
    CDXFBoundaryEdgeEllipse() { } 
    CDXFBoundaryEdgeEllipse(const CDXFBoundaryEdgeEllipse &rhs);
    virtual ~CDXFBoundaryEdgeEllipse() { }

// Attributes
public:
    CDXFGePoint2D m_centerPoint;
    CDXFGePoint2D m_endPoint;
    double m_dMinorAxisLength   {0.};
    double m_dStartAngle        {0.};
    double m_dEndAngle          {0.};
    int32_t m_nCounterclockwise {0};

// Overrides
public:
    virtual void Accept(CDXFBoundaryEdgeVis &vis) override { vis.OnVisitEdgeEllipse(*this); }
};

class CDXFBoundaryEdgeSpline: public CDXFBoundaryEdgeBase
{
// Construction/Destruction
public:
    CDXFBoundaryEdgeSpline() { }
    CDXFBoundaryEdgeSpline(const CDXFBoundaryEdgeSpline &rhs) {
        m_dDegree                = rhs.m_dDegree;
        m_dRational              = rhs.m_dRational;
        m_dPeriodic              = rhs.m_dPeriodic;
        m_knotValuesArray        = rhs.m_knotValuesArray;
        m_controlPointsArray     = rhs.m_controlPointsArray;
    }
    virtual ~CDXFBoundaryEdgeSpline() { }

// Attributes
public:
    double m_dDegree   {0.};
    double m_dRational {0.};
    double m_dPeriodic {0.};
    std::vector<double> m_knotValuesArray;
    std::vector<CDXFSplineControlPoint> m_controlPointsArray;

// Overrides
public:
    virtual void Accept(CDXFBoundaryEdgeVis &vis) override { vis.OnVisitEdgeSpline(*this); }
};

class CDXFAddBoundaryEdgeVis : public CDXFBoundaryEdgeVis
{
// Construction/Destruction
public:
    CDXFAddBoundaryEdgeVis(CDXFNonPolylineBoundary* pBand) : m_pBand(pBand) { }
    virtual ~CDXFAddBoundaryEdgeVis() { }

// Overrides
public:
    virtual void OnVisitEdgeArc(CDXFBoundaryEdgeArc &edge) override {
        m_pBand->push_back(new CDXFBoundaryEdgeArc(edge));
    }

    virtual void OnVisitEdgeLine(CDXFBoundaryEdgeLine &edge) override {
        m_pBand->push_back(new CDXFBoundaryEdgeLine(edge));
    }

    virtual void OnVisitEdgeEllipse(CDXFBoundaryEdgeEllipse &edge) override {
        m_pBand->push_back(new CDXFBoundaryEdgeEllipse(edge));
    }

    virtual void OnVisitEdgeSpline(CDXFBoundaryEdgeSpline &edge) override {
        m_pBand->push_back(new CDXFBoundaryEdgeSpline(edge));
    }

// Attributes
private:
    CDXFNonPolylineBoundary* m_pBand;
};

class CDXFHatchBoundaryVis;

class CDXFHatchBoundary
{
// Construction/Destruction
public:
    CDXFHatchBoundary() { }
    virtual ~CDXFHatchBoundary() { }

public:
    int32_t GetType() const { return m_nType; };

// Overrides
public:
    virtual void Accept(CDXFHatchBoundaryVis &vis) = 0;

// Attributes
public:
    int32_t m_nType;
};

class CDXFHatchBoundaryVis 
{
    friend class CDXFHatchPolylineBoundary;
    friend class CDXFHatchNonPolylineBoundary;

// Overrides
protected:
    virtual void OnVisitPolyLine(CDXFHatchPolylineBoundary       *pBoundry) = 0;
    virtual void OnVisitNonPolyLine(CDXFHatchNonPolylineBoundary *pBoundry) = 0;    
};

class CDXFHatchPolylineBoundary : public CDXFHatchBoundary
{
// Construction/Destruction
public:
    CDXFHatchPolylineBoundary(int32_t nType) {
        m_nType     = nType;
        m_nType    |= eBoundaryTypePolyline;
    }
    CDXFHatchPolylineBoundary(const CDXFHatchPolylineBoundary &boundary) {
        m_nType     = boundary.m_nType;
        m_nClosed   = boundary.m_nClosed;
        m_nHasBulge = boundary.m_nHasBulge;
        m_vertices  = boundary.m_vertices;
    }
    virtual ~CDXFHatchPolylineBoundary() { }

// Operations
public:
    void GetVertices(std::vector<CDXFGeBulgePoint2D> &vertices) const { vertices = m_vertices; }
    void SetVertices(const std::vector<CDXFGeBulgePoint2D> &vertices) { m_vertices = vertices; }

    void SetVertices(const std::vector<CDXFGePoint2D> &vertices) {
        size_t nVerticesCnt = vertices.size();
        m_vertices.clear();
        m_vertices.reserve(nVerticesCnt);
        for (const auto &pt : vertices) {
            m_vertices.push_back(CDXFGeBulgePoint2D(pt, 0.));
        }
    }

// Overrides
public:
    virtual void Accept(CDXFHatchBoundaryVis &vis) { vis.OnVisitPolyLine(this); }

// Attributes
public:
    int32_t m_nHasBulge {1};
    int32_t m_nClosed   {1};
    std::vector<CDXFGeBulgePoint2D> m_vertices;
};

class CDXFGeCurve2D;
class DXFDLLAPI CDXFHatchNonPolylineBoundary : public CDXFHatchBoundary
{
    friend class CDXFHatch;

public:
    CDXFHatchNonPolylineBoundary(int32_t nType);
    CDXFHatchNonPolylineBoundary(const CDXFHatchNonPolylineBoundary &boundary);
    virtual ~CDXFHatchNonPolylineBoundary() { delete m_pBand; }

public:
    // Add boundary segment
    void AddLine(const CDXFGePoint2D &startPoint, const CDXFGePoint2D &endPoint);
    void AddLines(const std::vector<CDXFGePoint2D> &points) {
        for (auto it1 = ++points.begin(); it1 != points.end(); ++it1) {
            AddLine(*std::prev(it1), *it1);
        }
    }
    void AddArc(const CDXFGePoint2D &centerPoint, double dStartAngle, double dEndAngle, double dRadius, int32_t bCounterclockwise);
    void AddEllipse(const CDXFGePoint2D &centerPoint, const CDXFGePoint2D &endPoint, 
                    double dMinorAxisLength, double dStartAngle, double dEndAngle, int32_t nCounterclockwise = 0);
    void AddSpline(double dDegree, double dRational, double dPeriodic, const std::vector<double> &knotValuesArray, 
                   const std::vector<CDXFSplineControlPoint> &controlPointsArray);

    CDXFNonPolylineBoundary *GetBand() { return m_pBand; }
    void SetBand(CDXFNonPolylineBoundary &band);

protected:
    void SetNumEdges(size_t nNumEdges);
    void SetEdgeDataLine(size_t nEdgeIndex, CDXFGeLineSeg2D *pEdgeData);

    virtual void Accept(CDXFHatchBoundaryVis &vis) override { vis.OnVisitNonPolyLine(this); }

public:
    CDXFNonPolylineBoundary *m_pBand;
};

class DXFDLLAPI CDXFHatch : public CDXFEntity
{
// Nested classes
public:
    class CDXFHatchPattern
    {
    // Construction/Destruction
    public:
        CDXFHatchPattern() { }
        CDXFHatchPattern(const CDXFHatchPattern &pattern) { *this = pattern; }
        ~CDXFHatchPattern() { }

    // Operators
    public:
        CDXFHatchPattern &operator=(const CDXFHatchPattern &pattern) {
            m_dAngle     = pattern.m_dAngle;
            m_basePoint  = pattern.m_basePoint;
            m_offset     = pattern.m_offset;
            m_dashes     = pattern.m_dashes;
            return *this;
        }

    // Operations
    public:
        void SetAngle(double dAngle)                 { m_dAngle     = dAngle;     }
        void SetBasePoint(CDXFGePoint2D basePoint)   { m_basePoint  = basePoint;  }
        void SetOffset(const CDXFGePoint2D  &offset) { m_offset     = offset;     }
        void AddDashe(double dashe)                  { m_dashes.push_back(dashe); }

    // Attributes
    public:
        double m_dAngle {0.};
        CDXFGePoint2D m_basePoint;
        CDXFGePoint2D m_offset;
        std::vector<double> m_dashes;
    };

// Construction/Destruction
public:
    CDXFHatch() : CDXFEntity("HATCH") { m_sPatternName = "LINE"; }
    CDXFHatch(const char *sPatternName, double dAngle, double dScale, const CDXFObjectID &layerID)  
        : CDXFEntity("HATCH") {
        this->SetPattern(ePatternPredefined, sPatternName, dAngle, dScale);
        this->SetLayerID(layerID);
    }
    CDXFHatch(const CDXFHatch &hatch);
    virtual ~CDXFHatch() { 
        for (auto *pBoundry : m_boundaryPtrs) {
            delete pBoundry;
        }
        m_boundaryPtrs.clear();
    }

public:
    CDXFHatchPolylineBoundary *CreatePolylineBoundary() {
        CDXFHatchPolylineBoundary *pBoundary = new CDXFHatchPolylineBoundary(eBoundaryTypePolyline);
        m_boundaryPtrs.push_back(pBoundary);
        return pBoundary;
    }

    CDXFHatchNonPolylineBoundary *CreateNonPolylineBoundary() {
        CDXFHatchNonPolylineBoundary *pBoundary = new CDXFHatchNonPolylineBoundary(eBoundaryTypeExternal);
        m_boundaryPtrs.push_back(pBoundary);
        return pBoundary;
    }

    // line_points {0, 1, 2} => lines: {0, 1}; {1, 2)
    CDXFHatchNonPolylineBoundary *CreateNonPolylineBoundary(const std::vector<CDXFGePoint2D> &line_points) {
        CDXFHatchNonPolylineBoundary *pBoundry = CreateNonPolylineBoundary();
        pBoundry->AddLines(line_points);
        return pBoundry;
    }

    void AddBoundaryLines(int32_t nType, const std::vector<CDXFGeLineSeg2D *> &edges);
    void AddBoundary(int32_t nType, const std::vector<CDXFGePoint2D> &vertices);

    void SetPattern(enum EDXFHatchPatternType eType, const char* sName, double dAngle, double dScale);

    void SetPatternAngle(double dAngle) { m_dPatternAngle = dAngle; }
    void SetPatternScale(double dScale) { m_dPatternScale = dScale; }

    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }
    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }
    
// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFHatch(*this); }
    
// Attributes    
public:
    CDXF3DPoint m_elevation;
    std::string m_sPatternName;

    EDXFHatchStyle m_eStyle             {eNormalStyle};       // code 75
    EDXFHatchPatternType m_ePatternType {ePatternPredefined}; // code 76

    double m_dDensity {0.};

    double m_dPatternAngle {0.};
    double m_dPatternScale {1.};

    int32_t m_fSolidFill;        // solid fill  = 1; pattern fill    = 0
    int32_t m_fAssociative {0};    // associative = 1; non-associative = 0
    std::vector<CDXFHatchBoundary *> m_boundaryPtrs;
    std::vector<CDXF3DPoint>         m_seedPoints;
    std::vector<CDXFHatchPattern>    m_patternLines;

    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif
