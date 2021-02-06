#ifndef __DXF_ENTITY_SET_LINE_WEIGHT_VISITOR_H__
#define __DXF_ENTITY_SET_LINE_WEIGHT_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "entity/DXFEntityVisitor.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "base/DXFLineWeight.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "../entity/DXFEntity.h"
#endif

class CDXFEntitySetLineWeightVisitor final : public CDXFEntityVisitor
{
// Construction/Destruction
public:
    CDXFEntitySetLineWeightVisitor(EDXFLineWeight eLineWeight);
    virtual ~CDXFEntitySetLineWeightVisitor() { }

// Overrides
protected:
    virtual void VisitDXFArc(CDXFArc &entity) override;
    virtual void VisitDXFCircle(CDXFCircle &entity) override;
    virtual void VisitDXFHatch(CDXFHatch &entity) override;
    virtual void VisitDXFLine(CDXFLine &entity) override;
    virtual void VisitDXFPolyline(CDXFPolyline &entity) override;

    virtual void VisitDXFDimension(CDXFDimension &entity) override { }
    virtual void VisitDXFMText(CDXFMText &entity) override { }
    virtual void VisitDXFPoint(CDXFPoint &entity) override { }
    virtual void VisitDXFSolid(CDXFSolid &entity) override { }
    virtual void VisitDXFText(CDXFText &entity) override { }
    virtual void VisitDXFInsert(CDXFInsert &entity) override { }
    virtual void VisitDXFEllipse(CDXFEllipse &entity) override { }
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) override { }

// Attributes
private:
    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_100}; // default: 1mm.
};

#endif