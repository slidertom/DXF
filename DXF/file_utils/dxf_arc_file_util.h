#ifndef __DXF_ARC_FILE_UTIL_H__
#define __DXF_ARC_FILE_UTIL_H__
#pragma once

class CDXFArc;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_arc_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFArc *pArc);
    void WriteToFile(CDXFWriter &writer, const CDXFArc &arc);
};

#endif