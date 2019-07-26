#include "stdafx.h"
#include "DXFBlockCloneVisitor.h"

#include "entity/DXFArc.h"
#include "entity/DXFCircle.h"
#include "entity/DXFHatch.h"
#include "entity/DXFLine.h"
#include "entity/DXFMText.h"
#include "entity/DXFText.h"
#include "entity/DXFPoint.h"
#include "entity/DXFPolyline.h"
#include "entity/DXFSolid.h"
#include "entity/DXFInsert.h"
#include "DXFDatabase.h"
#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFLineTypeTableRecord.h"
#include "tables/DXFTextStyleTableRecord.h"
#include "entity/DXFAlignedDimension.h"
#include "entity/DXFAngledDimension.h"
#include "entity/DXFRotatedDimension.h"
#include "entity/DXFEllipse.h"
#include "entity/DXFBlockBegin.h"

#include "DXFDeepCloneRecordUtil.h"

CDXFBlockCloneVisitor::CDXFBlockCloneVisitor(CDXFDatabase *pDB, CDXFDatabase *pSrcDB)
{
    m_pDB = pDB;
    m_pSrcDB = pSrcDB;
}

void CDXFBlockCloneVisitor::VisitDXFArc(CDXFArc &entity)
{
    m_pObj = new CDXFArc(entity);
}

void CDXFBlockCloneVisitor::VisitDXFCircle(CDXFCircle &entity)
{ 
    m_pObj = new CDXFCircle(entity);
}

void CDXFBlockCloneVisitor::VisitDXFHatch(CDXFHatch &entity)
{
    m_pObj = new CDXFHatch(entity);
}

void CDXFBlockCloneVisitor::VisitDXFLine(CDXFLine &entity)
{
    m_pObj = new CDXFLine(entity);
}

void CDXFBlockCloneVisitor::VisitDXFMText(CDXFMText &entity)
{
    CDXFMText *pObjText = new CDXFMText(entity);
    m_pObj = pObjText;

    CDXFObjectID textStyleIDSrc = entity.GetTextStyleID();
	if ( textStyleIDSrc.IsNull() ) {
        return;
	}

  	CDXFTextStyleTableRecord *pTextStyleSrc = (CDXFTextStyleTableRecord *)textStyleIDSrc.GetObject();
	const std::string sTextStyle = pTextStyleSrc->GetName();
    CDXFTextStyleTable *pTextStyleTrg = m_pDB->GetTextStyleTable();
	CDXFObjectID textStyleIDTrg;

	// Get text style of the original text entity
	pTextStyleTrg->GetTableRecordId(sTextStyle.c_str(), textStyleIDTrg);
	if (textStyleIDTrg.IsNull()) {
		return;
    }

	pObjText->SetTextStyleID(textStyleIDTrg);
}

void CDXFBlockCloneVisitor::VisitDXFPoint(CDXFPoint &entity)
{
    m_pObj= new CDXFPoint(entity);
}

void CDXFBlockCloneVisitor::VisitDXFPolyline(CDXFPolyline &entity)
{
    m_pObj = new CDXFPolyline(entity);
}

void CDXFBlockCloneVisitor::VisitDXFSolid(CDXFSolid &entity)
{
    m_pObj = new CDXFSolid(entity);
}

void CDXFBlockCloneVisitor::VisitDXFText(CDXFText &entity)
{
    CDXFText *pObj = new CDXFText(entity);
    m_pObj = pObj;

    CDXFObjectID textStyleIDSrc = entity.GetTextStyle();
	if ( textStyleIDSrc.IsNull() ) {
        return;
	}

  	CDXFTextStyleTableRecord *pTextStyleSrc = (CDXFTextStyleTableRecord *)textStyleIDSrc.GetObject();
	const std::string sTextStyle = pTextStyleSrc->GetName();
    CDXFTextStyleTable *pTextStyleTrg = m_pDB->GetTextStyleTable();
	CDXFObjectID textStyleIDTrg;

	// Get text style of the original text entity
	pTextStyleTrg->GetTableRecordId(sTextStyle.c_str(), textStyleIDTrg);
	if (textStyleIDTrg.IsNull()) {
		return;
    }

	pObj->SetTextStyle(textStyleIDTrg);
}

