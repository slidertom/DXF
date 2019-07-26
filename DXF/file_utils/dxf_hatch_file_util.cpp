#include "stdafx.h"
#include "dxf_hatch_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_hatch_bound_file_util.h"
#include "dxf_hatch_nonpolybound_file_util.h"
#include "dxf_hatch_polybound_file_util.h"

#include "DXFReader.h"
#include "DXFWriter.h"

#include "entity/DXFHatch.h"
#include "DXFDatabase.h"

#include "file_utils/dxf_hatch_pattern_file_util.h"

#include "Utils/DXFMath.h"

namespace dxf_hatch_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatch *pHatch)
    {
        ASSERT(reader.GetDatabase());

        DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pHatch);

        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        CDXF3DPoint seedPoint;
        CDXFHatch::CDXFHatchPattern pattern;

        reader.ReadItem(item);
        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pHatch->m_sPatternName = item.m_resString;
                break;
            case 70:
                pHatch->m_fSolidFill = item.m_resInt;
                break;
            case 71:
                pHatch->m_fAssociative = item.m_resInt;
                break;
            case 91:
            {
                eError = dxf_hatch_file_util::ReadBoundariesFromFile(reader, pHatch, item.m_resInt);
                if (eError != DXF_OK) {
                    return eError;
                }
            }
            break;
            case 75:
                pHatch->m_eStyle = (EDXFHatchStyle)item.m_resInt;
                break;
            case 76:
                pHatch->m_ePatternType = (EDXFHatchPatternType)item.m_resInt;
                break;
            case 78:
            {
                const int32_t nNumPatternLines = item.m_resInt;
                pHatch->m_patternLines.clear();

                for (auto i = 0; i < nNumPatternLines; i++) {
                    eError = dxf_hatch_pattern_file_util::ReadFromFile(reader, pattern);
                    if (eError != DXF_OK) {
                        return eError;
                    }
                    pHatch->m_patternLines.push_back(pattern);
                }
            }
            break;
            case 47:
                pHatch->m_dDensity = item.m_resDouble;
                break;
            case 98:
            {
                const int32_t nNumSeedPoints = item.m_resInt;
                pHatch->m_seedPoints.clear();

                for (int32_t i = 0; i < nNumSeedPoints; ++i)
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 10) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }
                       
                    seedPoint.x = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 20) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    seedPoint.y = item.m_resDouble;

                    pHatch->m_seedPoints.push_back(seedPoint);
                }
            }
            break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    DXF_ERROR ReadBoundariesFromFile(CDXFReader &reader, CDXFHatch *pHatch, int32_t nBoundaryCnt)
    {
        CDXFReaderItem item;
        CDXFHatchBoundary *pBoundary;
        int32_t nType;

        DXF_ERROR eError = DXF_OK;

        for (int32_t i = 0; i < nBoundaryCnt; ++i)
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != 92) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            nType = item.m_resInt;
            if (nType & eBoundaryTypePolyline)
            {
                pBoundary = new CDXFHatchPolylineBoundary(nType);
                eError = dxf_hatch_polybound_file_util::ReadFromFile(reader, (CDXFHatchPolylineBoundary *)pBoundary);
                if (eError != DXF_OK) {
                    delete pBoundary;
                    return eError;
                }
            }
            else
            {
                pBoundary = new CDXFHatchNonPolylineBoundary(nType);
                eError = dxf_hatch_nonpoly_file_util::ReadFromFile(reader, (CDXFHatchNonPolylineBoundary *)pBoundary);
                if (eError != DXF_OK) {
                    delete pBoundary;
                    return eError;
                }
            }

            eError = dxf_hatch_bound_file_util::ReadFromFile(reader, pBoundary);
            ASSERT(eError == DXF_OK);
            if (eError != DXF_OK) {
                delete pBoundary;
                return eError;
            }

            pHatch->m_boundaryPtrs.push_back(pBoundary);
        }

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFHatch &hatch)
    {
        dxf_entity_file_util::WriteToFile(writer, hatch);

        writer.Write("  100\nAcDbHatch\n");

        writer.Write("  10\n0.0\n  20\n0.0\n  30\n0.0\n");
        writer.Write("  210\n0.0\n  220\n0.0\n  230\n1.0\n");

        writer.WriteText(hatch.m_sPatternName.c_str(), 2);
        writer.WriteNumber(hatch.m_fSolidFill, 70);
        writer.WriteNumber(hatch.m_fAssociative, 71);
        writer.WriteText(std::to_string(hatch.m_boundaryPtrs.size()).c_str(), 91);

        {
            const size_t nCnt = hatch.m_boundaryPtrs.size();
            for (size_t i = 0; i < nCnt; ++i) {
                dxf_hatch_bound_file_util::PreWriteToFile(writer, *hatch.m_boundaryPtrs[i]);

            }
        }

        writer.WriteNumber(hatch.m_eStyle, 75);
        writer.WriteNumber(hatch.m_ePatternType, 76);

        if (!hatch.m_fSolidFill)
        {
            writer.WriteFloatingPoint(hatch.m_dPatternAngle, 52);
            writer.WriteFloatingPoint(hatch.m_dPatternScale, 41);
            writer.Write("  77\n0\n");

            if ( !hatch.m_patternLines.empty() ) {
                writer.WriteNumber((int32_t)hatch.m_patternLines.size(), 78);
                for (auto it : hatch.m_patternLines) {
                    dxf_hatch_pattern_file_util::WriteToFile(writer, it);
                }
            }
            else if (hatch.m_sPatternName == "LINE") {
                writer.Write("  78\n1\n");
                writer.WriteFloatingPoint(hatch.m_dPatternAngle, 53);
                writer.Write("  43\n0.0\n  44\n0.0\n");
                double x = -sin(hatch.m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * hatch.m_dPatternScale;
                double y = cos(hatch.m_dPatternAngle / 180.0 * DXF_PI) * /*3.175*/ 0.125 * hatch.m_dPatternScale;
                writer.WriteFloatingPoint(x, 45);
                writer.WriteFloatingPoint(y, 46);
                writer.Write("  79\n0\n");
            }
        }

        if (!dxf_math::eq0(hatch.m_dDensity)) {
            writer.WriteFloatingPoint(hatch.m_dDensity, 47);
        }

        if (!hatch.m_seedPoints.empty()) {
            writer.WriteNumber((int32_t)hatch.m_seedPoints.size(), 98);

            for (const auto &point : hatch.m_seedPoints) {
                writer.Write("  10\n%.15f\n  20\n%.15f\n", point.x, point.y);
            }
        }
        else {
            writer.Write("  98\n1\n");
            writer.Write("  10\n0.0\n  20\n0.0\n");
        }
    }
};