#include "stdafx.h"
#include "DXFDatabaseEx.h"

#include "DXFDatabase.h"
#include "entity/DXFLine.h"
#include "entity/DXFCircle.h"
#include "entity/DXFPolyline.h"
#include "entity/DXFAlignedDimension.h"
#include "entity/DXFAngledDimension.h"
#include "entity/DXFRotatedDimension.h"
#include "entity/DXFInsert.h"
#include "entity/DXFArc.h"
#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFLineTypeTableRecord.h"
#include "tables/DXFTextStyleTableRecord.h"
#include "entity/DXFSolid.h"
#include "entity/DXFHatch.h"
#include "entity/DXFEllipse.h"

#include "Utils/DXFMath.h"
#include "Utils/DXFTextUtils.h"
#include "Utils/DXFDeepCloneRecordUtil.h"

#include "file_utils/dxf_database_file_util.h"

CDXFDatabaseEx::CDXFDatabaseEx(CDXFDatabase *pDatabase)
{
	ASSERT(pDatabase);
	m_pDatabase = pDatabase;
	Init();
}

bool CDXFDatabaseEx::SetLayer(const char *sLayerName)
{
	CDXFObjectID newLayerID;

	m_pDatabase->GetLayerTable()->GetTableRecordId(sLayerName, newLayerID);
	if (newLayerID.IsNull()) {
		return false;
	}
	m_layerID = newLayerID;

	return true;
}

void CDXFDatabaseEx::SetTextSize(double dTextSize)
{
	m_dTextSize = dTextSize;
}

void CDXFDatabaseEx::SetTextAngle(double dAngle)
{
	m_dTextAngle = dAngle;
}

void CDXFDatabaseEx::SetTextAlign(ETextHorzAlignment horz, ETextVertAlignment vert)
{
	m_horzAlign = horz;
	m_vertAlign = vert;
}

bool CDXFDatabaseEx::SetBlock(const char *sBlockName)
{
	CDXFBlockTable *pBlockTable = m_pDatabase->GetBlockTable();
	if (!pBlockTable) {
		return false;
    }

    CDXFBlockTableRecord *pBlock = pBlockTable->GetBlockTableRecord(sBlockName);
	if (!pBlock) {
		return false;
	}

	m_pBlock = pBlock;
	return true;
}

void CDXFDatabaseEx::InsertBlock(const char *sSourceBlockName,
								 double posx, double posy,
								 double scalex, double scaley,
								 double dAngle)
{
	ASSERT(m_pBlock);

	if (_stricmp(sSourceBlockName, DXF_MODEL_SPACE) == 0) {
		return;
    }

	CDXFBlockTableRecord *pSourceBlock = m_pDatabase->GetBlockTable()->GetBlockTableRecord(sSourceBlockName);
	if ( !pSourceBlock ) {
		return;
    }

	if (::strcmpi(m_pBlock->GetName(), sSourceBlockName) == 0) {
		return;
    }

    CDXFInsert *pInsert = CDXFInsert::CreateInsert(pSourceBlock->GetObjectID());
	pInsert->SetInsertPoint(CDXF3DPoint(posx, posy, 0.0));
	pInsert->SetAngle(dAngle);
	pInsert->SetScale(scalex, scaley, 1.0);
    pInsert->SetLayerID(m_layerID);

	m_pBlock->AddEntity(pInsert, m_pDatabase);
}

bool CDXFDatabaseEx::CreateBlock(const char *sBlockName, const char *sLayerName)
{
	CDXFBlockTable *pBlockTable = m_pDatabase->GetBlockTable();
	if (!pBlockTable) {
		return false;
    }

	CDXFLayerTable *pLayerTable = m_pDatabase->GetLayerTable();
	if (!pLayerTable) {
		return false;
    }

    CDXFObjectID blockID;
	CDXFObjectID layerID;

	pLayerTable->GetTableRecordId(sLayerName, layerID);
	if (layerID.IsNull())
	{
		if (!CreateLayer(sLayerName)) {
			return false;
        }

		pLayerTable->GetTableRecordId(sLayerName, layerID);
		ASSERT(!layerID.IsNull());
	}

	pBlockTable->GetTableRecordId(sBlockName, blockID);

	if (blockID.IsNull()) {
		m_pBlock = pBlockTable->CreateBlockTableRecord(sBlockName, m_pDatabase);
	}
	
	m_pBlock->SetLayer(layerID);

	return true;
}

