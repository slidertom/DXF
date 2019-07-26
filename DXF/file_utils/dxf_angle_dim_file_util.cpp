#include "stdafx.h"
#include "dxf_angle_dim_file_util.h"

#include "dxf_dim_file_util.h"
#include "dxf_entity_file_util.h"

#include "DXFReader.h"
#include "DXFWriter.h"

#include "entity/DXFAngledDimension.h"
#include "DXFDatabase.h"

#include "Utils/DXFMath.h"
#include "Utils/DXFTextUtils.h"

namespace dxf_angle_dim_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFAngledDimension &dim, CDXFDatabase *pDB)
    {
        dxf_dim_file_util::WriteTofile(writer, dim);

        writer.Write("  100\nAcDb2LineAngularDimension\n");

        const CDXF3DPoint &ptCenter = dim.GetCenterPoint();
        const CDXF3DPoint &pt2      = dim.GetSecondPoint();
        const CDXF3DPoint &pt3      = dim.GetThirdPoint();

        writer.WritePoint(ptCenter, 13, 23, 33);
        writer.WritePoint(pt2,      14, 24, 34);
        writer.WritePoint(ptCenter, 15, 25, 35);
        writer.WritePoint(pt3,      16, 26, 36);

        dxf_dim_file_util::PostWriteToFile(writer, dim, pDB);
    }
};