#include "stdafx.h"
#include "dxf_block_begin_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_table_record_file_util.h"

#include "entity/DXFBlockBegin.h"
#include "DXFDatabase.h"
#include "tables/DXFBlockTableRecord.h"

#include "DXFWriter.h"

namespace dxf_block_begin_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFBlockBegin *pBlockBegin)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, (CDXFEntity *)pBlockBegin);
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        bool bContinue = true;

        reader.ReadItem(item);
        if (item.m_nGroupCode != 100) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmpi(item.m_resString, "AcDbBlockBegin") != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        double dptX = 0.0;
        double dptY = 0.0;
        double dptZ = 0.0;

        while (bContinue)
        {
            reader.ReadItem(item);
            switch (item.m_nGroupCode)
            {
            case 2:
                pBlockBegin->SetBlockName(item.m_resString);
                break;
            case 70:
                pBlockBegin->SetFlags(item.m_resInt);
                break;
            case 10:
                dptX = item.m_resDouble;
                break;
            case 20:
                dptY = item.m_resDouble;
                break;
            case 30:
                dptZ = item.m_resDouble;
                break;
            case 3:
                break;
            case 1:
                pBlockBegin->SetXRefPathName(item.m_resString);
                break;
            case 4:
                pBlockBegin->SetDescr(item.m_resString);
                break;
            default:
                bContinue = false;
                break;
            }
        }

        pBlockBegin->SetBasePoint(CDXF3DPoint(dptX, dptY, dptZ));
        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFBlockBegin &blockbegin, CDXFDatabase *pDatabase)
    {
        dxf_entity_file_util::WriteToFile(writer, blockbegin);

        writer.Write("  100\nAcDbBlockBegin\n");

        CDXFBlockTableRecord *pBlock = pDatabase->GetBlockTable()->GetBlockTableRecord(blockbegin.GetBlockName());
        std::string sTempName = pBlock->GetName();

        writer.WriteText(sTempName.c_str(), 2);
        writer.WriteNumber(blockbegin.GetFlags(), 70);
        writer.WritePoint(blockbegin.GetBasePoint(), 10, 20, 30);
        writer.WriteText(sTempName.c_str(), 3);
        writer.WriteText(blockbegin.GetXRefPathName(), 1);

        const char *sDescr = blockbegin.GetDescr();
        if ( 0 != strlen(sDescr) ) {
            writer.WriteText(sDescr, 4);
        }
    }
};