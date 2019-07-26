#include "stdafx.h"
#include "DXFEntityBoundingBoxVisitor.h"

#include "entity/DXFCircle.h"
#include "entity/DXFLine.h"
#include "entity/DXFMText.h"
#include "entity/DXFPolyline.h"
#include "entity/DXFSolid.h"
#include "entity/DXFDimension.h"
#include "entity/DXFInsert.h"

void CDXFEntityBoundingBoxVisitor::VisitDXFCircle(CDXFCircle &entity)
{ 
    CDXF3DPoint minPoint;
    CDXF3DPoint maxPoint;

    const CDXF3DPoint point_center = entity.GetCenterPoint();
    const double dRadius           = entity.GetRadius();

    minPoint    = point_center - dRadius;
    minPoint.z  = point_center.z;
    maxPoint    = point_center + dRadius;
    maxPoint.z  = point_center.z;

    m_box.SetMinPoint(minPoint);
    m_box.SetMaxPoint(maxPoint);
}

void CDXFEntityBoundingBoxVisitor::VisitDXFLine(CDXFLine &entity)
{
    CDXF3DPoint minPoint;
    CDXF3DPoint maxPoint;

    if (entity.m_pointStart.x > entity.m_pointEnd.x)
    {
        minPoint.x = entity.m_pointEnd.x;
        maxPoint.x = entity.m_pointStart.x;
    }
    else {
        minPoint.x = entity.m_pointStart.x;
        maxPoint.x = entity.m_pointEnd.x;
    }

    if (entity.m_pointStart.y > entity.m_pointEnd.y)
    {
        minPoint.y = entity.m_pointEnd.y;
        maxPoint.y = entity.m_pointStart.y;
    }
    else {
        minPoint.y = entity.m_pointStart.y;
        maxPoint.y = entity.m_pointEnd.y;
    }

    if (entity.m_pointStart.z > entity.m_pointEnd.z)
    {
        minPoint.z = entity.m_pointEnd.z;
        maxPoint.z = entity.m_pointStart.z;
    }
    else {
        minPoint.z = entity.m_pointStart.z;
        maxPoint.z = entity.m_pointEnd.z;
    }

    m_box.SetMinPoint(minPoint);
    m_box.SetMaxPoint(maxPoint);
}

void CDXFEntityBoundingBoxVisitor::VisitDXFMText(CDXFMText &entity)
{
    CDXF3DPoint minPoint, maxPoint;

    maxPoint.y = maxPoint.y + entity.GetTextHeight();
    maxPoint.x = maxPoint.x + entity.GetRectangleWidth();

    m_box.SetMinPoint(minPoint);
    m_box.SetMaxPoint(maxPoint);
}

void CDXFEntityBoundingBoxVisitor::VisitDXFPolyline(CDXFPolyline &entity)
{
    CDXF3DPoint min, max;
    CDXF3DPoint vertex;

    min.SetAll(1.0E+10);
    max.SetAll(-1.0E+10);

    for (const CDXFGeBulgePoint3D &bulge_pt : entity.m_vertices) 
    {
        vertex = bulge_pt.m_point;

        if (vertex.x < min.x) {
            min.x = vertex.x;
        }

        if (vertex.y < min.y) {
            min.y = vertex.y;
        }

        if (vertex.z < min.z) {
            min.z = vertex.z;
        }

        if (vertex.x > max.x) {
            max.x = vertex.x;
        }

        if (vertex.y > max.y) {
            max.y = vertex.y;
        }

        if (vertex.z > max.z) {
            max.z = vertex.z;
        }
    }

    m_box.SetMinPoint(min);
    m_box.SetMaxPoint(max);
}

