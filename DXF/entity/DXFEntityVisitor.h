#ifndef __DXF_ENTITY_VISITOR_H__
#define __DXF_ENTITY_VISITOR_H__
#pragma once

class CDXFEntityVisitor
{
    friend class CDXFArc;
    friend class CDXFCircle;
    friend class CDXFDimension;
    friend class CDXFHatch;
    friend class CDXFLine;
    friend class CDXFMText;
    friend class CDXFPoint;
    friend class CDXFPolyline;
    friend class CDXFSolid;
    friend class CDXFText;
    friend class CDXFInsert;
    friend class CDXFEllipse;
    friend class CDXFBlockBegin;

// Overrides
protected:
    virtual void VisitDXFArc(CDXFArc &entity) = 0;
    virtual void VisitDXFCircle(CDXFCircle &entity) = 0;
    virtual void VisitDXFDimension(CDXFDimension &entity) = 0;
    virtual void VisitDXFHatch(CDXFHatch &entity) = 0;
    virtual void VisitDXFLine(CDXFLine &entity) = 0;
    virtual void VisitDXFMText(CDXFMText &entity) = 0;
    virtual void VisitDXFPoint(CDXFPoint &entity) = 0;
    virtual void VisitDXFPolyline(CDXFPolyline &entity) = 0;
    virtual void VisitDXFSolid(CDXFSolid &entity) = 0;
    virtual void VisitDXFText(CDXFText &entity) = 0;
    virtual void VisitDXFInsert(CDXFInsert &entity) = 0;
    virtual void VisitDXFEllipse(CDXFEllipse &entity) = 0;
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) = 0;

protected:
    CDXFEntityVisitor() { }   
};

#endif
