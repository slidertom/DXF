#ifndef __DXF_DATABASE_FILE_UTIL_H__
#define __DXF_DATABASE_FILE_UTIL_H__
#pragma once

class CDXFDatabase;
class CDXFLayerTable;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_database_file_util
{
    DXF_ERROR OpenDatabase(const wchar_t *sFileName, CDXFDatabase *pDatabase);
    bool SaveDatabase(const wchar_t *sFileName, CDXFDatabase *pDatabase);
    void WriteHeaderToFile(CDXFWriter &writer, const CDXFDatabase *pDatabase);
    DXF_ERROR ReadTablesFromFile(CDXFReader &reader, CDXFDatabase *pDatabase);
    void ReadClassesFromFile(CDXFReader &reader, CDXFDatabase *pDatabase);
};

#endif