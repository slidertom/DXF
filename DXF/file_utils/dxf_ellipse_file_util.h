#ifndef __DXF_ELLIPSE_FILE_UTIL_H__
#define __DXF_ELLIPSE_FILE_UTIL_H__
#pragma once

class CDXFEllipse;
class CDXFDatabase;
class CDXFWriter;

namespace dxf_ellipse_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFEllipse &ellipse);
};

#endif