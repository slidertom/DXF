#include "stdafx.h"
#include "DXFReader.h"

#include "DXFDatabase.h"

#include "algorithm"

// DXF Data Block contains all data (group codes and data associated with them)
// starting from the group code 0 and ending at the next group code 0.
class CDXFDataBlock
{
// Construction/Destruction
public:
	CDXFDataBlock() { }
	~CDXFDataBlock() {}

// Operators
public:
	CDXFDataBlock &operator=(const CDXFDataBlock & block) {
        m_items.clear();
	    m_items.insert(m_items.end(), block.m_items.begin(), block.m_items.end());
	    return *this;
    }

// Attributes
public:
	std::vector<CDXFReaderItem> m_items;
};

namespace internal
{
    static inline bool IsIntNumber(char *sNumber)
    {
	    bool bNumber = true;

	    // Skip all whitespace characters
	    while (*sNumber != 0)
	    {
		    if (::iswspace(*sNumber)) {
			    sNumber++;
            }
		    else {
			    break;
            }
	    }

	    if (*sNumber == '-' || *sNumber == '+') {
		    sNumber++;
        }

	    if (*sNumber == 0) {
		    bNumber = false;
        }

	    while (*sNumber != 0 && bNumber) {
		    if (::iswspace(*sNumber)) {
			    break;
            }
		    bNumber = bNumber && (::isdigit(*sNumber) ? true : false);
		    ++sNumber;
	    }

	    return bNumber;
    }
};

CDXFReader::CDXFReader(CDXFDatabase *pDatabase)
{
	ASSERT(pDatabase);
	m_pDatabase = pDatabase;
}

CDXFReader::~CDXFReader()
{
	DeleteDataBlocks();
	if (m_pData) {
		delete[] m_pData;
	}
}

bool CDXFReader::Open(const wchar_t *sFilePath)
{
	if (m_pData) {
		delete[] m_pData;
		m_pData = nullptr;
	}
	
	if (::wcslen(sFilePath) == 0) {
		return false;
    }

	int32_t nFileSize;

	m_bItemInBuffer = false;

	FILE *pFile = ::_wfopen(sFilePath, L"rb");
	if ( !pFile ) {
		return false;
    }

	::fseek(pFile, 0, SEEK_END);
	nFileSize = ftell(pFile);
	::fseek(pFile, 0, SEEK_SET);

	m_pData = m_pCursor = new char[nFileSize + 1];
	::fread(m_pData, 1, nFileSize, pFile);

	m_pData[nFileSize] = 0;

	::fclose(pFile);

	m_nLine = 0;
	m_bEOF = false;

	if ( ReadDataBlocks() != DXF_OK ) {
		return false;
    }

	m_DataBlockIt = m_dataBlocks.begin();
	m_pDataBlock = *m_DataBlockIt;
	m_DataBlockIt++;

	m_DataBlockItemIt = m_pDataBlock->m_items.begin();

	return true;
}

bool CDXFReader::ReadItem(CDXFReaderItem &item)
{
	bool bRetrieved = true;

	if (m_DataBlockItemIt == m_pDataBlock->m_items.end())
	{
		if (m_DataBlockIt == m_dataBlocks.end())
			bRetrieved = false;
		else
		{
			m_pDataBlock = *m_DataBlockIt;
			m_DataBlockIt++;
			m_DataBlockItemIt = m_pDataBlock->m_items.begin();
			item = *m_DataBlockItemIt;
			m_DataBlockItemIt++;
		}
	}
	else
	{
		item = *m_DataBlockItemIt;
		m_DataBlockItemIt++;
	}

	return bRetrieved;
}

