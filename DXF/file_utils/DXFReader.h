#ifndef _DXF_READER_H__
#define _DXF_READER_H__
#pragma once

#include "vector"

class CDXFDatabase;

enum DXF_ERROR
{
    DXF_ERROR_EOF = 1,
    DXF_ERROR_BAD_GROUPCODE,
    DXF_ERROR_BAD_GROUPDATA,
    DXF_ERROR_SYMBOL_TABLE_MISSING,
    DXF_ERROR_SECTION_MISSING,
    DXF_ERROR_HEADER_SECTION_MISSING,
    DXF_ERROR_TABLES_SECTION_MISSING,
    DXF_ERROR_BLOCKS_SECTION_MISSING,
    DXF_ERROR_ENTITIES_SECTION_MISSING,
    DXF_ERROR_UNRECOGNIZED_BLOCK,
    DXF_ERROR_UNRECOGNIZED_LINETYPE,
    DXF_ERROR_UNRECOGNIZED_LAYER,
    DXF_ERROR_MODELSPACE_BLOCK_MISSING,
    DXF_ERROR_INCOMPLETE_FILE,
    DXF_ERROR_GROUPDATA_MISSING,
    DXF_ERROR_UNSUPPORTED_VERSION,
    DXF_ERROR_TABLE_EXPECTED,
    DXF_ERROR_VPORT_TABLE_EXPECTED,
    DXF_ERROR_LTYPE_TABLE_EXPECTED,
    DXF_ERROR_LAYER_TABLE_EXPECTED,
    DXF_ERROR_STYLE_TABLE_EXPECTED,
    DXF_ERROR_VIEW_TABLE_EXPECTED,
    DXF_ERROR_UCS_TABLE_EXPECTED,
    DXF_ERROR_APPID_TABLE_EXPECTED,
    DXF_ERROR_DIMSTYLE_TABLE_EXPECTED,
    DXF_ERROR_BLOCK_RECORD_TABLE_EXPECTED,
    DXF_ERROR_CAN_NOT_OPEN_FILE,
    DXF_OK
};

enum EItemType
{
    eDxfInt,
    eDxfDouble,
    eDxfString,
};

class CDXFReaderItem
{
// Construction/Destruction
public:
    CDXFReaderItem() { }
    ~CDXFReaderItem() { }

// Attributes
public:
    int32_t m_resInt {0};
    double  m_resDouble;
    char    m_resString[256];

    int32_t m_nGroupCode {-1};
    EItemType m_itemType {eDxfInt};
};

class CDXFDataBlock;

class CDXFReader  
{
    typedef std::vector<CDXFReaderItem> CDXFReaderItemVector;
    typedef CDXFReaderItemVector::iterator CDXFReaderItemIt;

    typedef std::vector<CDXFDataBlock *> CDXFDataBlockVector;
    typedef CDXFDataBlockVector::iterator CDXFBlockIt;

// Construction/Destruction
public:
    CDXFReader(CDXFDatabase *pDatabase);
    ~CDXFReader();

// Operations
public:
    bool Open(const wchar_t *sFilePath);

    bool ReadItem(CDXFReaderItem &item);
    void PushBackItem();

    // Analyzes the current data block and tries to detect if that block describes
    // any known database table.
    // sTable    If the current data block describes database table, then this argument is filled
    //            with that name, otherwise it is set to empty string.
    // return: true  - if the current data block describes database table
    //           false - otherwise
    bool GetTableName(char *sTable);
    
    // Analyzes the current data block and tries to detect if it describes an entity.
    // If it does, then name of the entity subclass is taken and returned.
    // sSubclassName - This parameter is filled in with the name of the entity subclass.
    // return: true - if name of the entity subclass can be extracted;
    //           false - otherwise.
    bool GetEntitySubclassName(char *sSubclassName);

    CDXFDatabase * GetDatabase() const;
    unsigned long ReadHandle();

private:
    DXF_ERROR NextItem(CDXFReaderItem *pItem);
    bool NextToken();
    DXF_ERROR ReadDataBlocks();
    void DeleteDataBlocks();

// Attributes
private:
    char *m_pData {nullptr}; // Buffer, where all file is read
    char *m_pCursor;         // This is the current cursor position in the buffer
    char *m_sToken;             // Token that is read from the buffer using NextToken() function
    int32_t m_nLine;         // Current line
    bool m_bEOF {true};      // Cursor is at the end of file
    bool m_bItemInBuffer {false};

    CDXFBlockIt m_DataBlockIt;
    CDXFDataBlock *m_pDataBlock {nullptr};
    CDXFReaderItemIt m_DataBlockItemIt;

    CDXFDatabase *m_pDatabase {nullptr};

    CDXFDataBlockVector m_dataBlocks;
};

#endif
