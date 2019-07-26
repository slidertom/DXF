#include "stdafx.h"
#include "DXFSymbolTable.h"

#include "DXFDatabase.h"

void CDXFSymbolTable::AddTableRecordId(CDXFSymbolTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB)
{
	pDB->AddObject(pRecord, objectID);	
	m_listRecords.push_back(objectID);
	pRecord->m_ownerID = m_objectID;
}

CDXFSymbolTableRecord * CDXFSymbolTable::GetTableRecordByName(const char *sName)
{
    CDXFObjectID object_id;
    GetTableRecordId(sName, object_id);
	
    if ( object_id.IsNull() ) {
        return nullptr;
    }

    CDXFSymbolTableRecord *pRec = (CDXFSymbolTableRecord *)object_id.GetObject();
    return pRec;
}

void CDXFSymbolTable::GetTableRecordId(const char *sName, CDXFObjectID &id)
{
	id.SetNull();

	for (CDXFObjectID &object_id : m_listRecords) { 
		CDXFSymbolTableRecord *pRecord = (CDXFSymbolTableRecord *)object_id.GetObject();
		if (::strcmpi(pRecord->GetName(), sName) == 0) {
			id = object_id;
			break;
		}
	}
}

