#ifndef __DXF_TEXT_STYLE_TABLE_H__
#define __DXF_TEXT_STYLE_TABLE_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFTextStyleTable : public CDXFSymbolTable  
{
// Construction/Destruction
public:
    CDXFTextStyleTable() : CDXFSymbolTable("STYLE") { }
    virtual ~CDXFTextStyleTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitTextStyleTable(*this); }

// Operations
public:
    void AddTableRecord(CDXFSymbolTableRecord *pRecord, CDXFDatabase *pDB) {
        CDXFSymbolTable::AddTableRecord(pRecord, pDB);
    }
    void GetTableRecord(const char *sName, CDXFTextStyleTableRecord * &pRecord) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        pRecord = (CDXFTextStyleTableRecord *)pRec;
    }
};

#endif