void CDXFBlockCloneVisitor::VisitDXFInsert(CDXFInsert &entity)
{ 
    if ( entity.IsNullBlockID() ) {
        m_pObj = nullptr;
        return;
    }

    CDXFBlockTableRecord *pBlock    = entity.GetBlock();
    CDXFBlockTable *pBlockTable     = m_pDB->GetBlockTable();

    CDXFBlockTableRecord *pNewBlock = pBlockTable->GetBlockTableRecord(pBlock->GetName());
    
    CDXFBlockCloneVisitor vis(m_pDB, m_pSrcDB);
    if ( !pNewBlock ) {
        pBlock->AcceptRecordVis(vis);
        pNewBlock = (CDXFBlockTableRecord *)vis.GetObj();
    }

    m_pObj = CDXFInsert::CreateInsert(entity, pNewBlock->GetObjectID());
}

class CDXFDimBlockCloneVisitor : public CDXFDimensionVisitor
{
// Construction/Destruction
public:
    CDXFDimBlockCloneVisitor(CDXFDatabase *pDB, const CDXFObjectID &dimStyleIDDst) : m_pDB(pDB), m_dimStyleIDDst(dimStyleIDDst) { };
	virtual ~CDXFDimBlockCloneVisitor() { }

// Overrides CDXFEntityVisitor
protected:
    virtual void VisitDXFAlignedDimension(CDXFAlignedDimension &entity)     override;
    virtual void VisitDXFAngledDimension(CDXFAngledDimension &entity)       override;
    virtual void VisitDXFRotatedDimension(CDXFRotatedDimension &entity)     override;

// Operations
public:
    CDXFObject *GetObj() const { return m_pObj; };

// Attributes
private:
    CDXFObject *m_pObj  {nullptr};
    CDXFDatabase *m_pDB {nullptr};
    CDXFObjectID m_dimStyleIDDst;
};

void CDXFDimBlockCloneVisitor::VisitDXFAlignedDimension(CDXFAlignedDimension &entity)
{
    CDXFAlignedDimension *pObj = new CDXFAlignedDimension(entity);
    m_pObj = pObj;

    if (!m_dimStyleIDDst.IsNull()) {
		pObj->SetDimStyle(m_dimStyleIDDst);
	}

    CDXFAlignedDimension::Init(pObj, m_pDB);
}

void CDXFDimBlockCloneVisitor::VisitDXFAngledDimension(CDXFAngledDimension &entity)
{ 
    CDXFAngledDimension *pObj = new CDXFAngledDimension(entity);
    m_pObj = pObj;
    if (!m_dimStyleIDDst.IsNull()) {
		pObj->SetDimStyle(m_dimStyleIDDst);
	}
}

void CDXFDimBlockCloneVisitor::VisitDXFRotatedDimension(CDXFRotatedDimension &entity)
{
    CDXFRotatedDimension *pObj = new CDXFRotatedDimension(entity);
    m_pObj = pObj;

    if (!m_dimStyleIDDst.IsNull()) {
		pObj->SetDimStyle(m_dimStyleIDDst);
	}

    CDXFRotatedDimension::Init(pObj, m_pDB);
}

