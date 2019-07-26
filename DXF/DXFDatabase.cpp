#include "stdafx.h"
#include "DXFDatabase.h"

#include "tables/DXFLineTypeTableRecord.h"
#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFBlockTableRecord.h"
#include "tables/DXFDimStyleTableRecord.h"
#include "tables/DXFTextStyleTableRecord.h"
#include "tables/DXFViewportTable.h"
#include "entity/DXFLine.h"
#include "base/DXFObject.h"
#include "DXFDictionary.h"
#include "base/DXFMlineStyle.h"
#include "DXFPlaceHolder.h"

#include "file_utils/dxf_database_file_util.h"

namespace internal
{
    static void CreateStandardBlocks(CDXFDatabase *pDB)
    {
	    CDXFBlockTable *pBlockTable = pDB->GetBlockTable();
        {
	        // Create __ClosedBlankArrow block for dimensions
            CDXFBlockTableRecord *pBlockTableRecord = pBlockTable->CreateBlockTableRecord("__ClosedBlankArrow", pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(0.0,  0.0, 1.0, 0.2), pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(1.0,  0.2, 0.75, 0.0), pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(0.75, 0.0, 1.0, -0.2), pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(1.0, -0.2, 0.0, 0.0), pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(0.0,  0.0, 1.0, 0.0), pDB);
        }
        {
	        // Create __ObliqueTick block for dimensions
	        CDXFBlockTableRecord *pBlockTableRecord = pBlockTable->CreateBlockTableRecord("__ObliqueTick", pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(0.0,  -0.5, 0.0, 0.5), pDB);
	        pBlockTableRecord->AddEntity(new CDXFLine(-0.5, -0.5, 0.5, 0.5), pDB);
        }
	    // Create _None block for dimensions
        pBlockTable->CreateBlockTableRecord("__None", pDB);
    }

    namespace tables_init
    {
        static void InitTextStyleTable(CDXFDatabase *pDB)
        {
	        CDXFTextStyleTable *pTextStyleTable = pDB->GetTextStyleTable();
	        pTextStyleTable->AddTableRecord(new CDXFTextStyleTableRecord("STANDARD"), pDB);
        }

        static void InitAppIDTable(CDXFDatabase *pDB)
        {
	        CDXFAppIDTable *pAppIDTable = pDB->GetAppIDTable();
	        pAppIDTable->AddTableRecord(new CDXFAppIDTableRecord("ACAD"), pDB);
        }

        static void InitDimStyleTable(CDXFDatabase *pDB)
        {
	        CDXFDimStyleTable *pDimStyleTable = pDB->GetDimStyleTable();
            pDimStyleTable->AddTableRecord(new CDXFDimStyleTableRecord("STANDARD"), pDB);
	        pDimStyleTable->AddTableRecord(new CDXFDimStyleTableRecord("ISO-25"),   pDB);
        }

        static void InitLayerTable(CDXFDatabase *pDB)
        {
	        CDXFLayerTable *pLayerTable = pDB->GetLayerTable();
	        pLayerTable->AddTableRecord(new CDXFLayerTableRecord("0"), pDB);
        }

        static void InitBlocksTable(CDXFDatabase *pDB)
        {
	        CDXFBlockTable *pBlockTable = pDB->GetBlockTable();
	        pBlockTable->CreateBlockTableRecord(DXF_MODEL_SPACE, pDB);
	        pBlockTable->CreateBlockTableRecord(DXF_PAPER_SPACE, pDB);
        }

