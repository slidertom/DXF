#ifndef __DXF_MLINESTYLE_H__
#define __DXF_MLINESTYLE_H__
#pragma once

#ifndef __DXF_OBJECT_H__
	#include "DXFObject.h"
#endif

class CDXFMlineStyle : public CDXFObject
{
// Construction/Destruction
public:
    CDXFMlineStyle(const char *sStyleName) : CDXFObject("MLINESTYLE") { m_sStyleName = sStyleName; }
    virtual ~CDXFMlineStyle() { }

// Overrides
public:
    virtual void Accept(CDXFObjectVisitorAbs &vis) override { vis.VisitDXFLineStyle(*this); }

// Attributes
public:
	std::string m_sStyleName;
	std::string m_sStyleDescription;
    int32_t m_nFlags     {0};
    int32_t m_nFillColor {256};
    float m_fStartAngle  {90.0f};
    float m_fEndAngle    {90.0f};
};

#endif
