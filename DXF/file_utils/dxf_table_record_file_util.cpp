#include "stdafx.h"
#include "dxf_table_record_file_util.h"

#include "dxf_obj_file_util.h"
#include "tables/DXFLineTypeTableRecord.h"
#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFBlockTableRecord.h"
#include "tables/DXFTextStyleTableRecord.h"
#include "tables/DXFSymbolTableRecord.h"
#include "DXFDatabase.h"
#include "base/DXFObject.h"
#include "tables/DXFDimStyleTableRecord.h"
#include "tables/DXFViewTableRecord.h"

#include "algorithm"

#include "DXFWriter.h"

namespace dxf_table_record_file_util
{
    namespace internal
    {
        void PrintFloatvar(CDXFWriter &writer, int32_t nNum, double dVal)
        {
            writer.WriteFloatingPoint(dVal, nNum);
        }

        void PrintIntvar(CDXFWriter &writer, int32_t nNum, int32_t nVal)
        {
            writer.WriteNumber(nVal, nNum);
        }
    }

    void ReadFromFileSymbolTableRecord(CDXFReader &reader, CDXFSymbolTableRecord *pRecord)
    {
        dxf_obj_file_util::ReadFromFile(reader, pRecord);

        CDXFReaderItem item;

        reader.ReadItem(item);

        while (item.m_nGroupCode != 100) {
            reader.ReadItem(item);
        }
    }

