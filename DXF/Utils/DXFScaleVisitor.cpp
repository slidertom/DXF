#include "stdafx.h"
#include "DXFScaleVisitor.h"

#include "../entity/DXFHatch.h"
#include "DXFUtils.h"

CDXFScaleVisitor::CDXFScaleVisitor(double dScaleX, double dScaleY)
{
    m_ScaleMatrix.SetIdentity();
    m_ScaleMatrix.m11 = dScaleX;
    m_ScaleMatrix.m22 = dScaleY;
    m_ScaleMatrix.m33 = 1.0;
}

void CDXFScaleVisitor::ScaleDXFGePoint2D(CDXFGePoint2D &dxfGePoint)
{
    CDXF3DPoint point;

    point.x = dxfGePoint.m_x;
    point.y = dxfGePoint.m_y;
    point = MatrixPointMultiply(m_ScaleMatrix, point);
    dxfGePoint.m_x = point.x;
    dxfGePoint.m_y = point.y;
}

namespace internal
{
    void GenerateBoundaryFromLines(CDXFNonPolylineBoundary &newBand, CDXF2DLineVector &lines, CDXFMatrix3D &matrix)
    {
        CDXF3DPoint pointStart;
        CDXF3DPoint pointEnd;

        for (const CDXF2DLineSegment &seg : lines)
        {
            pointStart.x = seg.m_x1;
            pointStart.y = seg.m_y1;
            pointEnd.x   = seg.m_x2;
            pointEnd.y   = seg.m_y2;

            pointStart = MatrixPointMultiply(matrix, pointStart);
            pointEnd   = MatrixPointMultiply(matrix, pointEnd);

            CDXFBoundaryEdgeLine *pLine = new CDXFBoundaryEdgeLine;
           
            pLine->m_startPoint.m_x = pointStart.x;
            pLine->m_startPoint.m_y = pointStart.y;

            pLine->m_endPoint.m_x = pointEnd.x;
            pLine->m_endPoint.m_y = pointEnd.y;

            newBand.push_back(pLine);
        }
    }
};

class CDXFScaleAddBoundaryEdgeVis : public CDXFBoundaryEdgeVis
{
// Construction/Destruction
public:
    CDXFScaleAddBoundaryEdgeVis(CDXFNonPolylineBoundary* pBand, CDXFMatrix3D scaleMatrix) : m_pBand(pBand), m_ScaleMatrix(scaleMatrix) { }
    virtual ~CDXFScaleAddBoundaryEdgeVis() { }

// Overrides
public:
    virtual void OnVisitEdgeArc(CDXFBoundaryEdgeArc &edge) override {
        CDXF2DLineVector lines;
        CArcGenerator::ConvertArcToLines(lines, edge.m_centerPoint.m_x, edge.m_centerPoint.m_y, edge.m_dStartAngle, edge.m_dEndAngle, edge.m_dRadius);
        internal::GenerateBoundaryFromLines(*m_pBand, lines, m_ScaleMatrix);
    }

    virtual void OnVisitEdgeLine(CDXFBoundaryEdgeLine &edge) override {
        CDXFBoundaryEdgeLine *pSeg = new CDXFBoundaryEdgeLine(edge);
        ScaleDXFGePoint2D(pSeg->m_endPoint);
        ScaleDXFGePoint2D(pSeg->m_startPoint);
        m_pBand->push_back(pSeg);
    }

    virtual void OnVisitEdgeEllipse(CDXFBoundaryEdgeEllipse &edge) override {
        CDXFBoundaryEdgeEllipse* pSeg = new CDXFBoundaryEdgeEllipse(edge);
           ScaleDXFGePoint2D(pSeg->m_endPoint); 
        ScaleDXFGePoint2D(pSeg->m_centerPoint); // incorrect, scale can not be performed on the center point
        m_pBand->push_back(pSeg);
    }

    virtual void OnVisitEdgeSpline(CDXFBoundaryEdgeSpline &edge) override {
        CDXFBoundaryEdgeSpline* pSeg = new CDXFBoundaryEdgeSpline(edge);
        pSeg->m_controlPointsArray.clear();

        for (auto &controlPoint : edge.m_controlPointsArray) {
            ScaleDXFGePoint2D(controlPoint.m_point);
            pSeg->m_controlPointsArray.push_back(controlPoint);
        }
        m_pBand->push_back(pSeg);
    }

private:
    void ScaleDXFGePoint2D(CDXFGePoint2D &dxfGePoint)
    {
        CDXF3DPoint point;
        point.x = dxfGePoint.m_x;
        point.y = dxfGePoint.m_y;
        point = MatrixPointMultiply(m_ScaleMatrix, point);
        dxfGePoint.m_x = point.x;
        dxfGePoint.m_y = point.y;
    }

// Attributes
private:
    CDXFNonPolylineBoundary* m_pBand;
    CDXFMatrix3D m_ScaleMatrix;
};

void CDXFScaleVisitor::VisitDXFHatch(CDXFHatch &entity)
{
    {   // update seed points
        CDXF3DPoint *pPoint;
        auto end_it = entity.m_seedPoints.end();
        for (auto it = entity.m_seedPoints.begin(); it != end_it; ++it)
        {
            pPoint = &(*it);
            *pPoint = MatrixPointMultiply(m_ScaleMatrix, *pPoint);
        }
    }

    CDXFHatchBoundary *pBoundary = nullptr;
    int32_t nType = -1;

    auto end_it = entity.m_boundaryPtrs.end();
    for (auto it = entity.m_boundaryPtrs.begin(); it != end_it; ++it) {
        pBoundary = (*it);
        nType = pBoundary->GetType();
        if (nType & eBoundaryTypePolyline)
        {
            CDXFHatchPolylineBoundary *pPopylineBoundary = (CDXFHatchPolylineBoundary *)pBoundary;

            std::vector<CDXFGeBulgePoint2D> vertices;
            pPopylineBoundary->GetVertices(vertices);

            CDXF3DPoint point;

            size_t nVerticesCnt = vertices.size();
            for (size_t i = 0; i < nVerticesCnt; i++)
            {
                point.x = vertices[i].m_point.m_x;
                point.y = vertices[i].m_point.m_y;
                point = MatrixPointMultiply(m_ScaleMatrix, point);    
                
                vertices[i].m_point.m_x = point.x;
                vertices[i].m_point.m_y = point.y;
            }

            pPopylineBoundary->SetVertices(vertices);
        }
        else
        {
            CDXFHatchNonPolylineBoundary *pNonPopylineBoundary = (CDXFHatchNonPolylineBoundary *)pBoundary;
            CDXFNonPolylineBoundary *pBand = pNonPopylineBoundary->GetBand();
            CDXFNonPolylineBoundary newBand;
            CDXFScaleAddBoundaryEdgeVis visitor(&newBand, m_ScaleMatrix);
            
            for (auto *pBase : *pBand) {
                if (!pBase) {
                    ASSERT(false);
                    continue;
                }

                pBase->Accept(visitor);
            }

            pNonPopylineBoundary->SetBand(newBand);
        }
    }
}