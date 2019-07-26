#include "stdafx.h"
#include "dxf_hatch_pattern_file_util.h"

#include "DXFWriter.h"

namespace dxf_hatch_pattern_file_util
{
	void WriteToFile(CDXFWriter &writer, const CDXFHatch::CDXFHatchPattern &pattern)
    {
        writer.WriteFloatingPoint(pattern.m_dAngle, 53);
        writer.WriteFloatingPoint(pattern.m_basePoint.m_x, 43);
        writer.WriteFloatingPoint(pattern.m_basePoint.m_y, 44);
        writer.WriteFloatingPoint(pattern.m_offset.m_x, 45);
        writer.WriteFloatingPoint(pattern.m_offset.m_y, 46);
        writer.WriteNumber((int32_t)pattern.m_dashes.size(), 79);
	    for (auto dDash : pattern.m_dashes) {
            writer.WriteFloatingPoint(dDash, 49);
	    }
    }

	DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFHatch::CDXFHatchPattern &pattern)
    {
	    CDXFReaderItem item;
	    reader.ReadItem(item);

	    if (item.m_nGroupCode != 53) {
		    return DXF_ERROR_BAD_GROUPCODE;
        }
	    pattern.m_dAngle = item.m_resDouble;

	    reader.ReadItem(item);
	    if (item.m_nGroupCode != 43) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

	    pattern.m_basePoint.m_x = item.m_resDouble;

	    reader.ReadItem(item);
	    if (item.m_nGroupCode != 44) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

	    pattern.m_basePoint.m_y = item.m_resDouble;

	    reader.ReadItem(item);
	    if (item.m_nGroupCode != 45) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

	    pattern.m_offset.m_x = item.m_resDouble;

	    reader.ReadItem(item);
	    if (item.m_nGroupCode != 46) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

	    pattern.m_offset.m_y = item.m_resDouble;

	    reader.ReadItem(item);
	    if (item.m_nGroupCode != 79) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
	    const int32_t nNumDashes = item.m_resInt;

	    pattern.m_dashes.resize(nNumDashes);
	    for (auto i = 0; i < nNumDashes; ++i)
	    {
		    reader.ReadItem(item);
            if (item.m_nGroupCode != 49) {
                return DXF_ERROR_BAD_GROUPCODE;
            }

		    pattern.m_dashes[i] = item.m_resDouble;
	    }

        return DXF_OK;
    }

}