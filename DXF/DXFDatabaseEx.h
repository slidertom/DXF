#ifndef __DXF_DATABASEEX_H__
#define __DXF_DATABASEEX_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

#ifndef __DXF_BLOCKTABLERECORD_H__
    #include "tables/DXFBlockTableRecord.h"
#endif

#ifndef __DXF_TEXT_H__
    #include "entity/DXFText.h"
#endif

#ifndef __DXF_MTEXT_H__
    #include "entity/DXFMText.h"
#endif

#ifndef __DXF_ENTITY_VISITOR_H__
    #include "DXFEntityVisitor"
#endif

class CDXFDatabase;
class CDXFLine;
class CDXFCircle;
class CDXFArc;
class CDXFText;
class CDXFPolyline;
class CDXFAlignedDimension;
class CDXFRotatedDimension;
class CDXFHatch;
class CDXFDimStyleTableRecord;
class CDXFAlignedDimensionEx;
class CDXFEllipse;
class CDXFAngledDimension;
class CDXFLayerTableRecord;
class CDXFGeBulgePoint3D;
class CDXFGePoint2D;

class DXFDLLAPI CDXFDatabaseEx final
{
// Construction/Destruction
public:
    CDXFDatabaseEx(CDXFDatabase *pDatabase);
    virtual ~CDXFDatabaseEx() { }

// Operations
public:
    bool SetLayer(const char *sLayerName);
    void SetTextSize(double dTextSize);
    void SetTextAngle(double dAngle);
    void SetTextAlign(ETextHorzAlignment horz, ETextVertAlignment vert);
    bool SetBlock(const char *sBlockName);
    bool SetDimStyle(const char *sDimStyleName);
    bool SetTextStyle(const char *sTextStyleName);
    void InsertBlock(const char *sSourceBlockName,
                     double posx, double posy,
                     double scalex, double scaley,
                     double dAngle);

    CDXFLayerTableRecord *GetLayer() const;
    CDXFObjectID &GetLayerID();
    CDXFBlockTableRecord *GetBlock() const;
    CDXFDimStyleTableRecord *GetDimStyle() const;
    CDXFTextStyleTableRecord *GetTextStyle() const;
    CDXFDatabase *GetDatabase() const;
    double GetTextSize();

    bool CreateBlock(const char *sBlockName, const char *sLayerName = "0");

    //******Not supporting*****
    CDXFHatch *Hatch(const std::vector<CDXFGePoint2D> &vertices, const char* sPatternName, double dAngle, double dScale);
    //*************************
    CDXFHatch *Hatch(const char *sPatternName, double dAngle, double dScale);

    CDXFLine * Line(double x1, double y1, double x2, double y2);
    CDXFArc * Arc(double centerx, double centery, double radius, double dStartAngle, double dEndAngle);
    CDXFEllipse *Ellipse(double dCenterX, double dCenterY, double dRadiusX, double dRadiusY);
    CDXFCircle *Circle(double centerx, double centery, double radius);
    CDXFPolyline* Polyline(const std::vector<CDXFGeBulgePoint3D>& verts);
    CDXFPolyline *Polyline(double x1, double y1, double x2, double y2);
    CDXFAlignedDimension *AlignedDimension(double x1, double y1, double x2, double y2, double dHeight = 10.0);
    CDXFRotatedDimension *LinearDimension(double x1, double y1, double x2, double y2, double dimx, double dimy, const char *sDimText,
                                          double dAngle = 0.0);
    void LinearDimension(double x1, double y1, double x2, double y2, double dimx, double dimy, double dAngle = 0.0);

    void SaveDatabase(const wchar_t *sFileName);
    bool LoadTemplate(const wchar_t *sFileName);
    bool LoadBlock(const wchar_t *sFileName, const char *sBlockName);

    CDXFText *Text(const char *sText, double x, double y);
    CDXFMText *MText(const char *sText, double x, double y, double dRectWidth, double dAngle = 0.0);

private:
    void Init();    
    bool CreateLayer(const char *sLayerName);
    
private:
    CDXFDatabase *m_pDatabase;
    CDXFObjectID m_layerID;
    CDXFObjectID m_dimStyleID;
    CDXFObjectID m_textStyleID;
    CDXFBlockTableRecord *m_pBlock  {nullptr};
    double m_dTextSize  {5.0};
    double m_dTextAngle {0.0};
    ETextHorzAlignment m_horzAlign {eCentre};
    ETextVertAlignment m_vertAlign {eBaseline};
};

#endif