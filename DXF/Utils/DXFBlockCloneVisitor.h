#ifndef __DXF_BLOCK_CLONE_VISITOR_H__
#define __DXF_BLOCK_CLONE_VISITOR_H__
#pragma once

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "entity/DXFEntityVisitor.h"
#endif

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "tables/DXFSymbolTable.h"
#endif

#ifndef __DXF_RECORD_VISITOR_H__
    #include "tables/DXFRecordVisitor.h"
#endif

class CDXFBlockCloneVisitor final : public CDXFEntityVisitor,
                                    public CDXFRecordVisitor
{
// Construction/Destruction
public:
    CDXFBlockCloneVisitor(CDXFDatabase *pDatabase, const CDXFDatabase *pSrcDB);
    virtual ~CDXFBlockCloneVisitor() { }

// Overrides CDXFEntityVisitor
protected:
    virtual void VisitDXFArc(CDXFArc &entity) override;
    virtual void VisitDXFCircle(CDXFCircle &entity) override;
    virtual void VisitDXFHatch(CDXFHatch &entity) override;
    virtual void VisitDXFLine(CDXFLine &entity) override;
    virtual void VisitDXFMText(CDXFMText &entity) override;
    virtual void VisitDXFPoint(CDXFPoint &entity) override;
    virtual void VisitDXFPolyline(CDXFPolyline &entity) override;
    virtual void VisitDXFSolid(CDXFSolid &entity) override;
    virtual void VisitDXFText(CDXFText &entity) override;
    virtual void VisitDXFInsert(CDXFInsert &entity) override;
    virtual void VisitDXFDimension(CDXFDimension &entity) override;
    virtual void VisitDXFEllipse(CDXFEllipse &entity) override;
    virtual void VisitDXFBlockBegin(CDXFBlockBegin &entity) override;

// Overrides CDXFRecordVisitor
public:
    virtual void VisitLineTypeTableRecord(CDXFLineTypeTableRecord &record) override;
    virtual void VisitLayerTableRecord(CDXFLayerTableRecord &record) override;
    virtual void VisitTextStyleTableRecord(CDXFTextStyleTableRecord &record) override;
    virtual void VisitDimStyletableRecord(CDXFDimStyleTableRecord &record) override;
    virtual void VisitBlockTableRecord(CDXFBlockTableRecord &record) override;

// Operations
public:
    CDXFObject *GetObj() const;

// Attributes
private:
    CDXFObject *m_pObj {nullptr};
    CDXFDatabase *m_pDB;
    const CDXFDatabase *m_pSrcDB;
};

#endif