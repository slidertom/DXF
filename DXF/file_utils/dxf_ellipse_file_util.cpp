#include "stdafx.h"
#include "dxf_ellipse_file_util.h"

#include "dxf_entity_file_util.h"

#include "entity/DXFEllipse.h"
#include "DXFDatabase.h"

#include "DXFWriter.h"

namespace dxf_ellipse_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFEllipse &ellipse)
    {
        dxf_entity_file_util::WriteToFile(writer, ellipse);

        writer.Write("  100\nAcDbEllipse\n");

        const CDXF3DPoint &ptCenter = ellipse.GetCenterPoint();
        const CDXF3DPoint &ptEnd    = ellipse.GetEndPoint();

        writer.WritePoint(ptCenter, 10, 20, 30);
        writer.WritePoint(ptEnd, 11, 21, 31);

        writer.WriteFloatingPoint(ellipse.GetMinorAxisRatio(), 40);
        writer.WriteFloatingPoint(ellipse.GetStartValue(), 41);
        writer.WriteFloatingPoint(ellipse.GetEndValue(), 42);
    }
};