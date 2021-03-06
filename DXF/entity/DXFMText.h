#ifndef __DXF_MTEXT_H__
#define __DXF_MTEXT_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_ENTITY_H__
    #include "DXFEntity.h"
#endif

#ifndef __DXF_3DPOINT_H__
    #include "../base/DXF3DPoint.h"
#endif

#ifndef __DXF_LINE_WEIGHT_H__
    #include "../base/DXFLineWeight.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor.h"
#endif

enum EMTextAttachement
{
    eAttachTopLeft = 1,
    eAttachTopCenter,
    eAttachTopRight,
    eAttachMiddleLeft,
    eAttachMiddleCenter,
    eAttachMiddleRight,
    eAttachBottomLeft,
    eAttachBottomCenter,
    eAttachBottomRight
};

enum EMTextDirection
{
    eDirLeftToRight = 1,
    eDirTopToBottom = 3,
    eDirByStyle     = 5
};

class DXFDLLAPI CDXFMText : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFMText() : CDXFEntity("MTEXT") { }
    CDXFMText(const char *sText, const CDXF3DPoint &insertPoint, double dAngle, double dRectWidth, const CDXFObjectID &textStyleID) 
        : CDXFEntity("MTEXT"), 
          m_sText(sText), 
          m_pointInsert(insertPoint), 
          m_dAngle(dAngle),
          m_dRectangleWidth(dRectWidth),
          m_textStyleID(textStyleID) { }
    CDXFMText(const CDXFMText &entity)
        : CDXFEntity(entity, "MTEXT"),
          m_pointInsert(entity.GetInsertPoint()), 
          m_dTextHeight(entity.GetTextHeight()), 
          m_dRectangleWidth(entity.GetRectangleWidth()),
          m_eAttachement(entity.GetAttachement()), 
          m_eDirection(entity.GetDirection()), 
          m_sText(entity.GetText()), 
          m_dAngle(entity.GetAngle()),
          m_textStyleID(entity.m_textStyleID) { }
    virtual ~CDXFMText() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { vis.VisitDXFMText(*this); }

// Operations
public:
    CDXF3DPoint GetInsertPoint() const { return m_pointInsert; }
    void SetInsertPoint(double x, double y, double z) { m_pointInsert.SetValues(x, y, z); }
    void SetInsertPoint(const CDXF3DPoint &insertPoint) { m_pointInsert = insertPoint; }

    double GetTextHeight() const { return m_dTextHeight; }
    void SetTextHeight(double dHeight) { m_dTextHeight = dHeight; }

    double GetRectangleWidth() const { return m_dRectangleWidth; }
    void SetRectangleWidth(double dWidth) { m_dRectangleWidth = dWidth; }

    enum EMTextAttachement GetAttachement() const { return m_eAttachement; }
    void SetAttachement(enum EMTextAttachement attachement) { m_eAttachement = attachement; }

    enum EMTextDirection GetDirection() const { return m_eDirection; }
    void SetDirection(enum EMTextDirection direction) { m_eDirection = direction; }

    const char *GetText() const { return m_sText.c_str(); }
    void SetText(const char *sText) { m_sText = sText; }

    const CDXFObjectID &GetTextStyleID() const { return m_textStyleID; }
    void SetTextStyleID(const CDXFObjectID &textStyleID) { m_textStyleID = textStyleID; }

    double GetAngle() const { return m_dAngle; }
    void SetAngle(double dAngle) { m_dAngle = dAngle; }

    // Static operations
public:
    // autocad: text always contains predefined text style
    template <class TDXFBlockTableRecord, class TDXFTextStyleTableRecord>
    static CDXFMText *Create(const char *sText, double x, double y, double dRectWidth, double dAngle,
        CDXFDatabase *pDB, TDXFBlockTableRecord *pBlock,
        const CDXFObjectID &layerID, TDXFTextStyleTableRecord *pTextStyle, double dTextSize = 5.0) {
        CDXFMText *pMText = new CDXFMText(sText, CDXF3DPoint(x, y, 0.0), dAngle, dRectWidth, pTextStyle->GetObjectID());
        pMText->SetLayerID(layerID);

        if (pTextStyle->GetTextHeight() > 0.0) {
            pMText->SetTextHeight(pTextStyle->GetTextHeight());
        }
        else {
            pMText->SetTextHeight(dTextSize);
        }

        pBlock->AddEntity(pMText, pDB);

        return pMText;
    }

// Attributes
private:
    std::string m_sText;

    EMTextAttachement m_eAttachement {eAttachTopLeft};
    CDXFObjectID m_textStyleID; // Text style name (STANDARD if not provided) (optional)
    EMTextDirection m_eDirection {eDirByStyle};

    CDXF3DPoint m_pointInsert;

    // Nominal (initial) text height in drawing units. Units defined in DXF file header, under $INSUNITS variable group code 70.
    double m_dTextHeight     {0.};
    double m_dAngle          {0.};
    double m_dRectangleWidth {0.};
//    EDXFLineWeight m_eLineWeight {EDXFLineWeight::LW_ByLayer}; // default: By Layer.
};

#endif