        static void InitLineTypeTable(CDXFDatabase *pDB)
        {
	        CDXFLineTypeTable *pLinetypeTable = pDB->GetLineTypeTable();
            {
	            CDXFLineTypeTableRecord *pLinetypeTableRecord = new CDXFLineTypeTableRecord(DXF_LINE_TYPE_CONTINUOUS);
	            pLinetypeTableRecord->SetDescr("Solid line");
	            pLinetypeTable->AddTableRecord(pLinetypeTableRecord, pDB);
            }
            {
	            CDXFLineTypeTableRecord *pLinetypeTableRecord = new CDXFLineTypeTableRecord(DXF_LINE_TYPE_DASHED);
	            pLinetypeTableRecord->SetDescr("__ __ __ __ __ __ __");
	            pLinetypeTableRecord->SetNumDashes(2);
	            pLinetypeTableRecord->SetDashLength(0, 12.5);
	            pLinetypeTableRecord->SetDashLength(1, -6.25);
	            pLinetypeTable->AddTableRecord(pLinetypeTableRecord, pDB);
            }
            {
	            CDXFLineTypeTableRecord *pLinetypeTableRecord = new CDXFLineTypeTableRecord(DXF_LINE_TYPE_DIVIDE);
	            pLinetypeTableRecord->SetDescr("__ . . __ . . __ . . __");
	            pLinetypeTableRecord->SetNumDashes(6);
	            pLinetypeTableRecord->SetDashLength(0, 12.7);
	            pLinetypeTableRecord->SetDashLength(1, -6.35);
	            pLinetypeTableRecord->SetDashLength(2, 0.0);
	            pLinetypeTableRecord->SetDashLength(3, -6.35);
	            pLinetypeTableRecord->SetDashLength(4, 0.0);
	            pLinetypeTableRecord->SetDashLength(5, -6.35);
	            pLinetypeTable->AddTableRecord(pLinetypeTableRecord, pDB);
            }
            {
	            CDXFLineTypeTableRecord *pLinetypeTableRecord = new CDXFLineTypeTableRecord(DXF_LINE_TYPE_DASHDOT);
	            pLinetypeTableRecord->SetDescr("__ . __ . __ . __ . __");
	            pLinetypeTableRecord->SetNumDashes(4);
	            pLinetypeTableRecord->SetDashLength(0, 12.7);
	            pLinetypeTableRecord->SetDashLength(1, -6.35);
	            pLinetypeTableRecord->SetDashLength(2, 0.0);
	            pLinetypeTableRecord->SetDashLength(3, -6.35);
	            pLinetypeTable->AddTableRecord(pLinetypeTableRecord, pDB);
            }
            {
	            CDXFLineTypeTableRecord *pLinetypeTableRecord = new CDXFLineTypeTableRecord(DXF_LINE_TYPE_DOT);
	            pLinetypeTableRecord->SetDescr(". . . . . . . . . . . ");
	            pLinetypeTableRecord->SetNumDashes(2);
	            pLinetypeTableRecord->SetDashLength(0, 0.0);
	            pLinetypeTableRecord->SetDashLength(1, -6.35);
	            pLinetypeTable->AddTableRecord(pLinetypeTableRecord, pDB);
            }
	        pLinetypeTable->AddTableRecord(new CDXFLineTypeTableRecord("BYLAYER"), pDB);
	        pLinetypeTable->AddTableRecord(new CDXFLineTypeTableRecord("BYBLOCK"), pDB);
        }
    };

    static void InitTables(CDXFDatabase *pDB)
    {
        internal::tables_init::InitLineTypeTable(pDB);
        internal::tables_init::InitLayerTable(pDB);
        internal::tables_init::InitTextStyleTable(pDB);
	    internal::tables_init::InitBlocksTable(pDB);
        internal::tables_init::InitAppIDTable(pDB);
	    internal::tables_init::InitDimStyleTable(pDB);
    }

    static void CreateAllDictionaries(CDXFDatabase *pDB, CDXFObjectID &namedObjectsDictionaryID)
    {
        CDXFDictionary *pDictionary = new CDXFDictionary;
	    pDB->AddObject(pDictionary, namedObjectsDictionaryID);

        CDXFDictionary *pMLineStyleDictionary = new CDXFDictionary;

	    pDictionary->SetAt("ACAD_GROUP",      new CDXFDictionary,    pDB);
	    pDictionary->SetAt("ACAD_MLINESTYLE", pMLineStyleDictionary, pDB);

	    pMLineStyleDictionary->SetAt("STANDARD", new CDXFMlineStyle("STANDARD"), pDB);
	
        CDXFDictionaryWithDefault *pDefDict = new CDXFDictionaryWithDefault;
        pDictionary->SetAt("ACAD_PLOTSTYLENAME", pDefDict, pDB);
        CDXFObjectID objectID;
        pDefDict->SetAt("Normal", new CDXFPlaceHolder, objectID, pDB);
        pDefDict->SetDefaultID(objectID);
    }

