#ifndef __DXF_ENTITY_WRITER_VISITOR_H__
#define __DXF_ENTITY_WRITER_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "entity/DXFEntityVisitor.h"
#endif

class CDXFDatabase;
class CDXFWriter;

class CDXFEntityWriterVisitor: public CDXFEntityVisitor
{
// Construction/Destruction
public:
    CDXFEntityWriterVisitor(CDXFWriter &writer, CDXFDatabase *pDB);
    virtual ~CDXFEntityWriterVisitor() { }

// Overrides
protected:
    virtual void VisitDXFArc(CDXFArc &entity) override;
    virtual void VisitDXFCircle(CDXFCircle &entity) override;
    virtual void VisitDXFDimension(CDXFDimension &entity) override;
    virtual void VisitDXFHatch(CDXFHatch &entity) override;
    virtual void VisitDXFLine(CDXFLine &entity) override;
    virtual void VisitDXFMText(CDXFMText &entity) override;
    virtual void VisitDXFPoint(CDXFPoint &entity) override;
    virtual void VisitDXFPolyline(CDXFPolyline &entity) override;
    virtual void VisitDXFSolid(CDXFSolid &entity) override;
    virtual void VisitDXFText(CDXFText &entity) override;
    virtual void VisitDXFInsert(CDXFInsert &entity) override;
    virtual void VisitDXFEllipse(CDXFEllipse &entity) override;

    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) override { }

// Attributes
private:
    CDXFWriter &m_writer;
    CDXFDatabase *m_pDB;
};

#endif