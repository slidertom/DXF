#include "stdafx.h"
#include "dxf_text_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_table_record_file_util.h"

#include "entity/DXFText.h"
#include "tables/DXFTextStyleTableRecord.h"

#include "Utils/DXFMath.h"
#include "Utils/DXFTextUtils.h"
#include "DXFDatabase.h"

#include "DXFWriter.h"

namespace dxf_text_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFText *pText)
    {
        ASSERT(reader.GetDatabase());

        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pText);
        
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        CDXF3DPoint point_ins;
        CDXF3DPoint point_align;
        CDXFObjectID text_id;
        double dRotAngle                = 0.0;
        double dScaleX                  = 1.0;
        double dObliqueAngle            = 0.0;
        double dHeight                  = 0.;
        unsigned char text_flags        = 0;
        ETextHorzAlignment align_horz   = eLeft;
        ETextVertAlignment align_vert   = eBaseline;
        std::string sText;

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 10:
                point_ins.x = item.m_resDouble;
                break;
            case 20:
                point_ins.y = item.m_resDouble;
                break;
            case 30:
                point_ins.z = item.m_resDouble;
                break;
            case 11:
                point_align.x = item.m_resDouble;
                break;
            case 21:
                point_align.y = item.m_resDouble;
                break;
            case 31:
                point_align.z = item.m_resDouble;
                break;
            case 40:
                dHeight = item.m_resDouble;
                break;
            case 41:
                dScaleX = item.m_resDouble;
                break;
            case 71:
                text_flags = item.m_resInt;
                break;
            case 72:
                align_horz = (ETextHorzAlignment)item.m_resInt;
                break;
            case 73:
                align_vert = (ETextVertAlignment)item.m_resInt;
                break;
            case 1:
                sText = item.m_resString;
                break;
            case 7:
                reader.GetDatabase()->GetTextStyleTable()->GetTableRecordId(item.m_resString, text_id);
                if ( text_id.IsNull() ) {
                    return DXF_ERROR_BAD_GROUPDATA;
                }
                break;
            case 50:
                dRotAngle = item.m_resDouble * DXF_PI / 180.0;
                break;

            case 51:
                dObliqueAngle = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        pText->SetInsertPoint(point_ins);
        pText->SetAlignPoint(point_align);
        pText->SetTextStyle(text_id);
        pText->SetRotationAngle(dRotAngle);
        pText->SetObliqueAngle(dObliqueAngle);
        pText->SetScaleX(dScaleX);
        pText->SetTextHeight(dHeight);
        pText->SetTextFlags(text_flags);
        pText->SetHorzAlignment(align_horz);
        pText->SetVertAlignment(align_vert);
        pText->SetText(sText.c_str());

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFText &text)
    {
        dxf_entity_file_util::WriteToFile(writer, text);

        writer.Write("  100\nAcDbText\n");

        const CDXF3DPoint &point_ins            = text.GetInsertPoint();
        const CDXF3DPoint &point_align          = text.GetAlingmentPoint();
        const double dHeight                    = text.GetTextHeight();
        const double dRotAngle                  = text.GetRotationAngle();
        const double dScaleX                    = text.GetScaleX();
        const double dObliqueAngle              = text.GetObliqueAngle();
        const CDXFObjectID &text_id             = text.GetTextStyle();
        const unsigned char text_flags          = text.GetTextFlags();
        const ETextHorzAlignment &align_horz    = text.GetHorzAlignment();
        const ETextVertAlignment &align_vert    = text.GetVertAlignment();
        std::string sText                       = text.GetText();

        writer.WritePoint(point_ins, 10, 20, 30);
        writer.WriteFloatingPoint(dHeight, 40);

        dxf_text_utils::FindAndReplaceEURO(sText);
        writer.WriteText(sText.c_str(), 1);

        if (!dxf_math::eq0(dRotAngle)) {
            writer.WriteFloatingPoint(dRotAngle * 180.0 / DXF_PI, 50);
        }

        if (dScaleX < 1.0 - DXF_EPSILON || dScaleX > 1.0 + DXF_EPSILON) {
            writer.WriteFloatingPoint(dScaleX, 41);
        }

        if ( !dxf_math::eq0(dObliqueAngle) ) {
            writer.WriteFloatingPoint(dObliqueAngle, 51);
        }

        if ( !text_id.IsNull() ) {
            CDXFTextStyleTableRecord *pRecord = (CDXFTextStyleTableRecord *)text_id.GetObject();
            writer.WriteText(pRecord->GetName(), 7);
        }

        if ( text_flags != 0) {
            writer.WriteNumber(text_flags, 71);
        }

        if (align_horz != 0) {
            writer.WriteNumber(align_horz, 72);
        }

        if (align_vert != 0 || align_horz != 0) {
            // The first alignment point is ignored
            writer.WritePoint(point_align, 11, 21, 31);
        }

        writer.Write("  100\nAcDbText\n");

        if (align_vert != 0)
        {
            writer.WriteNumber(align_vert, 73);
        }
    }
};