    static void CreateAllTables(CDXFDatabase *pDB, std::array<CDXFObjectID, CDXFDatabase::DXF_TABLE_COUNT> &tablesIDs)
    {
	    CDXFObjectID tableID;

	    pDB->AddObject(new CDXFBlockTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_BLOCK_TABLE] = tableID;

	    pDB->AddObject(new CDXFLineTypeTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_LINETYPE_TABLE] = tableID;

	    pDB->AddObject(new CDXFLayerTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_LAYER_TABLE] = tableID;

	    pDB->AddObject(new CDXFAppIDTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_APPID_TABLE] = tableID;

	    pDB->AddObject(new CDXFDimStyleTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_DIMSTYLE_TABLE] = tableID;

	    pDB->AddObject(new CDXFTextStyleTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_TEXTSTYLE_TABLE] = tableID;

	    pDB->AddObject(new CDXFUCSTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_UCS_TABLE] = tableID;

	    pDB->AddObject(new CDXFViewTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_VIEW_TABLE] = tableID;

	    pDB->AddObject(new CDXFViewportTable, tableID);
	    tablesIDs[CDXFDatabase::DXF_VIEWPORT_TABLE] = tableID;
    }
};

CDXFDatabase::CDXFDatabase()
{
	InitDatabase();
}

CDXFDatabase::~CDXFDatabase()
{
	DeleteAllObjects();
}

bool CDXFDatabase::OpenDatabase(const wchar_t *sFileName)
{
    DeleteAllObjects();
    const bool bRet =  dxf_database_file_util::OpenDatabase(sFileName, this) == DXF_OK;
    internal::CreateAllDictionaries(this, m_namedObjectsDictionaryID);
    return bRet;
}

CDXFBlockTable *CDXFDatabase::GetBlockTable() const
{
	return (CDXFBlockTable *)m_tablesIDs[DXF_BLOCK_TABLE].GetObject();
}

CDXFLayerTable * CDXFDatabase::GetLayerTable() const
{
	return (CDXFLayerTable *)m_tablesIDs[DXF_LAYER_TABLE].GetObject();
}

CDXFLineTypeTable * CDXFDatabase::GetLineTypeTable() const
{
	return (CDXFLineTypeTable *)m_tablesIDs[DXF_LINETYPE_TABLE].GetObject();
}

CDXFAppIDTable *CDXFDatabase::GetAppIDTable() const
{
	return (CDXFAppIDTable *)m_tablesIDs[DXF_APPID_TABLE].GetObject();
}

CDXFDimStyleTable *CDXFDatabase::GetDimStyleTable() const
{
	return (CDXFDimStyleTable *)m_tablesIDs[DXF_DIMSTYLE_TABLE].GetObject();
}

CDXFTextStyleTable *CDXFDatabase::GetTextStyleTable() const
{
	return (CDXFTextStyleTable *)m_tablesIDs[DXF_TEXTSTYLE_TABLE].GetObject();
}

CDXFUCSTable *CDXFDatabase::GetUCSTable() const
{
	return (CDXFUCSTable *)m_tablesIDs[DXF_UCS_TABLE].GetObject();
}

CDXFViewportTable *CDXFDatabase::GetViewPortTable() const
{
	return (CDXFViewportTable *)m_tablesIDs[DXF_VIEWPORT_TABLE].GetObject();
}

void CDXFDatabase::SetViewPortTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_VIEWPORT_TABLE] = id;
}

CDXFViewTable *CDXFDatabase::GetViewTable() const
{
	return (CDXFViewTable *)m_tablesIDs[DXF_VIEW_TABLE].GetObject();
}

void CDXFDatabase::SetViewTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_VIEW_TABLE] = id;
}

void CDXFDatabase::SetUCSTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_UCS_TABLE] = id;
}

void CDXFDatabase::SetTextStyleTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_TEXTSTYLE_TABLE] = id;
}

void CDXFDatabase::SetDimStyleTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_DIMSTYLE_TABLE] = id;
}

void CDXFDatabase::SetAppIDTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_APPID_TABLE] = id;
}

void CDXFDatabase::SetLineTypeTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_LINETYPE_TABLE] = id;
}

