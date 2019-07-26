#ifndef __DXF_HATCH_FILE_UTIL_H__
#define __DXF_HATCH_FILE_UTIL_H__
#pragma once

class CDXFHatch;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_hatch_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatch *pHatch);
    DXF_ERROR ReadBoundariesFromFile(CDXFReader &reader, CDXFHatch *pHatch, int32_t nBoundaryCnt);
    void WriteToFile(CDXFWriter &writer, const CDXFHatch &hatch);
};

#endif