bool CDXFDatabaseEx::CreateLayer(const char *sLayerName)
{
	CDXFLayerTable *pLayerTable = m_pDatabase->GetLayerTable();
	if (!pLayerTable) {
		return false;
    }

	CDXFLineTypeTable *pLinetypeTable = m_pDatabase->GetLineTypeTable();
	if (!pLinetypeTable) {
		return false;
    }

	CDXFLayerTableRecord *pLayer = pLayerTable->GetLayerTableRecord(sLayerName);
    if (!pLayer) {
		pLayer = new CDXFLayerTableRecord(sLayerName);
		pLayerTable->AddTableRecord(pLayer, m_pDatabase);
	}

    CDXFObjectID linetypeID;
    pLinetypeTable->GetTableRecordId(DXF_LINE_TYPE_CONTINUOUS, linetypeID);
	pLayer->SetLinetype(linetypeID);

	pLayer->SetColor(EDXFColor::EDXFColorWhite);

	return true;
}

CDXFLine *CDXFDatabaseEx::Line(double x1, double y1, double x2, double y2, bool bAdd)
{
	ASSERT(m_pBlock);
	CDXFLine *pLine = new CDXFLine(CDXF3DPoint(x1, y1, 0.0), CDXF3DPoint(x2, y2, 0.0));
    pLine->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pLine, m_pDatabase);
    }

	return pLine;
}

CDXFPolyline *CDXFDatabaseEx::Polyline(double x1, double y1, double x2, double y2, bool bAdd)
{
	ASSERT(m_pBlock);

	CDXFPolyline *pPolyline = new CDXFPolyline;

	pPolyline->AddVertex(x1, y1, 0.0, 0.0);
	pPolyline->AddVertex(x2, y2, 0.0, 0.0);
    pPolyline->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pPolyline, m_pDatabase);
    }

	return pPolyline;
}

CDXFPolyline *CDXFDatabaseEx::Polyline(double *verts, int32_t nNumVerts, bool bAdd)
{
	ASSERT(m_pBlock);

	CDXFPolyline *pPolyline = new CDXFPolyline;

	for (auto i = 0; i < nNumVerts; ++i, verts += 2) {
		pPolyline->AddVertex(*verts, *(verts + 1), 0.0, 0.0);
	}
    pPolyline->SetLayerID(m_layerID);
	
	if (bAdd) {
		m_pBlock->AddEntity(pPolyline, m_pDatabase);
    }

	return pPolyline;
}

CDXFPolyline * CDXFDatabaseEx::Polyline(const std::vector<CDXFGeBulgePoint3D> &verts, int32_t nNumVerts, bool bAdd)
{
	ASSERT(m_pBlock);

	CDXFPolyline *pPolyline = new CDXFPolyline;

	for (auto i = 0; i < nNumVerts; ++i) {
		pPolyline->AddVertex(verts[i]);
	}
    pPolyline->SetLayerID(m_layerID);
	
	if (bAdd) {
		m_pBlock->AddEntity(pPolyline, m_pDatabase);
    }

	return pPolyline;
}

CDXFCircle * CDXFDatabaseEx::Circle(double centerx, double centery, double radius, bool bAdd)
{
	ASSERT(m_pBlock);
	CDXFCircle *pCircle = new CDXFCircle(CDXF3DPoint(centerx, centery, 0.0), radius);
	pCircle->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pCircle, m_pDatabase);
    }

	return pCircle;
}

CDXFAlignedDimension * CDXFDatabaseEx::AlignedDimension(double x1, double y1,
														double x2, double y2,
														double dHeight, bool bAdd)
{
	ASSERT(m_pBlock);

	if (m_dimStyleID.IsNull()) {
		return nullptr;
    }
	
    const bool bNoDimLines = false;
	CDXFAlignedDimension *pDimension = new CDXFAlignedDimension(CDXF3DPoint(x1, y1, 0.0), CDXF3DPoint(x2, y2, 0.0), dHeight, bNoDimLines);
	pDimension->SetDimStyle(m_dimStyleID);
    pDimension->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pDimension, m_pDatabase);
    }

    CDXFAlignedDimension::InitWithDimLines(pDimension, m_pDatabase);
	return pDimension;
}

