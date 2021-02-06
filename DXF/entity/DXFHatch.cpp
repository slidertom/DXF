#include "stdafx.h"
#include "DXFHatch.h"

#include "Utils/DXFMath.h"
#include "DXFEntityVisitor.h"

#include "algorithm"

//********Class CDXFBoundaryEdgeArc**********
CDXFBoundaryEdgeArc::CDXFBoundaryEdgeArc(const CDXFBoundaryEdgeArc &rhs)
{
    m_centerPoint  = rhs.m_centerPoint;
    m_dStartAngle  = rhs.m_dStartAngle;
    m_dEndAngle    = rhs.m_dEndAngle;
    m_dRadius      = rhs.m_dRadius;
    m_nCounterclockwise = rhs.m_nCounterclockwise;
}

//********Class CDXFBoundaryEdgeEllipse**********
CDXFBoundaryEdgeEllipse::CDXFBoundaryEdgeEllipse(const CDXFBoundaryEdgeEllipse &rhs)
{
    m_centerPoint       = rhs.m_centerPoint;
    m_endPoint          = rhs.m_endPoint;
    m_dMinorAxisLength  = rhs.m_dMinorAxisLength;
    m_dStartAngle       = rhs.m_dStartAngle;
    m_dEndAngle         = rhs.m_dEndAngle;
    m_nCounterclockwise = rhs.m_nCounterclockwise;
}

class CDXFHatchBoundaryCloneVisitor : private CDXFHatchBoundaryVis
{
// Construction/Destruction
private:
    CDXFHatchBoundaryCloneVisitor() { }
    virtual ~CDXFHatchBoundaryCloneVisitor() { }

// Static operations
public:
    static CDXFHatchBoundary *Clone(CDXFHatchBoundary &boundry)  { 
        CDXFHatchBoundaryCloneVisitor vis;
        boundry.Accept(vis);
        return vis.m_pBound;
    }

// Overrides
private:
    virtual void OnVisitPolyLine(CDXFHatchPolylineBoundary       *pBoundry) override { m_pBound = new CDXFHatchPolylineBoundary(*pBoundry);    }
    virtual void OnVisitNonPolyLine(CDXFHatchNonPolylineBoundary *pBoundry) override { m_pBound = new CDXFHatchNonPolylineBoundary(*pBoundry); }

public:
    CDXFHatchBoundary *GetBoundary() const { return m_pBound; }

// Attributes
private:
    CDXFHatchBoundary *m_pBound {nullptr};
};

CDXFHatch::CDXFHatch(const CDXFHatch &hatch) 
: CDXFEntity(hatch, "HATCH")
{
    m_sPatternName   = hatch.m_sPatternName.c_str();
    m_dPatternAngle  = hatch.m_dPatternAngle;
    m_dPatternScale  = hatch.m_dPatternScale;

    m_dDensity = hatch.m_dDensity;
    
    m_fSolidFill   = hatch.m_fSolidFill;        // solid fill = 1; pattern fill = 0
    m_fAssociative = hatch.m_fAssociative;
    m_eLineWeight  = hatch.m_eLineWeight;

    for (CDXFHatchBoundary *pObj : hatch.m_boundaryPtrs) {
        auto *pBoundry = CDXFHatchBoundaryCloneVisitor::Clone(*pObj);
        ASSERT(pBoundry);
        m_boundaryPtrs.push_back(pBoundry);
    }

    m_seedPoints   = hatch.m_seedPoints;
    m_eStyle       = hatch.m_eStyle;
    m_ePatternType = hatch.m_ePatternType;
    m_patternLines = hatch.m_patternLines;
}

void CDXFHatch::AddBoundaryLines(int32_t nType, const std::vector<CDXFGeLineSeg2D *> &edges)
{
    size_t nNumEdges = edges.size();
    CDXFHatchNonPolylineBoundary *pBoundary = new CDXFHatchNonPolylineBoundary(nType);

    pBoundary->SetNumEdges(nNumEdges);
    for (size_t i = 0; i < nNumEdges; ++i) {
        pBoundary->SetEdgeDataLine(i, edges[i]);
    }

    m_boundaryPtrs.push_back(pBoundary);
}

void CDXFHatch::AddBoundary(int32_t nType, const std::vector<CDXFGePoint2D> &vertices)
{
    CDXFHatchPolylineBoundary *pBoundary = new CDXFHatchPolylineBoundary(nType);
    pBoundary->SetVertices(vertices);
    m_boundaryPtrs.push_back(pBoundary);
}

