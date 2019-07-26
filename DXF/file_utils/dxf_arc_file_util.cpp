#include "stdafx.h"
#include "dxf_arc_file_util.h"

#include "dxf_entity_file_util.h"

#include "DXFDatabase.h"
#include "entity/DXFArc.h"

#include "DXFWriter.h"

namespace dxf_arc_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFArc *pArc)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pArc);

        if ( eError != DXF_OK ) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);
        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 10:
                pArc->m_ptCenter.x = item.m_resDouble;
                break;
            case 20:
                pArc->m_ptCenter.y = item.m_resDouble;
                break;
            case 30:
                pArc->m_ptCenter.z = item.m_resDouble;
                break;
            case 40:
                pArc->m_dRadius = item.m_resDouble;
                break;
            case 50:
                pArc->m_dStartAngle = item.m_resDouble;
                break;
            case 51:
                pArc->m_dEndAngle = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFArc &arc)
    {
        dxf_entity_file_util::WriteToFile(writer, arc);

        writer.Write("  100\nAcDbCircle\n");
        
        writer.WritePoint(arc.m_ptCenter, 10, 20, 30);
        writer.WriteFloatingPoint(arc.m_dRadius, 40);

        writer.Write("  100\nAcDbArc\n");

        writer.WriteFloatingPoint(arc.m_dStartAngle, 50);
        writer.WriteFloatingPoint(arc.m_dEndAngle, 51);
    }
};