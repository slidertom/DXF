#include "stdafx.h"
#include "dxf_block_table_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_block_record_file_util.h"
#include "dxf_block_begin_file_util.h"

#include "tables/DXFBlockTable.h"
#include "tables/DXFBlockTableRecord.h"	
#include "entity/DXFBlockBegin.h"
#include "DXFDatabase.h"

#include "Utils/DXFMath.h"
#include "Utils/DXFTextUtils.h"

namespace dxf_block_table_file_util
{
    void WriteBlocksToFile(CDXFWriter &writer, const CDXFBlockTable &table, CDXFDatabase *pDB)
    {
        const size_t nCnt = table.m_listRecords.size();
        for (size_t i = 0; i < nCnt; ++i) {
            dxf_block_record_file_util::WriteBlockToFile(writer, *((CDXFBlockTableRecord *)(table.m_listRecords[i]).GetObject()), pDB);
        }
    }

    void WriteEntitiesToFile(CDXFWriter &writer, const CDXFBlockTable &table, CDXFDatabase *pDB)
    {
        CDXFBlockTableRecord *pRecord;

        // Search for *MODEL_SPACE table record and DxfOut all entities in it

        const size_t nCnt = table.m_listRecords.size();
        for (size_t i = 0; i < nCnt; ++i) {
            pRecord = (CDXFBlockTableRecord *)(table.m_listRecords[i]).GetObject();
            if (::strcmpi(pRecord->GetName(), DXF_MODEL_SPACE) == 0)
            {
                dxf_block_record_file_util::WriteEntititiesToFile(writer, *pRecord, pDB);
                break;
            }
        }
    }

    DXF_ERROR ReadBlocksFromFile(CDXFReader &reader, CDXFBlockTable *pTable)
    {
        CDXFReaderItem item;
        bool bContinue = true;

        DXF_ERROR eError = DXF_OK;

        while (bContinue)
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != 0) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            if (::strcmp(item.m_resString, "ENDSEC") == 0)
            {
                bContinue = false;
            }
            else if (::strcmp(item.m_resString, "BLOCK") == 0)
            {
                CDXFBlockBegin *pBlockBegin = new CDXFBlockBegin;

                dxf_entity_file_util::PreReadFromFile(reader, pBlockBegin);
                eError = dxf_block_begin_file_util::ReadFromFile(reader, pBlockBegin);

                if (eError != DXF_OK) {
                    return eError;
                }

                CDXFDatabase *pDB = reader.GetDatabase();

                const char *sBlock = pBlockBegin->GetBlockName();
                CDXFBlockTableRecord *pBlock = pTable->GetBlockTableRecord(sBlock);
                if ( !pBlock ) {
                    pBlock = pTable->CreateBlockTableRecord(sBlock, pDB);
                }

                eError = dxf_block_record_file_util::ReadBlockFromFile(reader, pBlock);

                if (eError != DXF_OK) {
                    return eError;
                }

                CDXFBlockTableRecord::SetBlockBeginID(pBlock, pBlockBegin->GetObjectID(), pDB);
            }
            else {
                return DXF_ERROR_BAD_GROUPDATA;
            }
        }

        return DXF_OK;
    }

    DXF_ERROR ReadEntitiesFromFile(CDXFReader &reader, CDXFBlockTable *pTable)
    {
        CDXFBlockTableRecord *pRecord = pTable->GetBlockTableRecord(DXF_MODEL_SPACE);
        if (pRecord) {
            return dxf_block_record_file_util::ReadEntitiesFromFile(reader, pRecord);
        }

        return DXF_OK;
    }
};