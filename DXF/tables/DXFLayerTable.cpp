#include "stdafx.h"
#include "DXFLayerTable.h"

#include "DXFLayerTableRecord.h"
#include "DXFLineTypeTableRecord.h"
#include "DXFDatabase.h"
#include "DXFDictionary.h"

void CDXFLayerTable::AddTableRecord(CDXFLayerTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB)
{
	if (pRecord->GetLinetypeID().IsNull())
	{
        CDXFObjectID id;
		pDB->GetLineTypeTable()->GetTableRecordId(DXF_LINE_TYPE_CONTINUOUS, id);
		pRecord->SetLinetype(id);

        CDXFDictionaryWithDefault *pDictionary = (CDXFDictionaryWithDefault *)pDB->GetNamedObjectsDictionary()->GetObjectByName("ACAD_PLOTSTYLENAME");
		ASSERT(pDictionary);
		pRecord->SetPlotStyleName(pDictionary->GetDefaultID());
	}

	CDXFSymbolTable::AddTableRecordId(pRecord, objectID, pDB);
}

void CDXFLayerTable::AddTableRecord(CDXFLayerTableRecord *pRecord, CDXFDatabase *pDB)
{
	CDXFObjectID objectID;
	AddTableRecord(pRecord, objectID, pDB);
}