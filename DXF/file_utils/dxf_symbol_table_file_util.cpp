#include "stdafx.h"
#include "dxf_symbol_table_file_util.h"

#include "dxf_table_record_file_util.h"
#include "dxf_obj_file_util.h"

#include "base/DXFObjectID.h"
#include "DXFDatabase.h"
#include "base/DXFObject.h"

#include "tables/DXFSymbolTableRecord.h"
#include "tables/DXFLineTypeTableRecord.h"
#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFTextStyleTableRecord.h"
#include "tables/DXFViewTableRecord.h"
#include "tables/DXFBlockTableRecord.h"
#include "tables/DXFDimStyleTableRecord.h"
#include "tables/DXFViewportTable.h"

#include "DXFWriter.h"

class CDXFTableReadVisitor : public CDXFTableVisitor
{
// Construction/Destruction
public:
    CDXFTableReadVisitor(CDXFReader &reader) : m_reader(reader) { }
    virtual ~CDXFTableReadVisitor() { }

// Overrides
public:
    virtual void VisitSymbolTable(CDXFSymbolTable &table) override {
        dxf_symbol_table_file_util::ReadFromFileSymbolTable(m_reader, &table);
    }

    virtual void VisitDimStyleTable(CDXFDimStyleTable &table) override {
        dxf_symbol_table_file_util::ReadFromDimStyleTableFull(m_reader, &table);
    }

