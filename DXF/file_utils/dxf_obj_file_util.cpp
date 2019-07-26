#include "stdafx.h"
#include "dxf_obj_file_util.h"

#include "DXFReader.h"
#include "base/DXFObjectID.h"
#include "base/DXFObject.h"
#include "DXFDatabase.h"
#include "tables/DXFDimStyleTableRecord.h"

#include "DXFWriter.h"

namespace dxf_obj_file_util
{
    void ReadFromFile(CDXFReader &reader, CDXFObject *pObj)
    {
        CDXFReaderItem item;
        bool bContinue = true;
        bool bOpenControlString = false;

        while (bContinue)
        {
            reader.ReadItem(item);
            switch (item.m_nGroupCode) {
            case 102:
                bOpenControlString = !bOpenControlString;
                break;

            case 330:			// Owner object
            {
                if (!bOpenControlString)
                {
                    CDXFObjectID ownerID;

                    reader.GetDatabase()->GetObjectID(ownerID, item.m_resInt);
                    pObj->SetOwnerID(ownerID);
                }
            }
            break;

            default:
                if (!bOpenControlString)
                    bContinue = false;
                break;
            }
        }

        reader.PushBackItem();
    }

    void WriteToFile(CDXFWriter &writer, const CDXFObject &obj)
    {
        if (dynamic_cast<const CDXFDimStyleTableRecord *>(&obj) == NULL) {
            writer.WriteHex(obj.m_objectID.GetHandle(), 5);
        }
        else {
            writer.WriteHex(obj.m_objectID.GetHandle(), 105);
        }

        //fprintf(pFile, "  330\n%X\n", m_ownerID.GetHandle());
    }
};