#include "stdafx.h"
#include "dxf_hatch_polybound_file_util.h"

#include "entity/DXFHatch.h"

#include "DXFWriter.h"

namespace dxf_hatch_polybound_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatchPolylineBoundary *pHatch)
    {
        CDXFGeBulgePoint2D vertex;

        CDXFReaderItem item;
        reader.ReadItem(item);

        if (item.m_nGroupCode != 72) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        pHatch->m_nHasBulge = item.m_resInt;

        reader.ReadItem(item);
        if (item.m_nGroupCode != 73) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        pHatch->m_nClosed = item.m_resInt;

        reader.ReadItem(item);
        if (item.m_nGroupCode != 93) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        int32_t nVerticesCnt = item.m_resInt;

        pHatch->m_vertices.clear();
        pHatch->m_vertices.resize(nVerticesCnt);

        for (int32_t i = 0; i < nVerticesCnt; i++)
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != 10) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            vertex.m_point.m_x = item.m_resDouble;

            reader.ReadItem(item);
            if (item.m_nGroupCode != 20) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            vertex.m_point.m_y = item.m_resDouble;

            reader.ReadItem(item);
            if (item.m_nGroupCode == 42)
            {
                vertex.m_dBulge = item.m_resDouble;
            }
            else
            {
                vertex.m_dBulge = 0.0;
                reader.PushBackItem();
            }
            pHatch->m_vertices[i] = vertex;
        }

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFHatchPolylineBoundary &hatch)
    {
        writer.WriteNumber(hatch.m_nHasBulge, 72);
        writer.WriteNumber(hatch.m_nClosed, 73);
        size_t nVerticesCnt = hatch.m_vertices.size();
        writer.WriteText(std::to_string(nVerticesCnt).c_str(), 93);

        if (hatch.m_nHasBulge)
        {
            for (size_t i = 0; i < nVerticesCnt; ++i)
            {
                const CDXFGeBulgePoint2D *pVertex = &hatch.m_vertices[i];
                writer.WriteFloatingPoint(pVertex->m_point.m_x, 10);
                writer.WriteFloatingPoint(pVertex->m_point.m_y, 20);
                writer.WriteFloatingPoint(pVertex->m_dBulge, 42);
            }
        }
        else
        {
            for (size_t i = 0; i < nVerticesCnt; i++)
            {
                const CDXFGeBulgePoint2D *pVertex = &hatch.m_vertices[i];
                writer.WriteFloatingPoint(pVertex->m_point.m_x, 10);
                writer.WriteFloatingPoint(pVertex->m_point.m_y, 20);
            }
        }
    }
};