    DXF_ERROR GetObjectHandle(CDXFReader &reader, const char *sObjectName, unsigned long &ulHandle)
    {
        CDXFReaderItem item;
        reader.ReadItem(item);

        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        if (::strcmpi(item.m_resString, sObjectName) != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        ulHandle = reader.ReadHandle();
        return DXF_OK;
    }

    DXF_ERROR PreReadFromFileSymbolTableRecord(CDXFReader &reader, CDXFSymbolTableRecord *pRecord)
    {
        unsigned long ulHandle;
        const DXF_ERROR ret_val = GetObjectHandle(reader, pRecord->GetObjectName(), ulHandle);
        if ( ret_val != DXF_OK ) {
            return ret_val;
        }
        
        CDXFObjectID objectID;
        reader.GetDatabase()->AddObject(pRecord, objectID, ulHandle);

        return DXF_OK;
    }

    void ReadFromFileViewReportRecord(CDXFReader &reader, CDXFViewportTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            case 10:
                pRecord->m_lowerLeftCorner.x = item.m_resDouble;
                break;
            case 20:
                pRecord->m_lowerLeftCorner.y = item.m_resDouble;
                break;
            case 11:
                pRecord->m_upperRightCorner.x = item.m_resDouble;
                break;
            case 21:
                pRecord->m_upperRightCorner.y = item.m_resDouble;
                break;
            case 12:
                pRecord->m_pointCenter.x = item.m_resDouble;
                break;
            case 22:
                pRecord->m_pointCenter.y = item.m_resDouble;
                break;
            case 13:
                pRecord->m_snapBase.x = item.m_resDouble;
                break;
            case 23:
                pRecord->m_snapBase.y = item.m_resDouble;
                break;
            case 14:
                pRecord->m_snapSpacing.x = item.m_resDouble;
                break;
            case 24:
                pRecord->m_snapSpacing.y = item.m_resDouble;
                break;
            case 15:
                pRecord->m_gridSpacing.x = item.m_resDouble;
                break;
            case 25:
                pRecord->m_gridSpacing.y = item.m_resDouble;
                break;
            case 16:
                pRecord->m_viewDirection.x = item.m_resDouble;
                break;
            case 26:
                pRecord->m_viewDirection.y = item.m_resDouble;
                break;
            case 36:
                pRecord->m_viewDirection.z = item.m_resDouble;
                break;
            case 17:
                pRecord->m_pointTarget.x = item.m_resDouble;
                break;
            case 27:
                pRecord->m_pointTarget.y = item.m_resDouble;
                break;
            case 37:
                pRecord->m_pointTarget.z = item.m_resDouble;
                break;
            case 40:
                pRecord->m_dHeight = item.m_resDouble;
                break;
            case 41:
                pRecord->m_dWidth = item.m_resDouble * pRecord->m_dHeight;
                break;
            case 43:
                pRecord->m_dFrontClipDistance = item.m_resDouble;
                break;
            case 44:
                pRecord->m_dBackClipDistance = item.m_resDouble;
                break;
            case 75:
                pRecord->m_fSnapON = item.m_resInt;
                break;
            case 76:
                pRecord->m_fGridON = item.m_resInt;
                break;
            case 65:
                pRecord->m_bUCSAttached = item.m_resInt == 1 ? true : false;
                break;
            case 110:
                pRecord->m_ucsOrigin.x = item.m_resDouble;
                break;
            case 120:
                pRecord->m_ucsOrigin.y = item.m_resDouble;
                break;
            case 130:
                pRecord->m_ucsOrigin.z = item.m_resDouble;
                break;
            case 111:
                pRecord->m_ucsXAxis.x = item.m_resDouble;
                break;
            case 121:
                pRecord->m_ucsXAxis.y = item.m_resDouble;
                break;
            case 131:
                pRecord->m_ucsXAxis.z = item.m_resDouble;
                break;
            case 112:
                pRecord->m_ucsYAxis.x = item.m_resDouble;
                break;
            case 122:
                pRecord->m_ucsYAxis.y = item.m_resDouble;
                break;
            case 132:
                pRecord->m_ucsYAxis.z = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    void ReadFromFileLineTypeRecord(CDXFReader &reader, CDXFLineTypeTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        reader.ReadItem(item);

        int32_t nNumElements = -1;

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            case 3:
                pRecord->SetDescr(item.m_resString);
                break;
            case 72:
                pRecord->m_nAlignmentCode = item.m_resInt;
                break;
            case 73:
                pRecord->m_nNumDashes = item.m_resInt;
                break;
            case 40:
                pRecord->m_dPatternLength = item.m_resDouble;
                break;
            case 49:
                pRecord->m_dDashLengths[++nNumElements] = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    DXF_ERROR ReadFromFileLayerRecord(CDXFReader &reader, CDXFLayerTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        std::string sLineType;        

        CDXFReaderItem item;
        reader.ReadItem(item);
        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            case 62:
                pRecord->SetColor((EDXFColor)item.m_resInt);
                break;
            case 6:
                sLineType = item.m_resString;
                break;
            case 290:
                pRecord->SetPlottable(1 == item.m_resInt);
                break;
            case 370:
                pRecord->SetLineWeight((EDXFLineWeight)item.m_resInt);
                break;
            }

            reader.ReadItem(item);
        }

        if (sLineType != "")
        {
            CDXFObjectID linetypeID;
            reader.GetDatabase()->GetLineTypeTable()->GetTableRecordId(sLineType.c_str(), linetypeID);
            if (linetypeID.IsNull()) {
                return DXF_ERROR_UNRECOGNIZED_LINETYPE;
            }
            pRecord->SetLinetype(linetypeID);
        }
        else {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void ReadFromFileTextStyleRecord(CDXFReader &reader, CDXFTextStyleTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;

        reader.ReadItem(item);
        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            case 40:
                pRecord->m_dTextHeight = item.m_resDouble;
                break;
            case 41:
                pRecord->m_dWidthFactor = item.m_resDouble;
                break;
            case 50:
                pRecord->m_dObliqueAngle = item.m_resDouble;
                break;
            case 71:
                pRecord->m_fTextFlags = item.m_resInt;
                break;
            case 42:
                pRecord->m_dPriorTextHeight = item.m_resDouble;
                break;
            case 3:
                pRecord->m_sFontFile = item.m_resString;
                break;
            case 4:
                pRecord->m_sBigFontFile = item.m_resString;
                break;
                //extended data handling
            case 1001:
                pRecord->m_bExDataInitiated = true;
                pRecord->m_sExApplName = item.m_resString;
                break;
            case 1000:
                pRecord->m_sExFontName = item.m_resString;
                break;
            case 1071:
                pRecord->m_nExFontCode = item.m_resInt;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    void ReadFromFileViewRecord(CDXFReader &reader, CDXFViewTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            case 40:
                pRecord->m_dHeight = item.m_resDouble;
                break;
            case 10:
                pRecord->m_pointCenter.x = item.m_resDouble;
                break;
            case 20:
                pRecord->m_pointCenter.y = item.m_resDouble;
                break;
            case 41:
                pRecord->m_dWidth = item.m_resDouble;
                break;
            case 11:
                pRecord->m_viewDirection.x = item.m_resDouble;
                break;
            case 21:
                pRecord->m_viewDirection.y = item.m_resDouble;
                break;
            case 31:
                pRecord->m_viewDirection.z = item.m_resDouble;
                break;
            case 12:
                pRecord->m_pointTarget.x = item.m_resDouble;
                break;
            case 22:
                pRecord->m_pointTarget.y = item.m_resDouble;
                break;
            case 32:
                pRecord->m_pointTarget.z = item.m_resDouble;
                break;
            case 43:
                pRecord->m_dFrontClipDistance = item.m_resDouble;
                break;
            case 44:
                pRecord->m_dBackClipDistance = item.m_resDouble;
                break;
            case 72:
                if (item.m_resInt == 0) {
                    pRecord->m_bUCSAttached = false;
                }
                else {
                    pRecord->m_bUCSAttached = true;
                }
                break;
            case 110:
                pRecord->m_ucsOrigin.x = item.m_resDouble;
                break;
            case 120:
                pRecord->m_ucsOrigin.y = item.m_resDouble;
                break;
            case 130:
                pRecord->m_ucsOrigin.z = item.m_resDouble;
                break;
            case 111:
                pRecord->m_ucsXAxis.x = item.m_resDouble;
                break;
            case 121:
                pRecord->m_ucsXAxis.y = item.m_resDouble;
                break;
            case 131:
                pRecord->m_ucsXAxis.z = item.m_resDouble;
                break;
            case 112:
                pRecord->m_ucsYAxis.x = item.m_resDouble;
                break;
            case 122:
                pRecord->m_ucsYAxis.y = item.m_resDouble;
                break;
            case 132:
                pRecord->m_ucsYAxis.z = item.m_resDouble;
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    void ReadFromFileUCSRecord(CDXFReader &reader, CDXFUCSTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0) {
            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    void ReadFromFileAppIdRecord(CDXFReader &reader, CDXFAppIDTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        reader.ReadItem(item);
        while (item.m_nGroupCode) {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            }
            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    void ReadFromFileDimStyleRecord(CDXFReader &reader, CDXFDimStyleTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        unsigned long ulDimTxStyHandle(0);

        reader.ReadItem(item);
        while (item.m_nGroupCode != 0) {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            case 70:
                pRecord->SetFlags(item.m_resInt);
                break;
            case 3:
                pRecord->m_sDimpost = item.m_resString;
                break;
            case 4:
                pRecord->m_sDimapost = item.m_resString;
                break;
            case 40:
                pRecord->m_dDimScale = item.m_resDouble;
                break;
            case 41:
                pRecord->m_dDimASz = item.m_resDouble;
                break;
            case 42:
                pRecord->m_dDimExo = item.m_resDouble;
                break;
            case 43:
                pRecord->m_dDimDli = item.m_resDouble;
                break;
            case 44:
                pRecord->m_dDimExe = item.m_resDouble;
                break;
            case 45:
                pRecord->m_dDimRnd = item.m_resDouble;
                break;
            case 46:
                pRecord->m_dDimDle = item.m_resDouble;
                break;
            case 47:
                pRecord->m_dDimTp = item.m_resDouble;
                break;
            case 48:
                pRecord->m_dDimTm = item.m_resDouble;
                break;
            case 140:
                pRecord->m_dDimTxt = item.m_resDouble;
                break;
            case 141:
                pRecord->m_dDimCen = item.m_resDouble;
                break;
            case 142:
                pRecord->m_dDimTsz = item.m_resDouble;
                break;
            case 143:
                pRecord->m_dDimAltf = item.m_resDouble;
                break;
            case 144:
                pRecord->m_dDimLFac = item.m_resDouble;
                break;
            case 145:
                pRecord->m_dDimTvp = item.m_resDouble;
                break;
            case 146:
                pRecord->m_dDimTFac = item.m_resDouble;
                break;
            case 147:
                pRecord->m_dDimGap = item.m_resDouble;
                break;
            case 148:
                pRecord->m_dDimAltRnd = item.m_resDouble;
                break;
            case 71:
                pRecord->m_nDimTol = item.m_resInt;
                break;
            case 72:
                pRecord->m_nDimLim = item.m_resInt;
                break;
            case 73:
                pRecord->m_nDimTih = item.m_resInt;
                break;
            case 74:
                pRecord->m_nDimToh = item.m_resInt;
                break;
            case 75:
                pRecord->m_nDimSe1 = item.m_resInt;
                break;
            case 76:
                pRecord->m_nDimSe2 = item.m_resInt;
                break;
            case 77:
                pRecord->m_nDimTad = item.m_resInt;
                break;
            case 78:
                pRecord->m_nDimZin = item.m_resInt;
                break;
            case 79:
                pRecord->m_nDimAzin = item.m_resInt;
                break;
            case 170:
                pRecord->m_nDimAlt = item.m_resInt;
                break;
            case 171:
                pRecord->m_nDimAltd = item.m_resInt;
                break;
            case 172:
                pRecord->m_nDimTofl = item.m_resInt;
                break;
            case 173:
                pRecord->m_nDimSah = item.m_resInt;
                break;
            case 174:
                pRecord->m_nDimTix = item.m_resInt;
                break;
            case 175:
                pRecord->m_nDimSoxd = item.m_resInt;
                break;
            case 176:
                pRecord->m_nDimClrd = item.m_resInt;
                break;
            case 177:
                pRecord->m_nDimClre = item.m_resInt;
                break;
            case 178:
                pRecord->m_nDimClrt = item.m_resInt;
                break;
            case 179:
                pRecord->m_nDimADec = item.m_resInt;
                break;
            case 271:
                pRecord->m_nDimDec = item.m_resInt;
                break;
            case 272:
                pRecord->m_nDimTDec = item.m_resInt;
                break;
            case 273:
                pRecord->m_nDimAltU = item.m_resInt;
                break;
            case 274:
                pRecord->m_nDimAltTD = item.m_resInt;
                break;
            case 275:
                pRecord->m_nDimAUnit = item.m_resInt;
                break;
            case 276:
                pRecord->m_nDimFrac = item.m_resInt;
                break;
            case 277:
                pRecord->m_nDimLUnit = item.m_resInt;
                break;
            case 278:
                pRecord->m_nDimDSep = item.m_resInt;
                break;
            case 279:
                pRecord->m_nDimTMove = item.m_resInt;
                break;
            case 280:
                pRecord->m_nDimJust = item.m_resInt;
                break;
            case 281:
                pRecord->m_nDimSD1 = item.m_resInt;
                break;
            case 282:
                pRecord->m_nDimSD2 = item.m_resInt;
                break;
            case 283:
                pRecord->m_nDimTolJ = item.m_resInt;
                break;
            case 284:
                pRecord->m_nDimTZin = item.m_resInt;
                break;
            case 285:
                pRecord->m_nDimAltZ = item.m_resInt;
                break;
            case 286:
                pRecord->m_nDimAltTZ = item.m_resInt;
                break;
            case 287:
                pRecord->m_nDimAtfFit = item.m_resInt;
                break;
            case 288:
                pRecord->m_nDimUPt = item.m_resInt;
                break;
            case 340:
                ulDimTxStyHandle = item.m_resInt;
                break;
            case 341:
                pRecord->m_ulDimLdrBlkHandle = item.m_resInt;
                break;
            case 342:
                pRecord->m_ulDimBlkHandle = item.m_resInt;
                break;
            case 343:
                pRecord->m_ulDimBlk1Handle = item.m_resInt;
                break;
            case 344:
                pRecord->m_ulDimBlk2Handle = item.m_resInt;
                break;
            }

            reader.ReadItem(item);
        }

        //TextStyleID
        {
            CDXFObjectID id;
            if(0 != ulDimTxStyHandle) {
                id = reader.GetDatabase()->GetDXFObjectID(ulDimTxStyHandle);
            }

            //Default ID name is "STANDARD"
            if (id.IsNull()) {
                reader.GetDatabase()->GetTextStyleTable()->GetTableRecordId("STANDARD", id);
            }

            if (!id.IsNull()) {
                pRecord->SetDimTxSty(id);
            }
        }

        reader.PushBackItem();
    }

    void ReadFromFileDimBlockRecord(CDXFReader &reader, CDXFBlockTableRecord *pRecord)
    {
        dxf_table_record_file_util::ReadFromFileSymbolTableRecord(reader, pRecord);

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 2:
                pRecord->SetName(item.m_resString);
                break;
            }

            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    void WriteToFileSymbolTableRecord(CDXFWriter &writer, const CDXFSymbolTableRecord &record)
    {
        dxf_obj_file_util::WriteToFile(writer, record);

        writer.Write("  100\nAcDbSymbolTableRecord\n");
    }

    void PreWriteToFileSymbolTableRecord(CDXFWriter &writer, const CDXFSymbolTableRecord &record)
    {
        writer.WriteText(record.GetObjectName(), 0);
    }

    void WriteToFileViewReportRecord(CDXFWriter &writer, const CDXFViewportTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbViewportTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);

        writer.Write2DPoint(record.m_lowerLeftCorner, 10, 20);
        writer.Write2DPoint(record.m_upperRightCorner, 11, 21);
        writer.Write2DPoint(record.m_pointCenter, 12, 22);
        writer.Write2DPoint(record.m_snapBase, 13, 23);
        writer.Write2DPoint(record.m_snapSpacing, 14, 24);
        writer.Write2DPoint(record.m_gridSpacing, 15, 25);
        writer.WritePoint(record.m_viewDirection, 16, 26, 36);
        writer.WritePoint(record.m_pointTarget, 17, 27, 37);
        writer.WriteFloatingPoint(record.m_dHeight, 40);
        writer.WriteFloatingPoint(record.m_dWidth / record.m_dHeight, 41);

        writer.WriteFloatingPoint(record.m_dFrontClipDistance, 43);
        writer.WriteFloatingPoint(record.m_dBackClipDistance, 44);

        writer.WriteNumber(record.m_fSnapON, 75);
        writer.WriteNumber(record.m_fGridON, 76);

        writer.Write("  281\n0\n");

        writer.WriteNumber(record.m_bUCSAttached ? 1 : 0, 65);
        if (record.m_bUCSAttached) {
            writer.WritePoint(record.m_ucsOrigin, 110, 120, 130);
            writer.WritePoint(record.m_ucsXAxis, 111, 121, 131);
            writer.WritePoint(record.m_ucsYAxis, 112, 122, 132);
            writer.Write("  79\n0\n");
            writer.Write("  146\n0.0\n");
        }
    }

    void WriteToFileLineTypeRecord(CDXFWriter &writer, const CDXFLineTypeTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbLinetypeTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);
        writer.WriteText(record.GetDescr(), 3);
        writer.WriteNumber(record.m_nAlignmentCode, 72);
        writer.WriteNumber(record.m_nNumDashes, 73);
        writer.WriteFloatingPoint(record.m_dPatternLength, 40);

        for (auto i = 0; i < record.m_nNumDashes; ++i) {
            writer.WriteFloatingPoint(record.m_dDashLengths[i], 49);
            writer.Write("  74\n0\n");
        }
    }

    void WriteToFileLayerRecord(CDXFWriter &writer, const CDXFLayerTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        ASSERT(!record.GetLinetypeID().IsNull());

        writer.Write("  100\nAcDbLayerTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);
        writer.WriteNumber((int32_t)record.GetColor(), 62);
        CDXFLineTypeTableRecord *pRec = (CDXFLineTypeTableRecord *)record.GetLinetypeID().GetObject();
        writer.WriteText(pRec->GetName(), 6);

        if (!record.IsPlottable()) {
            //Plotting flag. If set to 0, do not plot this layer
            writer.WriteNumber(0, 290);
        }

        writer.WriteNumber((int32_t)record.GetLineWeight(), 370);
        writer.WriteHex(record.GetPlotStyleNameID().GetHandle(), 390);
    }

    void WriteToFileTextStyleRecord(CDXFWriter &writer, const CDXFTextStyleTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbTextStyleTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);
        writer.WriteFloatingPoint(record.m_dTextHeight, 40);
        writer.WriteFloatingPoint(record.m_dWidthFactor, 41);
        writer.WriteFloatingPoint(record.m_dObliqueAngle, 50);
        writer.WriteNumber(record.m_fTextFlags, 71);
        writer.WriteFloatingPoint(record.m_dPriorTextHeight, 42);
        writer.WriteText(record.m_sFontFile.c_str(), 3);
        writer.WriteText(record.m_sBigFontFile.c_str(), 4);

        if (record.m_bExDataInitiated) {
            writer.WriteText(record.m_sExApplName.c_str(), 1001);
            writer.WriteText(record.m_sExFontName.c_str(), 1000);
            writer.WriteNumber(record.m_nExFontCode, 1071);
        }
    }

    void WriteToFileViewRecord(CDXFWriter &writer, const CDXFViewTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbViewTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);
        writer.WriteFloatingPoint(record.m_dHeight, 40);
        writer.Write2DPoint(record.m_pointCenter, 10, 20);
        writer.WriteFloatingPoint(record.m_dWidth, 41);
        writer.WritePoint(record.m_viewDirection, 11, 21, 31);
        writer.WritePoint(record.m_pointTarget, 12, 22, 32);
        writer.WriteFloatingPoint(record.m_dLensLength, 42);
        writer.WriteFloatingPoint(record.m_dFrontClipDistance, 43);
        writer.WriteFloatingPoint(record.m_dBackClipDistance, 44);

        writer.Write("  281\n0\n");
        writer.WriteNumber((record.m_bUCSAttached ? 1 : 0), 72);

        if (record.m_bUCSAttached) {
            writer.WritePoint(record.m_ucsOrigin, 110, 120, 130);
            writer.WritePoint(record.m_ucsXAxis, 111, 121, 131);
            writer.WritePoint(record.m_ucsYAxis, 112, 122, 132);
            writer.Write("  79\n0\n");
            writer.Write("  146\n0.0\n");
        }
    }

    void WriteToFileUCSRecord(CDXFWriter &writer, const CDXFUCSTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);
    }

    void WriteToFileAppIdRecord(CDXFWriter &writer, const CDXFAppIDTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbRegAppTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);
    }

    void WriteToFileDimStyleRecord(CDXFWriter &writer, CDXFDimStyleTableRecord &record, CDXFDatabase *pDB)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbDimStyleTableRecord\n");

        writer.WriteText(record.GetName(), 2);
        writer.WriteNumber(record.GetFlags(), 70);
        writer.WriteText(record.m_sDimpost.c_str(), 3);
        writer.WriteText(record.m_sDimapost.c_str(), 4);

        internal::PrintFloatvar(writer, 40, record.m_dDimScale);
        internal::PrintFloatvar(writer, 41, record.m_dDimASz);
        internal::PrintFloatvar(writer, 42, record.m_dDimExo);
        internal::PrintFloatvar(writer, 43, record.m_dDimDli);
        internal::PrintFloatvar(writer, 44, record.m_dDimExe);
        internal::PrintFloatvar(writer, 45, record.m_dDimRnd);
        internal::PrintFloatvar(writer, 46, record.m_dDimDle);
        internal::PrintFloatvar(writer, 47, record.m_dDimTp);
        internal::PrintFloatvar(writer, 48, record.m_dDimTm);
        internal::PrintFloatvar(writer, 140, record.m_dDimTxt);
        internal::PrintFloatvar(writer, 141, record.m_dDimCen);
        internal::PrintFloatvar(writer, 142, record.m_dDimTsz);
        internal::PrintFloatvar(writer, 143, record.m_dDimAltf);
        internal::PrintFloatvar(writer, 144, record.m_dDimLFac);
        internal::PrintFloatvar(writer, 145, record.m_dDimTvp);
        internal::PrintFloatvar(writer, 146, record.m_dDimTFac);
        internal::PrintFloatvar(writer, 147, record.m_dDimGap);
        //DxfOutFloatVar(148, m_dDimAltRnd);
        internal::PrintIntvar(writer, 71, record.m_nDimTol);
        internal::PrintIntvar(writer, 72, record.m_nDimLim);
        internal::PrintIntvar(writer, 73, record.m_nDimTih);
        internal::PrintIntvar(writer, 74, record.m_nDimToh);
        internal::PrintIntvar(writer, 75, record.m_nDimSe1);
        internal::PrintIntvar(writer, 76, record.m_nDimSe2);
        internal::PrintIntvar(writer, 77, record.m_nDimTad);
        internal::PrintIntvar(writer, 78, record.m_nDimZin);
        //DxfOutIntVar(79, m_nDimAzin);
        internal::PrintIntvar(writer, 170, record.m_nDimAlt);
        internal::PrintIntvar(writer, 171, record.m_nDimAltd);
        internal::PrintIntvar(writer, 172, record.m_nDimTofl);
        internal::PrintIntvar(writer, 173, record.m_nDimSah);
        internal::PrintIntvar(writer, 174, record.m_nDimTix);
        internal::PrintIntvar(writer, 175, record.m_nDimSoxd);
        internal::PrintIntvar(writer, 176, record.m_nDimClrd);
        internal::PrintIntvar(writer, 177, record.m_nDimClre);
        internal::PrintIntvar(writer, 178, record.m_nDimClrt);
        //DxfOutIntVar(179, m_nDimADec);
        // Here must go group code 270
        internal::PrintIntvar(writer, 271, record.m_nDimDec);
        internal::PrintIntvar(writer, 272, record.m_nDimTDec);
        /*DxfOutIntVar(273, m_nDimAltU);
        DxfOutIntVar(274, m_nDimAltTD);
        DxfOutIntVar(275, m_nDimAUnit);
        //DxfOutIntVar(276, m_nDimFrac);
        //DxfOutIntVar(277, m_nDimLUnit);
        //DxfOutIntVar(278, m_nDimDSep);
        //DxfOutIntVar(279, m_nDimTMove);
        DxfOutIntVar(280, m_nDimJust);
        DxfOutIntVar(281, m_nDimSD1);
        DxfOutIntVar(282, m_nDimSD2);
        DxfOutIntVar(283, m_nDimTolJ);
        DxfOutIntVar(284, m_nDimTZin);
        DxfOutIntVar(285, m_nDimAltZ);
        DxfOutIntVar(286, m_nDimAltTZ);
        // here must go group code 287*/
        internal::PrintIntvar(writer, 287, record.m_nDimAtfFit);
        //DxfOutIntVar(288, m_nDimUPt);

        const CDXFObjectID &dimtxSty = record.GetDimTxSty();

        writer.WriteHex(dimtxSty.GetHandle(), 340);

        const CDXFObjectID &dimdrBlk = record.GetDimLdrBlk(pDB);
        if (!dimdrBlk.IsNull()) {
            writer.WriteHex(dimdrBlk.GetHandle(), 341);
        }

        const CDXFObjectID &dimBlk0 = record.GetDimBlk0(pDB);
        if (!dimBlk0.IsNull()) {
            writer.WriteHex(dimBlk0.GetHandle(), 342);
        }

        const CDXFObjectID &dimBlk1 = record.GetDimBlk1(pDB);
        if (!dimBlk1.IsNull()) {
            writer.WriteHex(dimBlk1.GetHandle(), 343);
        }

        const CDXFObjectID &dimBlk2 = record.GetDimBlk2(pDB);
        if (!dimBlk2.IsNull()) {
            writer.WriteHex(dimBlk2.GetHandle(), 344);
        }
    }

    void WriteToFileDimBlockRecord(CDXFWriter &writer, const CDXFBlockTableRecord &record)
    {
        dxf_table_record_file_util::WriteToFileSymbolTableRecord(writer, record);

        writer.Write("  100\nAcDbBlockTableRecord\n");
        writer.WriteText(record.GetName(), 2);
    }
};