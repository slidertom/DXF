#ifndef __DXF_HATCH_POLYBOUND_FILE_UTIL_H__
#define __DXF_HATCH_POLYBOUND_FILE_UTIL_H__
#pragma once

class CDXFHatchPolylineBoundary;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_hatch_polybound_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatchPolylineBoundary *pHatch);
    void WriteToFile(CDXFWriter &writer, const CDXFHatchPolylineBoundary &hatch);
};

#endif