void CDXFDatabaseEx::AlignedDimensionNoDimLines(double x1, double y1, double x2, double y2)
{
	ASSERT(m_pBlock);

	if (m_dimStyleID.IsNull()) {
		return;
    }
	
    const bool bNoDimLines = true;
	CDXFAlignedDimension *pDimension = new CDXFAlignedDimension(CDXF3DPoint(x1, y1, 0.0), CDXF3DPoint(x2, y2, 0.0), 7, bNoDimLines);
	pDimension->SetDimStyle(m_dimStyleID);
    pDimension->SetOverrideDimText(true);
    pDimension->SetLayerID(m_layerID);
    
    CDXFAlignedDimension::InitNoDimLines(pDimension, m_pDatabase);
	
    AddEntity(pDimension, false);
}

void CDXFDatabaseEx::AngledDimension(double x, double y, double x1, double y1, double x2, double y2, double dRadius)
{
	ASSERT(m_pBlock);

	if (m_dimStyleID.IsNull()) {
		return;
    }
	
	CDXFAngledDimension *pDimension = new CDXFAngledDimension(CDXF3DPoint(x, y, 0.0),
															  CDXF3DPoint(x1, y1, 0.0),
															  CDXF3DPoint(x2, y2, 0.0));

	pDimension->SetDimStyle(m_dimStyleID);
    pDimension->SetLayerID(m_layerID);

	m_pBlock->AddEntity(pDimension, m_pDatabase);
    CDXFAngledDimension::Init(pDimension, m_pDatabase, dRadius);
}

void CDXFDatabaseEx::LinearDimension(double x1, double y1, double x2, double y2, double dimx, double dimy, double dAngle, bool bAdd)
{
	ASSERT(m_pBlock);
	
	if (m_dimStyleID.IsNull()) {
		return;
    }

	CDXFRotatedDimension *pDimension = new CDXFRotatedDimension(dAngle,
                                                                CDXF3DPoint(x1, y1, 0.0),
		                                                        CDXF3DPoint(x2, y2, 0.0),
		                                                        CDXF3DPoint(dimx, dimy, 0.0));
	pDimension->SetDimStyle(m_dimStyleID);
	pDimension->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pDimension, m_pDatabase);
    }
    CDXFRotatedDimension::Init(pDimension, m_pDatabase);
}

CDXFRotatedDimension *CDXFDatabaseEx::LinearDimension(double x1, double y1,
													  double x2, double y2,
													  double dimx, double dimy,
													  const char *sDimText,
													  double dAngle, bool bAdd)
{
	ASSERT(m_pBlock);
	
	if (m_dimStyleID.IsNull()) {
		return nullptr;
    }

	CDXFRotatedDimension *pDimension = new CDXFRotatedDimension(dAngle,
		                                                        CDXF3DPoint(x1, y1, 0.0),
		                                                        CDXF3DPoint(x2, y2, 0.0),
		                                                        CDXF3DPoint(dimx, dimy, 0.0),
		                                                        sDimText);

	pDimension->SetDimStyle(m_dimStyleID);
	pDimension->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pDimension, m_pDatabase);
    }

    CDXFRotatedDimension::Init(pDimension, m_pDatabase);
	return pDimension;
}

void CDXFDatabaseEx::Init()
{
	ASSERT(m_pDatabase);
	m_pDatabase->GetLayerTable()->GetTableRecordId("0", m_layerID);
	m_pBlock = m_pDatabase->GetBlockTable()->GetBlockTableRecord(DXF_MODEL_SPACE);
	m_pDatabase->GetDimStyleTable()->GetTableRecordId("STANDARD", m_dimStyleID);
	m_pDatabase->GetTextStyleTable()->GetTableRecordId("STANDARD", m_textStyleID);
	
	ASSERT(!m_layerID.IsNull());
	ASSERT(m_pBlock);
}

void CDXFDatabaseEx::SaveDatabase(const wchar_t *sFileName)
{
    dxf_database_file_util::SaveDatabase(sFileName, m_pDatabase);
}

bool CDXFDatabaseEx::LoadTemplate(const wchar_t *sFileName)
{
    if ( !m_pDatabase->OpenDatabase(sFileName) )
		return false;

	Init();

	return true;
}

CDXFArc *CDXFDatabaseEx::Arc(double centerx, double centery, double radius, double dStartAngle, double dEndAngle, bool bAdd)
{
	ASSERT(m_pBlock);
	CDXFArc *pArc = new CDXFArc(CDXF3DPoint(centerx, centery, 0.0), radius,  dStartAngle, dEndAngle);
    pArc->SetLayerID(m_layerID);
	
	if (bAdd) {
		m_pBlock->AddEntity(pArc, m_pDatabase);
    }
	return pArc;
}

