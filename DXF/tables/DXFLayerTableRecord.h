#ifndef __DXF_LAYER_TABLE_RECORD_H__
#define __DXF_LAYER_TABLE_RECORD_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
	#include "DXFSymbolTableRecord.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "../base/DXFLineWeight.h"
#endif

#ifndef __DXF_COLOR_H__
    #include "../base/DXFColor.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

class CDXFLayerTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFLayerTableRecord(const char *sLayerName) : CDXFSymbolTableRecord(sLayerName, "LAYER") { }
    CDXFLayerTableRecord(const CDXFLayerTableRecord &record) : CDXFSymbolTableRecord(record.GetName(), "LAYER")
    {
        m_fFlags     = record.m_fFlags;
        m_nColor     = record.m_nColor;
        m_nPlottable = record.m_nPlottable;
        SetLineWeight(record.GetLineWeight());
    }
    virtual ~CDXFLayerTableRecord() { }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitLayerTableRecord(*this); }

// Operations
public:
    const CDXFObjectID &GetLinetypeID() const { return m_linetypeID; }
    void SetLinetype(const CDXFObjectID &linetypeID) { m_linetypeID = linetypeID; }

    EDXFLineWeight GetLineWeight() const { return m_eLineWeight; }
    void SetLineWeight(EDXFLineWeight eLineWeight) { m_eLineWeight = eLineWeight; }

    const CDXFObjectID & GetPlotStyleNameID() const { return m_plotStyleNameID; }
    void SetPlotStyleName(const CDXFObjectID &plotStyleName) { m_plotStyleNameID = plotStyleName; }

    bool IsPlottable() const { return m_nPlottable == 0 ? false : true; }
    void SetPlottable(bool bPlottable) { m_nPlottable = bPlottable ? 1 : 0; }

    EDXFColor GetColor() const { return m_nColor; }
    void SetColor(EDXFColor nColor) { m_nColor = nColor; }

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

private:
    EDXFColor      m_nColor           {EDXFColor::EDXFColorWhite};
    int32_t        m_nPlottable       {1};
    CDXFObjectID   m_linetypeID;
    CDXFObjectID   m_plotStyleNameID;
    EDXFLineWeight m_eLineWeight      {EDXFLineWeight::LW_Default};
    unsigned char  m_fFlags           {0};
};

#endif
