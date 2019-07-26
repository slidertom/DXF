#ifndef __DXF_ANGLE_DIM_FILE_UTIL_H__
#define __DXF_ANGLE_DIM_FILE_UTIL_H__
#pragma once

class CDXFAngledDimension;
class CDXFDatabase;
class CDXFWriter;

namespace dxf_angle_dim_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFAngledDimension &dim, CDXFDatabase *pDB);
};

#endif