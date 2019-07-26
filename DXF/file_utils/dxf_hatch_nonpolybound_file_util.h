#ifndef __DXF_HATCH_NONPOLYBOUND_FILE_UTIL_H__
#define __DXF_HATCH_NONPOLYBOUND_FILE_UTIL_H__
#pragma once

class CDXFHatchNonPolylineBoundary;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_hatch_nonpoly_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatchNonPolylineBoundary *pHatch);
    void WriteToFile(CDXFWriter &writer, const CDXFHatchNonPolylineBoundary &hatch);
};

#endif