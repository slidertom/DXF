#include "stdafx.h"
#include "dxf_mlinestyle_file_util.h"

#include "DXFDatabase.h"
#include "../base/DXFMlineStyle.h"

#include "DXFWriter.h"

namespace dxf_mline_style_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFMlineStyle &line)
    {
        writer.WriteText(line.GetObjectName(), 0);
        writer.WriteHex(line.m_objectID.GetHandle(), 5);
        writer.Write("  102\n{ACAD_REACTORS\n");
        writer.WriteHex(line.m_ownerID.GetHandle(), 330);
        writer.Write("  102\n}\n");
        writer.Write("  100\nAcDbMlineStyle\n");
        writer.WriteText(line.m_sStyleName.c_str(), 2);
        writer.WriteNumber(line.m_nFlags, 70);
        writer.WriteText(line.m_sStyleDescription.c_str(), 3);
        writer.WriteNumber(line.m_nFillColor, 62);
        writer.WriteFloatingPoint(line.m_fStartAngle, 51);
        writer.WriteFloatingPoint(line.m_fEndAngle, 52);
        writer.Write("  71\n2\n");
        writer.Write("  49\n0.5\n  62\n256\n  6\nBYLAYER\n");
        writer.Write("  49\n-0.5\n  62\n256\n  6\nBYLAYER\n");
    }
};