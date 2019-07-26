#ifndef __DXF_MLINE_STYLE_FILE_UTIL_H__
#define __DXF_MLINE_STYLE_FILE_UTIL_H__
#pragma once

class CDXFMlineStyle;
class CDXFWriter;

namespace dxf_mline_style_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFMlineStyle &line);
};

#endif