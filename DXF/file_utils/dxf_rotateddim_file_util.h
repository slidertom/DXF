#ifndef __DXF_ROTATED_DIM_FILE_UTIL_H__
#define __DXF_ROTATED_DIM_FILE_UTIL_H__
#pragma once

class CDXFRotatedDimension;
class CDXFDatabase;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"    
#endif

namespace dxf_rotated_dim_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFRotatedDimension *pDim);
    void WriteToFile(CDXFWriter &writer, const CDXFRotatedDimension &dim, CDXFDatabase *pDB);
};

#endif