void CDXFDatabase::SetLayerTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_LAYER_TABLE] = id;
}

void CDXFDatabase::SetBlockTable(const CDXFObjectID &id)
{
     m_tablesIDs[DXF_BLOCK_TABLE] = id;
}

void CDXFDatabase::AddObject(CDXFObject *pObject, CDXFObjectID &id)
{
	CDXFObjectID *pObjectID = new CDXFObjectID;

    ASSERT(m_objectIDPtrs.find(m_vars.m_nHandSeed) == m_objectIDPtrs.end());
 	m_objectIDPtrs[m_vars.m_nHandSeed] = pObjectID;

	pObjectID->m_pObject  = pObject;
	pObjectID->m_ulHandle = m_vars.m_nHandSeed++;

	id = *pObjectID;
	pObject->m_objectID = *pObjectID;
}

bool CDXFDatabase::AddObject(CDXFObject *pObject, CDXFObjectID &id, unsigned long ulHandle)
{	
	if (ulHandle == 0) {
		AddObject(pObject, id);
        return true;
    }

	CDXFObjectID *pObjectID = new CDXFObjectID;

	auto found = m_objectIDPtrs.find(ulHandle);
	if (found != m_objectIDPtrs.end() /*&& found ->second*/) {
		return false;
	}

	ASSERT(m_objectIDPtrs.find(ulHandle) == m_objectIDPtrs.end());
    m_objectIDPtrs[ulHandle] = pObjectID;
	pObjectID->m_pObject  = pObject;
	pObjectID->m_ulHandle = ulHandle;

	id = *pObjectID;
	pObject->m_objectID = *pObjectID;

	if (ulHandle > m_vars.m_nHandSeed) {
		m_vars.m_nHandSeed = ulHandle + 1;
	}
	
	return true;
}

bool CDXFDatabase::GetObjectID(CDXFObjectID &id, unsigned long ulHandle) const
{
	bool bFound = false;

    auto found = m_objectIDPtrs.find(ulHandle);
	if (found != m_objectIDPtrs.end()) {
		bFound = true;
		id = *found->second;
	}
	
	id.SetNull();
	return bFound;
}   

CDXFDictionary *CDXFDatabase::GetNamedObjectsDictionary()
{
	return (CDXFDictionary *)m_namedObjectsDictionaryID.GetObject();
}

void CDXFDatabase::DeleteAllObjects()
{
	CDXFObjectID *pObjectID = nullptr;
	for (auto &elem : m_objectIDPtrs) {
		pObjectID = elem.second;
		if (pObjectID) {
			delete pObjectID->m_pObject;
			delete pObjectID;
		}
	}

	m_objectIDPtrs.clear();
	for (CDXFObjectID &obj_id : m_tablesIDs) { 
        obj_id.SetNull(); 
    }
	m_namedObjectsDictionaryID.SetNull();
}

void CDXFDatabase::InitDatabase()
{
	internal::CreateAllTables(this, m_tablesIDs);
    internal::CreateAllDictionaries(this, m_namedObjectsDictionaryID);
    internal::InitTables(this);
    internal::CreateStandardBlocks(this);
}

CDXFObjectID CDXFDatabase::GetDXFObjectID(unsigned long ulHandle) const
{
	CDXFObjectID objectID;
	auto found = m_objectIDPtrs.find(ulHandle);
	if ( found != m_objectIDPtrs.end() /*&& found->second*/) { // can be null
		objectID = *found->second;
	}
	return objectID;
}

bool CDXFDatabase::DeleteObject(const CDXFObjectID &id)
{
    CDXFObjectID *pObjectID = nullptr;
	const auto end_it = m_objectIDPtrs.end();
	for (auto it = m_objectIDPtrs.begin(); it != end_it; ++it)
	{
		pObjectID = it->second;
		if (!pObjectID || *pObjectID != id) {
            continue;
        }
        delete pObjectID->m_pObject;
        delete pObjectID;
        m_objectIDPtrs.erase(it);
        return true;
	}
	return false;
}

