#ifndef __DXF_TEXT_FILE_UTIL_H__
#define __DXF_TEXT_FILE_UTIL_H__
#pragma once

class CDXFText;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_text_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFText *pText);
    void WriteToFile(CDXFWriter &writer, const CDXFText &text);
};

#endif