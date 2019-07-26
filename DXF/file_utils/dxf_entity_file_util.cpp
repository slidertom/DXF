#include "stdafx.h"
#include "dxf_entity_file_util.h"

#include "dxf_obj_file_util.h"
#include "dxf_symbol_table_file_util.h"

#include "DXFReader.h"
#include "tables/DXFLayerTableRecord.h"
#include "entity/DXFEntity.h"
#include "DXFDatabase.h"
#include "base/DXFObject.h"

#include "../Utils/DXFEntitySetLineWeightVisitor.h"

#include "DXFWriter.h"

namespace dxf_entity_file_util
{
    void PreReadFromFile(CDXFReader &reader, CDXFObject *pObj)
    {
        CDXFObjectID objectID;
        unsigned long ulHandle = reader.ReadHandle();
        reader.GetDatabase()->AddObject(pObj, objectID, ulHandle);
    }

    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFEntity *pEntity)
    {
        ASSERT(reader.GetDatabase());

        dxf_obj_file_util::ReadFromFile(reader, pEntity);

        CDXFReaderItem item;
        bool bContinue = true;

        while (bContinue)
        {
            reader.ReadItem(item);

            switch (item.m_nGroupCode)
            {
            case 100:
                if (0 != ::strcmpi(item.m_resString, "AcDbEntity"))
                {
                    bContinue = false;
                }
                break;
            case 67:
                break;
            case 8:
            {
                CDXFObjectID layerID;
                reader.GetDatabase()->GetLayerTable()->GetTableRecordId(item.m_resString, layerID);
                if (layerID.IsNull()) {
                    return DXF_ERROR_UNRECOGNIZED_LAYER;
                }
                pEntity->SetLayerID(layerID);
            }
            break;

            case 5:
            case 6:
            case 48:
            case 60:
            case 62:
            case 92:
            case 102:
            case 310:
            case 330:
            case 360:
            break;
            case 370:
            {
                CDXFEntitySetLineWeightVisitor vis((EDXFLineWeight)item.m_resInt);
                pEntity->Accept(vis);
            }
            break;
            case 410:
            case 420:
            case 430:
            case 440:
                break;

            default:
                bContinue = false;
                break;
            }
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void PreWriteToFile(CDXFWriter &writer, const CDXFEntity &entity)
    {
        ASSERT(!entity.GetLayerID().IsNull());
        writer.WriteText(entity.GetObjectName(), 0);
    }

    void WriteToFile(CDXFWriter &writer, const CDXFEntity &entity)
    {
        dxf_obj_file_util::WriteToFile(writer, entity);

        writer.Write("  100\nAcDbEntity\n");
        if (!entity.GetInModelSpace()) {
            writer.Write("  67\n1\n");
        }

        CDXFLayerTableRecord *pRecord = (CDXFLayerTableRecord *)entity.GetLayerID().GetObject();
        writer.WriteText(pRecord->GetName(), 8);
    }
};