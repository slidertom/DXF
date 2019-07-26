#ifndef __DXF_SYMBOL_TABLE_FILE_UTIL_H__
#define __DXF_SYMBOL_TABLE_FILE_UTIL_H__
#pragma once

class CDXFSymbolTable;
class CDXFDimStyleTable;
class CDXFViewportTableRecord;
class CDXFLineTypeTableRecord;
class CDXFLayerTableRecord;
class CDXFTextStyleTableRecord;
class CDXFSymbolTableRecord;
class CDXFWriter;

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "tables/DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_RECORD_FILE_UTIL_H__
    #include "dxf_table_record_file_util.h"
#endif

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_symbol_table_file_util
{
    void ReadFromFileSymbolTable(CDXFReader &reader, CDXFSymbolTable *pTable);
    void ReadFromDimStyleTableFull(CDXFReader &reader, CDXFDimStyleTable *pTable);
    DXF_ERROR ReadFromFileTable(CDXFReader &reader, CDXFSymbolTable *pTable);
    void PreWriteFile(CDXFWriter &writer, CDXFSymbolTable &table, CDXFDatabase *pDB);
    void WriteToFile(CDXFWriter &writer, const CDXFSymbolTable &table);
    void WriteToDimStyleTable(CDXFWriter &writer, const CDXFDimStyleTable &table);
};

#endif