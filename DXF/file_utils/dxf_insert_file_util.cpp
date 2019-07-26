#include "stdafx.h"
#include "dxf_insert_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_table_record_file_util.h"

#include "entity/DXFInsert.h"
#include "DXFDatabase.h"

#include "Utils/DXFMath.h"

#include "DXFWriter.h"

namespace dxf_insert_file_util
{
    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFInsert *pInsert)
    {
        ASSERT(reader.GetDatabase());

        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pInsert);
        
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        double dScaleX(1.0);
        double dScaleY(1.0);
        double dScaleZ(1.0);

        CDXF3DPoint pointIns;

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
            {
                CDXFObjectID id;
                reader.GetDatabase()->GetBlockTable()->GetTableRecordId(item.m_resString, id);
                if (id.IsNull()) {
                    return DXF_ERROR_UNRECOGNIZED_BLOCK;
                }
                else {
                    pInsert->SetBlockTableRecordID(id);
                    CDXFBlockTableRecord *pBlock = pInsert->GetBlock();
                    pBlock->SetLineWeight(pInsert->GetLineWeight());
                }
            }
            break;
            case 10:
                pointIns.x = item.m_resDouble;
                break;
            case 20:
                pointIns.y = item.m_resDouble;
                break;
            case 30:
                pointIns.z = item.m_resDouble;
                break;
            case 41:
                dScaleX = item.m_resDouble;
                break;
            case 42:
                dScaleY = item.m_resDouble;
                break;
            case 43:
                dScaleZ = item.m_resDouble;
                break;
            case 50:
                pInsert->SetAngle(item.m_resDouble * DXF_PI / 180.0);
                break;
            }

            reader.ReadItem(item);
        }

        pInsert->SetScale(dScaleX, dScaleY, dScaleZ);
        pInsert->SetInsertPoint(pointIns);

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteToFile(CDXFWriter &writer, const CDXFInsert &insert)
    {
        if (insert.IsNullBlockID())
        {
            //the only case noticed when this assert occures
            //is multiple times blocked into itself blocks.
            //this case is not supported.
            ASSERT(false);
            return;
        }

        dxf_entity_file_util::WriteToFile(writer, insert);

        writer.Write("  100\nAcDbBlockReference\n");

        CDXFBlockTableRecord *pRecord = insert.GetBlock();
        writer.WriteText(pRecord->GetName(), 2);
        writer.WritePoint(insert.GetInsertPoint(), 10, 20, 30);

        const CDXF3DPoint ptScale = insert.GetScale();

        if (ptScale.x != 1.0 || ptScale.y != 1.0 || ptScale.z != 1.0) {
            writer.WritePoint(ptScale, 41, 42, 43);
        }

        const double dAngle = insert.GetAngle();
        if ( !dxf_math::eq0(dAngle) ) {
            writer.WriteFloatingPoint(dAngle * 180.0 / DXF_PI, 50);
        }
    }
};