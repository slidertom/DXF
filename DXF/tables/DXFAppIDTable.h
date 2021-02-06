#ifndef __DXF_APP_ID_TABLE_H__
#define __DXF_APP_ID_TABLE_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
    #include "DXFSymbolTableRecord.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFAppIDTable : public CDXFSymbolTable  
{
// Construction/Destruction
public:
    CDXFAppIDTable() : CDXFSymbolTable("APPID") { }
    virtual ~CDXFAppIDTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitAppIDtable(*this); }

// Operations
public:
    void AddTableRecord(CDXFSymbolTableRecord *pRecord, CDXFDatabase *pDB) {
        CDXFSymbolTable::AddTableRecord(pRecord, pDB);
    }

    CDXFAppIDTableRecord *GetAppIDTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFAppIDTableRecord *pRecord = (CDXFAppIDTableRecord *)pRec;
        return pRecord;
    }

private:
    unsigned char m_fFlags {0};
};

#endif
