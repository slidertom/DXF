#include "stdafx.h"
#include "dxf_block_record_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_block_begin_file_util.h"
#include "dxf_block_end_file_util.h"
#include "dxf_circle_file_util.h"
#include "dxf_line_file_util.h"
#include "dxf_arc_file_util.h"
#include "dxf_polyline_file_util.h"
#include "dxf_insert_file_util.h"
#include "dxf_hatch_file_util.h"
#include "dxf_point_file_util.h"
#include "dxf_text_file_util.h"
#include "dxf_aligndim_file_util.h"
#include "dxf_rotateddim_file_util.h"
#include "dxf_solid_file_util.h"
#include "dxf_mtext_file_util.h"

#include "tables/DXFBlockTableRecord.h"
#include "entity/DXFBlockBegin.h"
#include "entity/DXFBlockEnd.h"
#include "DXFDatabase.h"
#include "entity/DXFLine.h"
#include "entity/DXFCircle.h"
#include "entity/DXFArc.h"
#include "entity/DXFPolyline.h"
#include "entity/DXFInsert.h"
#include "entity/DXFHatch.h"
#include "entity/DXFPoint.h"
#include "entity/DXFAlignedDimension.h"
#include "entity/DXFRotatedDimension.h"
#include "entity/DXFSolid.h"
#include "entity/DXFMText.h"
#include "entity/DXFText.h"

#include "DXFEntityWriterVisitor.h"

namespace dxf_block_record_file_util
{
    DXF_ERROR ReadEntitiesFromFile(CDXFReader &reader, CDXFBlockTableRecord *pBlock)
    {
        CDXFReaderItem item;
        bool bContinue = true;
        CDXFObjectID objectID;
        CDXFEntity *pEntity = nullptr;
        DXF_ERROR eError = DXF_OK;

        reader.ReadItem(item);
        while (bContinue)
        {
            if (item.m_nGroupCode == 0)
            {
                if (0 == ::strcmpi(item.m_resString, "ENDSEC")) {
                    bContinue = false;
                }
                else if (0 == ::strcmpi(item.m_resString, "ENDBLK")) {
                    bContinue = false;
                }
                else if (0 == ::strcmpi(item.m_resString, "LINE")) {
                    pEntity = new CDXFLine;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_line_file_util::ReadFromFile(reader, (CDXFLine *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (::strcmpi(item.m_resString, "CIRCLE") == 0) {
                    pEntity = new CDXFCircle();
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_circle_file_util::ReadFromFile(reader, (CDXFCircle *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "ARC")) {
                    pEntity = new CDXFArc;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_arc_file_util::ReadFromFile(reader, (CDXFArc *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "LWPOLYLINE")) {
                    pEntity = new CDXFPolyline;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_polyline_file_util::ReadFromFile(reader, (CDXFPolyline *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "INSERT")) {
                    pEntity = new CDXFInsert;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_insert_file_util::ReadFromFile(reader, (CDXFInsert *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "HATCH")) {
                    pEntity = new CDXFHatch;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_hatch_file_util::ReadFromFile(reader, (CDXFHatch *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "POINT")) {
                    pEntity = new CDXFPoint;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_point_file_util::ReadFromFile(reader, (CDXFPoint *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "TEXT")) {
                    pEntity = new CDXFText;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_text_file_util::ReadFromFile(reader, (CDXFText *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "DIMENSION"))
                {
                    char sSubclassName[255];

                    reader.GetEntitySubclassName(sSubclassName);
                    if (::strcmp(sSubclassName, "AcDbAlignedDimension") == 0) {
                        pEntity = new CDXFAlignedDimension;
                        dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                        eError = dxf_align_dim_file_util::ReadFromFile(reader, (CDXFAlignedDimension *)pEntity);
                        if (eError != DXF_OK) {
                            return eError;
                        }
                    }
                    else if (::strcmp(sSubclassName, "AcDbRotatedDimension") == 0) {
                        pEntity = new CDXFRotatedDimension;
                        dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                        eError = dxf_rotated_dim_file_util::ReadFromFile(reader, (CDXFRotatedDimension *)pEntity);
                        if (eError != DXF_OK) {
                            return eError;
                        }
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "SOLID")) {
                    pEntity = new CDXFSolid;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_solid_file_util::ReadFromFile(reader, (CDXFSolid *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
                else if (0 == ::strcmpi(item.m_resString, "MTEXT")) {
                    pEntity = new CDXFMText;
                    dxf_entity_file_util::PreReadFromFile(reader, pEntity);
                    eError = dxf_mtext_file_util::ReadFromFile(reader, (CDXFMText *)pEntity);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                }
            }

            if (bContinue)
            {
                if (pEntity)
                {
                    if (pEntity->GetOwnerID().IsNull()) {
                        pEntity->SetOwnerID(pBlock->m_objectID);
                    }
                    pBlock->m_entities.push_back(pEntity->GetObjectID());
                    pEntity = nullptr;
                }

                reader.ReadItem(item);
            }
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    DXF_ERROR ReadBlockFromFile(CDXFReader &reader, CDXFBlockTableRecord *pBlock)
    {
        ASSERT(reader.GetDatabase());

        // Read all entities
        DXF_ERROR eError = dxf_block_record_file_util::ReadEntitiesFromFile(reader, pBlock);
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        if (::strcmpi(item.m_resString, "ENDBLK") != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        CDXFEntity *pEntity = new CDXFBlockEnd;
        dxf_entity_file_util::PreReadFromFile(reader, pEntity);
        eError = dxf_block_end_file_util::ReadFromFile(reader, pEntity);

        if (eError != DXF_OK) {
            return eError;
        }

        CDXFBlockTableRecord::SetBlockEndID(pBlock, pEntity->GetObjectID(), reader.GetDatabase());
        return DXF_OK;
    }

    void WriteBlockToFile(CDXFWriter &writer, const CDXFBlockTableRecord &block, CDXFDatabase *pDB)
    {
        const CDXFBlockBegin *pObj = block.GetBlockBegin();
        dxf_entity_file_util::PreWriteToFile(writer, *pObj);
        dxf_block_begin_file_util::WriteToFile(writer, *pObj, pDB);

        if (::strcmpi(block.GetName(), DXF_MODEL_SPACE) != 0) {
            dxf_block_record_file_util::WriteEntititiesToFile(writer, block, pDB);
        }

        CDXFBlockEnd *pBlockEnd = (CDXFBlockEnd *)block.GetBlockEnd();
        dxf_entity_file_util::PreWriteToFile(writer,*pBlockEnd);
        dxf_block_end_file_util::WriteToFile(writer,*pBlockEnd);
    }

    void WriteEntititiesToFile(CDXFWriter &writer, const CDXFBlockTableRecord &block, CDXFDatabase *pDB)
    {
        CDXFEntityWriterVisitor vis(writer, pDB);

        const size_t nCnt = block.m_entities.size();
        for (size_t i = 0; i < nCnt; ++i) {
            CDXFEntity *pObj = (CDXFEntity *)(block.m_entities[i]).GetObject();
            dxf_entity_file_util::PreWriteToFile(writer, *pObj);
            pObj->Accept(vis);
        }
    }
};