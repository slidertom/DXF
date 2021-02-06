#ifndef __DXF_DIM_STYLE_TABLE_RECORD_H__
#define __DXF_DIM_STYLE_TABLE_RECORD_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
    #include "DXFSymbolTableRecord.h"
#endif

class DXFDLLAPI CDXFDimStyleTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFDimStyleTableRecord(const CDXFDimStyleTableRecord &x) : CDXFSymbolTableRecord(x.GetName(), "DIMSTYLE") { *this = x; }
    CDXFDimStyleTableRecord(const char *sName) : CDXFSymbolTableRecord(sName, "DIMSTYLE") { }
    virtual ~CDXFDimStyleTableRecord() { }

// Operators
public:
    CDXFDimStyleTableRecord &operator=(const CDXFDimStyleTableRecord &point);

public:
    void SetDimBlk0(const CDXFObjectID &id);
    void SetDimBlk1(const CDXFObjectID &id);
    void SetDimBlk2(const CDXFObjectID &id);
    void SetDimTxSty(const CDXFObjectID &id);

    const CDXFObjectID &GetDimLdrBlk(CDXFDatabase *pDB);
    const CDXFObjectID &GetDimBlk0(const CDXFDatabase *pDB);
    const CDXFObjectID &GetDimBlk1(const CDXFDatabase *pDB);
    const CDXFObjectID &GetDimBlk2(const CDXFDatabase *pDB);
    const CDXFObjectID &GetDimTxSty();

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitDimStyletableRecord(*this); }

public:
    int32_t m_nDimUPt    {0};
    int32_t m_nDimAltTZ  {0};
    int32_t m_nDimAltZ   {0};
    int32_t m_nDimTZin   {0};
    int32_t m_nDimTolJ   {1};
    int32_t m_nDimSD2    {0};
    int32_t m_nDimSD1    {0};
    int32_t m_nDimJust   {0};
    int32_t m_nDimTMove  {0};
    int32_t m_nDimDSep   {44};
    int32_t m_nDimLUnit  {6};
    int32_t m_nDimFrac   {0};
    int32_t m_nDimAUnit  {0};
    int32_t m_nDimAltTD  {4};
    int32_t m_nDimAltU   {8};
    int32_t m_nDimTDec   {4};
    int32_t m_nDimDec    {4};
    int32_t m_nDimADec   {0};
    int32_t m_nDimClrt   {0};
    int32_t m_nDimClre   {0};
    int32_t m_nDimClrd   {0};
    int32_t m_nDimSoxd   {0};
    int32_t m_nDimTix    {0};
    int32_t m_nDimSah    {0};
    int32_t m_nDimTofl   {1};
    int32_t m_nDimAltd   {4};
    int32_t m_nDimAlt    {0};
    int32_t m_nDimAzin   {0};
    int32_t m_nDimZin    {8};
    int32_t m_nDimTad    {1};
    int32_t m_nDimSe2    {0};
    int32_t m_nDimSe1    {0};
    int32_t m_nDimToh    {0};
    int32_t m_nDimTih    {0};
    int32_t m_nDimLim    {0};
    int32_t m_nDimTol    {0};
    int32_t m_nDimAtfFit {3};
    double m_dDimAltRnd  {0.0};
    double m_dDimGap     {0.625};
    double m_dDimTFac    {1.0};
    double m_dDimTvp     {0.0};
    double m_dDimLFac    {1.0};
    double m_dDimAltf    {0.0394};
    double m_dDimTsz     {0.0};
    double m_dDimCen     {2.5};
    double m_dDimTxt     {2.5};
    double m_dDimTm      {0.0};
    double m_dDimTp      {0.0};
    double m_dDimDle     {0.0};
    double m_dDimRnd     {0.0};
    double m_dDimExe     {1.25};
    double m_dDimDli     {3.75};
    double m_dDimExo     {0.0625};
    double m_dDimASz     {2.5};
    double m_dDimScale   {1.0};

    std::string m_sDimpost;
    std::string m_sDimapost;

    //BLOCK_RECORD table is read later than DIMSTYLE.
    //The handle provides a possibility to connect to required block by the unique value.
    unsigned long m_ulDimLdrBlkHandle {0};
    unsigned long m_ulDimBlkHandle    {0};
    unsigned long m_ulDimBlk1Handle   {0};
    unsigned long m_ulDimBlk2Handle   {0};

private:
    unsigned char m_fFlags {0};
    CDXFObjectID m_dimLdrBlkID;
    CDXFObjectID m_dimBlkID;
    CDXFObjectID m_dimBlk1ID;
    CDXFObjectID m_dimBlk2ID;
    CDXFObjectID m_dimTxStyID;
};

#endif
