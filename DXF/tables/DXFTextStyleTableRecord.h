#ifndef __DXF_TEXT_STYLE_TABLE_RECORD_H__
#define __DXF_TEXT_STYLE_TABLE_RECORD_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
	#include "DXFSymbolTableRecord.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

class CDXFTextStyleTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFTextStyleTableRecord(const char *sName) : CDXFSymbolTableRecord(sName, "STYLE") { }
    CDXFTextStyleTableRecord(const CDXFTextStyleTableRecord &record)  : CDXFSymbolTableRecord(record.GetName(), "STYLE")
    {
        m_fFlags            = record.m_fFlags;
        m_dObliqueAngle     = record.m_dObliqueAngle;
        m_dPriorTextHeight  = record.m_dPriorTextHeight;
        m_dTextHeight       = record.m_dTextHeight;
        m_dWidthFactor      = record.m_dWidthFactor;
        m_fTextFlags        = record.m_fTextFlags;
        m_sBigFontFile      = record.m_sBigFontFile;
        m_sFontFile         = record.m_sFontFile;
    }

    virtual ~CDXFTextStyleTableRecord() { }

// Operations
public: 
    double GetObliqueAngle() const { return m_dObliqueAngle; }
    void SetObliqueAngle(double dObliqueAngle) { m_dObliqueAngle = dObliqueAngle; }
    
	void SetUpsideDown(bool bUpsideDown) {
	    if (bUpsideDown) {
		    m_fTextFlags |= eUpsideDown;
	    }
	    else {
		    m_fTextFlags &= ~eUpsideDown;
	    }
    }

	bool GetUpsideDown() {
	    if (m_fTextFlags &= eUpsideDown) {
		    return true;
	    }
	    else {
		    return false;
	    }
    }

	void SetBackward(bool bBackward) {
	    if (bBackward) {
		    m_fTextFlags |= eBackward;
	    }
	    else {
		    m_fTextFlags &= ~eBackward;
	    }
    }

	bool GetBackward() {
	    if (m_fTextFlags &= eBackward) {
		    return true;
	    }
	    else {
		    return false;
	    }
    }

	// Default text height
    double GetTextHeight() const { return m_dTextHeight; }
    void SetTextHeight(double dTextHeight) { m_dTextHeight = dTextHeight; }

	// Text width factor
    double GetWidthFactor() const { return m_dWidthFactor; }
    void SetWidthFactor(double dWidthFactor) { m_dWidthFactor = dWidthFactor; }

    const char *GetFontFile() const { return m_sFontFile.c_str(); }
    void SetFontFile(const char *sFontFile) { m_sFontFile = sFontFile; }
	
    const char *GetBigFontFile() const { return m_sBigFontFile.c_str(); }
    void SetBigFontFile(const char *sFontFile) { m_sBigFontFile = sFontFile; }

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitTextStyleTableRecord(*this); }

// Attributes
public:
    unsigned char m_fTextFlags {0};
    double m_dObliqueAngle     {0.};
    double m_dTextHeight       {0.};
    double m_dWidthFactor      {1.};
    double m_dPriorTextHeight  {0.};
	std::string m_sBigFontFile;
    std::string m_sFontFile    {"txt"};

	// extended data
    bool        m_bExDataInitiated {false};
	std::string m_sExApplName;
	std::string m_sExFontName;
    int32_t     m_nExFontCode {0};

private:
    unsigned char m_fFlags {0};
	enum {
		eBackward   = 2,
		eUpsideDown = 4
	};
};

#endif