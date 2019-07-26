#include "stdafx.h"
#include "DXFEntitySetLineWeightVisitor.h"

#include "../entity/DXFLine.h"
#include "../entity/DXFArc.h"
#include "../entity/DXFCircle.h"
#include "../entity/DXFEllipse.h"
#include "../entity/DXFHatch.h"
#include "../entity/DXFPolyline.h"
#include "../entity/DXFSolid.h"

CDXFEntitySetLineWeightVisitor::CDXFEntitySetLineWeightVisitor(EDXFLineWeight eLineWeight) 
: m_eLineWeight(eLineWeight)
{

}

void CDXFEntitySetLineWeightVisitor::VisitDXFArc(CDXFArc &entity)
{
    entity.SetLineWeight(m_eLineWeight);
}

void CDXFEntitySetLineWeightVisitor::VisitDXFCircle(CDXFCircle &entity)
{
    entity.SetLineWeight(m_eLineWeight);
}

void CDXFEntitySetLineWeightVisitor::VisitDXFHatch(CDXFHatch &entity)
{
    entity.SetLineWeight(m_eLineWeight);
}

void CDXFEntitySetLineWeightVisitor::VisitDXFLine(CDXFLine &entity)
{
    entity.SetLineWeight(m_eLineWeight);
}

void CDXFEntitySetLineWeightVisitor::VisitDXFPolyline(CDXFPolyline &entity)
{
    entity.SetLineWeight(m_eLineWeight);
}