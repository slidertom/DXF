#ifndef __DXF_RECORD_VISITOR_H__
#define __DXF_RECORD_VISITOR_H__
#pragma once

class CDXFViewportTableRecord;
class CDXFLineTypeTableRecord;
class CDXFLayerTableRecord;
class CDXFTextStyleTableRecord;
class CDXFViewTableRecord;
class CDXFUCSTableRecord;
class CDXFAppIDTableRecord;
class CDXFDimStyleTableRecord;
class CDXFBlockTableRecord;
class CDXFTableVisitor;

class CDXFRecordVisitor
{
// Construction/Destruction
public:
    CDXFRecordVisitor() { }
    virtual ~CDXFRecordVisitor() { }

// Overrides
public:
    virtual void VisitViewReportTableRecord(CDXFViewportTableRecord &record) { }
    virtual void VisitLineTypeTableRecord(CDXFLineTypeTableRecord &record) { }
    virtual void VisitLayerTableRecord(CDXFLayerTableRecord &record) { }
    virtual void VisitTextStyleTableRecord(CDXFTextStyleTableRecord &record) { }
    virtual void VisitViewTableRecord(CDXFViewTableRecord &record) { }
    virtual void VisitUCSTableRecord(CDXFUCSTableRecord &record) { }
    virtual void VisitAppIDtableRecord(CDXFAppIDTableRecord &record) { }
    virtual void VisitDimStyletableRecord(CDXFDimStyleTableRecord &record) { }
    virtual void VisitBlockTableRecord(CDXFBlockTableRecord &record) { }
};

#endif
