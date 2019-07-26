#include "stdafx.h"
#include "dxf_line_file_util.h"

#include "dxf_entity_file_util.h"

#include "DXFDatabase.h"
#include "entity/DXFLine.h"    

#include "DXFWriter.h"

namespace dxf_line_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFLine *pLine)
    {
        ASSERT(reader.GetDatabase());

        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pLine);

        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 10:
                pLine->m_pointStart.x = item.m_resDouble;
                break;
            case 20:
                pLine->m_pointStart.y = item.m_resDouble;
                break;
            case 30:
                pLine->m_pointStart.z = item.m_resDouble;
                break;
            case 11:
                pLine->m_pointEnd.x = item.m_resDouble;
                break;
            case 21:
                pLine->m_pointEnd.y = item.m_resDouble;
                break;
            case 31:
                pLine->m_pointEnd.z = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFLine &line)
    {
        dxf_entity_file_util::WriteToFile(writer, line);

        writer.Write("  100\nAcDbLine\n");

        writer.WritePoint(line.m_pointStart, 10, 20, 30);
        writer.WritePoint(line.m_pointEnd, 11, 21, 31);
    }
};