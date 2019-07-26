#include "stdafx.h"
#include "dxf_circle_file_util.h"

#include "dxf_entity_file_util.h"

#include "DXFDatabase.h"
#include "entity/DXFCircle.h"

#include "DXFWriter.h"

namespace dxf_circle_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFCircle *pCircle)
    {
        ASSERT(reader.GetDatabase());

        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pCircle);
        if (eError != DXF_OK) {
            return eError;
        }

        CDXF3DPoint center_point;
        double dRadius = 0.;

        CDXFReaderItem item;
        reader.ReadItem(item);
        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 10:
                center_point.x = item.m_resDouble;
                break;
            case 20:
                center_point.y = item.m_resDouble;
                break;
            case 30:
                center_point.z = item.m_resDouble;
                break;
            case 40:
                dRadius = item.m_resDouble;
                break;
            }
            reader.ReadItem(item);
        }

        pCircle->SetCenterPoint(center_point);
        pCircle->SetRadius(dRadius);

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFCircle &circle)
    {
        dxf_entity_file_util::WriteToFile(writer, circle);

        writer.Write("  100\nAcDbCircle\n");

        const CDXF3DPoint &center_point = circle.GetCenterPoint();
        writer.WritePoint(center_point, 10, 20, 30);
        writer.WriteFloatingPoint(circle.GetRadius(), 40);
    }
};