bool CDXFDatabaseEx::LoadBlock(const wchar_t *sFileName, const char *sBlockName)
{
	CDXFDatabase srcDatabase;
    if ( !srcDatabase.OpenDatabase(sFileName) )
		return false;

    CDXFBlockTable *pSrcBlockTable = srcDatabase.GetBlockTable();
	if ( !pSrcBlockTable ) {
		return false;
    }

	CDXFBlockTableRecord *pSrcBlock = pSrcBlockTable->GetBlockTableRecord(sBlockName);
	if ( !pSrcBlock ) {
		return false;
    }

	// Check if block with the specified name already exists
	CDXFBlockTable *pBlockTable = m_pDatabase->GetBlockTable();
	if ( !pBlockTable ) {
		return false;
    }

	CDXFBlockTableRecord *pNewBlock = pBlockTable->GetBlockTableRecord(sBlockName);
	if ( !pNewBlock ) {
        pNewBlock = (CDXFBlockTableRecord *)deep_clone_util::DeepCloneBlockTableRecord(pSrcBlock, &srcDatabase, m_pDatabase);
	}

	return true;
}

CDXFHatch * CDXFDatabaseEx::Hatch(const std::vector<CDXFGePoint2D> &vertices, const char* strPatternName, double dAngle, double dScale, bool bAdd)
{
	ASSERT(vertices.size() >= 3);

	CDXFHatch *pHatch = new CDXFHatch;
	pHatch->SetPattern(ePatternPredefined, strPatternName, dAngle, dScale);

	if (::strcmpi(strPatternName, "SOLID") == 0)
	{
		pHatch->AddBoundary(eBoundaryTypePolyline, vertices);
	}
	else if (::strcmpi(strPatternName, "LINE") == 0)
	{
		std::vector<CDXFGeLineSeg2D *> edges;
		size_t nNumVertices = vertices.size();
		std::vector<CDXFGeLineSeg2D> segs;

		edges.resize(nNumVertices);
		segs.resize(nNumVertices);

		segs[0].m_start = vertices[nNumVertices - 1];
		segs[0].m_end = vertices[0];

		edges[0] = &segs[0];

		for (size_t i = 0; i < nNumVertices - 1; i++)
		{
			segs[i + 1].m_start = vertices[i];
			segs[i + 1].m_end = vertices[i + 1];
			edges[i + 1] = &segs[i + 1];
		}

		pHatch->AddBoundaryLines(eBoundaryTypeExternal, edges);
	}

    pHatch->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pHatch, m_pDatabase);
    }
	return pHatch;
}

CDXFHatch * CDXFDatabaseEx::Hatch(const char* strPatternName, double dAngle, double dScale)
{
	CDXFHatch *pHatch = new CDXFHatch;
	pHatch->SetPattern(ePatternPredefined, strPatternName, dAngle, dScale);
    pHatch->SetLayerID(m_layerID);
    m_pBlock->AddEntity(pHatch, m_pDatabase);
	return pHatch;
}

bool CDXFDatabaseEx::SetDimStyle(const char *sDimStyleName)
{
	CDXFObjectID dimStyleID;
	
	CDXFDimStyleTable *pDimStyleTable = m_pDatabase->GetDimStyleTable();
	if (!pDimStyleTable) {
		return false;
    }

	pDimStyleTable->GetTableRecordId(sDimStyleName, dimStyleID);
	if (dimStyleID.IsNull()) {
		return false;
    }
	
	m_dimStyleID = dimStyleID;
	return true;
}

bool CDXFDatabaseEx::SetTextStyle(const char *sTextStyleName)
{
	CDXFObjectID textStyleID;

	CDXFTextStyleTable *pTextStyleTable = m_pDatabase->GetTextStyleTable();
	if (!pTextStyleTable) {
		return false;
    }

	pTextStyleTable->GetTableRecordId(sTextStyleName, textStyleID);
	if (textStyleID.IsNull()) {
		return false;
    }

	m_textStyleID = textStyleID;

	return true;
}

bool CDXFDatabaseEx::AddEntity(CDXFEntity *pEntity, bool bInit)
{
	ASSERT(pEntity);
	ASSERT(m_pBlock);

	if (bInit) {
        pEntity->SetLayerID(m_layerID);
	}

	return m_pBlock->AddEntity(pEntity, m_pDatabase);
}

CDXFDatabase * CDXFDatabaseEx::GetDatabase() const
{
	return m_pDatabase;
}

