#ifndef __DXF_LINE_FILE_UTIL_H__
#define __DXF_LINE_FILE_UTIL_H__
#pragma once

class CDXFLine;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_line_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFLine *pLine);
    void WriteToFile(CDXFWriter &writer, const CDXFLine &line);
};

#endif