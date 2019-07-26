#include "stdafx.h"
#include "dxf_block_end_file_util.h"

#include "dxf_entity_file_util.h"
#include "entity/DXFEntity.h"

#include "DXFDatabase.h"
#include "entity/DXFBlockEnd.h"

#include "DXFWriter.h"

namespace dxf_block_end_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFEntity *pBlockEnd)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pBlockEnd);

        if (eError != DXF_OK) {
            return eError;
        }


        CDXFReaderItem item;
        reader.ReadItem(item);
        if (item.m_nGroupCode != 100) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        if (0 != ::strcmpi(item.m_resString, "AcDbBlockEnd")) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFBlockEnd &blockend)
    {
        dxf_entity_file_util::WriteToFile(writer, blockend);
        writer.Write("  100\nAcDbBlockEnd\n");
    }
};