#ifndef __DXF_MTEXT_FILE_UTIL_H__
#define __DXF_MTEXT_FILE_UTIL_H__
#pragma once

class CDXFMText;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_mtext_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFMText *pText);
    void WriteToFile(CDXFWriter &writer, const CDXFMText &text);
};

#endif