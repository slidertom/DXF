#ifndef __DXF_CIRCLE_FILE_UTIL_H__
#define __DXF_CIRCLE_FILE_UTIL_H__
#pragma once

class CDXFCircle;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_circle_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFCircle *pCircle);
    void WriteToFile(CDXFWriter &writer, const CDXFCircle &circle);
};

#endif