void CDXFHatch::SetPattern(enum EDXFHatchPatternType eType, const char* sName, double dAngle, double dScale)
{
    if (::strcmpi(sName, "SOLID") == 0) {
        m_fSolidFill = 1;
    }
    else {
        m_fSolidFill = 0;
    }

    m_sPatternName  = sName;
    m_ePatternType  = eType;
    m_dPatternAngle = dAngle;
    m_dPatternScale = dScale;
    
    if (m_sPatternName == "SOLID") {
        return;
    }

    if (m_sPatternName == "LINE") {
        return;
    }
    
    if (m_sPatternName == "ANSI31")
    {
        CDXFHatchPattern hatchPatern1;
        m_eStyle           = eNormalStyle;
        m_ePatternType     = ePatternPredefined;
        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x = -::sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y = ::cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ANSI32")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle           = eNormalStyle;
        m_ePatternType     = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x = -::sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y =  ::cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);
        
        hatchPatern2.SetAngle(45.0);
        basePoint.m_x = 0.176776695;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        x = -::sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y =  ::cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "ANSI33")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        
        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45.0);
        basePoint.m_x = 0.176776695;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.0625);
        m_patternLines.push_back(hatchPatern2);
        
        return;
    }

    if (m_sPatternName == "ANSI34")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        
        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45.0);
        basePoint.m_x = 0.176776695;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(45.0);
        basePoint.m_x = 0.353553391;
        basePoint.m_y = 0.0;
        hatchPatern3.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern3.SetOffset(offset);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(45.0);
        basePoint.m_x = 0.530330086;
        basePoint.m_y = 0.0;
        hatchPatern4.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern4.SetOffset(offset);
        m_patternLines.push_back(hatchPatern4);
        return;

    }

    if (m_sPatternName == "ANSI35")
    {

        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
                
        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45.0);
        basePoint.m_x = 0.176776695;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.3125);
        hatchPatern2.AddDashe(-0.0625);
        hatchPatern2.AddDashe(0.0);
        hatchPatern2.AddDashe(-0.0625);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "ANSI36")
    {
        CDXFHatchPattern hatchPatern1;
                        
        m_eStyle       = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x = -::sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y = ::cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.3125);
        hatchPatern1.AddDashe(-0.0625);
        hatchPatern1.AddDashe(0.0);
        hatchPatern1.AddDashe(-0.0625);
        m_patternLines.push_back(hatchPatern1);

        return;
    }

    if (m_sPatternName == "ANSI37")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
                
        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(135.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);
    }

    if (m_sPatternName == "ANSI38")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
                
        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(45.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        double x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        double y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        CDXFGePoint2D offset(x, y);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(135.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        x=-sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        y=cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
        offset.m_x = x;
        offset.m_y = y;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);
    }

    if (m_sPatternName == "ISO02W100")
    {
        m_sPatternName = "ACAD_ISO02W100";

        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.5);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(1.2);
        hatchPatern1.AddDashe(-0.3);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO03W100")
    {
        m_sPatternName = "ACAD_ISO03W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.9);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO04W100")
    {
        m_sPatternName = "ACAD_ISO04W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 5.0);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(24.0);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.5);
        hatchPatern1.AddDashe(-3.0);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO05W100")
    {
        m_sPatternName = "ACAD_ISO05W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.5);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(2.4);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.05);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.05);
        hatchPatern1.AddDashe(-3.0);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO06W100")
    {
        m_sPatternName = "ACAD_ISO06W100";
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(1.2);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.325);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.25;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(-1.7);
        hatchPatern2.AddDashe(0.025);
        hatchPatern2.AddDashe(-0.15);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "ISO07W100")
    {
        m_sPatternName = "ACAD_ISO07W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.15);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO08W100")
    {
        m_sPatternName = "ACAD_ISO08W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.1);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.48);
        hatchPatern1.AddDashe(-0.06);
        hatchPatern1.AddDashe(0.12);
        hatchPatern1.AddDashe(-0.06);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO09W100")
    {
        m_sPatternName = "ACAD_ISO09W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.1);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.48);
        hatchPatern1.AddDashe(-0.06);
        hatchPatern1.AddDashe(0.12);
        hatchPatern1.AddDashe(-0.06);
        hatchPatern1.AddDashe(0.12);
        hatchPatern1.AddDashe(-0.06);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO10W100")
    {
        m_sPatternName = "ACAD_ISO10W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.15);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO11W100")
    {
        m_sPatternName = "ACAD_ISO11W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.15);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO12W100")
    {
        m_sPatternName = "ACAD_ISO12W100";
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 5.0);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(12.0);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.5);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.5);
        hatchPatern1.AddDashe(-3.0);
        m_patternLines.push_back(hatchPatern1);
        return;
    }

    if (m_sPatternName == "ISO13W100")
    {
        m_sPatternName = "ACAD_ISO13W100";
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.325);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.25;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(-1.6757);
        hatchPatern2.AddDashe(0.025);
        hatchPatern2.AddDashe(-0.15);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "ISO14W100")
    {
        m_sPatternName = "ACAD_ISO14W100";
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 5.0);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(12.0);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.5);
        hatchPatern1.AddDashe(-3.0);
        hatchPatern1.AddDashe(0.5);
        hatchPatern1.AddDashe(-6.5);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 5.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(-22.0);
        hatchPatern2.AddDashe(0.5);
        hatchPatern2.AddDashe(-3.0);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "ISO15W100")
    {
        m_sPatternName = "ACAD_ISO15W100";
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.25);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.6);
        hatchPatern1.AddDashe(-0.15);
        hatchPatern1.AddDashe(0.025);
        hatchPatern1.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.25;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(-1.675);
        hatchPatern2.AddDashe(0.025);
        hatchPatern2.AddDashe(-0.15);
        hatchPatern2.AddDashe(0.025);
        hatchPatern2.AddDashe(-0.15);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "ANGLE")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.275);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.2);
        hatchPatern1.AddDashe(-0.075);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.275;
        offset.m_y = 0.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.2);
        hatchPatern2.AddDashe(-0.075);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "AR-B816")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 8.0);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -8.0;
        offset.m_y = 8.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(8.0);
        hatchPatern2.AddDashe(-8.0);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "AR-B816C")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(8.0, 8.0);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(15.625);
        hatchPatern1.AddDashe(-0.375);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0.0);
        basePoint.m_x = -8.0;
        basePoint.m_y = 0.375;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 8.0;
        offset.m_y = 8.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(15.625);
        hatchPatern2.AddDashe(-0.375);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(90.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -8.0;
        offset.m_y = 8.0;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(-8.375);
        hatchPatern3.AddDashe(7.625);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(90.0);
        basePoint.m_x = -0.375;
        basePoint.m_y = 0.0;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -8.0;
        offset.m_y = 8.0;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(-8.375);
        hatchPatern4.AddDashe(7.625);
        m_patternLines.push_back(hatchPatern4);
        return;
    }

    if (m_sPatternName == "AR-B88")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 8.0);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -4.0;
        offset.m_y = 8.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(8.0);
        hatchPatern2.AddDashe(-8.0);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "AR-BRELM")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.5334);
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.7625);
        hatchPatern1.AddDashe(-0.0375);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.225;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.5334;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.7625);
        hatchPatern2.AddDashe(-0.0375);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0.0);
        basePoint.m_x = 0.2;
        basePoint.m_y = 0.2667;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.5334;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.3625);
        hatchPatern3.AddDashe(-0.0375);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(0.0);
        basePoint.m_x = 0.2;
        basePoint.m_y = 0.4917;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.5334;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.3625);
        hatchPatern4.AddDashe(-0.0375);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(90.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = -0.8;
        offset.m_y = 0.0;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.225);
        hatchPatern5.AddDashe(-0.3084);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(90.0);
        basePoint.m_x = -0.0375;
        basePoint.m_y = 0.0;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -0.8;
        offset.m_y = 0.0;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.225);
        hatchPatern6.AddDashe(-0.3084);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(90.0);
        basePoint.m_x = 0.2;
        basePoint.m_y = 0.2667;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = -0.4;
        offset.m_y = 0.0;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(0.225);
        hatchPatern7.AddDashe(-0.3084);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(90.0);
        basePoint.m_x = 0.1625;
        basePoint.m_y = 0.2667;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = -0.4;
        offset.m_y = 0.0;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(0.225);
        hatchPatern8.AddDashe(-0.3084);
        m_patternLines.push_back(hatchPatern8);
    
        return;
    }

    if (m_sPatternName == "AR-BRSTD")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;

        hatchPatern1.SetAngle(0.0);
        CDXFGePoint2D basePoint(0.0, 0.0);
        hatchPatern1.SetBasePoint(basePoint);
        CDXFGePoint2D offset(0.0, 0.2667);
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90.0);
        basePoint.m_x = 0.0;
        basePoint.m_y = 0.0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.4;
        offset.m_y = 0.2667;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.2667);
        hatchPatern2.AddDashe(-0.2667);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName == "AR-CONC")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;
        CDXFHatchPattern hatchPatern9;
        CDXFHatchPattern hatchPatern10;
        CDXFHatchPattern hatchPatern11;
        CDXFHatchPattern hatchPatern12;
        CDXFHatchPattern hatchPatern13;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;
        //double x, y;

        hatchPatern1.SetAngle(50);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 7.172601826007534;
        offset.m_y = -0.627521349825951;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.750);
        hatchPatern1.AddDashe(-8.250);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(355);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -1.38751333613;
        offset.m_y = 7.521921337000118;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.6);
        hatchPatern2.AddDashe(-6.6);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(100.4514);
        basePoint.m_x = 0.5977168;
        basePoint.m_y = -0.0522934;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 5.785093817140215;
        offset.m_y = 6.894395420609535;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.6374019);
        hatchPatern3.AddDashe(-7.01142112);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(46.1842);
        basePoint.m_x = 0;
        basePoint.m_y = 2;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 10.672410486256441;
        offset.m_y = -1.655190143290181;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(1.125);
        hatchPatern4.AddDashe(-12.375);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(96.6356);
        basePoint.m_x = 0.88936745;
        basePoint.m_y = 1.86206693;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = 9.346627055487033;
        offset.m_y = 9.741178709004418;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.95610288);
        hatchPatern5.AddDashe(-10.51713);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(351.1842);
        basePoint.m_x = 0;
        basePoint.m_y = 2;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = 9.346619451336270;
        offset.m_y = 9.741186009253246;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.9);
        hatchPatern6.AddDashe(-9.9);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(21);
        basePoint.m_x = 1;
        basePoint.m_y = 1.5;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = 5.969070521184616;
        offset.m_y = -4.026188906653388;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(0.750);
        hatchPatern7.AddDashe(-8.250);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(326);
        basePoint.m_x = 1;
        basePoint.m_y = 1.5;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = 2.433153319215817;
        offset.m_y = 7.251500450468530;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(0.6);
        hatchPatern8.AddDashe(-6.6);
        m_patternLines.push_back(hatchPatern8);

        hatchPatern9.SetAngle(71.451400000000007);
        basePoint.m_x = 1.49742233;
        basePoint.m_y = 1.16448394;
        hatchPatern9.SetBasePoint(basePoint);
        offset.m_x = 8.4022262858149;
        offset.m_y = 3.225304967099523;
        hatchPatern9.SetOffset(offset);
        hatchPatern9.AddDashe(0.6374019);
        hatchPatern9.AddDashe(-7.01142112);
        m_patternLines.push_back(hatchPatern9);

        hatchPatern10.SetAngle(37.5);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern10.SetBasePoint(basePoint);
        offset.m_x = 0.121598553172907;
        offset.m_y = 3.328938538313114;
        hatchPatern10.SetOffset(offset);
        hatchPatern10.AddDashe(0);
        hatchPatern10.AddDashe(-6.52);
        hatchPatern10.AddDashe(0);
        hatchPatern10.AddDashe(-6.7);
        hatchPatern10.AddDashe(0);
        hatchPatern10.AddDashe(-6.625);
        m_patternLines.push_back(hatchPatern10);

        hatchPatern11.SetAngle(7.4);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern11.SetBasePoint(basePoint);
        offset.m_x = 2.630695374421486;
        offset.m_y = 3.944117118823602;
        hatchPatern11.SetOffset(offset);
        hatchPatern11.AddDashe(0);
        hatchPatern11.AddDashe(-3.82);
        hatchPatern11.AddDashe(0);
        hatchPatern11.AddDashe(-6.370);
        hatchPatern11.AddDashe(0);
        hatchPatern11.AddDashe(-2.525);
        m_patternLines.push_back(hatchPatern11);

        hatchPatern12.SetAngle(327.5);
        basePoint.m_x = -2.23;
        basePoint.m_y = 0;
        hatchPatern12.SetBasePoint(basePoint);
        offset.m_x = 5.338224361724090;
        offset.m_y = -0.225548717343798;
        hatchPatern12.SetOffset(offset);
        hatchPatern12.AddDashe(0);
        hatchPatern12.AddDashe(-2.5);
        hatchPatern12.AddDashe(0);
        hatchPatern12.AddDashe(-7.8);
        hatchPatern12.AddDashe(0);
        hatchPatern12.AddDashe(-10.35);
        m_patternLines.push_back(hatchPatern12);

        hatchPatern13.SetAngle(317.5);
        basePoint.m_x = -3.23;
        basePoint.m_y = 0;
        hatchPatern13.SetBasePoint(basePoint);
        offset.m_x = 5.831861693423862;
        offset.m_y = 1.001049823323176;
        hatchPatern13.SetOffset(offset);
        hatchPatern13.AddDashe(0);
        hatchPatern13.AddDashe(-3.250);
        hatchPatern13.AddDashe(0);
        hatchPatern13.AddDashe(-5.18);
        hatchPatern13.AddDashe(0);
        hatchPatern13.AddDashe(-7.35);
        m_patternLines.push_back(hatchPatern13);

        CDXF3DPoint seedPoint1;

        seedPoint1.x = 3.511598984064049;
        seedPoint1.y = -0.501227241547014;
        m_seedPoints.push_back(seedPoint1);

        return;
    }

    if (m_sPatternName == "AR-HBONE")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;
        
        hatchPatern1.SetAngle(45);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 5.656854249492379;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(12);
        hatchPatern1.AddDashe(-4);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(135);
        basePoint.m_x = 2.828427125;
        basePoint.m_y = 2.828427125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 5.656854249492379;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(12);
        hatchPatern2.AddDashe(-4);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "AR-PARQ1")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;
        CDXFHatchPattern hatchPatern9;
        CDXFHatchPattern hatchPatern10;
        CDXFHatchPattern hatchPatern11;
        CDXFHatchPattern hatchPatern12;
        CDXFHatchPattern hatchPatern13;
        CDXFHatchPattern hatchPatern14;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(12);
        hatchPatern1.AddDashe(-12);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 2;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(12);
        hatchPatern2.AddDashe(-12);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(90);
        basePoint.m_x = 4;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(12);
        hatchPatern3.AddDashe(-12);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(90);
        basePoint.m_x = 6;
        basePoint.m_y = 0;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(12);
        hatchPatern4.AddDashe(-12);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(90);
        basePoint.m_x = 8;
        basePoint.m_y = 0;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(12);
        hatchPatern5.AddDashe(-12);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(90);
        basePoint.m_x = 10;
        basePoint.m_y = 0;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(12);
        hatchPatern6.AddDashe(-12);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(90);
        basePoint.m_x = 12;
        basePoint.m_y = 0;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = -12;
        offset.m_y = 12;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(12);
        hatchPatern7.AddDashe(-12);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 12;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(12);
        hatchPatern8.AddDashe(-12);
        m_patternLines.push_back(hatchPatern8);

        hatchPatern9.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 14;
        hatchPatern9.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern9.SetOffset(offset);
        hatchPatern9.AddDashe(12);
        hatchPatern9.AddDashe(-12);
        m_patternLines.push_back(hatchPatern9);

        hatchPatern10.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 16;
        hatchPatern10.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern10.SetOffset(offset);
        hatchPatern10.AddDashe(12);
        hatchPatern10.AddDashe(-12);
        m_patternLines.push_back(hatchPatern10);

        hatchPatern11.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 18;
        hatchPatern11.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern11.SetOffset(offset);
        hatchPatern11.AddDashe(12);
        hatchPatern11.AddDashe(-12);
        m_patternLines.push_back(hatchPatern11);

        hatchPatern12.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 20;
        hatchPatern12.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern12.SetOffset(offset);
        hatchPatern12.AddDashe(12);
        hatchPatern12.AddDashe(-12);
        m_patternLines.push_back(hatchPatern12);

        hatchPatern13.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 22;
        hatchPatern13.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern13.SetOffset(offset);
        hatchPatern13.AddDashe(12);
        hatchPatern13.AddDashe(-12);
        m_patternLines.push_back(hatchPatern13);

        hatchPatern14.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 24;
        hatchPatern14.SetBasePoint(basePoint);
        offset.m_x = 12;
        offset.m_y = -12;
        hatchPatern14.SetOffset(offset);
        hatchPatern14.AddDashe(12);
        hatchPatern14.AddDashe(-12);
        m_patternLines.push_back(hatchPatern14);

        return;
    }

    if (m_sPatternName == "AR-RROOF")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 2.2;
        offset.m_y = 1;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(15);
        hatchPatern1.AddDashe(-2);
        hatchPatern1.AddDashe(5);
        hatchPatern1.AddDashe(-1);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 1.330;
        basePoint.m_y = 0.5;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -1;
        offset.m_y = 1.330;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(3);
        hatchPatern2.AddDashe(-0.330);
        hatchPatern2.AddDashe(6);
        hatchPatern2.AddDashe(-0.750);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0.5;
        basePoint.m_y = 0.850;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 5.2;
        offset.m_y = 0.670;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(8);
        hatchPatern3.AddDashe(-1.4);
        hatchPatern3.AddDashe(4);
        hatchPatern3.AddDashe(-1);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "AR-RSHKE")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;
        CDXFHatchPattern hatchPatern9;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 25.5;
        offset.m_y = 12;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(6);
        hatchPatern1.AddDashe(-5);
        hatchPatern1.AddDashe(7);
        hatchPatern1.AddDashe(-3);
        hatchPatern1.AddDashe(9);
        hatchPatern1.AddDashe(-4);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 6;
        basePoint.m_y = 0.5;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 25.5;
        offset.m_y = 12;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(5);
        hatchPatern2.AddDashe(-19);
        hatchPatern2.AddDashe(4);
        hatchPatern2.AddDashe(-6);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 18;
        basePoint.m_y = -0.750;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 25.5;
        offset.m_y = 12;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(3);
        hatchPatern3.AddDashe(-31);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -8.5;
        offset.m_y = 12;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(11.5);
        hatchPatern4.AddDashe(-36.5);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(90);
        basePoint.m_x = 6;
        basePoint.m_y = 0;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = -8.5;
        offset.m_y = 12;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(11.250);
        hatchPatern5.AddDashe(-36.750);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(90);
        basePoint.m_x = 11;
        basePoint.m_y = 0;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -8.5;
        offset.m_y = 12;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(10.5);
        hatchPatern6.AddDashe(-37.5);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(90);
        basePoint.m_x = 18;
        basePoint.m_y = -0.750;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = -8.5;
        offset.m_y = 12;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(11.5);
        hatchPatern7.AddDashe(-36.5);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(90);
        basePoint.m_x = 21;
        basePoint.m_y = -0.750;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = -8.5;
        offset.m_y = 12;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(11.5);
        hatchPatern8.AddDashe(-36.5);
        m_patternLines.push_back(hatchPatern8);

        hatchPatern9.SetAngle(90);
        basePoint.m_x = 30;
        basePoint.m_y = 0;
        hatchPatern9.SetBasePoint(basePoint);
        offset.m_x = -8.5;
        offset.m_y = 12;
        hatchPatern9.SetOffset(offset);
        hatchPatern9.AddDashe(11);
        hatchPatern9.AddDashe(-37);
        m_patternLines.push_back(hatchPatern9);

        return;
    }

    if (m_sPatternName == "AR-SAND")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(37.5);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -0.062993358109608;
        offset.m_y = 1.926823769013158;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0);
        hatchPatern1.AddDashe(-1.520);
        hatchPatern1.AddDashe(0);
        hatchPatern1.AddDashe(-1.7);
        hatchPatern1.AddDashe(0);
        hatchPatern1.AddDashe(-1.625);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(7.5);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 1.769776705267727;
        offset.m_y = 2.822146065229740;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0);
        hatchPatern2.AddDashe(-0.820);
        hatchPatern2.AddDashe(0);
        hatchPatern2.AddDashe(-1.370);
        hatchPatern2.AddDashe(0);
        hatchPatern2.AddDashe(-0.525);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(327.5);
        basePoint.m_x = -1.23;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 3.114141861751495;
        offset.m_y = 0.005659053536964;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0);
        hatchPatern3.AddDashe(-0.5);
        hatchPatern3.AddDashe(0);
        hatchPatern3.AddDashe(-1.8);
        hatchPatern3.AddDashe(0);
        hatchPatern3.AddDashe(-2.350);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(317.5);
        basePoint.m_x = -1.23;
        basePoint.m_y = 0;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 3.006126604572293;
        offset.m_y = 0.877675564934249;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0);
        hatchPatern4.AddDashe(-0.250);
        hatchPatern4.AddDashe(0);
        hatchPatern4.AddDashe(-1.18);
        hatchPatern4.AddDashe(0);
        hatchPatern4.AddDashe(-1.350);
        m_patternLines.push_back(hatchPatern4);

        return;
    }

    if (m_sPatternName == "BOX")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -1;
        offset.m_y = 0;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0.250;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -1;
        offset.m_y = 0;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 1;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(-0.250);
        hatchPatern3.AddDashe(0.250);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.250;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 1;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(-0.250);
        hatchPatern4.AddDashe(0.250);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.5;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 1;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.250);
        hatchPatern5.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.750;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 1;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.250);
        hatchPatern6.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(90);
        basePoint.m_x = 0.5;
        basePoint.m_y = 0;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = -1;
        offset.m_y = 0;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(0.250);
        hatchPatern7.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(90);
        basePoint.m_x = 0.750;
        basePoint.m_y = 0;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = -1;
        offset.m_y = 0;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(0.250);
        hatchPatern8.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern8);

        return;
    }

    if (m_sPatternName == "BRASS")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.0625);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName =="BRICK")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.250);
        hatchPatern2.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(90);
        basePoint.m_x = 0.250;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(-0.250);
        hatchPatern3.AddDashe(0.250);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "BRSTONE")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;

        m_eStyle       = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.330;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0.9;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.330;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.330);
        hatchPatern2.AddDashe(-0.330);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(90);
        basePoint.m_x = 0.8;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.330;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.330);
        hatchPatern3.AddDashe(-0.330);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(0);
        basePoint.m_x = 0.9;
        basePoint.m_y = 0.055;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.330;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(-0.9);
        hatchPatern4.AddDashe(0.1);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(0);
        basePoint.m_x = 0.9;
        basePoint.m_y = 0.110;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.330;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(-0.9);
        hatchPatern5.AddDashe(0.1);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(0);
        basePoint.m_x = 0.9;
        basePoint.m_y = 0.165;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.330;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(-0.9);
        hatchPatern6.AddDashe(0.1);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(0);
        basePoint.m_x = 0.9;
        basePoint.m_y = 0.220;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.330;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(-0.9);
        hatchPatern7.AddDashe(0.1);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(0);
        basePoint.m_x = 0.9;
        basePoint.m_y = 0.275;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.330;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(-0.9);
        hatchPatern8.AddDashe(0.1);
        m_patternLines.push_back(hatchPatern8);

        return;
    }

    if (m_sPatternName == "CLAY")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.1875;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.03125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.1875;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.0625;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.1875;
        hatchPatern3.SetOffset(offset);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.125;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.1875;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.1875);
        hatchPatern4.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern4);

        return;
    }

    if (m_sPatternName == "CORK")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.125;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(135);
        basePoint.m_x = 0.0625;
        basePoint.m_y = -0.0625;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.25;
        offset.m_y = -0.25;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.176776696);
        hatchPatern2.AddDashe(-0.176776696);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(135);
        basePoint.m_x = 0.09375;
        basePoint.m_y = -0.0625;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.25;
        offset.m_y = -0.25;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.176776696);
        hatchPatern3.AddDashe(-0.176776696);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(135);
        basePoint.m_x = 0.125;
        basePoint.m_y = -0.0625;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -0.25;
        offset.m_y = -0.25;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.176776696);
        hatchPatern4.AddDashe(-0.176776696);
        m_patternLines.push_back(hatchPatern4);

        return;    
    }

    if (m_sPatternName == "CROSS")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.250;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.375);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0.0625;
        basePoint.m_y = -0.0625;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.250;
        offset.m_y = 0.250;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.375);
        m_patternLines.push_back(hatchPatern2);

        return;
    }
    if (m_sPatternName == "DASH")
    {
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.125;
        offset.m_y = 0.125;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern1);

        return;
    }

    if (m_sPatternName == "DOLMIT")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.5;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.35355339);
        hatchPatern2.AddDashe(-0.70710768);
        m_patternLines.push_back(hatchPatern2);

        return;
    }
    if (m_sPatternName == "DOTS")
    {
        CDXFHatchPattern hatchPatern1;

        m_eStyle       = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.0234375;
        offset.m_y = 0.046875;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0);
        hatchPatern1.AddDashe(-0.046875);
        m_patternLines.push_back(hatchPatern1);

        return;

    }

    if (m_sPatternName == "EARTH")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.250;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.250);
        hatchPatern1.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.09375;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.250;
        offset.m_y = 0.250;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.250);
        hatchPatern2.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.1875;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0.250;
        offset.m_y = 0.250;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.250);
        hatchPatern3.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(90);
        basePoint.m_x = 0.03125;
        basePoint.m_y = 0.21875;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -0.250;
        offset.m_y = 0.250;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.250);
        hatchPatern4.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(90);
        basePoint.m_x = 0.125;
        basePoint.m_y = 0.21875;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = -0.250;
        offset.m_y = 0.250;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.250);
        hatchPatern5.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(90);
        basePoint.m_x = 0.21875;
        basePoint.m_y = 0.21875;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -0.250;
        offset.m_y = 0.250;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.250);
        hatchPatern6.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern6);

        return;
    }

    if (m_sPatternName == "ESCHER")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;
        CDXFHatchPattern hatchPatern9;
        CDXFHatchPattern hatchPatern10;
        CDXFHatchPattern hatchPatern11;
        CDXFHatchPattern hatchPatern12;
        CDXFHatchPattern hatchPatern13;
        CDXFHatchPattern hatchPatern14;
        CDXFHatchPattern hatchPatern15;
        CDXFHatchPattern hatchPatern16;
        CDXFHatchPattern hatchPatern17;
        CDXFHatchPattern hatchPatern18;
        CDXFHatchPattern hatchPatern19;
        CDXFHatchPattern hatchPatern20;
        CDXFHatchPattern hatchPatern21;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(60);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = -0.0;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(1.1);
        hatchPatern1.AddDashe(-0.1);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(180);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(1.1);
        hatchPatern2.AddDashe(-0.1);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(300);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 1.2;
        offset.m_y = -0.0;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(1.1);
        hatchPatern3.AddDashe(-0.1);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(60);
        basePoint.m_x = 0.1;
        basePoint.m_y = 0;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = -0.0;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.2);
        hatchPatern4.AddDashe(-1);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(300);
        basePoint.m_x = 0.1;
        basePoint.m_y = 0;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = 1.2;
        offset.m_y = -0.0;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.2);
        hatchPatern5.AddDashe(-1);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(60);
        basePoint.m_x = -0.050;
        basePoint.m_y = 0.08660254;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = -0.0;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.2);
        hatchPatern6.AddDashe(-1);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(180);
        basePoint.m_x = -0.050;
        basePoint.m_y = 0.08660254;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(0.2);
        hatchPatern7.AddDashe(-1);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(300);
        basePoint.m_x = -0.050;
        basePoint.m_y = -0.08660254;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = 1.2;
        offset.m_y = -0.0;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(0.2);
        hatchPatern8.AddDashe(-1);
        m_patternLines.push_back(hatchPatern8);

        hatchPatern9.SetAngle(180);
        basePoint.m_x = -0.050;
        basePoint.m_y = -0.08660254;
        hatchPatern9.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484;
        hatchPatern9.SetOffset(offset);
        hatchPatern9.AddDashe(0.2);
        hatchPatern9.AddDashe(-1);
        m_patternLines.push_back(hatchPatern9);

        hatchPatern10.SetAngle(60);
        basePoint.m_x = -0.4;
        basePoint.m_y = 0;
        hatchPatern10.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = -0.0;
        hatchPatern10.SetOffset(offset);
        hatchPatern10.AddDashe(0.2);
        hatchPatern10.AddDashe(-1);
        m_patternLines.push_back(hatchPatern10);

        hatchPatern11.SetAngle(300);
        basePoint.m_x = -0.4;
        basePoint.m_y = 0;
        hatchPatern11.SetBasePoint(basePoint);
        offset.m_x = 1.2;
        offset.m_y = -0.0;
        hatchPatern11.SetOffset(offset);
        hatchPatern11.AddDashe(0.2);
        hatchPatern11.AddDashe(-1);
        m_patternLines.push_back(hatchPatern11);

        hatchPatern12.SetAngle(60);
        basePoint.m_x = 0.2;
        basePoint.m_y = -0.346410161;
        hatchPatern12.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = -0.0;
        hatchPatern12.SetOffset(offset);
        hatchPatern12.AddDashe(0.2);
        hatchPatern12.AddDashe(-1);
        m_patternLines.push_back(hatchPatern12);

        hatchPatern13.SetAngle(180);
        basePoint.m_x = 0.2;
        basePoint.m_y = -0.346410161;
        hatchPatern13.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484;
        hatchPatern13.SetOffset(offset);
        hatchPatern13.AddDashe(0.2);
        hatchPatern13.AddDashe(-1);
        m_patternLines.push_back(hatchPatern13);

        hatchPatern14.SetAngle(300);
        basePoint.m_x = 0.2;
        basePoint.m_y = 0.346410161;
        hatchPatern14.SetBasePoint(basePoint);
        offset.m_x = 1.2;
        offset.m_y = -0.0;
        hatchPatern14.SetOffset(offset);
        hatchPatern14.AddDashe(0.2);
        hatchPatern14.AddDashe(-1);
        m_patternLines.push_back(hatchPatern14);

        hatchPatern15.SetAngle(180);
        basePoint.m_x = 0.2;
        basePoint.m_y = 0.346410161;
        hatchPatern15.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484;
        hatchPatern15.SetOffset(offset);
        hatchPatern15.AddDashe(0.2);
        hatchPatern15.AddDashe(-1);
        m_patternLines.push_back(hatchPatern15);

        hatchPatern16.SetAngle(0);
        basePoint.m_x = 0.2;
        basePoint.m_y = 0.173205081;
        hatchPatern16.SetBasePoint(basePoint);
        offset.m_x = -0.6;
        offset.m_y = 1.039230484;
        hatchPatern16.SetOffset(offset);
        hatchPatern16.AddDashe(0.7);
        hatchPatern16.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern16);

        hatchPatern17.SetAngle(0);
        basePoint.m_x = 0.2;
        basePoint.m_y = -0.173205081;
        hatchPatern17.SetBasePoint(basePoint);
        offset.m_x = -0.6;
        offset.m_y = 1.039230484;
        hatchPatern17.SetOffset(offset);
        hatchPatern17.AddDashe(0.7);
        hatchPatern17.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern17);

        hatchPatern18.SetAngle(120);
        basePoint.m_x = 0.050;
        basePoint.m_y = 0.259807621;
        hatchPatern18.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = 0.0;
        hatchPatern18.SetOffset(offset);
        hatchPatern18.AddDashe(0.7);
        hatchPatern18.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern18);

        hatchPatern19.SetAngle(120);
        basePoint.m_x = -0.250;
        basePoint.m_y = 0.08660254;
        hatchPatern19.SetBasePoint(basePoint);
        offset.m_x = -1.2;
        offset.m_y = 0.0;
        hatchPatern19.SetOffset(offset);
        hatchPatern19.AddDashe(0.7);
        hatchPatern19.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern19);

        hatchPatern20.SetAngle(240);
        basePoint.m_x = -0.250;
        basePoint.m_y = -0.08660254;
        hatchPatern20.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484270663;
        hatchPatern20.SetOffset(offset);
        hatchPatern20.AddDashe(0.7);
        hatchPatern20.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern20);

        hatchPatern21.SetAngle(240);
        basePoint.m_x = 0.050;
        basePoint.m_y = -0.259807621;
        hatchPatern21.SetBasePoint(basePoint);
        offset.m_x = 0.6;
        offset.m_y = -1.039230484270663;
        hatchPatern21.SetOffset(offset);
        hatchPatern21.AddDashe(0.7);
        hatchPatern21.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern21);

        return;
    }

    if (m_sPatternName == "FLEX")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.250);
        hatchPatern1.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45);
        basePoint.m_x = 0.250;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.25;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.0625);
        hatchPatern2.AddDashe(-0.228553391);
        hatchPatern2.AddDashe(0.0625);
        hatchPatern2.AddDashe(-0.353553391);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "GRASS")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -0.707106781;
        offset.m_y = 0.707106781;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.1875);
        hatchPatern1.AddDashe(-1.226713563);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.707106781186547;
        offset.m_y = 0.707106781186548;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.1875);
        hatchPatern2.AddDashe(-0.8125);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(135);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.707106781186548;
        offset.m_y = -0.707106781186547;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.1875);
        hatchPatern3.AddDashe(-0.8125);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "GRATE")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.03125;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.125;
        offset.m_y = 0;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        return;
    }
    if (m_sPatternName == "GRAVEL")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;
        CDXFHatchPattern hatchPatern7;
        CDXFHatchPattern hatchPatern8;
        CDXFHatchPattern hatchPatern9;
        CDXFHatchPattern hatchPatern10;
        CDXFHatchPattern hatchPatern11;
        CDXFHatchPattern hatchPatern12;
        CDXFHatchPattern hatchPatern13;
        CDXFHatchPattern hatchPatern14;
        CDXFHatchPattern hatchPatern15;
        CDXFHatchPattern hatchPatern16;
        CDXFHatchPattern hatchPatern17;
        CDXFHatchPattern hatchPatern18;
        CDXFHatchPattern hatchPatern19;
        CDXFHatchPattern hatchPatern20;
        CDXFHatchPattern hatchPatern21;
        CDXFHatchPattern hatchPatern22;
        CDXFHatchPattern hatchPatern23;
        CDXFHatchPattern hatchPatern24;
        CDXFHatchPattern hatchPatern25;
        CDXFHatchPattern hatchPatern26;
        CDXFHatchPattern hatchPatern27;
        CDXFHatchPattern hatchPatern28;
        CDXFHatchPattern hatchPatern29;
        CDXFHatchPattern hatchPatern30;
        CDXFHatchPattern hatchPatern31;
        CDXFHatchPattern hatchPatern32;
        CDXFHatchPattern hatchPatern33;
        CDXFHatchPattern hatchPatern34;
        CDXFHatchPattern hatchPatern35;
        CDXFHatchPattern hatchPatern36;
        CDXFHatchPattern hatchPatern37;
        CDXFHatchPattern hatchPatern38;
        CDXFHatchPattern hatchPatern39;
        CDXFHatchPattern hatchPatern40;
        CDXFHatchPattern hatchPatern41;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(228.0128);
        basePoint.m_x = 0.720;
        basePoint.m_y = 1;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -8.0;
        offset.m_y = -9.0;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.134536);
        hatchPatern1.AddDashe(-13.319088);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(184.9697);
        basePoint.m_x = 0.630;
        basePoint.m_y = 0.9;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 12.0;
        offset.m_y = 1.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.230868);
        hatchPatern2.AddDashe(-22.855925);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(132.5104);
        basePoint.m_x = 0.4;
        basePoint.m_y = 0.880;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 10.0;
        offset.m_y = -11.0;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.162788);
        hatchPatern3.AddDashe(-16.116032);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(267.2737);
        basePoint.m_x = 0.010;
        basePoint.m_y = 0.630;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 1.0;
        offset.m_y = 20.0;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.210238);
        hatchPatern4.AddDashe(-20.813558);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(292.8337);
        basePoint.m_x = 0;
        basePoint.m_y = 0.420;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = -5.0;
        offset.m_y = 12.0;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.206155);
        hatchPatern5.AddDashe(-20.409373);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(357.2737);
        basePoint.m_x = 0.080;
        basePoint.m_y = 0.230;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -20.0;
        offset.m_y = 1.0;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.210238);
        hatchPatern6.AddDashe(-20.813558);
        m_patternLines.push_back(hatchPatern6);

        hatchPatern7.SetAngle(37.6942);
        basePoint.m_x = 0.290;
        basePoint.m_y = 0.220;
        hatchPatern7.SetBasePoint(basePoint);
        offset.m_x = -13.0;
        offset.m_y = -10.0;
        hatchPatern7.SetOffset(offset);
        hatchPatern7.AddDashe(0.278029);
        hatchPatern7.AddDashe(-27.524849);
        m_patternLines.push_back(hatchPatern7);

        hatchPatern8.SetAngle(72.2553);
        basePoint.m_x = 0.510;
        basePoint.m_y = 0.390;
        hatchPatern8.SetBasePoint(basePoint);
        offset.m_x = 7.0;
        offset.m_y = 22.0;
        hatchPatern8.SetOffset(offset);
        hatchPatern8.AddDashe(0.262488);
        hatchPatern8.AddDashe(-25.986321);
        m_patternLines.push_back(hatchPatern8);

        hatchPatern9.SetAngle(121.4296);
        basePoint.m_x = 0.590;
        basePoint.m_y = 0.640;
        hatchPatern9.SetBasePoint(basePoint);
        offset.m_x = -8.0;
        offset.m_y = 13.0;
        hatchPatern9.SetOffset(offset);
        hatchPatern9.AddDashe(0.21095);
        hatchPatern9.AddDashe(-20.884073);
        m_patternLines.push_back(hatchPatern9);

        hatchPatern10.SetAngle(175.2364);
        basePoint.m_x = 0.480;
        basePoint.m_y = 0.820;
        hatchPatern10.SetBasePoint(basePoint);
        offset.m_x = 11.0;
        offset.m_y = -1.0;
        hatchPatern10.SetOffset(offset);
        hatchPatern10.AddDashe(0.240832);
        hatchPatern10.AddDashe(-11.800763);
        m_patternLines.push_back(hatchPatern10);

        hatchPatern11.SetAngle(222.3974);
        basePoint.m_x = 0.240;
        basePoint.m_y = 0.840;
        hatchPatern11.SetBasePoint(basePoint);
        offset.m_x = -12.0;
        offset.m_y = -11.0;
        hatchPatern11.SetOffset(offset);
        hatchPatern11.AddDashe(0.311448);
        hatchPatern11.AddDashe(-30.833375);
        m_patternLines.push_back(hatchPatern11);

        hatchPatern12.SetAngle(138.8141);
        basePoint.m_x = 1;
        basePoint.m_y = 0.620;
        hatchPatern12.SetBasePoint(basePoint);
        offset.m_x = -7.0;
        offset.m_y = 6.0;
        hatchPatern12.SetOffset(offset);
        hatchPatern12.AddDashe(0.106301);
        hatchPatern12.AddDashe(-10.523844);
        m_patternLines.push_back(hatchPatern12);

        hatchPatern13.SetAngle(171.4692);
        basePoint.m_x = 0.920;
        basePoint.m_y = 0.690;
        hatchPatern13.SetBasePoint(basePoint);
        offset.m_x = 13.0;
        offset.m_y = -2.0;
        hatchPatern13.SetOffset(offset);
        hatchPatern13.AddDashe(0.202237);
        hatchPatern13.AddDashe(-20.021511);
        m_patternLines.push_back(hatchPatern13);

        hatchPatern14.SetAngle(225);
        basePoint.m_x = 0.720;
        basePoint.m_y = 0.720;
        hatchPatern14.SetBasePoint(basePoint);
        offset.m_x = -0;
        offset.m_y = -1.0;
        hatchPatern14.SetOffset(offset);
        hatchPatern14.AddDashe(0.141421);
        hatchPatern14.AddDashe(-1.272792);
        m_patternLines.push_back(hatchPatern14);

        hatchPatern15.SetAngle(203.1986);
        basePoint.m_x = 0.650;
        basePoint.m_y = 0.840;
        hatchPatern15.SetBasePoint(basePoint);
        offset.m_x = 5.0;
        offset.m_y = 2.0;
        hatchPatern15.SetOffset(offset);
        hatchPatern15.AddDashe(0.076158);
        hatchPatern15.AddDashe(-7.539615);
        m_patternLines.push_back(hatchPatern15);

        hatchPatern16.SetAngle(291.8014);
        basePoint.m_x = 0.580;
        basePoint.m_y = 0.810;
        hatchPatern16.SetBasePoint(basePoint);
        offset.m_x = -1.0;
        offset.m_y = 3.0;
        hatchPatern16.SetOffset(offset);
        hatchPatern16.AddDashe(0.107703);
        hatchPatern16.AddDashe(-5.277462);
        m_patternLines.push_back(hatchPatern16);

        hatchPatern17.SetAngle(30.9638);
        basePoint.m_x = 0.620;
        basePoint.m_y = 0.710;
        hatchPatern17.SetBasePoint(basePoint);
        offset.m_x = 3.0;
        offset.m_y = 2.0;
        hatchPatern17.SetOffset(offset);
        hatchPatern17.AddDashe(0.174929);
        hatchPatern17.AddDashe(-5.656023);
        m_patternLines.push_back(hatchPatern17);

        hatchPatern18.SetAngle(161.5651);
        basePoint.m_x = 0.770;
        basePoint.m_y = 0.8;
        hatchPatern18.SetBasePoint(basePoint);
        offset.m_x = 2.0;
        offset.m_y = -1.0;
        hatchPatern18.SetOffset(offset);
        hatchPatern18.AddDashe(0.126491);
        hatchPatern18.AddDashe(-3.035787);
        m_patternLines.push_back(hatchPatern18);

        hatchPatern19.SetAngle(16.3895);
        basePoint.m_x = 0;
        basePoint.m_y = 0.810;
        hatchPatern19.SetBasePoint(basePoint);
        offset.m_x = 10.0;
        offset.m_y = 3.0;
        hatchPatern19.SetOffset(offset);
        hatchPatern19.AddDashe(0.1772);
        hatchPatern19.AddDashe(-17.542845);
        m_patternLines.push_back(hatchPatern19);

        hatchPatern20.SetAngle(70.3465);
        basePoint.m_x = 0.170;
        basePoint.m_y = 0.860;
        hatchPatern20.SetBasePoint(basePoint);
        offset.m_x = -4.0;
        offset.m_y = -11.0;
        hatchPatern20.SetOffset(offset);
        hatchPatern20.AddDashe(0.148661);
        hatchPatern20.AddDashe(-14.717408);
        m_patternLines.push_back(hatchPatern20);

        hatchPatern21.SetAngle(293.1986);
        basePoint.m_x = 0.770;
        basePoint.m_y = 1;
        hatchPatern21.SetBasePoint(basePoint);
        offset.m_x = -2.0;
        offset.m_y = 5.0;
        hatchPatern21.SetOffset(offset);
        hatchPatern21.AddDashe(0.152315);
        hatchPatern21.AddDashe(-7.463458);
        m_patternLines.push_back(hatchPatern21);

        hatchPatern22.SetAngle(343.6105);
        basePoint.m_x = 0.830;
        basePoint.m_y = 0.860;
        hatchPatern22.SetBasePoint(basePoint);
        offset.m_x = -10.0;
        offset.m_y = 3.0;
        hatchPatern22.SetOffset(offset);
        hatchPatern22.AddDashe(0.1772);
        hatchPatern22.AddDashe(-17.542844);
        m_patternLines.push_back(hatchPatern22);

        hatchPatern23.SetAngle(339.444);
        basePoint.m_x = 0;
        basePoint.m_y = 0.190;
        hatchPatern23.SetBasePoint(basePoint);
        offset.m_x = -5.0;
        offset.m_y = 2.0;
        hatchPatern23.SetOffset(offset);
        hatchPatern23.AddDashe(0.17088);
        hatchPatern23.AddDashe(-8.373124);
        m_patternLines.push_back(hatchPatern23);

        hatchPatern24.SetAngle(294.7751);
        basePoint.m_x = 0.160;
        basePoint.m_y = 0.130;
        hatchPatern24.SetBasePoint(basePoint);
        offset.m_x = -5.0;
        offset.m_y = 11.0;
        hatchPatern24.SetOffset(offset);
        hatchPatern24.AddDashe(0.143178);
        hatchPatern24.AddDashe(-14.174643);
        m_patternLines.push_back(hatchPatern24);

        hatchPatern25.SetAngle(66.8014);
        basePoint.m_x = 0.780;
        basePoint.m_y = 0;
        hatchPatern25.SetBasePoint(basePoint);
        offset.m_x = 2.0;
        offset.m_y = 5.0;
        hatchPatern25.SetOffset(offset);
        hatchPatern25.AddDashe(0.152315);
        hatchPatern25.AddDashe(-7.463458);
        m_patternLines.push_back(hatchPatern25);

        hatchPatern26.SetAngle(17.354);
        basePoint.m_x = 0.840;
        basePoint.m_y = 0.140;
        hatchPatern26.SetBasePoint(basePoint);
        offset.m_x = -13.0;
        offset.m_y = -4.0;
        hatchPatern26.SetOffset(offset);
        hatchPatern26.AddDashe(0.167631);
        hatchPatern26.AddDashe(-16.595424);
        m_patternLines.push_back(hatchPatern26);

        hatchPatern27.SetAngle(69.444);
        basePoint.m_x = 0.290;
        basePoint.m_y = 0;
        hatchPatern27.SetBasePoint(basePoint);
        offset.m_x = -2.0;
        offset.m_y = -5.0;
        hatchPatern27.SetOffset(offset);
        hatchPatern27.AddDashe(0.08544);
        hatchPatern27.AddDashe(-8.458564);
        m_patternLines.push_back(hatchPatern27);

        hatchPatern28.SetAngle(101.3099);
        basePoint.m_x = 0.720;
        basePoint.m_y = 0;
        hatchPatern28.SetBasePoint(basePoint);
        offset.m_x = -1.0;
        offset.m_y = 4.0;
        hatchPatern28.SetOffset(offset);
        hatchPatern28.AddDashe(0.05099);
        hatchPatern28.AddDashe(-5.048029);
        m_patternLines.push_back(hatchPatern28);

        hatchPatern29.SetAngle(165.9638);
        basePoint.m_x = 0.710;
        basePoint.m_y = 0.050;
        hatchPatern29.SetBasePoint(basePoint);
        offset.m_x = 3.0;
        offset.m_y = -1.0;
        hatchPatern29.SetOffset(offset);
        hatchPatern29.AddDashe(0.206155);
        hatchPatern29.AddDashe(-3.91695);
        m_patternLines.push_back(hatchPatern29);

        hatchPatern30.SetAngle(186.009);
        basePoint.m_x = 0.510;
        basePoint.m_y = 0.1;
        hatchPatern30.SetBasePoint(basePoint);
        offset.m_x = 10.0;
        offset.m_y = 1.0;
        hatchPatern30.SetOffset(offset);
        hatchPatern30.AddDashe(0.19105);
        hatchPatern30.AddDashe(-18.913923);
        m_patternLines.push_back(hatchPatern30);

        hatchPatern31.SetAngle(303.6901);
        basePoint.m_x = 0.620;
        basePoint.m_y = 0.620;
        hatchPatern31.SetBasePoint(basePoint);
        offset.m_x = -1.0;
        offset.m_y = 2.0;
        hatchPatern31.SetOffset(offset);
        hatchPatern31.AddDashe(0.144222);
        hatchPatern31.AddDashe(-3.461329);
        m_patternLines.push_back(hatchPatern31);

        hatchPatern32.SetAngle(353.1572);
        basePoint.m_x = 0.7;
        basePoint.m_y = 0.5;
        hatchPatern32.SetBasePoint(basePoint);
        offset.m_x = 17.0;
        offset.m_y = -2.0;
        hatchPatern32.SetOffset(offset);
        hatchPatern32.AddDashe(0.251794);
        hatchPatern32.AddDashe(-24.927563);
        m_patternLines.push_back(hatchPatern32);

        hatchPatern33.SetAngle(60.9454);
        basePoint.m_x = 0.950;
        basePoint.m_y = 0.470;
        hatchPatern33.SetBasePoint(basePoint);
        offset.m_x = -4.0;
        offset.m_y = -7.0;
        hatchPatern33.SetOffset(offset);
        hatchPatern33.AddDashe(0.102956);
        hatchPatern33.AddDashe(-10.192674);
        m_patternLines.push_back(hatchPatern33);

        hatchPatern34.SetAngle(90);
        basePoint.m_x = 1;
        basePoint.m_y = 0.560;
        hatchPatern34.SetBasePoint(basePoint);
        offset.m_x = -1;
        offset.m_y = 1;
        hatchPatern34.SetOffset(offset);
        hatchPatern34.AddDashe(0.060);
        hatchPatern34.AddDashe(-0.940);
        m_patternLines.push_back(hatchPatern34);

        hatchPatern35.SetAngle(120.2564);
        basePoint.m_x = 0.490;
        basePoint.m_y = 0.130;
        hatchPatern35.SetBasePoint(basePoint);
        offset.m_x = 4.0;
        offset.m_y = -7.0;
        hatchPatern35.SetOffset(offset);
        hatchPatern35.AddDashe(0.138924);
        hatchPatern35.AddDashe(-13.75352);
        m_patternLines.push_back(hatchPatern35);

        hatchPatern36.SetAngle(48.0128);
        basePoint.m_x = 0.420;
        basePoint.m_y = 0.250;
        hatchPatern36.SetBasePoint(basePoint);
        offset.m_x = 8.0;
        offset.m_y = 9.0;
        hatchPatern36.SetOffset(offset);
        hatchPatern36.AddDashe(0.269072);
        hatchPatern36.AddDashe(-13.184552);
        m_patternLines.push_back(hatchPatern36);

        hatchPatern37.SetAngle(0);
        basePoint.m_x = 0.6;
        basePoint.m_y = 0.450;
        hatchPatern37.SetBasePoint(basePoint);
        offset.m_x = 1;
        offset.m_y = 1;
        hatchPatern37.SetOffset(offset);
        hatchPatern37.AddDashe(0.260);
        hatchPatern37.AddDashe(-0.740);
        m_patternLines.push_back(hatchPatern37);

        hatchPatern38.SetAngle(325.3048);
        basePoint.m_x = 0.860;
        basePoint.m_y = 0.450;
        hatchPatern38.SetBasePoint(basePoint);
        offset.m_x = -10.0;
        offset.m_y = 7.0;
        hatchPatern38.SetOffset(offset);
        hatchPatern38.AddDashe(0.158114);
        hatchPatern38.AddDashe(-15.653274);
        m_patternLines.push_back(hatchPatern38);

        hatchPatern39.SetAngle(254.0546);
        basePoint.m_x = 0.990;
        basePoint.m_y = 0.360;
        hatchPatern39.SetBasePoint(basePoint);
        offset.m_x = -1.0;
        offset.m_y = -4.0;
        hatchPatern39.SetOffset(offset);
        hatchPatern39.AddDashe(0.145602);
        hatchPatern39.AddDashe(-7.134508);
        m_patternLines.push_back(hatchPatern39);

        hatchPatern40.SetAngle(207.646);
        basePoint.m_x = 0.950;
        basePoint.m_y = 0.220;
        hatchPatern40.SetBasePoint(basePoint);
        offset.m_x = -19.0;
        offset.m_y = -10.0;
        hatchPatern40.SetOffset(offset);
        hatchPatern40.AddDashe(0.237065);
        hatchPatern40.AddDashe(-23.469474);
        m_patternLines.push_back(hatchPatern40);

        hatchPatern41.SetAngle(175.4261);
        basePoint.m_x = 0.740;
        basePoint.m_y = 0.110;
        hatchPatern41.SetBasePoint(basePoint);
        offset.m_x = -13.0;
        offset.m_y = 1.0;
        hatchPatern41.SetOffset(offset);
        hatchPatern41.AddDashe(0.250799);
        hatchPatern41.AddDashe(-24.829074);
        m_patternLines.push_back(hatchPatern41);

        return;
    }

    if (m_sPatternName == "HEX")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.216506351;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(120);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.187500000046670;
        offset.m_y = -0.1082531755;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(60);
        basePoint.m_x = 0.125;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.187500000046670;
        offset.m_y = 0.1082531755;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.125);
        hatchPatern3.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "HONEY")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.1875;
        offset.m_y = 0.108253175;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(120);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.1875;
        offset.m_y = 0.108253175709582;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(60);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0.0;
        offset.m_y = 0.216506350709582;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(-0.250);
        hatchPatern3.AddDashe(0.125);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "HOUND")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.250;
        offset.m_y = 0.0625;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(1);
        hatchPatern1.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.0625;
        offset.m_y = -0.250;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(1);
        hatchPatern2.AddDashe(-0.5);
        m_patternLines.push_back(hatchPatern2);

        CDXF3DPoint seedPoint1;

        seedPoint1.x = 0;
        seedPoint1.y = 0;
        m_seedPoints.push_back(seedPoint1);

        return;
    } 

    if (m_sPatternName == "INSUL")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.375;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.375;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.250;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.375;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.125);
        hatchPatern3.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern3);

        CDXF3DPoint seedPoint1;

        seedPoint1.x = 0;
        seedPoint1.y = 0;
        m_seedPoints.push_back(seedPoint1);

        return;
    }

    if (m_sPatternName =="MUDST")
    {
        CDXFHatchPattern hatchPatern1;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.250);
        hatchPatern1.AddDashe(-0.250);
        hatchPatern1.AddDashe(0);
        hatchPatern1.AddDashe(-0.250);
        hatchPatern1.AddDashe(0);
        hatchPatern1.AddDashe(-0.250);
        m_patternLines.push_back(hatchPatern1);

        return;
    }

    if (m_sPatternName =="NET")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.125;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.125;
        offset.m_y = 0;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);
        return;
    }

    if (m_sPatternName =="NET3")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.125;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(60);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.108253175473055;
        offset.m_y = 0.0625;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(120);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.108253175473055;
        offset.m_y = -0.0625;
        hatchPatern3.SetOffset(offset);
        m_patternLines.push_back(hatchPatern3);
        return;
    }

    if (m_sPatternName == "PLAST")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.03125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.0625;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern3.SetOffset(offset);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName =="PLASTI")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.03125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.0625;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern3.SetOffset(offset);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.15625;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern4.SetOffset(offset);
        m_patternLines.push_back(hatchPatern4);

        return;
    }

    if (m_sPatternName =="SACNCR")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(45);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -0.066291260736239;
        offset.m_y = 0.066291260736239;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45);
        basePoint.m_x = 0.066291261;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.066291260736239;
        offset.m_y = 0.066291260736239;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0);
        hatchPatern2.AddDashe(-0.09375);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "SQUARE")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.125;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.125;
        offset.m_y = 0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "STARS")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.216506351;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(60);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.187500000046670;
        offset.m_y = 0.1082531755;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(120);
        basePoint.m_x = 0.0625;
        basePoint.m_y = 0.108253176;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.187500000046670;
        offset.m_y = -0.1082531755;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.125);
        hatchPatern3.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "STEEL")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(45);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -0.088388347648318;
        offset.m_y = 0.088388347648319;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(45);
        basePoint.m_x = 0;
        basePoint.m_y = 0.0625;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.088388347648318;
        offset.m_y = 0.088388347648319;
        hatchPatern2.SetOffset(offset);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "SWAMP")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;
        CDXFHatchPattern hatchPatern4;
        CDXFHatchPattern hatchPatern5;
        CDXFHatchPattern hatchPatern6;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.5;
        offset.m_y = 0.866025403;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.875);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0.0625;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.866025403;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.0625);
        hatchPatern2.AddDashe(-1.669550806);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(90);
        basePoint.m_x = 0.078125;
        basePoint.m_y = 0;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.866025403;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.050);
        hatchPatern3.AddDashe(-1.682050806);
        m_patternLines.push_back(hatchPatern3);

        hatchPatern4.SetAngle(90);
        basePoint.m_x = 0.046875;
        basePoint.m_y = 0;
        hatchPatern4.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.866025403;
        hatchPatern4.SetOffset(offset);
        hatchPatern4.AddDashe(0.050);
        hatchPatern4.AddDashe(-1.682050806);
        m_patternLines.push_back(hatchPatern4);

        hatchPatern5.SetAngle(60);
        basePoint.m_x = 0.09375;
        basePoint.m_y = 0;
        hatchPatern5.SetBasePoint(basePoint);
        offset.m_x = -0.5;
        offset.m_y = 0.866025403392219;
        hatchPatern5.SetOffset(offset);
        hatchPatern5.AddDashe(0.040);
        hatchPatern5.AddDashe(-0.960);
        m_patternLines.push_back(hatchPatern5);

        hatchPatern6.SetAngle(120);
        basePoint.m_x = 0.03125;
        basePoint.m_y = 0;
        hatchPatern6.SetBasePoint(basePoint);
        offset.m_x = -1.0;
        offset.m_y = 0.0;
        hatchPatern6.SetOffset(offset);
        hatchPatern6.AddDashe(0.040);
        hatchPatern6.AddDashe(-0.960);
        m_patternLines.push_back(hatchPatern6);

        return;
    }

    if (m_sPatternName == "TRANS")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern1.SetOffset(offset);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0.125;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = 0;
        offset.m_y = 0.250;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    if (m_sPatternName == "TRIANG")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;
        CDXFHatchPattern hatchPatern3;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(60);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = -0.1875;
        offset.m_y = 0.324759526209582;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.1875);
        hatchPatern1.AddDashe(-0.1875);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(120);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.375;
        offset.m_y = 0.0;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.1875);
        hatchPatern2.AddDashe(-0.1875);
        m_patternLines.push_back(hatchPatern2);

        hatchPatern3.SetAngle(0);
        basePoint.m_x = -0.09375;
        basePoint.m_y = 0.162379763;
        hatchPatern3.SetBasePoint(basePoint);
        offset.m_x = 0.1875;
        offset.m_y = 0.324759526;
        hatchPatern3.SetOffset(offset);
        hatchPatern3.AddDashe(0.1875);
        hatchPatern3.AddDashe(-0.1875);
        m_patternLines.push_back(hatchPatern3);

        return;
    }

    if (m_sPatternName == "ZIGZAG")
    {
        CDXFHatchPattern hatchPatern1;
        CDXFHatchPattern hatchPatern2;

        m_eStyle = eNormalStyle;
        m_ePatternType = ePatternPredefined;
        CDXFGePoint2D basePoint;
        CDXFGePoint2D offset;

        hatchPatern1.SetAngle(0);
        basePoint.m_x = 0;
        basePoint.m_y = 0;
        hatchPatern1.SetBasePoint(basePoint);
        offset.m_x = 0.125;
        offset.m_y = 0.125;
        hatchPatern1.SetOffset(offset);
        hatchPatern1.AddDashe(0.125);
        hatchPatern1.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern1);

        hatchPatern2.SetAngle(90);
        basePoint.m_x = 0.125;
        basePoint.m_y = 0;
        hatchPatern2.SetBasePoint(basePoint);
        offset.m_x = -0.125;
        offset.m_y = 0.125;
        hatchPatern2.SetOffset(offset);
        hatchPatern2.AddDashe(0.125);
        hatchPatern2.AddDashe(-0.125);
        m_patternLines.push_back(hatchPatern2);

        return;
    }

    m_sPatternName = "LINE";
    CDXFHatchPattern hatchPatern1;
    m_eStyle = eNormalStyle;
    m_ePatternType = ePatternPredefined;
    hatchPatern1.SetAngle(0.0);
    CDXFGePoint2D basePoint(0.0, 0.0);
    hatchPatern1.SetBasePoint(basePoint);
    double x = -::sin(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
    double y =  ::cos(m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * m_dPatternScale;
    CDXFGePoint2D offset(x, y);
    hatchPatern1.SetOffset(offset);
    m_patternLines.push_back(hatchPatern1);
}

CDXFHatchNonPolylineBoundary::CDXFHatchNonPolylineBoundary(int32_t nType)
{
    m_nType = nType;
    if (m_nType & eBoundaryTypePolyline) {
        m_nType &= ~eBoundaryTypePolyline;
    }

    m_pBand = new CDXFNonPolylineBoundary;
}

CDXFHatchNonPolylineBoundary::CDXFHatchNonPolylineBoundary(const CDXFHatchNonPolylineBoundary &boundary)
{
    m_nType = boundary.m_nType;
    if (m_nType & eBoundaryTypePolyline) {
        m_nType &= ~eBoundaryTypePolyline;
    }

    m_pBand = new CDXFNonPolylineBoundary;
    CDXFAddBoundaryEdgeVis visitor(m_pBand);
    for (auto* pBase : *boundary.m_pBand) {
        ASSERT(pBase);
        pBase->Accept(visitor);
    }
}

void CDXFHatchNonPolylineBoundary::AddLine(const CDXFGePoint2D &startPoint, const CDXFGePoint2D &endPoint)
{
    CDXFBoundaryEdgeLine* pLine = new CDXFBoundaryEdgeLine;
    pLine->m_startPoint = startPoint;
    pLine->m_endPoint   = endPoint;
    m_pBand->push_back(pLine);
}
void CDXFHatchNonPolylineBoundary::AddArc(const CDXFGePoint2D &centerPoint, double dStartAngle, double dEndAngle,    
                                          double dRadius, int32_t nCounterclockwise)
{
    CDXFBoundaryEdgeArc* pArc = new CDXFBoundaryEdgeArc;
    pArc->m_nCounterclockwise = nCounterclockwise;
    pArc->m_dRadius     = dRadius;
    pArc->m_centerPoint = centerPoint;
    pArc->m_dEndAngle   = dEndAngle;
    pArc->m_dStartAngle = dStartAngle; 
    m_pBand->push_back(pArc);
}

void CDXFHatchNonPolylineBoundary::AddEllipse(const CDXFGePoint2D &centerPoint, const CDXFGePoint2D &endPoint, 
                                              double dMinorAxisLength, double dStartAngle, 
                                              double dEndAngle, int32_t nCounterclockwise)
{
    CDXFBoundaryEdgeEllipse *pEllipse = new CDXFBoundaryEdgeEllipse;
    pEllipse->m_centerPoint       = centerPoint;
    pEllipse->m_endPoint          = endPoint;
    pEllipse->m_dMinorAxisLength  = dMinorAxisLength;
    pEllipse->m_dStartAngle       = dStartAngle;
    pEllipse->m_dEndAngle         = dEndAngle;
    pEllipse->m_nCounterclockwise = nCounterclockwise;
    m_pBand->push_back(pEllipse);
}
void CDXFHatchNonPolylineBoundary::AddSpline(double dDegree, double dRational, double dPeriodic, const std::vector<double> &knotValuesArray,
                                             const std::vector<CDXFSplineControlPoint> &controlPointsArray)
{
    CDXFBoundaryEdgeSpline *pSpline = new CDXFBoundaryEdgeSpline;
    pSpline->m_dDegree   = dDegree;
    pSpline->m_dRational = dRational;
    pSpline->m_dPeriodic = dPeriodic;
    pSpline->m_knotValuesArray    = knotValuesArray;
    pSpline->m_controlPointsArray = controlPointsArray;
    m_pBand->push_back(pSpline);
}

void CDXFHatchNonPolylineBoundary::SetNumEdges(size_t nNumEdges)
{
    ASSERT(nNumEdges >= 0);
    m_pBand->resize(nNumEdges);
    for (size_t i = 0; i < nNumEdges; ++i) {
        (*m_pBand)[i] = nullptr;
    }
}

void CDXFHatchNonPolylineBoundary::SetEdgeDataLine(size_t nEdgeIndex, CDXFGeLineSeg2D *pEdgeData)
{
    ASSERT(nEdgeIndex >= 0 && nEdgeIndex < m_pBand->size());

    CDXFGeLineSeg2D *pEdgeLine  = new CDXFGeLineSeg2D (*pEdgeData);
    CDXFBoundaryEdgeLine *pLine = new CDXFBoundaryEdgeLine();
    pLine->m_startPoint = pEdgeLine->m_start;
    pLine->m_endPoint   = pEdgeLine->m_end;
    delete pEdgeLine;
    (*m_pBand)[nEdgeIndex] = pLine;
}

void CDXFHatchNonPolylineBoundary::SetBand(CDXFNonPolylineBoundary &band)
{
    m_pBand->Clear();
    CDXFAddBoundaryEdgeVis visitor(m_pBand);
    for (auto* pBase : band) {
        ASSERT(pBase);
        pBase->Accept(visitor);
    }
}