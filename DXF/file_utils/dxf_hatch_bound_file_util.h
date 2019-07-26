#ifndef __DXF_HATCH_BOUND_FILE_UTIL_H__
#define __DXF_HATCH_BOUND_FILE_UTIL_H__
#pragma once

class CDXFHatchBoundary;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_hatch_bound_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatchBoundary *pHatch);
    void PreWriteToFile(CDXFWriter &writer, CDXFHatchBoundary &hatch);
};

#endif