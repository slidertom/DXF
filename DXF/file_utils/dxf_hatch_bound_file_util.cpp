#include "stdafx.h"
#include "dxf_hatch_bound_file_util.h"

#include "entity/DXFHatch.h"

#include "dxf_hatch_polybound_file_util.h"
#include "dxf_hatch_nonpolybound_file_util.h"

#include "DXFWriter.h"

class CDXFHatchBoundaryWriterVisitor : public CDXFHatchBoundaryVis
{
// Construction/Destruction
public:
    CDXFHatchBoundaryWriterVisitor(CDXFWriter &writer) : m_writer(writer) { }
    virtual ~CDXFHatchBoundaryWriterVisitor() { }

// Overrides
public:
    virtual void OnVisitPolyLine(CDXFHatchPolylineBoundary *pBoundry) override {
        dxf_hatch_polybound_file_util::WriteToFile(m_writer, *pBoundry);
    }

    virtual void OnVisitNonPolyLine(CDXFHatchNonPolylineBoundary *pBoundry) override {
        dxf_hatch_nonpoly_file_util::WriteToFile(m_writer, *pBoundry);
    }

// Attributes
private:
    CDXFWriter &m_writer;
};

namespace dxf_hatch_bound_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatchBoundary *pHatch)
    {
        CDXFReaderItem item;
        reader.ReadItem(item);

        if (item.m_nGroupCode != 97) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        int32_t nNumBoundaryObjects = item.m_resInt;
        for (auto i = 0; i < nNumBoundaryObjects; ++i)
        {
            reader.ReadItem(item);
            if (item.m_nGroupCode != 330) {
                return DXF_ERROR_BAD_GROUPCODE;
            }
            //Do nothing
        }

        return DXF_OK;
    }

    void PreWriteToFile(CDXFWriter &writer, CDXFHatchBoundary &hatch)
    {
        writer.WriteNumber(hatch.m_nType, 92);
        CDXFHatchBoundaryWriterVisitor vis(writer);
        hatch.Accept(vis);
        writer.Write("  97\n0\n");
    }
};