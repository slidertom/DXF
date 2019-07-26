#include "stdafx.h"
#include "dxf_point_file_util.h"

#include "dxf_entity_file_util.h"
#include "entity/DXFPoint.h"
#include "DXFDatabase.h"

#include "DXFWriter.h"

namespace dxf_point_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFPoint *pPt)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pPt);
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
                pPt->m_point.x = item.m_resDouble;
                break;
            case 20:
                pPt->m_point.y = item.m_resDouble;
                break;
            case 30:
                pPt->m_point.z = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFPoint &pt)
    {
        dxf_entity_file_util::WriteToFile(writer, pt);

        writer.Write("  100\nAcDbPoint\n");

        writer.WritePoint(pt.m_point, 10, 20, 30);
    }
};