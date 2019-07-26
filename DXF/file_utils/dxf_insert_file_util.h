#ifndef __DXF_INSERT_FILE_UTIL_H__
#define __DXF_INSERT_FILE_UTIL_H__
#pragma once

class CDXFInsert;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_insert_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFInsert *pInsert);
    void WriteToFile(CDXFWriter &writer, const CDXFInsert &insert);
};

#endif