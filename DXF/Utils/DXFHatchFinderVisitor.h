#ifndef __DXF_HATCH_FINDER_VISITOR_H__
#define __DXF_HATCH_FINDER_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor"
#endif

#ifndef __DXF_HATCH_H__
    #include "../entity/DXFHatch.h"
#endif

class CDXFHatchFinderVisitor : public CDXFEntityVisitor
{
// Construction/Destruction
public:
    CDXFHatchFinderVisitor() { }
    virtual ~CDXFHatchFinderVisitor() { }

// Operations
public:
    bool HatchExists() const    { return m_bHatchExists; }
    CDXFHatch *GetHatchEntity() { return m_pHatchEntity; }

// Overrides
protected:
    virtual void VisitDXFHatch(CDXFHatch &entity) override {
        m_bHatchExists = true;
        m_pHatchEntity = &entity;
    }

    virtual void VisitDXFArc(CDXFArc &entity) override { }
    virtual void VisitDXFCircle(CDXFCircle &entity) override { }
    virtual void VisitDXFDimension(CDXFDimension &entity) override { }
    virtual void VisitDXFLine(CDXFLine &entity) override { }
    virtual void VisitDXFMText(CDXFMText &entity) override { }
    virtual void VisitDXFPoint(CDXFPoint &entity) override { }
    virtual void VisitDXFPolyline(CDXFPolyline &entity) override { }
    virtual void VisitDXFSolid(CDXFSolid &entity) override { }
    virtual void VisitDXFText(CDXFText &entity) override { }
    virtual void VisitDXFInsert(CDXFInsert &entity) override { }
    virtual void VisitDXFEllipse(CDXFEllipse &entity) override { }
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) override { }

// Attributes
private:
    bool m_bHatchExists       {false};
    CDXFHatch *m_pHatchEntity {nullptr};
};

#endif