void CDXFBlockCloneVisitor::VisitDXFDimension(CDXFDimension &entity)
{
    CDXFObjectID dimStyleIDSrc = entity.m_dimStyleID;
    CDXFObjectID dimStyleIDDst;

	if (!dimStyleIDSrc.IsNull()) {
		CDXFDimStyleTableRecord *pDimStyle = (CDXFDimStyleTableRecord *)dimStyleIDSrc.GetObject();
		const char *sDimStyle = pDimStyle->GetName();
       	CDXFDimStyleTable *pDimStyleTable = m_pDB->GetDimStyleTable();
    	ASSERT(pDimStyleTable);

		if ( 0 != strlen(sDimStyle) ) {
			pDimStyleTable->GetTableRecordId(sDimStyle, dimStyleIDDst); // Get dim style of the original dim entity
		}
		else {
			pDimStyleTable->GetTableRecordId("STANDARD", dimStyleIDDst);
		}
	}

    CDXFDimBlockCloneVisitor vis(m_pDB, dimStyleIDDst);
    entity.AcceptDim(vis);
    CDXFDimension *pObj = (CDXFDimension *)vis.GetObj();
    m_pObj = pObj;


    //const CDXFObjectID dimBlockID = entity.GetDimBlock();
    //if ( !dimBlockID.IsNull() ) {
    //    CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)dimBlockID.GetObject();
    //    CDXFBlockCloneVisitor vis(m_pDB, m_pSrcDB);
    //    pRecord->AcceptRecordVis(vis);
    //    CDXFBlockTableRecord *pBlockNew = (CDXFBlockTableRecord *)vis.GetObj();
    //    pObj->SetDimBlock(pBlockNew->GetObjectID());
    //}
}

void CDXFBlockCloneVisitor::VisitDXFEllipse(CDXFEllipse &entity)
{
    m_pObj = new CDXFEllipse(entity);
}

void CDXFBlockCloneVisitor::VisitDXFBlockBegin(CDXFBlockBegin &entity)
{
    m_pObj = new CDXFBlockBegin(entity);
}

void CDXFBlockCloneVisitor::VisitLineTypeTableRecord(CDXFLineTypeTableRecord &record)
{
    m_pObj = new CDXFLineTypeTableRecord(record);
}

void CDXFBlockCloneVisitor::VisitLayerTableRecord(CDXFLayerTableRecord &record)
{
    m_pObj = new CDXFLayerTableRecord(record);
}

void CDXFBlockCloneVisitor::VisitTextStyleTableRecord(CDXFTextStyleTableRecord &record)
{
    m_pObj = new CDXFTextStyleTableRecord(record);
}

void CDXFBlockCloneVisitor::VisitDimStyletableRecord(CDXFDimStyleTableRecord &record)
{
    CDXFDimStyleTable *pDimStyleTable;
    CDXFObjectID objectID;

    pDimStyleTable = m_pDB->GetDimStyleTable();
    CDXFDimStyleTableRecord *pNewRecord = pDimStyleTable->GetDimStyleTableRecord(record.GetName());

    if ( !pNewRecord ) {
        pNewRecord = new CDXFDimStyleTableRecord(record.GetName());
        pDimStyleTable->AddTableRecord(pNewRecord, m_pDB);
    }

    *pNewRecord = record;

    CDXFBlockCloneVisitor vis(m_pDB, m_pSrcDB);

    objectID = record.GetDimBlk0(m_pSrcDB);
    if ( !objectID.IsNull() ) {
        CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)objectID.GetObject();
        pRecord->AcceptRecordVis(vis);
        CDXFBlockTableRecord *pBlockNew = (CDXFBlockTableRecord *)vis.GetObj();
        pNewRecord->SetDimBlk0(pBlockNew->GetObjectID());
    }

    objectID = record.GetDimBlk1(m_pSrcDB);
    if ( !objectID.IsNull() ) {
        CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)objectID.GetObject();
        pRecord->AcceptRecordVis(vis);
        CDXFBlockTableRecord *pBlockNew = (CDXFBlockTableRecord *)vis.GetObj();
        pNewRecord->SetDimBlk1(pBlockNew->GetObjectID());
    }

    objectID = record.GetDimBlk2(m_pSrcDB);
    if ( !objectID.IsNull() ) {
        CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)objectID.GetObject();
        pRecord->AcceptRecordVis(vis);
        CDXFBlockTableRecord *pBlockNew = (CDXFBlockTableRecord *)vis.GetObj();
        pNewRecord->SetDimBlk2(pBlockNew->GetObjectID());
    }

	objectID = record.GetDimTxSty();
    if ( !objectID.IsNull() ) {
        const std::string sTextStyleName = ((CDXFTextStyleTableRecord *)objectID.GetObject())->GetName();
        CDXFTextStyleTableRecord *pTextStyle = nullptr;
        m_pDB->GetTextStyleTable()->GetTableRecord(sTextStyleName.c_str(), pTextStyle);
        if (pTextStyle) {
            pNewRecord->SetDimTxSty(pTextStyle->GetObjectID());
        }
    }

    m_pObj = pNewRecord;
}