DXF_ERROR CDXFReader::NextItem(CDXFReaderItem *pItem)
{
	// Read group code
	NextToken();
	if (m_bEOF) {
		return DXF_ERROR_INCOMPLETE_FILE;
    }

	int32_t nGroupCode = ::atoi(m_sToken);

	// It may be, that the group code is 0, and that moment it cant be decided
	// if it is error or not. So it must checked for this kind of situation.
	if (nGroupCode == 0 && !internal::IsIntNumber(m_sToken)) {
		return DXF_ERROR_BAD_GROUPCODE;
    }
	
	if (pItem) {
		pItem->m_nGroupCode = nGroupCode;
	}

	// Read data that follows the group code
	if (!NextToken())
		return DXF_ERROR_GROUPDATA_MISSING;
	
	if (nGroupCode >= 60 && nGroupCode <= 79 ||
		nGroupCode >= 90 && nGroupCode <= 99 ||
		nGroupCode >= 170 && nGroupCode <= 178 ||
		nGroupCode >= 270 && nGroupCode <= 290 ||
		nGroupCode >= 370 && nGroupCode <= 389 ||
		nGroupCode >= 1060 && nGroupCode <= 1071)
	{
		const int32_t nValue = ::atoi(m_sToken); // An integer number must be read

		if (nValue == 0 && !internal::IsIntNumber(m_sToken)) {
			return DXF_ERROR_BAD_GROUPDATA;
        }

		if (pItem) {
			pItem->m_itemType  = eDxfInt;
			pItem->m_resInt    = nValue;
			pItem->m_resDouble = (double)nValue;
			::_itoa(nValue, pItem->m_resString, 10);
		}
	}
	else if (nGroupCode == 105 || nGroupCode == 5 ||
		     nGroupCode >= 320 && nGroupCode <= 369 ||
		     nGroupCode >= 390 && nGroupCode <= 399)
	{
		if (pItem) {
			pItem->m_itemType = eDxfInt;
			::strcpy(pItem->m_resString, m_sToken);
			pItem->m_resInt    = ::strtol(m_sToken, nullptr, 16);
			pItem->m_resDouble = (double)pItem->m_resInt;
		}
	}
	else if (nGroupCode >= 300 && nGroupCode <= 319 ||
		nGroupCode == 999 || nGroupCode == 100 || nGroupCode == 102 ||
		nGroupCode == 105 || nGroupCode >= 1000 && nGroupCode <= 1009 ||
		nGroupCode >= 0 && nGroupCode <= 4 ||
		nGroupCode >= 6 && nGroupCode <= 9)
	{
		if (pItem) {
			pItem->m_itemType = eDxfString;
			::strcpy(pItem->m_resString, m_sToken);
			pItem->m_resInt    = 0;
			pItem->m_resDouble = 0.0;
		}
	}
	else if (nGroupCode >= 10 && nGroupCode <= 59 ||
		nGroupCode >= 110 && nGroupCode <= 147 ||
		nGroupCode >= 210 && nGroupCode <= 240 ||
		nGroupCode >= 1010 && nGroupCode <= 1059)
	{
		if (pItem) {
			pItem->m_itemType = eDxfDouble;
			::strcpy(pItem->m_resString, m_sToken);
			pItem->m_resDouble = ::strtod(m_sToken, nullptr);
			pItem->m_resInt    = (int32_t)pItem->m_resDouble;
		}
	}

    return DXF_OK;
}

void CDXFReader::PushBackItem()
{
	if (!m_pDataBlock) {
		return;
    }

	CDXFReaderItemIt itemHeadPos = m_pDataBlock->m_items.begin();
	if (itemHeadPos == m_DataBlockItemIt) {
		CDXFBlockIt blockHeadPos = m_dataBlocks.begin();
		if (blockHeadPos != m_DataBlockIt) {
			m_DataBlockIt--;
		}
	}
	else {
		if (m_DataBlockItemIt == m_pDataBlock->m_items.end()) {
			m_DataBlockItemIt = m_pDataBlock->m_items.end();
			m_DataBlockItemIt--;
		}
		else {
			m_DataBlockItemIt--;
		}
	}
}

unsigned long CDXFReader::ReadHandle()
{
	CDXFReaderItem item;
	unsigned long ulHandle = 0;

    item.m_resInt = 0;
	ReadItem(item);
    if (item.m_nGroupCode != 5 && item.m_nGroupCode != 105) {
        ASSERT(FALSE);
        //DXF_ERROR_BAD_GROUPCODE;
    }

	ulHandle = item.m_resInt;
	
	return ulHandle;
}

