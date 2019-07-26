#ifndef __DXF_BLOCK_TABLE_RECORD_H__
#define __DXF_BLOCK_TABLE_RECORD_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
	#include "DXFSymbolTableRecord.h"
#endif

#ifndef __DXF_3DPOINT_H__
	#include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "../base/DXFLineWeight.h"
#endif

#include "vector"

class CDXFBlockBegin;
class CDXFBlockEnd;
class CDXFEntityVisitor;

class DXFDLLAPI CDXFBlockTableRecord : public CDXFSymbolTableRecord  
{
	friend class CDXFBlockTable; // It's used as a factory to create this object

// Construction/Destruction
public:
    virtual ~CDXFBlockTableRecord() { }

private:
    CDXFBlockTableRecord(const char *sBlockName) : CDXFSymbolTableRecord(sBlockName, "BLOCK_RECORD") { }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitBlockTableRecord(*this); }

// Static operations (utils)
public:
    static void SetBlockBeginID(CDXFBlockTableRecord *pRecord, const CDXFObjectID &id, CDXFDatabase *pDB);
	static void SetBlockEndID(CDXFBlockTableRecord *pRecord, const CDXFObjectID &id, CDXFDatabase *pDB);
    static bool DeleteEntity(CDXFBlockTableRecord *pRecord, const CDXFObjectID &objectID, CDXFDatabase *pDB);

// Methods
public:
	bool SetName(const char *sName);
    void SetLayer(const CDXFObjectID &layerID) { m_layerID = layerID; }
	bool AddEntity(CDXFEntity *pEntity, CDXFObjectID &objectID, CDXFDatabase *pDB);
	bool AddEntity(CDXFEntity *pEntity, CDXFDatabase *pDB);
	bool GetInsertEntityID(CDXFObjectID &objectID);
	
	void VisitEntities(CDXFEntityVisitor &vis);

    const CDXF3DPoint &GetOrigin() const { return m_pointOrigin; }
    void SetOrigin(const CDXF3DPoint &origin);

private:
	void CreateBeginEndEntities(CDXFDatabase *pDB);

public:
    const CDXFBlockBegin *GetBlockBegin() const;
    const CDXFBlockEnd   *GetBlockEnd() const;

    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }
    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }

// Attributes
public:
	std::vector<CDXFObjectID> m_entities;

	CDXFObjectID m_layerID;
	CDXFObjectID m_insertEntityID;

private:
    CDXFObjectID m_blockBeginID;
	CDXFObjectID m_blockEndID;

    CDXF3DPoint m_pointOrigin {0., 0., 0.};                    // m_blockBeginID object attribute -> base point
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif
