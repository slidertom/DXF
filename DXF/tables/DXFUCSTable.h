#ifndef __DXF_UCS_TABLE_H__
#define __DXF_UCS_TABLE_H__
#pragma once

#ifndef __DXF_RECORD_VISITOR_H__
    #include "DXFRecordVisitor.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

class CDXFUCSTable : public CDXFSymbolTable  
{
// Construction/Destruction
public:
    CDXFUCSTable() : CDXFSymbolTable("UCS") { } 
    virtual ~CDXFUCSTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitUCSTable(*this); }

// Operations
public:
    CDXFUCSTableRecord *GetUSCTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFUCSTableRecord *pRecord = (CDXFUCSTableRecord *)pRec;
        return pRecord;
    }
};

#endif
