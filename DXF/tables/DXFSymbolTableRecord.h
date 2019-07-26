#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
#define __DXF_SYMBOL_TABLE_RECORD_H__
#pragma once

#ifndef __DXF_OBJECT_H__
	#include "../base/DXFObject.h"
#endif

class CDXFRecordVisitor;

class CDXFSymbolTableRecord : public CDXFObject
{	
// Construction/Destruction
public:
    CDXFSymbolTableRecord() = delete;
    CDXFSymbolTableRecord(const char *sName, const char *sObjectName) : CDXFObject(sObjectName), m_sName(sName)  {  }
	CDXFSymbolTableRecord(const CDXFSymbolTableRecord &record, const char *sObjectName) 
        : CDXFObject(sObjectName), m_sName(record.m_sName){ }
    virtual ~CDXFSymbolTableRecord() { }

// Operations
public:
    const char *GetName() const { return m_sName.c_str(); }
    void SetName(const char *sName) { m_sName = sName; }

// Overrides 
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) = 0;
    virtual void Accept(CDXFObjectVisitorAbs &vis) override { vis.VisitDXFSymbolTableRecord(*this); }

// Operations
public:
    CDXFSymbolTableRecord &operator=(const CDXFSymbolTableRecord &record) {
        m_sName = record.m_sName.c_str();
        return *this;
    }

// Attributes
private:
    std::string m_sName;
};

#ifndef __DXF_RECORD_VISITOR_H__
    #include "DXFRecordVisitor.h"
#endif

class CDXFAppIDTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFAppIDTableRecord(const char *sName) : CDXFSymbolTableRecord(sName, "APPID") { }
    virtual ~CDXFAppIDTableRecord() { }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitAppIDtableRecord(*this); }

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

private:
    unsigned char m_fFlags {0};
};

class CDXFUCSTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFUCSTableRecord() : CDXFSymbolTableRecord("", "UCS") { }
    virtual ~CDXFUCSTableRecord() { }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitUCSTableRecord(*this); }
};

#endif
