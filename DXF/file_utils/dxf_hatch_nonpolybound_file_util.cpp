#include "stdafx.h"
#include "dxf_hatch_nonpolybound_file_util.h"

#include "entity/DXFHatch.h"

#include "DXFWriter.h"

namespace dxf_hatch_nonpoly_file_util
{
    namespace internal
    {
        DXF_ERROR LoadCoord(CDXFReader &reader, CDXFReaderItem &item, int32_t nCode, double &dDest)
        {
            reader.ReadItem(item);					
            if (item.m_nGroupCode != nCode) {
                return DXF_ERROR_BAD_GROUPCODE;
            }
            
            dDest = item.m_resDouble;
            return DXF_OK;
        }

        DXF_ERROR LoadValue(CDXFReader &reader, CDXFReaderItem &item, int32_t nCode, double &dDest)
        {
            reader.ReadItem(item);					
            if (item.m_nGroupCode != nCode) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            dDest = item.m_resDouble;
            return DXF_OK;
        }

        DXF_ERROR LoadValue(CDXFReader &reader, CDXFReaderItem &item, int32_t nCode, int32_t &nDest) 
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != nCode) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            nDest = item.m_resInt;
            return DXF_OK;
        }
    }

    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatchNonPolylineBoundary *pHatch)
    {
        CDXFReaderItem item;

        pHatch->m_pBand->Clear();

        reader.ReadItem(item);
        if (item.m_nGroupCode != 93) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        int32_t nEdgesCnt = item.m_resInt;
        pHatch->m_pBand->resize(nEdgesCnt);

        int32_t nType;
        for (auto i = 0; i < nEdgesCnt; ++i)
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != 72) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

            nType = item.m_resInt;

            switch (nType)
            {
            case 1:
            {
                CDXFBoundaryEdgeLine *pSeg = new CDXFBoundaryEdgeLine;

                DXF_ERROR eError = internal::LoadCoord(reader, item, 10, pSeg->m_startPoint.m_x);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadCoord(reader, item, 20, pSeg->m_startPoint.m_y);
                
                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadCoord(reader, item, 11, pSeg->m_endPoint.m_x);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadCoord(reader, item, 21, pSeg->m_endPoint.m_y);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                (*pHatch->m_pBand)[i] = pSeg;
                break;
            }
            case 2:
            {
                CDXFBoundaryEdgeArc *pSeg = new CDXFBoundaryEdgeArc;
                DXF_ERROR eError = internal::LoadValue(reader, item, 10, pSeg->m_centerPoint.m_x);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 20, pSeg->m_centerPoint.m_y);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 40, pSeg->m_dRadius);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 50, pSeg->m_dStartAngle);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 51, pSeg->m_dEndAngle);
                
                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 73, pSeg->m_nCounterclockwise);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                (*pHatch->m_pBand)[i] = pSeg;
                break;
            }
            case 3:
            {
                CDXFBoundaryEdgeEllipse *pSeg = new CDXFBoundaryEdgeEllipse;
                DXF_ERROR eError = internal::LoadValue(reader, item, 10, pSeg->m_centerPoint.m_x);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 20, pSeg->m_centerPoint.m_y);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 11, pSeg->m_endPoint.m_x);
                
                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 21, pSeg->m_endPoint.m_y);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 40, pSeg->m_dMinorAxisLength);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 50, pSeg->m_dStartAngle);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 51, pSeg->m_dEndAngle);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 73, pSeg->m_nCounterclockwise);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                (*pHatch->m_pBand)[i] = pSeg;

                break;
            }
            case 4:
            {
                CDXFBoundaryEdgeSpline *pSeg = new CDXFBoundaryEdgeSpline;

                DXF_ERROR eError = internal::LoadValue(reader, item, 94, pSeg->m_dDegree);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 73, pSeg->m_dRational);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                eError = internal::LoadValue(reader, item, 74, pSeg->m_dPeriodic);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                int32_t nNumberOfKnots(0);
                eError = internal::LoadValue(reader, item, 95, nNumberOfKnots);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                int32_t nNumberOfControlPoints(0);
                eError = internal::LoadValue(reader, item, 96, nNumberOfControlPoints);

                if (eError != DXF_OK) {
                    delete pSeg;
                    return eError;
                }

                std::vector<double> dKnotsArray;
                dKnotsArray.resize(nNumberOfKnots);
                std::vector<CDXFSplineControlPoint> controlPointsArray;
                controlPointsArray.resize(nNumberOfControlPoints);

                int32_t	j;
                
                for (j = 0; j < nNumberOfKnots; j++)
                {
                    eError = internal::LoadValue(reader, item, 40, dKnotsArray[j]);
                    if (eError != DXF_OK) {
                        delete pSeg;
                        return eError;
                    }
                }
                for (j = 0; j < nNumberOfControlPoints; j++)
                {
                    eError = internal::LoadValue(reader, item, 10, controlPointsArray[j].m_point.m_x);
                    
                    if (eError != DXF_OK) {
                        delete pSeg;
                        return eError;
                    }

                    eError = internal::LoadValue(reader, item, 20, controlPointsArray[j].m_point.m_y);

                    if (eError != DXF_OK) {
                        delete pSeg;
                        return eError;
                    }

                    reader.ReadItem(item);
                    if (item.m_nGroupCode == 42)
                    {
                        controlPointsArray[j].m_dWeight = item.m_resDouble;
                    }
                    else
                    {
                        controlPointsArray[j].m_dWeight = 1;
                        reader.PushBackItem();
                    }

                }

                pSeg->m_knotValuesArray = dKnotsArray;
                pSeg->m_controlPointsArray = controlPointsArray;

                (*pHatch->m_pBand)[i] = pSeg;
                break;
            }
            default:
                break;
            }
        }

        return DXF_OK;
    }

    class CDXFWriteBoundaryEdgeVis : public CDXFBoundaryEdgeVis
    {
    // Construction/Destruction
    public:
        CDXFWriteBoundaryEdgeVis(CDXFWriter &writer) : m_writer(writer) { }
        virtual ~CDXFWriteBoundaryEdgeVis() { }

    // Overrides
    public:
        virtual void OnVisitEdgeArc(CDXFBoundaryEdgeArc &edge) override
        {
            m_writer.WriteNumber(2, 72);
            m_writer.WriteFloatingPoint(edge.m_centerPoint.m_x, 10);
            m_writer.WriteFloatingPoint(edge.m_centerPoint.m_y, 20);
            m_writer.WriteFloatingPoint(edge.m_dRadius, 40);
            m_writer.WriteFloatingPoint(edge.m_dStartAngle, 50);
            m_writer.WriteFloatingPoint(edge.m_dEndAngle, 51);
            m_writer.WriteNumber(edge.m_nCounterclockwise, 73);
        }

        virtual void OnVisitEdgeLine(CDXFBoundaryEdgeLine &edge) override
        {
            m_writer.WriteNumber(1, 72);
            m_writer.WriteFloatingPoint(edge.m_startPoint.m_x, 10);
            m_writer.WriteFloatingPoint(edge.m_startPoint.m_y, 20);
            m_writer.WriteFloatingPoint(edge.m_endPoint.m_x, 11);
            m_writer.WriteFloatingPoint(edge.m_endPoint.m_y, 21);
        }

        virtual void OnVisitEdgeEllipse(CDXFBoundaryEdgeEllipse &edge) override
        {
            m_writer.WriteNumber(3, 72);
            m_writer.WriteFloatingPoint(edge.m_centerPoint.m_x, 10);
            m_writer.WriteFloatingPoint(edge.m_centerPoint.m_y, 20);
            m_writer.WriteFloatingPoint(edge.m_endPoint.m_x, 10);
            m_writer.WriteFloatingPoint(edge.m_endPoint.m_y, 20);
            m_writer.WriteFloatingPoint(edge.m_dMinorAxisLength, 40);
            m_writer.WriteFloatingPoint(edge.m_dStartAngle, 50);
            m_writer.WriteFloatingPoint(edge.m_dEndAngle, 51);
            m_writer.WriteNumber(edge.m_nCounterclockwise, 73);
        }

        virtual void OnVisitEdgeSpline(CDXFBoundaryEdgeSpline &edge) override
        {
            m_writer.WriteNumber(4, 72);
            m_writer.WriteFloatingPoint(edge.m_dDegree, 94);
            m_writer.WriteFloatingPoint(edge.m_dRational, 73);
            m_writer.WriteFloatingPoint(edge.m_dPeriodic, 74);
            m_writer.WriteNumber((int32_t)edge.m_knotValuesArray.size(), 95);
            m_writer.WriteNumber((int32_t)edge.m_controlPointsArray.size(), 96);

            for (auto dKnot : edge.m_knotValuesArray) {
                m_writer.WriteFloatingPoint(dKnot, 40);
            }

            for (const auto& controlPoint : edge.m_controlPointsArray) 
            {
                m_writer.WriteFloatingPoint(controlPoint.m_point.m_x, 10);
                m_writer.WriteFloatingPoint(controlPoint.m_point.m_y, 20);
                if (controlPoint.m_dWeight != 1) {
                    m_writer.WriteFloatingPoint(controlPoint.m_dWeight, 42);
                }
            }
        }

    private:
        CDXFWriter &m_writer;
    };

    void WriteToFile(CDXFWriter &writer, const CDXFHatchNonPolylineBoundary &hatch)
    {
        const size_t nEdgesCnt = hatch.m_pBand->size();
        writer.WriteText(std::to_string(nEdgesCnt).c_str(), 93);

        CDXFWriteBoundaryEdgeVis visitor(writer);
        for (auto* pBase : *hatch.m_pBand) {
		    if (!pBase) {
                continue;
            }
            pBase->Accept(visitor);
	    }
    }
};