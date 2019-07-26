#ifndef __DXF_DIMENSION_VISITOR_H__
#define __DXF_DIMENSION_VISITOR_H__
#pragma once

class CDXFDimensionVisitor
{
    friend class CDXFAlignedDimension;
    friend class CDXFAngledDimension;
    friend class CDXFRotatedDimension;

// Overrides
protected:
    virtual void VisitDXFAlignedDimension(CDXFAlignedDimension& entity) { }
    virtual void VisitDXFAngledDimension(CDXFAngledDimension&   entity) { }
    virtual void VisitDXFRotatedDimension(CDXFRotatedDimension& entity) { }

protected:
    CDXFDimensionVisitor() { }
};

#endif