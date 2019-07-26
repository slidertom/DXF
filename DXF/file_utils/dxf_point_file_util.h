#ifndef __DXF_POINT_FILE_UTIL_H__
#define __DXF_POINT_FILE_UTIL_H__
#pragma once

class CDXFPoint;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_point_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFPoint *pPt);
    void WriteToFile(CDXFWriter &writer, const CDXFPoint &pt);
};

#endif