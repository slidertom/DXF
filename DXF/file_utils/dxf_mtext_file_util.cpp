#include "stdafx.h"
#include "dxf_mtext_file_util.h"

#include "tables/DXFTextStyleTableRecord.h"

#include "dxf_entity_file_util.h"
#include "dxf_table_record_file_util.h"

#include "entity/DXFMText.h"

#include "Utils/DXFTextUtils.h"
#include "Utils/DXFMath.h"

#include "DXFDatabase.h"

#include "DXFWriter.h"

namespace dxf_mtext_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFMText *pText)
    {
        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pText);
        
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);
        CDXFObjectID id;
        double dptX = 0.0;
        double dptY = 0.0;
        double dptZ = 0.0;

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 10:
                dptX = item.m_resDouble;
                break;
            case 20:
                dptY = item.m_resDouble;
                break;
            case 30:
                dptZ = item.m_resDouble;
                break;
            case 40:
                pText->SetTextHeight(item.m_resDouble);
                break;
            case 41:
                pText->SetRectangleWidth(item.m_resDouble);
                break;
            case 71:
                pText->SetAttachement((EMTextAttachement)item.m_resInt);
                break;
            case 72:
                pText->SetDirection((EMTextDirection)item.m_resInt);
                break;
            case 1:
                pText->SetText(item.m_resString);
                break;
            case 7:
                reader.GetDatabase()->GetTextStyleTable()->GetTableRecordId(item.m_resString, id);
                pText->SetTextStyleID(id);
                if ( id.IsNull() ) {
                    return DXF_ERROR_BAD_GROUPDATA;
                }
                break;
            case 50:
                pText->SetAngle(item.m_resDouble);
                break;
            }

            reader.ReadItem(item);
        }

        pText->SetInsertPoint(dptX, dptY, dptZ);
        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFMText &text)
    {
        dxf_entity_file_util::WriteToFile(writer, text);

        writer.Write("  100\nAcDbMText\n");

        const CDXF3DPoint &point = text.GetInsertPoint();

        writer.WritePoint(point, 10, 20, 30);
        writer.WriteFloatingPoint(text.GetTextHeight(), 40);
        writer.WriteFloatingPoint(text.GetRectangleWidth(), 41);
        writer.WriteNumber(text.GetAttachement(), 71);
        writer.WriteNumber(text.GetDirection(), 72);

        std::string sText = text.GetText();
        dxf_text_utils::FindAndReplaceEURO(sText);

        writer.WriteText(sText.c_str(), 1);

        if (!text.GetTextStyleID().IsNull()) {
            CDXFTextStyleTableRecord *pRecord = (CDXFTextStyleTableRecord *)text.GetTextStyleID().GetObject();
            writer.WriteText(pRecord->GetName(), 7);
        }

        if (text.GetAngle() - DXF_EPSILON < 0.0 || text.GetAngle() + DXF_EPSILON > 0.0) {
            writer.WriteFloatingPoint(text.GetAngle(), 50);
        }

        writer.Write("  73\n1\n  44\n1.0\n");
    }
};