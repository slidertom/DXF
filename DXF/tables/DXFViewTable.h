#ifndef __DXF_VIEW_TABLE_H__
#define __DXF_VIEW_TABLE_H__
#pragma once

#ifndef __DXF_RECORD_VISITOR_H__
    #include "DXFRecordVisitor.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFViewTable : public CDXFSymbolTable  
{
// Construction/Destruction
public:
    CDXFViewTable() : CDXFSymbolTable("VIEW") { }
    virtual ~CDXFViewTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitViewTable(*this); }

// Operations
public:
    void AddTableRecord(CDXFSymbolTableRecord *pRecord, CDXFDatabase *pDB) {
        CDXFSymbolTable::AddTableRecord(pRecord, pDB);
    }

    CDXFViewTableRecord *GetViewTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFViewTableRecord *pRecord = (CDXFViewTableRecord *)pRec;
        return pRecord;
    }
};

#endif
