#ifndef __DXF_LAYER_TABLE_H__
#define __DXF_LAYER_TABLE_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFLayerTableRecord;

class DXFDLLAPI CDXFLayerTable : public CDXFSymbolTable
{
// Construction/Destruction
public:
    CDXFLayerTable() : CDXFSymbolTable("LAYER") { }
    virtual ~CDXFLayerTable() { }

// Override
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitLayerTable(*this); }

// Operations
public:
    void AddTableRecord(CDXFLayerTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB);
    void AddTableRecord(CDXFLayerTableRecord *pRecord, CDXFDatabase *pDB);
    CDXFLayerTableRecord *GetLayerTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFLayerTableRecord *pRecord = (CDXFLayerTableRecord *)pRec;
        return pRecord;
    }
};

#endif