void CDXFBlockCloneVisitor::VisitBlockTableRecord(CDXFBlockTableRecord &record)
{
    CDXFBlockTable          *pBlockTable    = m_pDB->GetBlockTable();
    CDXFLineTypeTable       *pLinetypeTable = m_pDB->GetLineTypeTable();
    CDXFLineTypeTableRecord *pLinetype      = nullptr;
    CDXFLayerTable          *pLayerTable    = m_pDB->GetLayerTable();
    CDXFEntity              *pEntity        = nullptr;
    CDXFEntity              *pNewEntity     = nullptr;
    const char *pRecordName                 = record.GetName();
    CDXFObjectID            layerID;

    CDXFBlockTableRecord *pBlock = pBlockTable->GetBlockTableRecord(pRecordName);

    if ( pBlock ) {
        m_pObj = pBlock;
        return;
    }

    if ( !record.m_layerID.IsNull() )
    {
        CDXFLayerTableRecord *pLayer = (CDXFLayerTableRecord *)record.m_layerID.GetObject();

        CDXFObjectID linetypeID = pLayer->GetLinetypeID();
        if ( !linetypeID.IsNull() )
        {
            pLinetype = (CDXFLineTypeTableRecord *)linetypeID.GetObject();
            CDXFLineTypeTableRecord *pNewLinetype = pLinetypeTable->GetLineTypeTableRecord(pLinetype->GetName());

            if ( !pNewLinetype ) {
                pNewLinetype = new CDXFLineTypeTableRecord(*pLinetype);
                pLinetypeTable->AddTableRecordId(pNewLinetype, linetypeID, m_pDB);
            }
            else {
                linetypeID = pNewLinetype->GetObjectID();
            }
        }

        CDXFLayerTableRecord *pNewLayer = pLayerTable->GetLayerTableRecord(pLayer->GetName());

        if ( !pNewLayer ) {
            pNewLayer = new CDXFLayerTableRecord(*pLayer);
            pLayerTable->AddTableRecord(pNewLayer, layerID, m_pDB);
            pNewLayer->SetLinetype(linetypeID);
        }
        else {
            layerID = pNewLayer->GetObjectID();
        }
    }

    pBlock = pBlockTable->CreateBlockTableRecord(pRecordName, m_pDB);

    pBlock->SetOrigin(record.GetOrigin());
    pBlock->SetLayer(layerID);

    auto it_end = record.m_entities.end();
    for (auto pEntityObj = record.m_entities.begin(); pEntityObj != it_end; ++pEntityObj)
    {
        pEntity = (CDXFEntity *)pEntityObj->GetObject();

        layerID = pEntity->GetLayerID();
        if ( !layerID.IsNull() ) {
            CDXFLayerTableRecord *pLayer = (CDXFLayerTableRecord *)layerID.GetObject();
            CDXFLayerTableRecord *pNewLayer = pLayerTable->GetLayerTableRecord(pLayer->GetName());

            if ( !pNewLayer ) {
                pNewLayer = (CDXFLayerTableRecord *)deep_clone_util::DeepCloneLayerTableRecord(pLayer, m_pDB);
                layerID   = pNewLayer->GetObjectID();
            }
            else {
                layerID = pNewLayer->GetObjectID();
            }
        }

        CDXFBlockCloneVisitor vis(m_pDB, m_pSrcDB);
        pEntity->Accept(vis);
        pNewEntity = (CDXFEntity *)vis.GetObj();
        ASSERT(pNewEntity);
        pNewEntity->SetLayerID(layerID);
        pBlock->AddEntity(pNewEntity, m_pDB);
    }

    m_pObj = pBlock;
}

CDXFObject *CDXFBlockCloneVisitor::GetObj() const
{
    return m_pObj;
}