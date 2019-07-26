#ifndef __DXF_TABLE_VISITOR_H__
#define __DXF_TABLE_VISITOR_H__
#pragma once

class CDXFSymbolTable;
class CDXFBlockTable;
class CDXFViewportTable;
class CDXFLineTypeTable;
class CDXFLayerTable;
class CDXFTextStyleTable;
class CDXFViewTable;
class CDXFUCSTable;
class CDXFAppIDTable;
class CDXFDimStyleTable;

class CDXFTableVisitor
{
// Construction/Destruction
public:
    CDXFTableVisitor() { }
    virtual ~CDXFTableVisitor() { }

// Overrides
public:
    virtual void VisitSymbolTable(CDXFSymbolTable &table) { }
    virtual void VisitDimStyleTable(CDXFDimStyleTable &table) { }
    virtual void VisitBlockTable(CDXFBlockTable &table) { }
    virtual void VisitViewReportTable(CDXFViewportTable &table) { }
    virtual void VisitLineTypeTable(CDXFLineTypeTable &table) { }
    virtual void VisitLayerTable(CDXFLayerTable &table) { }
    virtual void VisitTextStyleTable(CDXFTextStyleTable &table) { }
    virtual void VisitViewTable(CDXFViewTable &table) { }
    virtual void VisitUCSTable(CDXFUCSTable &table) { }
    virtual void VisitAppIDtable(CDXFAppIDTable &table) { }
};

#endif
