#include "stdafx.h"
#include "dxf_dim_file_util.h"

#include "dxf_entity_file_util.h"
#include "dxf_table_record_file_util.h"

#include "DXFReader.h"
#include "entity/DXFDimension.h"
#include "DXFDatabase.h"
#include "Utils/DXFTextUtils.h"

#include "DXFWriter.h"

namespace dxf_dim_file_util
{
    namespace internal
    {
        void PrintHandleVar(CDXFWriter &writer, const CDXFObjectID &overrideStyleId, const CDXFObjectID &dimstyleId, int32_t nCode, bool &bStarted)
        {
            if ( overrideStyleId != dimstyleId)
            {																	
                if (!bStarted)													
                {		
                    writer.Write("  1001\nACAD\n  1000\nDSTYLE\n  1002\n{\n");
                    bStarted = true;											
                }				

                writer.WriteNumber(nCode, 1070);
                writer.WriteNumber(overrideStyleId.GetHandle(), 1005);
            }
        }

        void PrintStringVar(CDXFWriter &writer, const std::string &sOverridden, const std::string &sDimStyle, int32_t nCode, bool &bStarted)
        {
            if  (sOverridden != sDimStyle)
            {
                if (!bStarted)
                {
                    writer.Write("  1001\nACAD\n  1000\nDSTYLE\n  1002\n{\n");
                    bStarted = true;
                }

                writer.WriteNumber(nCode, 1070);
                writer.WriteText(sOverridden.c_str(), 1000);
            }
        }

        void PrintIntVar(CDXFWriter &writer, const int32_t &nOverridden, const int32_t &nDimStyle, int32_t nCode, bool &bStarted)
        {
            if (nOverridden != nDimStyle)
            {
                if ( !bStarted )
                {
                    writer.Write("  1001\nACAD\n  1000\nDSTYLE\n  1002\n{\n");
                    bStarted = true;
                }

                writer.WriteNumber(nCode, 1070);
                writer.WriteNumber(nOverridden, 1070);
            }
        }

        void PrintFloatVar(CDXFWriter &writer, const double &dOverridden, const double &dDimStyle, int32_t nCode, bool &bStarted)
        {
            if (dOverridden != dDimStyle)						
            {																	
                if ( !bStarted ) {																
                    writer.Write("  1001\nACAD\n  1000\nDSTYLE\n  1002\n{\n");
                    bStarted = true;											
                }

                writer.WriteNumber(nCode, 1070);
                writer.WriteFloatingPoint(dOverridden, 1040);
            }
        }
    }

    void PostReadFromFile(CDXFReader &reader)
    {
        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0) {
            reader.ReadItem(item);
        }

