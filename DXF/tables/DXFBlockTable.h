#ifndef __DXF_BLOCK_TABLE_H__
#define __DXF_BLOCK_TABLE_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

class CDXFBlockTableRecord;

class DXFDLLAPI CDXFBlockTable : public CDXFSymbolTable
{
// Construction/Destruction
public:
    CDXFBlockTable() : CDXFSymbolTable("BLOCK_RECORD") { }
    virtual ~CDXFBlockTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitBlockTable(*this); }

// Operations
public:
    CDXFBlockTableRecord *CreateUniqueTableRecord(CDXFObjectID &objectID, CDXFDatabase *pDB);
    CDXFBlockTableRecord *CreateBlockTableRecord(const char *sBlockName, CDXFObjectID &objectID, CDXFDatabase *pDB);
    CDXFBlockTableRecord *CreateBlockTableRecord(const char *sBlockName, CDXFDatabase *pDB);
    CDXFBlockTableRecord *CreateBlockTableRecord();

    CDXFBlockTableRecord *GetBlockTableRecord(const char *sName);

private:
    void AddBlockTableRecord(CDXFBlockTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB);

// Attributes
private:
    int32_t m_nNumBlock {0}; // optimization for the CreateUniqueTableRecord function.
};

#endif