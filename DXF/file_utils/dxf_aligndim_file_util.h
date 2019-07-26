#ifndef __DXF_ALIGN_DIM_FILE_UTIL_H__
#define __DXF_ALIGN_DIM_FILE_UTIL_H__
#pragma once

class CDXFAlignedDimension;
class CDXFDatabase;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_align_dim_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFAlignedDimension *pDim);
    void WriteToFile(CDXFWriter &writer, const CDXFAlignedDimension &dim, CDXFDatabase *pDatabase);
};

#endif