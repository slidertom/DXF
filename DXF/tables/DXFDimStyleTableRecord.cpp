#include "stdafx.h"
#include "DXFDimStyleTableRecord.h"

#include "DXFDatabase.h"
#include "DXFBlockTableRecord.h"	
#include "DXFTextStyleTableRecord.h"

#include "entity/DXFLine.h"

CDXFDimStyleTableRecord &CDXFDimStyleTableRecord::operator=(const CDXFDimStyleTableRecord &record)
{
	CDXFSymbolTableRecord::operator=(record);

	m_nDimUPt    = record.m_nDimUPt;
	m_nDimAltTZ  = record.m_nDimAltTZ;
	m_nDimAltZ   = record.m_nDimAltZ;
	m_nDimTZin   = record.m_nDimTZin;
	m_nDimTolJ   = record.m_nDimTolJ;
	m_nDimSD2    = record.m_nDimSD2;
	m_nDimSD1    = record.m_nDimSD1;
	m_nDimJust   = record.m_nDimJust;
	m_nDimTMove  = record.m_nDimTMove;
	m_nDimDSep   = record.m_nDimDSep;
	m_nDimLUnit  = record.m_nDimLUnit;
	m_nDimFrac   = record.m_nDimFrac;
	m_nDimAUnit  = record.m_nDimAUnit;
	m_nDimAltTD  = record.m_nDimAltTD;
	m_nDimAltU   = record.m_nDimAltU;
	m_nDimTDec   = record.m_nDimTDec;
	m_nDimDec    = record.m_nDimDec;
	m_nDimADec   = record.m_nDimADec;
	m_nDimClrt   = record.m_nDimClrt;
	m_nDimClre   = record.m_nDimClre;
	m_nDimClrd   = record.m_nDimClrd;
	m_nDimSoxd   = record.m_nDimSoxd;
	m_nDimTix    = record.m_nDimTix;
	m_nDimSah    = record.m_nDimSah;
	m_nDimTofl   = record.m_nDimTofl;
	m_nDimAltd   = record.m_nDimAltd;
	m_nDimAlt    = record.m_nDimAlt;
	m_nDimAzin   = record.m_nDimAzin;
	m_nDimZin    = record.m_nDimZin;
	m_nDimTad    = record.m_nDimTad;
	m_nDimSe2    = record.m_nDimSe2;
	m_nDimSe1    = record.m_nDimSe1;
	m_nDimToh    = record.m_nDimToh;
	m_nDimTih    = record.m_nDimTih;
	m_nDimLim    = record.m_nDimLim;
	m_nDimTol    = record.m_nDimTol;
	m_dDimAltRnd = record.m_dDimAltRnd;
	m_dDimGap    = record.m_dDimGap;
	m_dDimTFac   = record.m_dDimTFac;
	m_dDimTvp    = record.m_dDimTvp;
	m_dDimLFac   = record.m_dDimLFac;
	m_dDimAltf   = record.m_dDimAltf;
	m_dDimTsz    = record.m_dDimTsz;
	m_dDimCen    = record.m_dDimCen;
	m_dDimTxt    = record.m_dDimTxt;
	m_dDimTm     = record.m_dDimTm;
	m_dDimTp     = record.m_dDimTp;
	m_dDimDle    = record.m_dDimDle;
	m_dDimRnd    = record.m_dDimRnd;
	m_dDimExe    = record.m_dDimExe;
	m_dDimDli    = record.m_dDimDli;
	m_dDimExo    = record.m_dDimExo;
	m_dDimASz    = record.m_dDimASz;
	m_dDimScale  = record.m_dDimScale;
	m_sDimpost   = record.m_sDimpost;
	m_sDimapost  = record.m_sDimapost;

    return *this;
}

void CDXFDimStyleTableRecord::SetDimBlk0(const CDXFObjectID & id)
{
	m_dimBlkID = id;
}

void CDXFDimStyleTableRecord::SetDimBlk1(const CDXFObjectID & id)
{
	m_dimBlk1ID = id;
}

void CDXFDimStyleTableRecord::SetDimBlk2(const CDXFObjectID & id)
{
	m_dimBlk2ID = id;
}

const CDXFObjectID &CDXFDimStyleTableRecord::GetDimLdrBlk(CDXFDatabase *pDB)
{
	if (m_dimLdrBlkID.IsNull()) {
		ASSERT(pDB);
		m_dimLdrBlkID = pDB->GetDXFObjectID(m_ulDimLdrBlkHandle);
	}
	return m_dimLdrBlkID;
}

const CDXFObjectID &CDXFDimStyleTableRecord::GetDimBlk0(CDXFDatabase *pDB)
{
    if (m_dimBlkID.IsNull()) {
        ASSERT(pDB);
        m_dimBlkID = pDB->GetDXFObjectID(m_ulDimBlkHandle);
    }

	return m_dimBlkID;
}

const CDXFObjectID &CDXFDimStyleTableRecord::GetDimBlk1(CDXFDatabase *pDB)
{
    if (m_dimBlk1ID.IsNull()) {
        ASSERT(pDB);
        m_dimBlk1ID = pDB->GetDXFObjectID(m_ulDimBlk1Handle);
    }
	return m_dimBlk1ID;
}

const CDXFObjectID &CDXFDimStyleTableRecord::GetDimBlk2(CDXFDatabase *pDB)
{
    if (m_dimBlk2ID.IsNull()) {
        ASSERT(pDB);
        m_dimBlk2ID = pDB->GetDXFObjectID(m_ulDimBlk2Handle);
    }
	return m_dimBlk2ID;
}

void CDXFDimStyleTableRecord::SetDimTxSty(const CDXFObjectID &id)
{
    m_dimTxStyID = id;
}

const CDXFObjectID &CDXFDimStyleTableRecord::GetDimTxSty()
{
	return m_dimTxStyID;
}