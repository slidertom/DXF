#include "stdafx.h"
#include "DXFDeepCloneRecordUtil.h"

#include "../entity/DXFEntity.h"

#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFLineTypeTableRecord.h"
#include "tables/DXFBlockTableRecord.h"
#include "tables/DXFTextStyleTableRecord.h"
#include "tables/DXFDimStyleTableRecord.h"

#include "DXFBlockCloneVisitor.h"
#include "DXFDatabase.h"

namespace deep_clone_util
{
    CDXFObject *DeepCloneLayerTableRecord(CDXFLayerTableRecord *pRecord, CDXFDatabase *pDatabase)
    {
        CDXFObjectID newLinetypeID;

        if (!pRecord->GetLinetypeID().IsNull())
        {
            CDXFLineTypeTableRecord *pLinetype = (CDXFLineTypeTableRecord *)pRecord->GetLinetypeID().GetObject();

            // Shouldn`t it be src database here? Was always only destination database provided. 
            CDXFLineTypeTable  *pLinetypeTable = pDatabase->GetLineTypeTable();
            CDXFLineTypeTableRecord *pNewLinetype = pLinetypeTable->GetLineTypeTableRecord(pLinetype->GetName());

            if (!pNewLinetype) {
                pNewLinetype = new CDXFLineTypeTableRecord(*pLinetype);
                pLinetypeTable->AddTableRecordId(pNewLinetype, newLinetypeID, pDatabase);
            }
            else {
                newLinetypeID = pNewLinetype->GetObjectID();
            }
        }

        CDXFLayerTableRecord *pNewLayer = new CDXFLayerTableRecord(*pRecord);

        if (!newLinetypeID.IsNull()) {
            pNewLayer->SetLinetype(newLinetypeID);
        }

        pDatabase->GetLayerTable()->AddTableRecord(pNewLayer, pDatabase);

        return pNewLayer;
    }

    CDXFObject *DeepCloneBlockTableRecord(CDXFBlockTableRecord *pRecord, CDXFDatabase *pSrcDB, CDXFDatabase *pDestDB)
    {
        CDXFLayerTable *pLayerTable;
        CDXFLineTypeTable *pLinetypeTable;
        CDXFLayerTableRecord *pLayer;
        CDXFLineTypeTableRecord *pLinetype;
        CDXFObjectID layerID;

        // Here is presented a very quick solution to text style & dimension style
        // loading to the destination database.

        // Iterate through every text style, clone it and add to the
        // text style table of the destination database
        CDXFTextStyleTable *pSrcTextStyleTable = pSrcDB->GetTextStyleTable();
        CDXFTextStyleTable *pDestTextStyleTable = pDestDB->GetTextStyleTable();        

        pSrcTextStyleTable->ForEach([pDestTextStyleTable, pDestDB](CDXFSymbolTableRecord *pRec)
        {
            CDXFTextStyleTableRecord *pNewTextStyle = nullptr;
            CDXFTextStyleTableRecord *pSrcRec = (CDXFTextStyleTableRecord *)pRec;
            pDestTextStyleTable->GetTableRecord(pRec->GetName(), pNewTextStyle);
            if ( !pNewTextStyle ) {
                pNewTextStyle = new CDXFTextStyleTableRecord(*pSrcRec);
                pDestTextStyleTable->AddTableRecord(pNewTextStyle, pDestDB);
            }
        });

        // Do the same for dimension styles
        // Iterate through every dimension style, clone it and add to the
        // dimension style table of the destination database
        CDXFDimStyleTable *pSrcDimStyleTable  = pSrcDB->GetDimStyleTable();
        CDXFDimStyleTable *pDestDimStyleTable = pDestDB->GetDimStyleTable();
        
        pSrcDimStyleTable->ForEach([pDestDimStyleTable, pDestDB, pSrcDB](CDXFSymbolTableRecord *pRec)
        {
            CDXFDimStyleTableRecord *pSrcDimStyle = (CDXFDimStyleTableRecord * )pRec;
            CDXFDimStyleTableRecord *pNewDimStyle = pDestDimStyleTable->GetDimStyleTableRecord(pSrcDimStyle->GetName());
            if ( !pNewDimStyle ) {
                CDXFBlockCloneVisitor vis(pDestDB, pSrcDB);
                pSrcDimStyle->AcceptRecordVis(vis);
                pNewDimStyle = pDestDimStyleTable->GetDimStyleTableRecord(pSrcDimStyle->GetName());
                ASSERT(pNewDimStyle);//To check if clonning was executed properly
            }
        });

        // Now cloning can be done
        pLinetypeTable = pDestDB->GetLineTypeTable();
        pLayerTable = pDestDB->GetLayerTable();

        if (!pRecord->m_layerID.IsNull())
        {
            pLayer = (CDXFLayerTableRecord *)pRecord->m_layerID.GetObject();

            CDXFObjectID linetypeID = pLayer->GetLinetypeID();
            if (!linetypeID.IsNull())
            {
                pLinetype = (CDXFLineTypeTableRecord *)linetypeID.GetObject();

                CDXFLineTypeTableRecord *pNewLinetype = pLinetypeTable->GetLineTypeTableRecord(pLinetype->GetName());
                if (pNewLinetype == NULL)
                {
                    pNewLinetype = new CDXFLineTypeTableRecord(*pLinetype);
                    pLinetypeTable->AddTableRecordId(pNewLinetype, linetypeID, pDestDB);
                }
                else
                {
                    linetypeID = pNewLinetype->GetObjectID();
                }
            }

            CDXFLayerTableRecord *pNewLayer = pLayerTable->GetLayerTableRecord(pLayer->GetName());
            if ( !pNewLayer ) {
                pNewLayer = new CDXFLayerTableRecord(*pLayer);
                pLayerTable->AddTableRecord(pNewLayer, layerID, pDestDB);
                pNewLayer->SetLinetype(linetypeID);
            }
            else {
                layerID = pNewLayer->GetObjectID();
            }
        }

        CDXFBlockTableRecord *pNewBlock = pDestDB->GetBlockTable()->CreateBlockTableRecord(pRecord->GetName(), pDestDB);
        pNewBlock->SetOrigin(pRecord->GetOrigin());
        pNewBlock->SetLineWeight(pRecord->GetLineWeight());
        pNewBlock->SetLayer(layerID);

        for (const auto &objectID : pRecord->m_entities)
        {
            CDXFEntity *pEntity = (CDXFEntity *)objectID.GetObject();
            layerID = pEntity->GetLayerID();
            if (!layerID.IsNull())
            {
                pLayer = (CDXFLayerTableRecord *)layerID.GetObject();
                CDXFLayerTableRecord *pNewLayer = pLayerTable->GetLayerTableRecord(pLayer->GetName());
                if (!pNewLayer) {
                    pNewLayer = (CDXFLayerTableRecord *)deep_clone_util::DeepCloneLayerTableRecord(pLayer, pDestDB);
                    layerID = pNewLayer->GetObjectID();
                }
                else {
                    layerID = pNewLayer->GetObjectID();
                }
            }

            CDXFBlockCloneVisitor vis(pDestDB, pSrcDB);
            pEntity->Accept(vis);
            CDXFEntity *pNewEntity = (CDXFEntity *)vis.GetObj();
            if (pNewEntity) {
                pNewEntity->SetLayerID(layerID);
                pNewBlock->AddEntity(pNewEntity, pDestDB);
            }
        }

        return pNewBlock;
    }
}

