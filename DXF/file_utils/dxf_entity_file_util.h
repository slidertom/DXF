#ifndef __DXF_ENTITY_FILE_UTIL_H__
#define __DXF_ENTITY_FILE_UTIL_H__
#pragma once

class CDXFObject;
class CDXFEntity;
class CDXFWriter;

#ifndef _DXF_READER_H__
    #include "DXFReader.h"
#endif

namespace dxf_entity_file_util
{
    void PreReadFromFile(CDXFReader &reader, CDXFObject *pObj);
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFEntity *pEntity);
    void PreWriteToFile(CDXFWriter &writer, const CDXFEntity &entity);
    void WriteToFile(CDXFWriter &writer, const CDXFEntity &entity);
};

#endif