void CDXFEntityBoundingBoxVisitor::VisitDXFSolid(CDXFSolid &entity)
{ 
    CDXF3DPoint minPoint;
    CDXF3DPoint maxPoint;

    minPoint.SetAll(1.0E+10);
    maxPoint.SetAll(-1.0E+10);

    for (auto i = 0; i < entity.GetNumPoints(); ++i)
    {
        CDXF3DPoint &point = entity.GetPoint(i);

        if (point.x < minPoint.x)
            minPoint.x = point.x;

        if (point.y < minPoint.y)
            minPoint.y = point.y;

        if (point.z < minPoint.z)
            minPoint.z = point.z;

        if (point.x > maxPoint.x)
            maxPoint.x = point.x;

        if (point.y > maxPoint.y)
            maxPoint.y = point.y;

        if (point.z > maxPoint.z)
            maxPoint.z = point.z;
    }

    m_box.SetMinPoint(minPoint);
    m_box.SetMaxPoint(maxPoint);
}

void CDXFEntityBoundingBoxVisitor::VisitDXFInsert(CDXFInsert &entity)
{ 
    if (entity.IsNullBlockID())
    {
        m_box.SetMinPoint(0.0, 0.0, 0.0);
        m_box.SetMaxPoint(0.0, 0.0, 0.0);
        return;
    }

    CDXFBlockTableRecord *pBlock = entity.GetBlock();
    CDXFBoundingBox tempBBox;
    CDXFMatrix3D matrix = entity.GetOCSToWCSMatrix();
    CDXF3DPoint points[8];
    
    tempBBox = CDXFEntityBoundingBoxVisitor::GetBoundingBox(*pBlock);

    points[0] = tempBBox.GetMinPoint();
    points[4] = tempBBox.GetMaxPoint();

    // Front side
    points[1].SetValues(points[4].x, points[0].y, points[0].z);
    points[2].SetValues(points[4].x, points[4].y, points[0].z);
    points[3].SetValues(points[0].x, points[4].y, points[0].z);

    // Back side
    points[5].SetValues(points[4].x, points[0].y, points[4].z);
    points[6].SetValues(points[0].x, points[0].y, points[4].z);
    points[7].SetValues(points[0].x, points[4].y, points[4].z);

    int32_t	i;

    // Transform all bounding box points
    for (i = 0; i < 8; ++i) {
        points[i] = MatrixPointMultiply(matrix, points[i]);
    }

    // Update bounding box extremas of the referenced block
    CDXF3DPoint min_pt(1.E+10,   1.E+10,  1.E+10);
    CDXF3DPoint max_pt(-1.E+10, -1.E+10, -1.E+10);

    for (i = 0; i < 8; ++i)
    {
        if (points[i].x < min_pt.x) {
            min_pt.x = points[i].x;
        }

        if (points[i].x > max_pt.x) {
            max_pt.x = points[i].x;
        }

        if (points[i].y < min_pt.y) {
            min_pt.y = points[i].y;
        }

        if (points[i].y > max_pt.y) {
            max_pt.y = points[i].y;
        }

        if (points[i].z < min_pt.z) {
            min_pt.z = points[i].z;
        }

        if (points[i].z > max_pt.z) {
            max_pt.z = points[i].z;
        }
    }

    m_box.SetMaxPoint(max_pt);
    m_box.SetMinPoint(min_pt);
}

void CDXFEntityBoundingBoxVisitor::VisitDXFDimension(CDXFDimension &entity)
{
    if ( entity.m_dimBlockID.IsNull() ) {
        return;
    }

    CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)entity.m_dimBlockID.GetObject();
    m_box = CDXFEntityBoundingBoxVisitor::GetBoundingBox(*pRecord);
}

const CDXFBoundingBox &CDXFEntityBoundingBoxVisitor::GetBoundingBox() const
{
    return m_box;
}

CDXFBoundingBox CDXFEntityBoundingBoxVisitor::GetBoundingBox(const CDXFBlockTableRecord &block)
{
    CDXFBoundingBox bbox;
	
	bbox.SetMinPoint(1.E+10,   1.E+10,  1.E+10);
	bbox.SetMaxPoint(-1.E+10, -1.E+10, -1.E+10);

    CDXFEntityBoundingBoxVisitor vis;

	for (const auto &objectID : block.m_entities) {
		CDXFEntity *pEntity = (CDXFEntity *)objectID.GetObject();
        pEntity->Accept(vis);
		bbox.Union(vis.GetBoundingBox());
	}

	return bbox;
}