#ifndef __DXF_ENTITY_BOUNDING_BOX_VISITOR_H__
#define __DXF_ENTITY_BOUNDING_BOX_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "entity/DXFEntityVisitor.h"
#endif

#ifndef __DXF_BOUNDINGBOX_H__
    #include "base/DXFBoundingBox.h"
#endif

class CDXFBlockTableRecord;
class CDXFBoundingBox;

class CDXFEntityBoundingBoxVisitor: public CDXFEntityVisitor
{
// Construction/Destruction
public:
    CDXFEntityBoundingBoxVisitor() { }
    virtual ~CDXFEntityBoundingBoxVisitor() { }

public:
    static CDXFBoundingBox GetBoundingBox(const CDXFBlockTableRecord &block);

// Overrides
protected:
    virtual void VisitDXFCircle(CDXFCircle &entity)       override;
    virtual void VisitDXFLine(CDXFLine &entity)           override;
    virtual void VisitDXFMText(CDXFMText &entity)         override;
    virtual void VisitDXFPolyline(CDXFPolyline &entity)   override;
    virtual void VisitDXFSolid(CDXFSolid &entity)         override;
    virtual void VisitDXFInsert(CDXFInsert &entity)       override;
    virtual void VisitDXFDimension(CDXFDimension &entity) override;

    virtual void VisitDXFArc(CDXFArc &entity) override { }
    virtual void VisitDXFHatch(CDXFHatch &entity) override { }
    virtual void VisitDXFPoint(CDXFPoint &entity) override { }
    virtual void VisitDXFText(CDXFText &entity) override { }
    virtual void VisitDXFEllipse(CDXFEllipse &entity) override { }
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) override { }

public:
    const CDXFBoundingBox &GetBoundingBox() const;

// Attributes
private:
    CDXFBoundingBox m_box;
};

#endif
