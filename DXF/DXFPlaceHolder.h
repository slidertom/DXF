#ifndef __DXF_PLACEHOLDER_H__
#define __DXF_PLACEHOLDER_H__
#pragma once

#ifndef __DXF_OBJECT_H__
    #include "DXFObject.h"
#endif

class CDXFPlaceHolder : public CDXFObject  
{
// Construction/Destruction
public:
    CDXFPlaceHolder() : CDXFObject("ACDBPLACEHOLDER") { }
    virtual ~CDXFPlaceHolder() { }

// Overrides
public:
    virtual void Accept(CDXFObjectVisitorAbs &vis) override { vis.VisitDXFPlaceHolder(*this); }
};

#endif