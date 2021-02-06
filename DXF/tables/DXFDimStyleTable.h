#ifndef __DXF_DIM_STYLE_TABLE_H__
#define __DXF_DIM_STYLE_TABLE_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFDimStyleTable : public CDXFSymbolTable
{
// Construction/Destruction
public:
    CDXFDimStyleTable() : CDXFSymbolTable("DIMSTYLE") { }
    virtual ~CDXFDimStyleTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitDimStyleTable(*this); }

// Operations
public:
    void AddTableRecord(CDXFSymbolTableRecord *pRecord, CDXFDatabase *pDB) {
        CDXFSymbolTable::AddTableRecord(pRecord, pDB);
    }

    CDXFDimStyleTableRecord *GetDimStyleTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFDimStyleTableRecord *pRecord = (CDXFDimStyleTableRecord *)pRec;
        return pRecord;
    }
};

#endif
