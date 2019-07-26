#ifndef __DXF_TABLE_RECORD_FILE_UTIL_H__
#define __DXF_TABLE_RECORD_FILE_UTIL_H__
#pragma once

class CDXFSymbolTableRecord;
class CDXFViewportTableRecord;
class CDXFLineTypeTableRecord;
class CDXFLayerTableRecord;
class CDXFTextStyleTableRecord;
class CDXFViewTableRecord;
class CDXFUCSTableRecord;
class CDXFAppIDTableRecord;
class CDXFDimStyleTableRecord;
class CDXFBlockTableRecord;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_table_record_file_util
{
    void ReadFromFileSymbolTableRecord(CDXFReader &reader, CDXFSymbolTableRecord *pRecord);
    DXF_ERROR PreReadFromFileSymbolTableRecord(CDXFReader &reader, CDXFSymbolTableRecord *pRecord);
    void ReadFromFileViewReportRecord(CDXFReader &reader, CDXFViewportTableRecord *pRecord);
    void ReadFromFileLineTypeRecord(CDXFReader &reader, CDXFLineTypeTableRecord *pRecord);
    DXF_ERROR ReadFromFileLayerRecord(CDXFReader &reader, CDXFLayerTableRecord *pRecord);
    void ReadFromFileTextStyleRecord(CDXFReader &reader, CDXFTextStyleTableRecord *pRecord);
    void ReadFromFileViewRecord(CDXFReader &reader, CDXFViewTableRecord *pRecord);
    void ReadFromFileUCSRecord(CDXFReader &reader, CDXFUCSTableRecord *pRecord);
    void ReadFromFileAppIdRecord(CDXFReader &reader, CDXFAppIDTableRecord *pRecord);
    void ReadFromFileDimStyleRecord(CDXFReader &reader, CDXFDimStyleTableRecord *pRecord);
    void ReadFromFileDimBlockRecord(CDXFReader &reader, CDXFBlockTableRecord *pRecord);

    void WriteToFileSymbolTableRecord(CDXFWriter &writer, const CDXFSymbolTableRecord &record);
    void PreWriteToFileSymbolTableRecord(CDXFWriter &writer, const CDXFSymbolTableRecord &record);
    void WriteToFileViewReportRecord(CDXFWriter &writer, const CDXFViewportTableRecord &record);
    void WriteToFileLineTypeRecord(CDXFWriter &writer, const CDXFLineTypeTableRecord &record);
    void WriteToFileLayerRecord(CDXFWriter &writer, const CDXFLayerTableRecord &record);
    void WriteToFileTextStyleRecord(CDXFWriter &writer, const CDXFTextStyleTableRecord &record);
    void WriteToFileViewRecord(CDXFWriter &writer, const CDXFViewTableRecord &record);
    void WriteToFileUCSRecord(CDXFWriter &writer, const CDXFUCSTableRecord &record);
    void WriteToFileAppIdRecord(CDXFWriter &writer, const CDXFAppIDTableRecord &record);
    void WriteToFileDimStyleRecord(CDXFWriter &writer, CDXFDimStyleTableRecord &record, CDXFDatabase *pDB);
    void WriteToFileDimBlockRecord(CDXFWriter &writer, const CDXFBlockTableRecord &record);
};

#endif