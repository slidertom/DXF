#ifndef __DXF_BLOCK_TABLE_FILE_UTIL_H__
#define __DXF_BLOCK_TABLE_FILE_UTIL_H__
#pragma once

class CDXFBlockTable;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_block_table_file_util
{
    void WriteBlocksToFile(CDXFWriter &writer, const CDXFBlockTable &table, CDXFDatabase *pDB);
    void WriteEntitiesToFile(CDXFWriter &writer, const CDXFBlockTable &table, CDXFDatabase *pDB);
    DXF_ERROR ReadBlocksFromFile(CDXFReader &reader, CDXFBlockTable *pTable);
    DXF_ERROR ReadEntitiesFromFile(CDXFReader &reader, CDXFBlockTable *pTable);
};

#endif