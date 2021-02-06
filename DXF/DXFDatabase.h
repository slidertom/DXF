#ifndef __DXF_DATABASE_H__
#define __DXF_DATABASE_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

#ifndef __DXF_VIEW_TABLE_H__
    #include "tables/DXFViewTable.h"
#endif

#ifndef __DXF_LINE_TYPETABLE_H__
    #include "tables/DXFLineTypeTable.h"    
#endif

#ifndef __DXF_LAYER_TABLE_H__
    #include "tables/DXFLayerTable.h"    
#endif

#ifndef __DXF_BLOCK_TABLE_H__
    #include "tables/DXFBlockTable.h"    
#endif

#ifndef __DXF_APPIDTABLE_H__
    #include "tables/DXFAppIDTable.h"
#endif

#ifndef __DXF_UCS_TABLE_H__
    #include "tables/DXFUCSTable.h"
#endif

#ifndef __DXF_TEXT_STYLE_TABLE_H__
    #include "tables/DXFTextStyleTable.h"
#endif

#ifndef __DXF_DIM_STYLE_TABLE_H__
    #include "tables/DXFDimStyleTable.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "base/DXF3DPoint.h"
#endif

#ifndef __DXF_OBJECTID_H__
    #include "base/DXFObjectID.h"
#endif

#include "array"
#include "unordered_map"

#define DXF_MODEL_SPACE "*MODEL_SPACE"
#define DXF_PAPER_SPACE "*PAPER_SPACE"

class CDXFObject;
class CDXFDictionary;

class CDXFVariables
{
public:
    double m_dTraceWid          {1.0};
    double m_dTextSize          {2.5};
    unsigned char m_nOSMode     {0};
    double m_dLtScale           {1.0};
    unsigned char m_nDragMode   {2};
    unsigned char m_nMirrText   {1};
    unsigned char m_nQTextMode  {0};
    unsigned char m_nFillMode   {1};
    unsigned char m_nRegenMode  {1};
    unsigned char m_nOrthoMode  {0};
    CDXF3DPoint m_pointLimMax   {1000., 1000., 1000.};
    CDXF3DPoint m_pointLimMin   {0., 0., 0.};
    CDXF3DPoint m_pointExtMax   {-1E+20, -1E+20, -1E+20};
    CDXF3DPoint m_pointExtMin   {1E+20, 1E+20, 1E+20};
    CDXF3DPoint m_pointInsBase  {0., 0., 0.};

    double m_dCeLtScale         {1.};
    unsigned long m_dwCeColor   {256};
    short m_nAUPrec             {0};
    short m_nAUnits             {0}; // Stands for Angle units in DXF documentation
    short m_nInsUnits           {4}; // Stands for default drawing units for AutoCAD DesignCenter blocks. 4 = mm.
    short m_nAttMode            {1};
    short m_nAngDir             {0};
    double m_dAngBase           {0.};
    int32_t m_nHandSeed         {1};
};

class DXFDLLAPI CDXFDatabase final  
{
    friend class CDXFBlockTableRecord;
    
// Construction/Destruction
public:
    CDXFDatabase(const CDXFDatabase &x) = delete;
    CDXFDatabase(CDXFDatabase &&x) = delete;
    CDXFDatabase();
    ~CDXFDatabase();

// Operators
public:
    CDXFDatabase &operator=(const CDXFDatabase &dxf) = delete;

// Operations
public:
    void AddObject(CDXFObject *pObject, CDXFObjectID &id);
    bool AddObject(CDXFObject *pObject, CDXFObjectID &id, unsigned long ulHandle);
    bool GetObjectID(CDXFObjectID &id, unsigned long ulHandle) const;

    CDXFViewTable      *GetViewTable() const;
    void                SetViewTable(const CDXFObjectID &id);

    CDXFViewportTable  *GetViewPortTable() const;
    void                SetViewPortTable(const CDXFObjectID &id);

    CDXFUCSTable       *GetUCSTable() const;
    void                SetUCSTable(const CDXFObjectID &id);

    CDXFTextStyleTable *GetTextStyleTable() const;
    void                SetTextStyleTable(const CDXFObjectID &id);

    CDXFDimStyleTable  *GetDimStyleTable() const;
    void                SetDimStyleTable(const CDXFObjectID &id);

    CDXFAppIDTable     *GetAppIDTable() const;
    void                SetAppIDTable(const CDXFObjectID &id);

    CDXFLineTypeTable  *GetLineTypeTable() const;
    void                SetLineTypeTable(const CDXFObjectID &id);

    CDXFLayerTable     *GetLayerTable() const;
    void                SetLayerTable(const CDXFObjectID &id);

    CDXFBlockTable     *GetBlockTable() const;
    void                SetBlockTable(const CDXFObjectID &id);

    CDXFDictionary     *GetNamedObjectsDictionary();

    CDXFObjectID GetDXFObjectID(unsigned long ulHandle) const;

    CDXFObjectID GetTextStyleIdByName(const char *sTextStyleName) const;
    CDXFObjectID GetLayerIdByName(const char *sLayerName) const;
    CDXFObjectID GetDimStyleIdByName(const char *sDimStyleName) const;

    // returns default object ids if item is not found by name
    CDXFObjectID GetTextStyleIdByNameEx(const char *sTextStyleName) const;
    CDXFObjectID GetLayerIdByNameEx(const char *sLayerName) const;
    CDXFObjectID GetDimStyleIdByNameEx(const char *sDimStyleName) const;

public:
    bool OpenDatabase(const wchar_t *sFileName);

// Static operations 
public:
    static bool ReadBlocks(const wchar_t *sFilePath, std::vector<std::string> &arDXFBlocks);

private:
    void DeleteAllObjects();
    bool DeleteObject(const CDXFObjectID &id);
    void InitDatabase();

// Attributes
public:
    CDXFVariables m_vars;

public:
    enum DXF_TABLE
    {
        DXF_BLOCK_TABLE     = 0,
        DXF_LINETYPE_TABLE  = 1,
        DXF_LAYER_TABLE     = 2,
        DXF_APPID_TABLE     = 3,
        DXF_DIMSTYLE_TABLE  = 4,
        DXF_TEXTSTYLE_TABLE = 5,
        DXF_UCS_TABLE       = 6,
        DXF_VIEW_TABLE      = 7,
        DXF_VIEWPORT_TABLE  = 8,

        DXF_TABLE_COUNT     = 9,
    };

private:
    std::array<CDXFObjectID, DXF_TABLE_COUNT> m_tablesIDs;
    std::unordered_map<unsigned long, CDXFObjectID *> m_objectIDPtrs;
    CDXFObjectID m_namedObjectsDictionaryID;
};

#endif