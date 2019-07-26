#ifndef __DXF_DIM_FILE_UTIL_H__
#define __DXF_DIM_FILE_UTIL_H__
#pragma once

class CDXFDimension;
class CDXFDatabase;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_dim_file_util
{
    void PostReadFromFile(CDXFReader &reader);
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFDimension *pDim);

    void WriteTofile(CDXFWriter &writer, const CDXFDimension &dim);
    void PostWriteToFile(CDXFWriter &writer, const CDXFDimension &dim, CDXFDatabase *pDB);
};

#endif