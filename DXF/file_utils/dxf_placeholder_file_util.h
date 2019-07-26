#ifndef __DXF_PLACEHOLDER_FILE_UTIL_H__
#define __DXF_PLACEHOLDER_FILE_UTIL_H__
#pragma once

class CDXFPlaceHolder;
class CDXFWriter;

namespace dxf_placeholder_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFPlaceHolder &place);
};

#endif