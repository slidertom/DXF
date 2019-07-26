#ifndef __DXF_DIMENSION_H__
#define __DXF_DIMENSION_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_ENTITY_H__ 
	#include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
	#include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_BLOCKTABLERECORD_H__
	#include "../tables/DXFBlockTableRecord.h"
#endif

#ifndef __DXF_DIMSTYLETABLERECORD_H__
	#include "../tables/DXFDimStyleTableRecord.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

#ifndef __DXF_DIMENSION_VISITOR_H__
    #include "DXFDimensionVisitor.h"
#endif

enum EAttachmentPoint
{
	eTopLeft = 1,
	eTopCenter,
	eTopRight,
	eMiddleLeft,
	eMiddleCenter,
	eMiddleRight,
};

class CDXFText;

class DXFDLLAPI CDXFDimension : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFDimension() : CDXFEntity("DIMENSION") { }
	CDXFDimension(const CDXFDimension &dimension);
    virtual ~CDXFDimension() { delete m_pOverrideStyle; }

// Operations
public:
	void SetDimStyle(const CDXFObjectID &dimStyleID);
    void SetDimBlock(const CDXFObjectID &dimBlockID) { m_dimBlockID = dimBlockID; }
    const CDXFObjectID &GetDimBlock() { return m_dimBlockID; }

    const char *GetDimText() const { return m_sDimensionText.c_str(); }
	void SetDimText(const char *sDimText);
    
	void SetDimTextHeight(double dHeight);

    bool GetOverrideDimText() const { return m_bOverrideDimText; }
	// sets dimension text to override mode, 
	// so that it was not recalculated after exploding dimensions block
    void SetOverrideDimText(bool bOverride) { m_bOverrideDimText = bOverride; }

    int32_t GetDimSah() const { ASSERT(m_pOverrideStyle); return m_pOverrideStyle->m_nDimSah; }
    void SetDimSah(int32_t nValue) { m_pOverrideStyle->m_nDimSah = nValue; }

    double GetDimScale() const      { ASSERT(m_pOverrideStyle); return m_pOverrideStyle->m_dDimScale;   }
    void SetDimScale(double dValue) { ASSERT(m_pOverrideStyle); m_pOverrideStyle->m_dDimScale = dValue; }

    double GetDimASz() const { ASSERT(m_pOverrideStyle); return m_pOverrideStyle->m_dDimASz; }
    void SetDimASz(double dValue) { ASSERT(m_pOverrideStyle); m_pOverrideStyle->m_dDimASz = dValue; }

    const CDXFObjectID &GetDimBlk0(CDXFDatabase *pDB) const         { ASSERT(m_pOverrideStyle); return m_pOverrideStyle->GetDimBlk0(pDB); }
    const CDXFObjectID &GetDimBlk1(CDXFDatabase *pDB) const         { ASSERT(m_pOverrideStyle); return m_pOverrideStyle->GetDimBlk1(pDB); }
    const CDXFObjectID &GetDimBlk2(CDXFDatabase *pDB) const         { ASSERT(m_pOverrideStyle); return m_pOverrideStyle->GetDimBlk2(pDB); }

    bool GetDrawText() const { return m_bDrawText; }
    void EnableDimText(bool bEnable) { m_bDrawText = bEnable; }

    void SetFlags(unsigned char flags) { m_fFlags = flags; }
    unsigned char GetFlags() const { return m_fFlags; }    

    const CDXF3DPoint &GetTextMiddlePoint() const { return m_textMiddlePoint; }
    void SetTextMiddlePoint(const CDXF3DPoint &midpoint) { m_textMiddlePoint = midpoint; }

    void EnableTextPosOverride(bool bEnable) { m_bExportTextAttachPoint = bEnable; }
    void EnableBlockRef(bool bEnable) { m_bEnableBlockRef = bEnable; }

    static bool Init(CDXFDimension *pDimension, CDXFDatabase *pDB);
    static void AddDefaultArrowBlock(CDXFDimension *pDim, CDXFBlockTableRecord *pDimBlock, CDXFDatabase *pDB, const CDXF3DPoint &dimPoint, double dAngle);
    static void AddArrowBlock(CDXFDimension *pDim, CDXFBlockTableRecord *pDimBlock, const CDXF3DPoint &dimPoint, const CDXFObjectID &arrowBlock, double dAngle, CDXFDatabase *pDB);
    static void GetArrowBlocks(CDXFDimension *pDim, CDXFDatabase *pDB, CDXFObjectID &arrowBlock1, CDXFObjectID &arrowBlock2);

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFDimension(*this); }
    virtual void AcceptDim(CDXFDimensionVisitor &vis) = 0;

public:
	CDXFObjectID m_dimBlockID;
	CDXFObjectID m_dimStyleID;
	CDXF3DPoint m_defPoint;

    std::string m_sOverridenDimText;
    std::string m_sDimensionText;

    bool m_bEnableBlockRef        {true};
    bool m_bExportTextAttachPoint {false};

    EAttachmentPoint m_eAttachmentPoint {eMiddleCenter};
    void SetDimTextEntity(CDXFText *pDimText);

    CDXFDimStyleTableRecord *m_pOverrideStyle {nullptr};

// Attributes
private:
    bool m_bOverrideDimText {false};
    CDXFText *m_pDimText    {nullptr};
    double m_dDimTextHeight {5.0};
    bool m_bDrawText        {true};
    CDXF3DPoint m_textMiddlePoint; // stored in the file
    unsigned char m_fFlags {0};
};

#endif
