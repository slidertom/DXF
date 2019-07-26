#include "stdafx.h"
#include "dxf_polyline_file_util.h"

#include "dxf_entity_file_util.h"
#include "DXFDatabase.h"
#include "entity/DXFPolyline.h"

#include "Utils/DXFMath.h"

#include "DXFWriter.h"

namespace dxf_polyline_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFPolyline *pPoly)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pPoly);
        if (eError != DXF_OK) {
            return eError;
        }

        bool bContinue = true;
        int32_t nNumVertices = 0;

        CDXFGeBulgePoint3D vertex;

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0 && bContinue)
        {
            switch (item.m_nGroupCode)
            {
            case 90:
                nNumVertices = item.m_resInt;
                break;
            case 70:
                pPoly->SetClosed(item.m_resInt == 1);//1=closed
                break;
            case 43:
                pPoly->SetConstantWidth(item.m_resDouble);
                break;
            case 10:
            {
                reader.PushBackItem();
                bContinue = false;
            }
            break;
            }

            if (bContinue) {
                reader.ReadItem(item);
            }
        }

        for (auto i = 0; i < nNumVertices; ++i)
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != 10) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            vertex.m_point.x = item.m_resDouble;

            reader.ReadItem(item);
            if (item.m_nGroupCode != 20) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            vertex.m_point.y = item.m_resDouble;

            reader.ReadItem(item);
            if (item.m_nGroupCode == 40) {
                // Do nothing (at the moment)
            }
            else {
                reader.PushBackItem();
            }

            reader.ReadItem(item);
            if (item.m_nGroupCode == 41) {
                // Do nothing (at the moment)
            }
            else {
                reader.PushBackItem();
            }

            reader.ReadItem(item);
            if (item.m_nGroupCode == 42) {
                vertex.m_dBulge = item.m_resDouble;
            }
            else {
                reader.PushBackItem();
                vertex.m_dBulge = 0.0;
            }

            pPoly->m_vertices.push_back(vertex);
        }

        reader.ReadItem(item);
        while (item.m_nGroupCode != 0) {
            reader.ReadItem(item);
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFPolyline &poly)
    {
        dxf_entity_file_util::WriteToFile(writer, poly);

        writer.Write("  100\nAcDbPolyline\n");
        writer.WriteText(std::to_string(poly.m_vertices.size()).c_str(), 90);
        writer.WriteNumber(poly.IsClosed() ? 1 : 0, 70);
        writer.WriteFloatingPoint(poly.GetConstantWidth(), 43);

        for (const auto &vertex : poly.m_vertices)
        {
            writer.Write2DPoint(vertex.m_point, 10, 20);
            if ( !dxf_math::eq0(vertex.m_dBulge) ) {
                writer.WriteFloatingPoint(vertex.m_dBulge, 42);
            }
        }
    }
};