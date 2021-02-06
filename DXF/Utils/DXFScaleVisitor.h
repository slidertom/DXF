#ifndef __DXF_SCALE_VISITOR_H__
#define __DXF_SCALE_VISITOR_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "../entity/DXFEntity.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "entity/DXFEntityVisitor.h"
#endif

#ifndef __DXF_MATRIX3D_H__
    #include "base/DXFMatrix3D.h"
#endif

class CDXFHatch;
class CDXFGePoint2D;

class DXFDLLAPI CDXFScaleVisitor: public CDXFEntityVisitor
{
// Construction/Destruction
public:
    CDXFScaleVisitor(double dScaleX, double dScaleY);
    virtual ~CDXFScaleVisitor() { }

// Overrides
protected:
    virtual void VisitDXFHatch(CDXFHatch &entity) override;

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

private:
    void ScaleDXFGePoint2D(CDXFGePoint2D &dxfGePoint);

// Attributes
private:
    CDXFMatrix3D m_ScaleMatrix;
};

#endif
