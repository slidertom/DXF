#ifndef __DXF_TEXT_FINDER_VISITOR_H__
#define __DXF_TEXT_FINDER_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_H__
	#include "../entity/DXFEntity.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor"
#endif

class CDXFMText;
class CDXFText;

class CDXFTextFinderVisitor: public CDXFEntityVisitor
{
// Construction/Destruction
public:
    CDXFTextFinderVisitor() { }
    virtual ~CDXFTextFinderVisitor() { }

// Operations
public:
	bool TextExists() const { return m_bTextExists; };

protected:
    virtual void VisitDXFMText(CDXFMText &entity) override { m_bTextExists = true; }
    virtual void VisitDXFText(CDXFText &entity) override { m_bTextExists = true; }

    virtual void VisitDXFArc(CDXFArc &entity) { }
    virtual void VisitDXFCircle(CDXFCircle &entity) { }
    virtual void VisitDXFDimension(CDXFDimension &entity) { }
    virtual void VisitDXFHatch(CDXFHatch &entity) { }
    virtual void VisitDXFLine(CDXFLine &entity) { }
    virtual void VisitDXFPoint(CDXFPoint &entity) { }
    virtual void VisitDXFPolyline(CDXFPolyline &entity) { }
    virtual void VisitDXFSolid(CDXFSolid &entity) { }
    virtual void VisitDXFInsert(CDXFInsert &entity) { }
    virtual void VisitDXFEllipse(CDXFEllipse &entity) { }
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) { }

// Attributes
private:
    bool m_bTextExists {false};
};

#endif
