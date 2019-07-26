#include "stdafx.h"
#include "dxf_solid_file_util.h"

#include "dxf_entity_file_util.h"

#include "entity/DXFSolid.h"
#include "DXFDatabase.h"

#include "DXFWriter.h"

namespace dxf_solid_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFSolid *pSolid)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pSolid);

        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        CDXF3DPoint points[4];
        int32_t nNumPoints = 3;

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 10:
            case 11:
            case 12:
                points[item.m_nGroupCode % 10].x = item.m_resDouble;
                break;
            case 13:
                nNumPoints = 4;
                points[item.m_nGroupCode % 10].x = item.m_resDouble;
                break;
            case 20:
            case 21:
            case 22:
            case 23:
                points[item.m_nGroupCode % 10].y = item.m_resDouble;
                break;
            case 30:
            case 31:
            case 32:
            case 33:
                points[item.m_nGroupCode % 10].z = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        pSolid->SetNumPoints(nNumPoints);
        for (int32_t i = 0; i < nNumPoints; ++i) {
            pSolid->SetPointAt(i, points[i]);
        }
       
        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFSolid &solid)
    {
        dxf_entity_file_util::WriteToFile(writer, solid);

        writer.Write("  100\nAcDbTrace\n");

        CDXF3DPoint points[3];
        for (int32_t i = 0; i < 3; ++i) {
            points[i] = solid.GetPoint(i);
            writer.WritePoint(points[i], 10 + i, 20 + i, 30 + i);
        }

        if (solid.GetNumPoints() > 3)
        {
            CDXF3DPoint &point = solid.GetPoint(3);
            writer.WritePoint(point, 13, 23, 33);
        }
        else
        {
            writer.WritePoint(points[2], 13, 23, 33);
        }
    }
};