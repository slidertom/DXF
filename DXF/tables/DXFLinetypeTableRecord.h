#ifndef __DXF_LINETYPETABLERECORD_H__
#define __DXF_LINETYPETABLERECORD_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
	#include "DXFSymbolTableRecord.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#define MAX_DASHES 255

#define DXF_LINE_TYPE_CONTINUOUS "CONTINUOUS"
#define DXF_LINE_TYPE_DASHDOT    "DASHDOT"
#define DXF_LINE_TYPE_DIVIDE     "DIVIDE"
#define DXF_LINE_TYPE_DASHED     "DASHED"
#define DXF_LINE_TYPE_DOT        "DOT"

class CDXFLineTypeTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFLineTypeTableRecord(const char *sName) : CDXFSymbolTableRecord(sName, "LTYPE") { }
	CDXFLineTypeTableRecord(const CDXFLineTypeTableRecord &record) : CDXFSymbolTableRecord(record, "LTYPE") {
	    m_dPatternLength = record.m_dPatternLength;
	    m_sDescription   = record.m_sDescription.c_str();
	    m_nAlignmentCode = record.m_nAlignmentCode;
	    m_nNumDashes     = record.m_nNumDashes;
        ::memcpy(m_dDashLengths, record.m_dDashLengths, sizeof m_dDashLengths[0] * m_nNumDashes);
    }
    virtual ~CDXFLineTypeTableRecord() { }

// Operations
public:
	void SetDashLength(int32_t nIndex, double dLength) {
	    ASSERT(nIndex < m_nNumDashes);
	    m_dDashLengths[nIndex] = dLength;
	    if (dLength > 0) {
		    m_dPatternLength += dLength;
	    }
	    else {
		    m_dPatternLength -= dLength;
	    }
    }

	void SetNumDashes(int32_t nNumDashes) {
	    if (nNumDashes >= 0 && nNumDashes <= MAX_DASHES) {
		    m_nNumDashes = nNumDashes;
	    }
    }

    const char *GetDescr() const { return m_sDescription.c_str(); }
    void SetDescr(const char *sDescr) { m_sDescription = sDescr; }

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitLineTypeTableRecord(*this); }

// Attributes
public:
    double m_dPatternLength  {0.};
    int32_t m_nAlignmentCode {65}; // Always set to 65
	double m_dDashLengths[MAX_DASHES];
    int32_t m_nNumDashes {0};

private:
    std::string m_sDescription;
    unsigned char m_fFlags {0};
};

#endif
