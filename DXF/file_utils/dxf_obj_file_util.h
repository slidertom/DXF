#ifndef __DXF_OBJ_FILE_UTIL_H__
#define __DXF_OBJ_FILE_UTIL_H__
#pragma once

class CDXFReader;
class CDXFObject;
class CDXFDatabase;
class CDXFWriter;

namespace dxf_obj_file_util
{
    void ReadFromFile(CDXFReader &reader, CDXFObject *pObj);
    void WriteToFile(CDXFWriter &writer, const CDXFObject &obj);
};

#endif