        reader.PushBackItem();
    }

    DXF_ERROR ReadFromFile(CDXFReader &reader, CDXFDimension *pDim)
    {
        ASSERT(reader.GetDatabase());

        const DXF_ERROR eError = dxf_entity_file_util::ReadFromFile(reader, pDim);
        if (eError != DXF_OK) {
            return eError;
        }

        CDXFReaderItem item;
        reader.ReadItem(item);

        std::string sBlock;
        std::string sDimStyle;
        bool bContinue = true;
        CDXF3DPoint midpoint;
        
        while (bContinue)
        {
            switch (item.m_nGroupCode)
            {
            case 1:
                pDim->m_sOverridenDimText = item.m_resString;
                break;
            case 2:
                sBlock = item.m_resString;
                break;
            case 3:
                sDimStyle = item.m_resString;
                bContinue = false;
                break;
            case 10:
                pDim->m_defPoint.x = item.m_resDouble;
                break;
            case 20:
                pDim->m_defPoint.y = item.m_resDouble;
                break;
            case 30:
                pDim->m_defPoint.z = item.m_resDouble;
                break;
            case 11:
                midpoint.x = item.m_resDouble;
                break;
            case 21:
                midpoint.y = item.m_resDouble;
                break;
            case 31:
                midpoint.z = item.m_resDouble;
                break;
            case 70:
                pDim->SetFlags(item.m_resInt);
                break;
            }

            reader.ReadItem(item);
        }

        pDim->SetTextMiddlePoint(midpoint);

        if (!sBlock.empty()) {
            reader.GetDatabase()->GetBlockTable()->GetTableRecordId(sBlock.c_str(), pDim->m_dimBlockID);
        }

        if (!sDimStyle.empty()) {
            reader.GetDatabase()->GetDimStyleTable()->GetTableRecordId(sDimStyle.c_str(), pDim->m_dimStyleID);
        }

        reader.PushBackItem();

        return DXF_OK;
    }

    void WriteTofile(CDXFWriter &writer, const CDXFDimension &dim)
    {
        dxf_entity_file_util::WriteToFile(writer, dim);

        writer.Write("  100\nAcDbDimension\n");

        if (dim.m_bEnableBlockRef) {
            ASSERT(!dim.m_dimBlockID.IsNull());
            CDXFBlockTableRecord *pRecord = (CDXFBlockTableRecord *)dim.m_dimBlockID.GetObject();
            writer.WriteText(pRecord->GetName(), 2);
        }

        const CDXF3DPoint &midpoint = dim.GetTextMiddlePoint();
        writer.WritePoint(dim.m_defPoint, 10, 20, 30);
        writer.WritePoint(midpoint, 11, 21, 31);
        writer.WriteNumber(dim.GetFlags(), 70);

        if (dim.m_bExportTextAttachPoint) {
            writer.WriteNumber((int32_t)dim.m_eAttachmentPoint, 71);
        }

        if ( dim.GetOverrideDimText() ) {
            writer.WriteText(dim.m_sDimensionText.c_str(), 1);
        }
        else if (!dim.m_sOverridenDimText.empty()) {
            writer.WriteText(dim.m_sOverridenDimText.c_str(), 1);
        }

        if (!dim.m_dimStyleID.IsNull()) {
            CDXFDimStyleTableRecord *pRecord = (CDXFDimStyleTableRecord *)dim.m_dimStyleID.GetObject();
            writer.WriteText(pRecord->GetName(), 3);
        }
        else {
            writer.Write("  3\nSTANDARD\n");
        }
    }

    void PostWriteToFile(CDXFWriter &writer, const CDXFDimension &dim, CDXFDatabase *pDB)
    {
        if (!dim.m_pOverrideStyle) {
            return;
        }

        bool bStarted = false;
        CDXFDimStyleTableRecord *pDimStyle = (CDXFDimStyleTableRecord *)dim.m_dimStyleID.GetObject();

        internal::PrintFloatVar(writer, dim.m_pOverrideStyle->m_dDimScale, pDimStyle->m_dDimScale, 40, bStarted);
        internal::PrintFloatVar(writer, dim.m_pOverrideStyle->m_dDimASz, pDimStyle->m_dDimASz, 41, bStarted);
        /*DXF_OUT_FLOAT_VAR(42, m_dDimExo);
        DXF_OUT_FLOAT_VAR(43, m_dDimDli);
        DXF_OUT_FLOAT_VAR(44, m_dDimExe);
        DXF_OUT_FLOAT_VAR(45, m_dDimRnd);
        DXF_OUT_FLOAT_VAR(46, m_dDimDle);
        DXF_OUT_FLOAT_VAR(47, m_dDimTp);
        DXF_OUT_FLOAT_VAR(48, m_dDimTm);
        DXF_OUT_FLOAT_VAR(140, m_dDimTxt);
        DXF_OUT_FLOAT_VAR(141, m_dDimCen);
        DXF_OUT_FLOAT_VAR(142, m_dDimTsz);
        DXF_OUT_FLOAT_VAR(143, m_dDimAltf);
        DXF_OUT_FLOAT_VAR(144, m_dDimLFac);
        DXF_OUT_FLOAT_VAR(145, m_dDimTvp);
        DXF_OUT_FLOAT_VAR(146, m_dDimTFac);
        DXF_OUT_FLOAT_VAR(147, m_dDimGap);
        DXF_OUT_FLOAT_VAR(148, m_dDimAltRnd);*/

        /*DXF_OUT_INT_VAR(71, m_nDimTol);
        DXF_OUT_INT_VAR(72, m_nDimLim);
        DXF_OUT_INT_VAR(73, m_nDimTih);
        DXF_OUT_INT_VAR(74, m_nDimToh);
        DXF_OUT_INT_VAR(75, m_nDimSe1);
        DXF_OUT_INT_VAR(76, m_nDimSe2);
        DXF_OUT_INT_VAR(77, m_nDimTad);
        DXF_OUT_INT_VAR(78, m_nDimZin);
        DXF_OUT_INT_VAR(79, m_nDimAzin);
        DXF_OUT_INT_VAR(170, m_nDimAlt);
        DXF_OUT_INT_VAR(171, m_nDimAltd);
        DXF_OUT_INT_VAR(172, m_nDimTofl);*/

        internal::PrintIntVar(writer, dim.m_pOverrideStyle->m_nDimSah, pDimStyle->m_nDimSah, 173, bStarted);

        /*DXF_OUT_INT_VAR(174, m_nDimTix);
        DXF_OUT_INT_VAR(175, m_nDimSoxd);
        DXF_OUT_INT_VAR(176, m_nDimClrd);
        DXF_OUT_INT_VAR(177, m_nDimClre);
        DXF_OUT_INT_VAR(178, m_nDimClrt);
        DXF_OUT_INT_VAR(179, m_nDimADec);*/

        /*DXF_OUT_INT_VAR(271, m_nDimDec);
        DXF_OUT_INT_VAR(272, m_nDimTDec);
        DXF_OUT_INT_VAR(273, m_nDimAltU);
        DXF_OUT_INT_VAR(274, m_nDimAltTD);
        DXF_OUT_INT_VAR(275, m_nDimAUnit);
        DXF_OUT_INT_VAR(276, m_nDimFrac);
        DXF_OUT_INT_VAR(277, m_nDimLUnit);
        DXF_OUT_INT_VAR(278, m_nDimDSep);
        DXF_OUT_INT_VAR(279, m_nDimTMove);
        DXF_OUT_INT_VAR(280, m_nDimJust);
        DXF_OUT_INT_VAR(281, m_nDimSD1);
        DXF_OUT_INT_VAR(282, m_nDimSD2);
        DXF_OUT_INT_VAR(283, m_nDimTolJ);
        DXF_OUT_INT_VAR(284, m_nDimTZin);
        DXF_OUT_INT_VAR(285, m_nDimAltZ);
        DXF_OUT_INT_VAR(286, m_nDimAltTZ);

        DXF_OUT_INT_VAR(287, m_nDimAtfFit);
        DXF_OUT_INT_VAR(288, m_nDimUPt);*/

        //DXF_OUT_HANDLE_VAR(340, GetDimTxSty());

        internal::PrintHandleVar(writer, dim.m_pOverrideStyle->GetDimBlk0(pDB), pDimStyle->GetDimBlk0(pDB), 342, bStarted);
        internal::PrintHandleVar(writer, dim.m_pOverrideStyle->GetDimBlk1(pDB), pDimStyle->GetDimBlk1(pDB), 343, bStarted);
        internal::PrintHandleVar(writer, dim.m_pOverrideStyle->GetDimBlk2(pDB), pDimStyle->GetDimBlk2(pDB), 344, bStarted);

        if (bStarted) {
            writer.Write("  1002\n}\n");
        }
    }
};