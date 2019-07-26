#include "stdafx.h"
#include "dxf_rotateddim_file_util.h"

#include "dxf_dim_file_util.h"

#include "DXFDatabase.h"
#include "entity/DXFRotatedDimension.h"

#include "Utils/DXFMath.h"

#include "DXFWriter.h"

namespace dxf_rotated_dim_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFRotatedDimension *pDim)
    {
        const DXF_ERROR eError = dxf_dim_file_util::ReadFromFile(reader, pDim);

        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0 && item.m_nGroupCode != 1001)
        {
            switch (item.m_nGroupCode)
            {
            case 13:
                pDim->m_defPoint1.x = item.m_resDouble;
                break;
            case 23:
                pDim->m_defPoint1.y = item.m_resDouble;
                break;
            case 33:
                pDim->m_defPoint1.z = item.m_resDouble;
                break;
            case 14:
                pDim->m_defPoint2.x = item.m_resDouble;
                break;
            case 24:
                pDim->m_defPoint2.y = item.m_resDouble;
                break;
            case 34:
                pDim->m_defPoint2.z = item.m_resDouble;
                break;
            case 50:
                pDim->SetRotation(item.m_resDouble * DXF_PI / 180.0);
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();

        dxf_dim_file_util::PostReadFromFile(reader);

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFRotatedDimension &dim, CDXFDatabase *pDB)
    {
        dxf_dim_file_util::WriteTofile(writer, dim);

        writer.Write("  100\nAcDbAlignedDimension\n");

        writer.WritePoint(dim.m_defPoint1, 13, 23, 33);
        writer.WritePoint(dim.m_defPoint2, 14, 24, 34);
        writer.WriteFloatingPoint(dim.GetRotation() * 180.0 / DXF_PI, 50);

        writer.Write("  100\nAcDbRotatedDimension\n");

        dxf_dim_file_util::PostWriteToFile(writer, dim, pDB);
    }
};