CDXFDatabase *CDXFReader::GetDatabase() const
{
	return m_pDatabase;
}

DXF_ERROR CDXFReader::ReadDataBlocks()
{
    DeleteDataBlocks();

	CDXFReaderItem item;
	CDXFDataBlock *pDataBlock = nullptr;
	
	while (!m_bEOF && !(item.m_itemType == eDxfString && 0 == ::strcmpi(item.m_resString, "EOF")))
	{
		NextItem(&item);

		if (item.m_nGroupCode == 0) {
			if (pDataBlock) {
				m_dataBlocks.push_back(pDataBlock);
			}
			pDataBlock = new CDXFDataBlock;
		}
		else if (!pDataBlock) {
			return DXF_ERROR_SECTION_MISSING;
        }
			
		pDataBlock->m_items.push_back(item);
	}

	if (pDataBlock) {
		m_dataBlocks.push_back(pDataBlock);
    }

	return DXF_OK;
}

bool CDXFReader::NextToken()
{
	bool bNewLineFound = false;

	m_sToken = m_pCursor;
	while (*m_pCursor != 13 && *m_pCursor != 10 && *m_pCursor != 0)
	{
		++m_pCursor;
	}

	if (*m_pCursor == 13) {
		bNewLineFound = true;

		*m_pCursor = 0;
		// Step through 13 and 10 codes (new line character)
		m_pCursor += 2;
	}
	else if (*m_pCursor == 10) {
		bNewLineFound = true;
		*m_pCursor = 0;
		++m_pCursor;
	}

	// If we are not at the end of the file, the new line symbol at the m_pCursor 
    // convert to string end cursor and move m_pCursor one character to right.
	if (*m_pCursor == 0 && bNewLineFound == false) {
		m_bEOF = true;
	}

	// It may happen that we are trying to read token while already being at the end
	// of the buffer. In that case no token is found and 'false' is returned.
	return (*m_sToken == 0 && !bNewLineFound) ? false : (m_nLine++, true);
}

void CDXFReader::DeleteDataBlocks()
{
    for (auto *pDataBlock : m_dataBlocks) {
        delete pDataBlock;
    }
	m_dataBlocks.clear();
}

bool CDXFReader::GetTableName(char *sTable)
{
	CDXFReaderItem item;
	bool bFound = false;
	static char sKnownTables[][32] =
	{
		"VPORT",
		"LTYPE",
		"LAYER",
		"STYLE",
		"VIEW",
		"UCS",
		"APPID",
		"DIMSTYLE",
		"BLOCK_RECORD",
	};

	::strcpy(sTable, "");

	ReadItem(item);

	if (item.m_nGroupCode != 0) {
		return false;
    }

	if (::strcmp(item.m_resString, "TABLE") != 0) {
		return false;
    }

	ReadItem(item);

	if (item.m_nGroupCode != 2) {
		return false;
    }

	// Now cycle through all known tables and try to find a match;
    const auto nCount = sizeof(sKnownTables) / sizeof(sKnownTables[0]);
	for (auto i = 0; i < nCount; ++i)
	{
		if (::strcmp(sKnownTables[i], item.m_resString) == 0)
		{
			::strcpy(sTable, sKnownTables[i]);
			bFound = true;
			break;
		}
	}

	return bFound;
}

bool CDXFReader::GetEntitySubclassName(char *sSubclassName)
{
	::strcpy(sSubclassName, "");

	if (m_pDataBlock->m_items.empty()) {
		return false;
    }

	const size_t nSize = m_pDataBlock->m_items.size();

	CDXFReaderItem item = m_pDataBlock->m_items[0];
	
	if (item.m_nGroupCode != 0) {
		return false;
    }

	for (size_t i = 1; i < nSize; ++i)
	{
		item = m_pDataBlock->m_items[i];
		if (item.m_nGroupCode == 100) {
			::strcpy(sSubclassName, item.m_resString);
        }
	}

	return ::strlen(sSubclassName) != 0 ? true : false;
}
