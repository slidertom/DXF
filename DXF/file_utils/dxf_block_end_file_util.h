#ifndef __DXF_BLOCK_END_FILE_UTIL_H__
#define __DXF_BLOCK_END_FILE_UTIL_H__
#pragma once

class CDXFEntity;
class CDXFBlockEnd;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_block_end_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFEntity *pBlockEnd);
    void WriteToFile(CDXFWriter &writer, const CDXFBlockEnd &blockend);
};

#endif