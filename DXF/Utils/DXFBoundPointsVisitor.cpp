#include "stdafx.h"
#include "DXFBoundPointsVisitor.h"

#include "../entity/DXFPolyline.h"
#include "../entity/DXFLine.h"
#include "../entity/DXFPoint.h"
#include "../entity/DXFArc.h"
#include "../entity/DXFCircle.h"
#include "../entity/DXFText.h"
#include "../entity/DXFMText.h"
#include "../entity/DXFSolid.h"
#include "../entity/DXFAlignedDimension.h"
#include "../entity/DXFRotatedDimension.h"
#include "../base/DXF3DPoint.h"
#include "../entity/DXFInsert.h"

#include "DXFUtils.h"

#include "algorithm"

namespace internal
{
	static void AddPointSegment(const CDXF2DPointSegment &point, CDXF2DPointVector *pPoints)
	{
		auto it_find = std::find(pPoints->begin(), pPoints->end(), point);
		if (it_find == pPoints->end()) {
			pPoints->push_back(point);
		}
	}

	static void AddLine(const CDXF2DLineSegment &line, CDXF2DPointVector *pPoints)
	{
		CDXF2DPointSegment point;
		point.m_x = line.m_x1;
		point.m_y = line.m_y1;

		AddPointSegment(point, pPoints);

		point.m_x = line.m_x2;
		point.m_y = line.m_y2;

		AddPointSegment(point, pPoints);
	}

	static void AddPoint(const CDXF3DPoint &dxf_point, CDXF2DPointVector *pPoints)
	{
		CDXF2DPointSegment point;
		point.m_x = dxf_point.x;
		point.m_y = dxf_point.y;

		AddPointSegment(point, pPoints);
	}

	static void AddLines(const CDXF2DLineVector &lines, CDXF2DPointVector *pPoints)
	{
		for (const auto &line : lines) {
			AddLine(line, pPoints);
		}
	}
}

void CDXFBoundPointsVisitor::VisitDXFArc(CDXFArc &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

    CDXF2DLineVector lines;
	CArcGenerator::ConvertArcToLines(lines,
									 entity.m_ptCenter.x,
									 entity.m_ptCenter.y,
									 entity.m_dStartAngle,
									 entity.m_dEndAngle,
									 entity.m_dRadius);

	internal::AddLines(lines, m_pPoints);
}

void CDXFBoundPointsVisitor::VisitDXFCircle(CDXFCircle &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

    CDXF2DLineVector lines;
	CArcGenerator::ConvertArcToLines(lines,
								     entity.GetCenterPoint().x,
								     entity.GetCenterPoint().y,
								     0.0,
								     360.0,
								     entity.GetRadius(),
								     10.0);

	internal::AddLines(lines, m_pPoints);
}

class CDXFDimensionBoundPointsVisitor : public CDXFDimensionVisitor
{
public:
	CDXFDimensionBoundPointsVisitor(CDXF2DPointVector *pPoints) : m_pPoints(pPoints) { }
    ~CDXFDimensionBoundPointsVisitor() { }

protected:
	virtual void VisitDXFAlignedDimension(CDXFAlignedDimension &entity) override;
	virtual void VisitDXFRotatedDimension(CDXFRotatedDimension &entity) override; 

// Attributes
private:
	CDXF2DPointVector *m_pPoints;
};

void CDXFDimensionBoundPointsVisitor::VisitDXFAlignedDimension(CDXFAlignedDimension &entity)
{
	CDXFBlockTableRecord *pBlock = entity.GetBlock();

	if (!pBlock) {
		return;
	}

	CDXFBoundPointsVisitor pointsVisitor(m_pPoints);
	pBlock->VisitEntities(pointsVisitor);
}

void CDXFDimensionBoundPointsVisitor::VisitDXFRotatedDimension(CDXFRotatedDimension &entity)
{
	CDXFBlockTableRecord *pBlock = entity.GetBlock();

	if (!pBlock) {
		return;
	}

    CDXFBoundPointsVisitor pointsVisitor(m_pPoints);
	pBlock->VisitEntities(pointsVisitor);
}

void CDXFBoundPointsVisitor::VisitDXFDimension(CDXFDimension &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

    CDXFDimensionBoundPointsVisitor vis(m_pPoints);
    entity.AcceptDim(vis);
}

void CDXFBoundPointsVisitor::VisitDXFHatch(CDXFHatch &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	//GetBoundingBox(..) not implemented in CDXFHatch;
}

void CDXFBoundPointsVisitor::VisitDXFLine(CDXFLine &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	internal::AddPoint(entity.m_pointStart, m_pPoints);
	internal::AddPoint(entity.m_pointEnd, m_pPoints);
}

void CDXFBoundPointsVisitor::VisitDXFMText(CDXFMText &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	internal::AddPoint(entity.GetInsertPoint(), m_pPoints);
}

void CDXFBoundPointsVisitor::VisitDXFPoint(CDXFPoint &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	internal::AddPoint(entity.m_point, m_pPoints);
}

void CDXFBoundPointsVisitor::VisitDXFPolyline(CDXFPolyline &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	auto vertices = entity.GetVertices();
	for (const auto &point : vertices) {
		internal::AddPoint(point.m_point, m_pPoints);
	}
}

void CDXFBoundPointsVisitor::VisitDXFSolid(CDXFSolid &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	const int32_t nSize = entity.GetNumPoints();
	CDXF3DPoint dxf_point;
	for (auto i = 0; i < nSize; ++i) {
		if (entity.GetPoint(i, dxf_point)) {
			internal::AddPoint(dxf_point, m_pPoints);
		}
	}
}

void CDXFBoundPointsVisitor::VisitDXFText(CDXFText &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	internal::AddPoint(entity.GetInsertPoint(), m_pPoints);
}

void CDXFBoundPointsVisitor::VisitDXFInsert(CDXFInsert &entity)
{
	if (!m_pPoints) {
		ASSERT(FALSE);
		return;
	}

	if ( entity.IsNullBlockID() ) {
		return;
	}
	
	CDXFBlockTableRecord *pBlock = entity.GetBlock();
	const CDXFMatrix3D matrix = entity.GetOCSToWCSMatrix();

	CDXF2DPointVector points;
	CDXFBoundPointsVisitor pointsVisitor(&points);

	pBlock->VisitEntities(pointsVisitor);

	CDXF3DPoint dxf_point;
	for (const auto &point : points) {
		dxf_point.x = point.m_x;
		dxf_point.y = point.m_y;

		dxf_point = MatrixPointMultiply(matrix, dxf_point);
		internal::AddPoint(dxf_point, m_pPoints);
	}
}