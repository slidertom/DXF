#ifndef __DXF_DICTIONARY_FILE_UTIL_H__
#define __DXF_DICTIONARY_FILE_UTIL_H__
#pragma once

class CDXFReader;
class CDXFDictionary;
class CDXFDatabase;
class CDXFWriter;

namespace dxf_dict_file_util
{
    void PreReadFromFile(CDXFReader &reader, CDXFDictionary *pDict);
    void ReadFromFile(CDXFReader &reader, CDXFDictionary *pDict);
    void PreWriteToFile(CDXFWriter &writer, CDXFDictionary &dict, const CDXFDatabase *pDB);
};

#endif