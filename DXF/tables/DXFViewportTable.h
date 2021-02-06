#ifndef __DXF_VIEW_PORT_TABLE_H__
#define __DXF_VIEW_PORT_TABLE_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

#ifndef __DXF_RECORD_VISITOR_H__
    #include "DXFRecordVisitor.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFViewportTable : public CDXFSymbolTable  
{
// Construction/Destruction
public:
    CDXFViewportTable() : CDXFSymbolTable("VPORT") { }
    virtual ~CDXFViewportTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitViewReportTable(*this); }

// Operations
public:
    CDXFViewportTableRecord *GetViewPortTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFViewportTableRecord *pRecord = (CDXFViewportTableRecord *)pRec;
        return pRecord;
    }
};

#endif
