#include "stdafx.h"
#include "DXFBlockTable.h"

#include "DXFBlockTableRecord.h"
#include "DXFDatabase.h"

void CDXFBlockTable::AddBlockTableRecord(CDXFBlockTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB)
{
	CDXFObjectID layerID;
	pDB->GetLayerTable()->GetTableRecordId("0", layerID);
	pRecord->m_layerID = layerID;

	CDXFSymbolTable::AddTableRecordId(pRecord, objectID, pDB);
    pRecord->CreateBeginEndEntities(pDB);
}

CDXFBlockTableRecord *CDXFBlockTable::CreateBlockTableRecord()
{
    CDXFBlockTableRecord *pBlock = new CDXFBlockTableRecord("");
    return pBlock;
}

CDXFBlockTableRecord *CDXFBlockTable::CreateBlockTableRecord(const char *sBlockName, CDXFObjectID &objectID, CDXFDatabase *pDB)
{
    CDXFBlockTableRecord *pBlock = new CDXFBlockTableRecord(sBlockName);
    AddBlockTableRecord(pBlock, objectID, pDB);
    return pBlock;
}

CDXFBlockTableRecord *CDXFBlockTable::CreateBlockTableRecord(const char *sBlockName, CDXFDatabase *pDB)
{
    CDXFBlockTableRecord *pBlock = new CDXFBlockTableRecord(sBlockName);
    CDXFObjectID objectID;
	AddBlockTableRecord(pBlock, objectID, pDB);
    return pBlock;
}

CDXFBlockTableRecord *CDXFBlockTable::CreateUniqueTableRecord(CDXFObjectID &objectID, CDXFDatabase *pDB)
{
    CDXFBlockTableRecord *pRecord = nullptr;
    std::string sBuffer;
	do {
        ++m_nNumBlock;
        sBuffer = "*D";
        sBuffer += std::to_string(m_nNumBlock);
		pRecord = GetBlockTableRecord(sBuffer.c_str());
	}
	while (pRecord);

	pRecord = new CDXFBlockTableRecord(sBuffer.c_str());
	AddBlockTableRecord(pRecord, objectID, pDB);
    return pRecord;
}

CDXFBlockTableRecord *CDXFBlockTable::GetBlockTableRecord(const char *sName)
{
	CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
    // TODO: static CDXFBlockTableRecord *CDXFBlockTableRecord::GetObject(CDXFSymbolTableRecord *pRec) and visitor reuse
	CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)pRec; 
    return pRecord;
}