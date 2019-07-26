#ifndef __DXF_BLOCK_BEGIN_FILE_UTIL_H__
#define __DXF_BLOCK_BEGIN_FILE_UTIL_H__
#pragma once

class CDXFBlockBegin;
class CDXFDatabase;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_block_begin_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFBlockBegin *pBlockBegin);
    void WriteToFile(CDXFWriter &writer, const CDXFBlockBegin &blockbegin, CDXFDatabase *pDatabase);
};

#endif