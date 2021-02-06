#ifndef __DXF_TEXT_H__
#define __DXF_TEXT_H__
#pragma once

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "base/DXF3DPoint.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

enum ETextHorzAlignment
{
    eLeft = 0,
    eCentre,
    eRight,
    eAligned,
    eMiddle,
    eFit,
};

enum ETextVertAlignment
{
    eBaseline = 0,
    eBottom,
    eVertAlignMiddle,
    eTop,
};

class CDXFText : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFText() : CDXFEntity("TEXT") { }
    CDXFText(const char *sText, 
             const CDXF3DPoint &insPoint, 
             double dRotationAngle,
             ETextHorzAlignment horz, ETextVertAlignment vert,
             const CDXFObjectID &textStyleID) 
        : CDXFEntity("TEXT"), 
          m_sText(sText), 
          m_pointIns(insPoint), 
          m_dRotationAngle(dRotationAngle),
          m_horzAlignment(horz),
          m_vertAlignment(vert),
          m_textStyleID(textStyleID) 
    { 
        UpdateAlignmentPoint();
    }
    CDXFText(const CDXFText &text) : CDXFEntity(text, "TEXT") {
        m_dObliqueAngle  = text.m_dObliqueAngle;
        m_dScaleX        = text.m_dScaleX;
        m_dRotationAngle = text.m_dRotationAngle;
        m_sText          = text.m_sText;
        m_dHeight        = text.m_dHeight;
        m_pointIns       = text.m_pointIns;
        m_pointAlignment = text.m_pointAlignment;
        m_horzAlignment  = text.m_horzAlignment;
        m_vertAlignment  = text.m_vertAlignment;
        m_fTextFlags     = text.m_fTextFlags;
        m_textStyleID    = text.m_textStyleID;
    }
    virtual ~CDXFText() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFText(*this); }

// Opertations
public:
    bool GetUpsideDown() const {
        if ((m_fTextFlags & eUpsideDown) == eUpsideDown) {
            return true;
        }
        else {
            return false;
        }
    }
    void SetUpsideDown(bool bUpsideDown) {
        if (bUpsideDown) {
            m_fTextFlags |= eUpsideDown;
        }
        else {
            m_fTextFlags &= ~eUpsideDown;
        }
    }

    bool GetBackward() const {
        if ((m_fTextFlags & eBackward) == eUpsideDown) {
            return true;
        }
        else {
            return false;
        }
    }
    void SetBackward(bool bBackward) {
        if (bBackward) {
            m_fTextFlags |= eBackward;
        }
        else {
            m_fTextFlags &= ~eBackward;
        }
    }

    void SetTextStyleID(const CDXFObjectID &textStyleID) { m_textStyleID = textStyleID; }
    const CDXFObjectID &GetTextStyleID() const { return m_textStyleID; }

    ETextHorzAlignment GetHorzAlignment() const { return m_horzAlignment; }
    ETextVertAlignment GetVertAlignment() const { return m_vertAlignment; }

    void SetHorzAlignment(ETextHorzAlignment horz) { m_horzAlignment = horz; } // ? UpdateAlignmentPoint();
    void SetVertAlignment(ETextVertAlignment vert) { m_vertAlignment = vert; } // ? UpdateAlignmentPoint();

    void SetTextAlign(ETextHorzAlignment horz, ETextVertAlignment vert) {
        m_horzAlignment = horz;
        m_vertAlignment = vert;
        UpdateAlignmentPoint();
    }

    double GetTextHeight() const { return m_dHeight; }
    void SetTextHeight(double dHeight) { m_dHeight = dHeight; }
    
    const char *GetText() const  { return m_sText.c_str();  }
    void SetText(const char *sText) { m_sText = sText; }

    double GetRotationAngle() const { return m_dRotationAngle; }
    void SetRotationAngle(double dAngle) { m_dRotationAngle = dAngle; }

    double GetObliqueAngle() const  { return m_dObliqueAngle;  }
    void SetObliqueAngle(double dAngle) { m_dObliqueAngle = dAngle; }

    double GetScaleX() const { return m_dScaleX; }
    void SetScaleX(double dScale) { m_dScaleX = dScale; }

    CDXF3DPoint GetInsertPoint() const { return m_pointIns; }
    void SetInsertPoint(const CDXF3DPoint &insPoint) {
        m_pointIns = insPoint;
        UpdateAlignmentPoint();
    }

    CDXF3DPoint GetAlingmentPoint() const { return m_pointAlignment; }
    void SetAlignPoint(const CDXF3DPoint &alignPoint) { m_pointAlignment = alignPoint; }

    unsigned char GetTextFlags() const { return m_fTextFlags; }
    void SetTextFlags(unsigned char text_flags) { m_fTextFlags = text_flags; }

private:
    void UpdateAlignmentPoint() { 
        if (m_vertAlignment != 0 || m_horzAlignment != 0) {
            m_pointAlignment = m_pointIns;
        }
    }

// Static operations
public:
    // autocad: text always contains predefined text style
    template <class TDXFBlockTableRecord, class TDXFTextStyleTableRecord> 
    static CDXFText *Create(const char *sText, double x, double y, double dTextAngle, 
                            ETextHorzAlignment horz, ETextVertAlignment vert,
                            CDXFDatabase *pDB, TDXFBlockTableRecord *pBlock, 
                            const CDXFObjectID &layerID, TDXFTextStyleTableRecord *pTextStyle, double dTextSize = 5.0) {
        CDXFText *pText = new CDXFText(sText, CDXF3DPoint(x, y, 0), dTextAngle, horz, vert, pTextStyle->GetObjectID());
        pText->SetLayerID(layerID);    

        ASSERT(pTextStyle);
        // this is the same workflow as autocad contains
        // text style height is used only for the entity creation
        if (pTextStyle->GetTextHeight() > 0.0 ) {
            pText->SetTextHeight(pTextStyle->GetTextHeight());
        }
        else {
            pText->SetTextHeight(dTextSize);
        }

        pBlock->AddEntity(pText, pDB);
        return pText;
    }

private:
    enum {
        eBackward   = 2,
        eUpsideDown = 4
    };

// Attributes
private:
    CDXFObjectID m_textStyleID; // Text style name (optional, default = STANDARD)
    unsigned char m_fTextFlags {0  };
    double m_dObliqueAngle     {0.0};
    double m_dScaleX           {1.0};
    double m_dRotationAngle    {0.0};
    double m_dHeight           {0. };
    CDXF3DPoint m_pointIns;
    // Second alignment point (in OCS) 
    // (optional)11DXF: X value; 
    // APP: 3D pointThis value is meaningful only if the value of a 72 or 73 group is nonzero 
    // (if the justification isanything other than baseline/left) 
    CDXF3DPoint m_pointAlignment;
    ETextHorzAlignment m_horzAlignment {eLeft};
    ETextVertAlignment m_vertAlignment {eBaseline};
    std::string m_sText;
};

#endif