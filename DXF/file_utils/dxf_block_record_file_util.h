#ifndef __DXF_BLOCK_RECORD_FILE_UTIL_H__
#define __DXF_BLOCK_RECORD_FILE_UTIL_H__
#pragma once

class CDXFBlockTableRecord;
class CDXFDatabase;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_block_record_file_util
{
    DXF_ERROR ReadEntitiesFromFile(CDXFReader &reader, CDXFBlockTableRecord *pBlock);
    DXF_ERROR ReadBlockFromFile(CDXFReader &reader, CDXFBlockTableRecord *pBlock);
    void WriteBlockToFile(CDXFWriter &writer, const CDXFBlockTableRecord &blockbegin, CDXFDatabase *pDB);
    void WriteEntititiesToFile(CDXFWriter &writer, const CDXFBlockTableRecord &blockbegin, CDXFDatabase *pDB);
};

#endif