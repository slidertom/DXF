#ifndef __DXF_BLOCKBEGIN_H__
#define __DXF_BLOCKBEGIN_H__
#pragma once

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

class CDXFBlockBegin : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFBlockBegin() : CDXFEntity("BLOCK") { }
    CDXFBlockBegin(const char *sBlockName, const CDXF3DPoint &base_point) : CDXFEntity("BLOCK"), m_sBlockName(sBlockName), m_basePoint(base_point) { }
    CDXFBlockBegin(const CDXFBlockBegin &blockBegin) : CDXFEntity(blockBegin, "BLOCK"),
        m_sBlockName(blockBegin.m_sBlockName),      
        m_sXRefPathName(blockBegin.m_sXRefPathName),
        m_sDescription(blockBegin.m_sDescription),
        m_nFlags(blockBegin.m_nFlags),
        m_basePoint(blockBegin.m_basePoint) { }
    virtual ~CDXFBlockBegin() { }

// Override
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFBlockBegin(*this); }

// Operations
public:    
    const char *GetBlockName() const { return m_sBlockName.c_str(); }
    void SetBlockName(const char *sName) { m_sBlockName = sName; }

    const char *GetXRefPathName() const { return m_sXRefPathName.c_str(); }
    void SetXRefPathName(const char *sXRefPath) { m_sXRefPathName = sXRefPath; }

    const char *GetDescr() const { return m_sDescription.c_str();  }
    void SetDescr(const char *sDescription) { m_sDescription = sDescription; }

    const CDXF3DPoint &GetBasePoint() const { return m_basePoint; }
    void SetBasePoint(const CDXF3DPoint &basePoint) { m_basePoint = basePoint; }

    const int32_t GetFlags() const { return m_nFlags; }
    void SetFlags(int32_t nFlags) { m_nFlags = nFlags; }

// Attributes
private:
    std::string m_sBlockName;
    std::string m_sXRefPathName;
    std::string m_sDescription;

    int32_t m_nFlags {0};

    CDXF3DPoint m_basePoint;
};

#endif
