#include "stdafx.h"
#include "DXFEntityWriterVisitor.h"

#include "file_utils/dxf_arc_file_util.h"
#include "file_utils/dxf_circle_file_util.h"
#include "file_utils/dxf_dim_file_util.h"
#include "file_utils/dxf_hatch_file_util.h"
#include "file_utils/dxf_line_file_util.h"
#include "file_utils/dxf_mtext_file_util.h"
#include "file_utils/dxf_point_file_util.h"
#include "file_utils/dxf_polyline_file_util.h"
#include "file_utils/dxf_solid_file_util.h"
#include "file_utils/dxf_text_file_util.h"
#include "file_utils/dxf_insert_file_util.h"
#include "file_utils/dxf_aligndim_file_util.h"
#include "file_utils/dxf_angle_dim_file_util.h"
#include "file_utils/dxf_rotateddim_file_util.h"
#include "file_utils/dxf_ellipse_file_util.h"

#include "entity/DXFDimension.h"

CDXFEntityWriterVisitor::CDXFEntityWriterVisitor(CDXFWriter &writer, CDXFDatabase *pDB)
: m_writer(writer),
  m_pDB(pDB)
{

}

void CDXFEntityWriterVisitor::VisitDXFArc(CDXFArc &entity) 
{
    dxf_arc_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFCircle(CDXFCircle &entity) 
{ 
    dxf_circle_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFHatch(CDXFHatch &entity) 
{
    dxf_hatch_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFLine(CDXFLine &entity) 
{
    dxf_line_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFMText(CDXFMText &entity) 
{
    dxf_mtext_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFPoint(CDXFPoint &entity) 
{
    dxf_point_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFPolyline(CDXFPolyline &entity) 
{
    dxf_polyline_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFSolid(CDXFSolid &entity) 
{ 
    dxf_solid_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFText(CDXFText &entity) 
{
    dxf_text_file_util::WriteToFile(m_writer, entity);
}

void CDXFEntityWriterVisitor::VisitDXFInsert(CDXFInsert &entity) 
{ 
    dxf_insert_file_util::WriteToFile(m_writer, entity);
}

class CDXFDimEntityWriterVisitor: public CDXFDimensionVisitor
{
// Construction/Destruction
public:
    CDXFDimEntityWriterVisitor(CDXFWriter &writer, CDXFDatabase *pDB) : m_writer(writer), m_pDB(pDB) {}
    virtual ~CDXFDimEntityWriterVisitor() { }

// Overrides
protected:
    virtual void VisitDXFAlignedDimension(CDXFAlignedDimension &entity)     override;
    virtual void VisitDXFAngledDimension(CDXFAngledDimension &entity)       override;
    virtual void VisitDXFRotatedDimension(CDXFRotatedDimension &entity)     override;

// Attributes
private:
    CDXFWriter &m_writer;
    CDXFDatabase *m_pDB;
};

void CDXFDimEntityWriterVisitor::VisitDXFAlignedDimension(CDXFAlignedDimension &entity) 
{
    dxf_align_dim_file_util::WriteToFile(m_writer, entity, m_pDB);
}

void CDXFDimEntityWriterVisitor::VisitDXFAngledDimension(CDXFAngledDimension &entity) 
{ 
    dxf_angle_dim_file_util::WriteToFile(m_writer, entity, m_pDB);
}

void CDXFDimEntityWriterVisitor::VisitDXFRotatedDimension(CDXFRotatedDimension &entity) 
{
    dxf_rotated_dim_file_util::WriteToFile(m_writer, entity, m_pDB);
}

void CDXFEntityWriterVisitor::VisitDXFDimension(CDXFDimension &entity) 
{
    CDXFDimEntityWriterVisitor vis(m_writer, m_pDB);
    entity.AcceptDim(vis);
}

void CDXFEntityWriterVisitor::VisitDXFEllipse(CDXFEllipse &entity)
{
    dxf_ellipse_file_util::WriteToFile(m_writer, entity);
}