#ifndef __DXF_POLYLINE_FILE_UTIL_H__
#define __DXF_POLYLINE_FILE_UTIL_H__
#pragma once

class CDXFPolyline;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_polyline_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFPolyline *pPoly);
    void WriteToFile(CDXFWriter &writer, const CDXFPolyline &poly);
};

#endif