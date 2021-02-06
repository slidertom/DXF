#ifndef __DXF_VIEW_TABLE_RECORD_H__
#define __DXF_VIEW_TABLE_RECORD_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_RECORD_H__
    #include "DXFSymbolTableRecord.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "DXF3DPoint.h"
#endif

class CDXFAbstractViewTableRecord : public CDXFSymbolTableRecord  
{
// Construction/Destruction
public:
    CDXFAbstractViewTableRecord() = delete;
    CDXFAbstractViewTableRecord(const char *sName, const char *sObjectName) : CDXFSymbolTableRecord(sName, sObjectName) { }
    virtual ~CDXFAbstractViewTableRecord() { }

// Operations
public:
    CDXF3DPoint GetViewTarget() const { return m_pointTarget; }
    void SetViewTarget(double x, double y, double z) { m_pointTarget.SetValues(x, y, z); }
    void SetViewTarget(const CDXF3DPoint &target) { m_pointTarget = target; }

    CDXF3DPoint GetCenter() const { return m_pointCenter; }
    void SetCenter(double x, double y) { m_pointCenter.x = x; m_pointCenter.y = y; }

    CDXF3DPoint GetViewDirection() const { return m_viewDirection; }
    void SetViewDirection(double x, double y, double z) {  m_viewDirection.SetValues(x, y, z); }
    void SetViewDirection(const CDXF3DPoint &direction) { m_viewDirection = direction; }

    double GetWidth() const { return m_dWidth; }
    void SetWidth(double dWidth) { m_dWidth  = dWidth;  }

    double GetHeight() const { return m_dHeight; }
    void SetHeight(double dHeight) { m_dHeight = dHeight; }

    double GetBackClipDistance() const  { return m_dBackClipDistance;  }
    void SetBackClipDistance(double dDistance)  { m_dBackClipDistance  = dDistance; }

    double GetFrontClipDistance() const { return m_dFrontClipDistance; }
    void SetFrontClipDistance(double dDistance) { m_dFrontClipDistance = dDistance; }

    bool GetUCS(CDXF3DPoint &origin, CDXF3DPoint &xAxis, CDXF3DPoint &yAxis) const {
        if (!m_bUCSAttached) {
            return false;
        }
        origin = m_ucsOrigin;
        xAxis  = m_ucsXAxis;
        yAxis  = m_ucsYAxis;
        return true;
    }

    void SetUCS(const CDXF3DPoint &origin, const CDXF3DPoint &xAxis, const CDXF3DPoint &yAxis) {
        m_bUCSAttached = true;
        m_ucsOrigin    = origin;
        m_ucsXAxis     = xAxis;
        m_ucsYAxis     = yAxis;
    }
   
    double GetLensLength() const { return m_dLensLength; }
    void SetLensLength(double dLensLength) { m_dLensLength = dLensLength; }

// Attributes
public:
    CDXF3DPoint m_pointTarget;
    CDXF3DPoint m_pointCenter;

    CDXF3DPoint m_viewDirection;

    double m_dBackClipDistance  {0.};
    double m_dFrontClipDistance {0.};

    double m_dHeight {0.};
    double m_dWidth  {0.};

    double m_dLensLength {50.};

    bool m_bUCSAttached {false};

    CDXF3DPoint m_ucsOrigin;
    CDXF3DPoint m_ucsXAxis;
    CDXF3DPoint m_ucsYAxis;
};

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "tables/DXFSymbolTable.h"
#endif

class CDXFViewTableRecord : public CDXFAbstractViewTableRecord  
{
// Construction/Destruction
public:
    CDXFViewTableRecord(const char *sName) : CDXFAbstractViewTableRecord(sName, "VIEW") { }
    virtual ~CDXFViewTableRecord() { }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitViewTableRecord(*this); }

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

private:
    unsigned char m_fFlags {0};
};

class CDXFViewportTableRecord : public CDXFAbstractViewTableRecord  
{
// Construction/Destruction
public:
    CDXFViewportTableRecord(const char *sName)  : CDXFAbstractViewTableRecord(sName, "VPORT"), m_snapSpacing(1, 1, 0), m_gridSpacing(1, 1, 0) { }
    virtual ~CDXFViewportTableRecord() { }

// Overrides
public:
    virtual void AcceptRecordVis(CDXFRecordVisitor &vis) override { vis.VisitViewReportTableRecord(*this); }

// Operations
public:
    void SnapON(bool bON) {
        if (bON) {
            m_fSnapON = 1;
        }
        else {
            m_fSnapON = 0;
        }
    }
    void GridON(bool bON) {
        if (bON) {
            m_fGridON = 1;
        }
        else {
            m_fGridON = 0;
        }
    }

    void SetUpperRightCorner(double x, double y) { m_upperRightCorner.SetValues(x, y, m_upperRightCorner.z); }
    void SetLowerLeftCorner(double x, double y)  { m_lowerLeftCorner.SetValues(x, y, m_lowerLeftCorner.z);   }

    unsigned char GetFlags() const { return m_fFlags; }
    void SetFlags(unsigned char fFlags) { m_fFlags = fFlags; }

// Attributes
public:
    unsigned char m_fGridON {0};
    unsigned char m_fSnapON {0};

    CDXF3DPoint m_gridSpacing;

    CDXF3DPoint m_snapBase;
    CDXF3DPoint m_snapSpacing;

    CDXF3DPoint m_upperRightCorner;
    CDXF3DPoint m_lowerLeftCorner;

private:
    unsigned char m_fFlags {0};
};

#endif