namespace internal
{
    inline char *GetFileData(const wchar_t *file_path, size_t &nFileSize)
    {
        FILE *pFile = ::_wfopen(file_path, L"rb");
	    if ( !pFile ) {
		    return nullptr;
	    }

	    ::fseek(pFile, 0, SEEK_END);
	    nFileSize = ftell(pFile);
	    ::fseek(pFile, 0, SEEK_SET);
	    char *pData = new char[nFileSize+1];
	    ::memset(pData, 0, nFileSize+1);
	    ::fread(pData, 1, nFileSize, pFile);
	    ::fclose(pFile);
        return pData;
    }

    inline ptrdiff_t GetBlockPos(const char *str, ptrdiff_t nStart) {
        const char *sBlockRecord = "acdbblocktablerecord";
        const char *match = ::strstr(str + nStart, sBlockRecord);
        return (nullptr == match) ? -1 : (match - str);
    }

    inline ptrdiff_t GetLineEndPosition(const char *str, ptrdiff_t nStart) {
        const ptrdiff_t nLineEnd = 10;
        const char *match = ::strchr(str + nStart, nLineEnd);
        return (nullptr == match) ? -1 : (match - str);
    }

    inline bool MoveToNextToken(const char *str, ptrdiff_t nPos, ptrdiff_t& nNewPos) {
        if (-1 == (nNewPos = GetLineEndPosition(str, nPos))) {
            return false;
        }
        nNewPos += 1; // Skip 2 characters 13&10
        return true;
    }

    //http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
    static char *trim(char *str)
    {
        size_t len = 0;
        char *frontp = str - 1;
        char *endp = nullptr;

        if ( str == nullptr ) {
            return nullptr;
        }

        if ( str[0] == '\0' ) {
            return str;
        }

        len = ::strlen(str);
        endp = str + len;

        // Move the front and back pointers to address
        // the first non-whitespace characters from
        // each end.
        while( ::isspace(*(++frontp)) );
        while( ::isspace(*(--endp)) && endp != frontp );

        if ( str + len - 1 != endp )
                *(endp + 1) = '\0';
        else if ( frontp != str &&  endp == frontp )
                *str = '\0';

        // Shift the string so that it starts at str so
        // that if it's dynamically allocated, we can
        // still free it on the returned pointer.  Note
        // the reuse of endp to mean the front of the
        // string buffer now.
        endp = str;
        if ( frontp != str ) {
            while( *frontp ) *endp++ = *frontp++;
            *endp = '\0';
        }

        return str;
    }

    inline char *GetDXFBlock1(const char *str, ptrdiff_t nPosBegin, ptrdiff_t nPosEnd) {
        const ptrdiff_t nSize = (nPosEnd - nPosBegin);
        char *pBlock = new char[nSize + 1];
        pBlock[nSize] = '\0';
        ::strncpy(pBlock, str + nPosBegin, nSize);
        pBlock = trim(pBlock);
        if (0 != ::strlen(pBlock) && pBlock[0] != '*') {
            return pBlock;
        }
        delete pBlock;
        return nullptr;
    }

    inline void DoReadBlocks(char *pData, size_t nFileSize, std::vector<std::string> &arDXFBlocks)
    {
        for (size_t i = 0; i < nFileSize; ++i) {
            pData[i] = ::tolower(pData[i]);
        }

        ptrdiff_t nPos = 0;
        ptrdiff_t nPosEnd = 0;

        while (-1 != (nPos = internal::GetBlockPos(pData, nPos)))
        {
            ++nPos;
            internal::MoveToNextToken(pData, nPos, nPos);
            internal::MoveToNextToken(pData, nPos, nPos);
            internal::MoveToNextToken(pData, nPos, nPosEnd);

            char *pBlock = internal::GetDXFBlock1(pData, nPos, nPosEnd);
            if ( pBlock ) {
                arDXFBlocks.push_back(pBlock);
                delete pBlock;
            }
        }
    }
};

bool CDXFDatabase::ReadBlocks(const wchar_t *sFilePath, std::vector<std::string> &arDXFBlocks)
{
    size_t nFileSize = 0;
    char *pData = internal::GetFileData(sFilePath, nFileSize);
    if ( nFileSize == 0 ) {
        delete [] pData;
        return false;
    }
    internal::DoReadBlocks(pData, nFileSize, arDXFBlocks);
    delete [] pData;
    return true;
}