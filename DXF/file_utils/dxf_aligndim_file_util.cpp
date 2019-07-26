#include "stdafx.h"
#include "dxf_aligndim_file_util.h"

#include "dxf_dim_file_util.h"

#include "DXFDatabase.h"
#include "entity/DXFAlignedDimension.h"

#include "DXFWriter.h"

namespace dxf_align_dim_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFAlignedDimension *pDim)
    {
        const DXF_ERROR eError = dxf_dim_file_util::ReadFromFile(reader, pDim);
        
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;

        reader.ReadItem(item);

        CDXF3DPoint defpoint1;
        CDXF3DPoint defpoint2;

        while (item.m_nGroupCode != 0 && item.m_nGroupCode != 1001)
        {
            switch (item.m_nGroupCode)
            {
            case 13:
                defpoint1.x = item.m_resDouble;
                break;
            case 23:
                defpoint1.y = item.m_resDouble;
                break;
            case 33:
                defpoint1.z = item.m_resDouble;
                break;
            case 14:
                defpoint2.x = item.m_resDouble;
                break;
            case 24:
                defpoint2.y = item.m_resDouble;
                break;
            case 34:
                defpoint2.z = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        pDim->SetFirstPoint(defpoint1);
        pDim->SetSecondPoint(defpoint2);

        reader.PushBackItem();

        dxf_dim_file_util::PostReadFromFile(reader);

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFAlignedDimension &dim, CDXFDatabase *pDatabase)
    {
        dxf_dim_file_util::WriteTofile(writer, dim);

        writer.Write("  100\nAcDbAlignedDimension\n");

        const CDXF3DPoint &defPoint1 = dim.GetFirstPoint();
        const CDXF3DPoint &defPoint2 = dim.GetSecondPoint();

        writer.WritePoint(defPoint1, 13, 23, 33);
        writer.WritePoint(defPoint2, 14, 24, 34);

        /*CDXFVector vector1 = m_defPoint2 - m_defPoint1;
        CDXFVector vector2(1, 0, 0);

        double dCosine = (vector1 * vector2) / vector1.Length();
        double dAngle = acos(dCosine);
        dAngle = dAngle * 180.0 / DXF_PI;


        fprintf(pFile, "  50\n%f\n", dAngle);*/

        dxf_dim_file_util::PostWriteToFile(writer, dim, pDatabase);
    }
};