CDXFLayerTableRecord * CDXFDatabaseEx::GetLayer() const
{
	if (m_layerID.IsNull()) {
		return nullptr;
    }
	else {
		return (CDXFLayerTableRecord *)m_layerID.GetObject();
    }
}

CDXFObjectID &CDXFDatabaseEx::GetLayerID()
{
	return m_layerID;
}

CDXFBlockTableRecord *CDXFDatabaseEx::GetBlock() const
{
	return m_pBlock;
}

CDXFDimStyleTableRecord * CDXFDatabaseEx::GetDimStyle() const
{
	if (m_dimStyleID.IsNull()) {
		return nullptr;
    }
	else {
		return (CDXFDimStyleTableRecord *)m_dimStyleID.GetObject();
    }
}

CDXFTextStyleTableRecord * CDXFDatabaseEx::GetTextStyle() const
{
	if (m_textStyleID.IsNull()) {
		return nullptr;
    }
	else {
		return (CDXFTextStyleTableRecord *)m_textStyleID.GetObject();
    }
}

double CDXFDatabaseEx::GetTextSize()
{
	return m_dTextSize;
}

CDXFMText * CDXFDatabaseEx::MText(const char *sText, double x, double y, double dRectWidth, double dAngle, bool bAdd)
{
	ASSERT(m_pBlock);

	CDXFTextStyleTableRecord *pTextStyle = (CDXFTextStyleTableRecord *)m_textStyleID.GetObject();

	CDXFMText *pMText = new CDXFMText;
	pMText->SetText(sText);
	
	pMText->SetTextStyleID(m_textStyleID);
	if (pTextStyle->GetTextHeight() > 0.0)
		pMText->SetTextHeight(pTextStyle->GetTextHeight());
	else
		pMText->SetTextHeight(m_dTextSize);
	
	pMText->SetAngle(dAngle);
	pMText->SetInsertPoint(x, y, 0.0);
	pMText->SetRectangleWidth(dRectWidth);
    pMText->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pMText, m_pDatabase);
    }
	return pMText;
}

CDXFText *CDXFDatabaseEx::Text(const char *sText, double x, double y, bool bAdd)
{
	CDXFText *pText = new CDXFText;
	if (!PrepareTextEntity(pText, sText, x, y, true)) {
		return nullptr;
    }

	if (bAdd) {
		m_pBlock->AddEntity(pText, m_pDatabase);
    }

	return pText;
}

CDXFText* CDXFDatabaseEx::Text2(const char *sText, double x, double y, bool bAdd, bool bUseStyleSize)
{
	CDXFText *pText = new CDXFText;
	if (!PrepareTextEntity(pText, sText, x, y, bUseStyleSize))
		return nullptr;

	if (bAdd) {
		m_pBlock->AddEntity(pText, m_pDatabase);
    }

	return pText;
}

CDXFEllipse *CDXFDatabaseEx::Ellipse(double dCenterX, double dCenterY, double dRadiusX, double dRadiusY, bool bAdd /* = true*/)
{
	CDXF3DPoint ptCenter(dCenterX, dCenterY, 0.0);
	CDXF3DPoint ptEnd(dRadiusX, 0.0, 0.0);

	const double dMinorAxisRatio = dRadiusY / dRadiusX;

	CDXFEllipse *pEllipse = new CDXFEllipse(ptCenter, ptEnd, dMinorAxisRatio);
    pEllipse->SetLayerID(m_layerID);

	if (bAdd) {
		m_pBlock->AddEntity(pEllipse, m_pDatabase);
	}

	return pEllipse;
}

bool CDXFDatabaseEx::PrepareTextEntity(CDXFText *pText, const char *sText, double x, double y, bool bUseStyleSize)
{
	ASSERT(m_pBlock);
	if (m_textStyleID.IsNull()) {
		return false;
    }
		
	CDXFTextStyleTableRecord *pTextStyle = (CDXFTextStyleTableRecord *)m_textStyleID.GetObject();
    pText->SetText(sText);
	pText->SetTextAlign(m_horzAlign, m_vertAlign);
	pText->SetRotationAngle(m_dTextAngle);
	pText->SetInsertPoint(CDXF3DPoint(x, y, 0.0));
    pText->SetLayerID(m_layerID);	
	pText->SetTextStyle(m_textStyleID);

	if (pTextStyle->GetTextHeight() > 0.0 && bUseStyleSize) {
		pText->SetTextHeight(pTextStyle->GetTextHeight());
    }
	else {
		pText->SetTextHeight(m_dTextSize);
    }
	
	return true;
}