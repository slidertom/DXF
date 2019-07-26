#include "stdafx.h"
#include "dxf_placeholder_file_util.h"

#include "DXFDatabase.h"
#include "DXFPlaceHolder.h"

#include "DXFWriter.h"

namespace dxf_placeholder_file_util
{
    void WriteToFile(CDXFWriter &writer, const CDXFPlaceHolder &holder)
    {
        writer.WriteText(holder.GetObjectName(), 0);
        writer.WriteHex(holder.m_objectID.GetHandle(), 5);
        writer.Write("  102\n{ACAD_REACTORS\n");
        writer.WriteHex(holder.m_ownerID.GetHandle(), 330);
        writer.Write("  102\n}\n");
        writer.WriteHex(holder.m_ownerID.GetHandle(), 330);
    }
};