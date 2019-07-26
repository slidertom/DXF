#ifndef __DXF_SOLID_FILE_UTIL_H__
#define __DXF_SOLID_FILE_UTIL_H__
#pragma once

class CDXFSolid;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_solid_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFSolid *pSolid);
    void WriteToFile(CDXFWriter &writer, const CDXFSolid &solid);
};

#endif