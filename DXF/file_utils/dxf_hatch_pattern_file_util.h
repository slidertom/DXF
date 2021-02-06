#ifndef __DXF_HATCH_PATTERN_FILE_UTIL_H__
#define __DXF_HATCH_PATTERN_FILE_UTIL_H__
#pragma once

#ifndef __DXF_HATCH_H__
    #include "../entity/DXFHatch.h"
#endif

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

class CDXFWriter;

namespace dxf_hatch_pattern_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFHatch::CDXFHatchPattern &pattern);
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatch::CDXFHatchPattern &pattern);
}
#endif