    virtual void VisitBlockTable(CDXFBlockTable &table) override {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitViewReportTable(CDXFViewportTable &table) override
    {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitLineTypeTable(CDXFLineTypeTable &table) override {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitLayerTable(CDXFLayerTable &table) override {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitTextStyleTable(CDXFTextStyleTable &table) override {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitViewTable(CDXFViewTable &table) override
    {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitUCSTable(CDXFUCSTable &table) override {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

    virtual void VisitAppIDtable(CDXFAppIDTable &table) override {
        CDXFTableReadVisitor::VisitSymbolTable(table);
    }

// Attributes
private:
    CDXFReader &m_reader;
};

class CDXFTableWriterVisitor : public CDXFTableVisitor
{
public:
    CDXFTableWriterVisitor(CDXFWriter &writer) : m_writer(writer) { }
    virtual ~CDXFTableWriterVisitor() { }

// Overrides
public:
    virtual void VisitSymbolTable(CDXFSymbolTable &table) override {
        dxf_symbol_table_file_util::WriteToFile(m_writer, table);
    }

    virtual void VisitDimStyleTable(CDXFDimStyleTable &table) override {
        dxf_symbol_table_file_util::WriteToDimStyleTable(m_writer, table);
    }

    virtual void VisitBlockTable(CDXFBlockTable &table) override {
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitViewReportTable(CDXFViewportTable &table) override { 
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitLineTypeTable(CDXFLineTypeTable &table) override {
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitLayerTable(CDXFLayerTable &table) override {
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitTextStyleTable(CDXFTextStyleTable &table) override {
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitViewTable(CDXFViewTable &table) override { 
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitUCSTable(CDXFUCSTable &table) override {
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

    virtual void VisitAppIDtable(CDXFAppIDTable &table) override {
        CDXFTableWriterVisitor::VisitSymbolTable(table);
    }

// Attributes
private:
    CDXFWriter &m_writer;
};

class CDXFSymbolTableRecordReaderVisitor : public CDXFTableVisitor
{
public:
    CDXFSymbolTableRecordReaderVisitor(CDXFReader &reader) : m_reader(reader) { }
    virtual ~CDXFSymbolTableRecordReaderVisitor() { }

// Overrides
public:
    virtual void VisitBlockTable(CDXFBlockTable &table) override
    {
        CDXFBlockTableRecord *pBlockTableRecord = table.CreateBlockTableRecord();
        m_pRecord = pBlockTableRecord;
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileDimBlockRecord(m_reader, pBlockTableRecord);
    }
    
    virtual void VisitViewReportTable(CDXFViewportTable &table) override
    {
        m_pRecord = new CDXFViewportTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileViewReportRecord(m_reader, (CDXFViewportTableRecord *)m_pRecord);
    }
    
    virtual void VisitLineTypeTable(CDXFLineTypeTable &table) override
    {
        m_pRecord = new CDXFLineTypeTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileLineTypeRecord(m_reader, (CDXFLineTypeTableRecord *)m_pRecord);
    }
    
    virtual void VisitLayerTable(CDXFLayerTable &table) override
    {
        m_pRecord = new CDXFLayerTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        m_eError = dxf_table_record_file_util::ReadFromFileLayerRecord(m_reader, (CDXFLayerTableRecord *)m_pRecord);
    }
    
    virtual void VisitTextStyleTable(CDXFTextStyleTable &table) override
    {
        m_pRecord = new CDXFTextStyleTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileTextStyleRecord(m_reader, (CDXFTextStyleTableRecord *)m_pRecord);
    }
    
    virtual void VisitViewTable(CDXFViewTable &table)  override
    {
        m_pRecord = new CDXFViewTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileViewRecord(m_reader, (CDXFViewTableRecord *)m_pRecord);
    }
    
    virtual void VisitUCSTable(CDXFUCSTable &table)  override
    {
        m_pRecord = new CDXFUCSTableRecord;
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileUCSRecord(m_reader, (CDXFUCSTableRecord *)m_pRecord);
    }
    
    virtual void VisitAppIDtable(CDXFAppIDTable &table) override
    {
        m_pRecord = new CDXFAppIDTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);

        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileAppIdRecord(m_reader, (CDXFAppIDTableRecord *)m_pRecord);
    }

    virtual void VisitDimStyleTable(CDXFDimStyleTable &record) override
    {
        m_pRecord = new CDXFDimStyleTableRecord("");
        m_eError = dxf_table_record_file_util::PreReadFromFileSymbolTableRecord(m_reader, m_pRecord);
        
        if (m_eError != DXF_OK) {
            return;
        }

        dxf_table_record_file_util::ReadFromFileDimStyleRecord(m_reader, (CDXFDimStyleTableRecord *)m_pRecord);
    }

public:
    CDXFSymbolTableRecord* GetRecord() const { return m_pRecord; }
    DXF_ERROR GetError() const { return m_eError; }

// Attributes
private: 
    CDXFReader &m_reader;
    CDXFSymbolTableRecord *m_pRecord {nullptr};
    DXF_ERROR m_eError {DXF_OK};
};

class CDXFSymbolTableRecordWriterVisitor : public CDXFRecordVisitor
{
public:
    CDXFSymbolTableRecordWriterVisitor(CDXFWriter &writer, CDXFDatabase *pDB) : m_writer(writer), m_pDB(pDB) { }
    virtual ~CDXFSymbolTableRecordWriterVisitor() { }

public:
    virtual void VisitViewReportTableRecord(CDXFViewportTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileViewReportRecord(m_writer, record);
    }

    virtual void VisitLineTypeTableRecord(CDXFLineTypeTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileLineTypeRecord(m_writer, record);
    }

    virtual void VisitLayerTableRecord(CDXFLayerTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileLayerRecord(m_writer, record);
    }

    virtual void VisitTextStyleTableRecord(CDXFTextStyleTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileTextStyleRecord(m_writer, record);
    }

    virtual void VisitViewTableRecord(CDXFViewTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileViewRecord(m_writer, record);
    }

    virtual void VisitUCSTableRecord(CDXFUCSTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileUCSRecord(m_writer, record);
    }

    virtual void VisitAppIDtableRecord(CDXFAppIDTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileAppIdRecord(m_writer, record);
    }

    virtual void VisitDimStyletableRecord(CDXFDimStyleTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileDimStyleRecord(m_writer, record, m_pDB);
    }

    virtual void VisitBlockTableRecord(CDXFBlockTableRecord &record) override {
        dxf_table_record_file_util::WriteToFileDimBlockRecord(m_writer, record);
    }

// Attributes
private:
    CDXFWriter &m_writer;
    CDXFDatabase *m_pDB;
};

namespace dxf_symbol_table_file_util
{
    namespace internal
    {
        CDXFSymbolTableRecord* ReadFromSymbolTableRecord(CDXFSymbolTable *pTable, CDXFSymbolTableRecordReaderVisitor &vis)
        {
            pTable->AcceptTableVis(vis);
            return vis.GetRecord();
        }
    }

    void ReadFromFileSymbolTable(CDXFReader &reader, CDXFSymbolTable *pTable)
    {
        dxf_obj_file_util::ReadFromFile(reader, pTable);

        CDXFReaderItem item;
        reader.ReadItem(item);
        while (item.m_nGroupCode != 70) {
            reader.ReadItem(item);
        }
    }

    void ReadFromDimStyleTableFull(CDXFReader &reader, CDXFDimStyleTable *pTable)
    {
        dxf_symbol_table_file_util::ReadFromFileSymbolTable(reader, pTable);

        CDXFReaderItem item;
        reader.ReadItem(item);
        while (item.m_nGroupCode != 0) {
            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    DXF_ERROR ReadFromFileTable(CDXFReader &reader, CDXFSymbolTable *pTable)
    {
        CDXFReaderItem item;
        unsigned long ulHandle;
        CDXFObjectID objectID;
        CDXFSymbolTableRecord *pRecord = nullptr;
        CDXFSymbolTableRecordReaderVisitor vis(reader);
        CDXFTableReadVisitor tablevis(reader);

        ulHandle = reader.ReadHandle();
        reader.GetDatabase()->AddObject(pTable, objectID, ulHandle);

        pTable->AcceptTableVis(tablevis);

        reader.ReadItem(item);

        DXF_ERROR eError = DXF_OK;

        while (0 != ::strcmpi(item.m_resString, "ENDTAB"))
        {
            reader.PushBackItem();

            pRecord = internal::ReadFromSymbolTableRecord(pTable, vis);
            eError  = vis.GetError();

            if (eError != DXF_OK) {
                return eError;
            }

            // If file version is 12 or lower, then owner information
            // is not saved in the file, and we have to set the ownership by ourselves.
            if (pRecord->GetOwnerID().IsNull())
                pRecord->SetOwnerID(pTable->m_objectID);
            pTable->m_listRecords.push_back(pRecord->GetObjectID());

            reader.ReadItem(item);
        }

        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        if (0 != ::strcmpi(item.m_resString, "ENDTAB")) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        return DXF_OK;
    }

    void PreWriteFile(CDXFWriter &writer, CDXFSymbolTable &table, CDXFDatabase *pDB)
    {
        writer.Write("  0\nTABLE\n");
        writer.WriteText(table.GetObjectName(), 2);

        CDXFTableWriterVisitor vis(writer);
        table.AcceptTableVis(vis);

        CDXFSymbolTableRecordWriterVisitor  visitor(writer, pDB);

        const size_t nCnt = table.m_listRecords.size();
        for (size_t i = 0; i < nCnt; ++i) {
            const CDXFObjectID &objectID = table.m_listRecords[i];
            CDXFSymbolTableRecord *pObj = (CDXFSymbolTableRecord *)objectID.GetObject();
            dxf_table_record_file_util::PreWriteToFileSymbolTableRecord(writer, *pObj);
            pObj->AcceptRecordVis(visitor);
        }

        writer.Write("  0\nENDTAB\n");
    }

    void WriteToFile(CDXFWriter &writer, const CDXFSymbolTable &table)
    {
        dxf_obj_file_util::WriteToFile(writer, table);
        writer.Write("  100\nAcDbSymbolTable\n");
        writer.WriteText(std::to_string(table.m_listRecords.size()).c_str(), 70);
    }

    void WriteToDimStyleTable(CDXFWriter &writer, const CDXFDimStyleTable &table)
    {
        dxf_symbol_table_file_util::WriteToFile(writer, table);
        writer.Write("  100\nAcDbDimStyleTable\n");
        writer.Write("  71\n0\n");
    }
};