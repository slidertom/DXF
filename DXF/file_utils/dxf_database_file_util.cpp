#include "stdafx.h"
#include "dxf_database_file_util.h"

#include "dxf_symbol_table_file_util.h"
#include "dxf_dictionary_file_util.h"
#include "dxf_block_table_file_util.h"

#include "DXFReader.h"
#include "DXFDatabase.h"
#include "DXFDictionary.h"

#include "tables/DXFBlockTableRecord.h"    
#include "tables/DXFLayerTableRecord.h"
#include "tables/DXFViewportTable.h"

#include "locale.h"

#include "DXFWriter.h"

class CSetLocale
{
public:
    CSetLocale() {
        m_sOldLocale = setlocale(LC_ALL, nullptr);
        setlocale(LC_ALL, "C");
    }

    ~CSetLocale() {
        setlocale(LC_ALL, m_sOldLocale.c_str());
    }

private:
    std::string m_sOldLocale;
};
    
namespace dxf_database_file_util
{
    namespace internal
    {
        void PrintFloatVar(CDXFWriter &writer, const char *str, int32_t nNum, double dVal)
        {
            writer.WriteHeader(str, 9);
            writer.WriteFloatingPoint(dVal, nNum);
        }

        void PrintIntVar(CDXFWriter &writer, const char *str, int32_t nNum, int32_t nVal) 
        {
            writer.WriteHeader(str, 9);
            writer.WriteNumber(nVal, nNum);
        }

        void PrintStringVar(CDXFWriter &writer, const char *str, int32_t nNum, const char *sVal)
        {
            writer.WriteHeader(str, 9);
            writer.WriteText(sVal, nNum);
        }

        void Print3DPoint(CDXFWriter &writer, const char *str, int32_t nNum, const CDXF3DPoint &point)
        {
            writer.WriteHeader(str, 9);
            writer.WritePoint(point, nNum, nNum + 10, nNum + 20);
        }

        void Print2Dpoint(CDXFWriter &writer, const char *str, int32_t nNum, const CDXF3DPoint &point)
        {
            writer.WriteHeader(str, 9);
            writer.Write2DPoint(point, nNum, nNum + 10);
        }
    }

    void WriteHeaderToFile(CDXFWriter &writer, const CDXFDatabase *pDatabase)
    {
        internal::PrintStringVar(writer, "ACADVER",    1, "AC1015");
        internal::Print3DPoint(writer,   "INSBASE",   10, pDatabase->m_vars.m_pointInsBase);
        internal::Print3DPoint(writer,   "EXTMIN",    10, pDatabase->m_vars.m_pointExtMin);
        internal::Print3DPoint(writer,   "EXTMAX",    10, pDatabase->m_vars.m_pointExtMax);
        internal::Print2Dpoint(writer,   "LIMMIN",    10, pDatabase->m_vars.m_pointLimMin);
        internal::Print2Dpoint(writer,   "LIMMAX",    10, pDatabase->m_vars.m_pointLimMax);
        internal::PrintIntVar(writer,    "ORTHOMODE", 70, pDatabase->m_vars.m_nOrthoMode);
        internal::PrintIntVar(writer,    "REGENMODE", 70, pDatabase->m_vars.m_nRegenMode);
        internal::PrintIntVar(writer,    "FILLMODE",  70, pDatabase->m_vars.m_nFillMode);
        internal::PrintIntVar(writer,    "QTEXTMODE", 70, pDatabase->m_vars.m_nQTextMode);
        internal::PrintIntVar(writer,    "MIRRTEXT",  70, pDatabase->m_vars.m_nMirrText);
        internal::PrintIntVar(writer,    "DRAGMODE",  70, pDatabase->m_vars.m_nDragMode);
        internal::PrintFloatVar(writer,  "LTSCALE",   40, pDatabase->m_vars.m_dLtScale);
        internal::PrintIntVar(writer,    "OSMODE",    70, pDatabase->m_vars.m_nOSMode);
        internal::PrintIntVar(writer,    "ATTMODE",   70, pDatabase->m_vars.m_nAttMode);
        internal::PrintFloatVar(writer,  "TEXTSIZE",  40, pDatabase->m_vars.m_dTextSize);
        internal::PrintFloatVar(writer,  "TRACEWID",  40, pDatabase->m_vars.m_dTraceWid);
        writer.Write("  9\n$TEXTSTYLE\n  7\nSTANDARD\n");
        writer.Write("  9\n$CLAYER\n  8\n0\n");
        writer.Write("  9\n$CELTYPE\n  6\nBYLAYER\n");
        internal::PrintIntVar(writer,   "CECOLOR",   62, pDatabase->m_vars.m_dwCeColor);
        internal::PrintFloatVar(writer, "CELTSCALE", 40, pDatabase->m_vars.m_dCeLtScale);

        writer.Write("  9\n$HANDSEED\n  5\n%X\n", pDatabase->m_vars.m_nHandSeed);

        internal::PrintFloatVar(writer, "ANGBASE",  50, pDatabase->m_vars.m_dAngBase);
        internal::PrintIntVar(writer,   "ANGDIR",   70, pDatabase->m_vars.m_nAngDir);
        internal::PrintIntVar(writer,   "AUNITS",   70, pDatabase->m_vars.m_nAUnits);
        internal::PrintIntVar(writer,   "AUPREC",   70, pDatabase->m_vars.m_nAUPrec);
        internal::PrintIntVar(writer,   "INSUNITS", 70, pDatabase->m_vars.m_nInsUnits);
    }

    static DXF_ERROR ReadHeaderFromFile(CDXFReader &reader, CDXFDatabase *pDatabase)
    {
        CDXFReaderItem item;
        reader.ReadItem(item);

        while (::strcmp(item.m_resString, "ENDSEC") != 0)
        {
            if (item.m_nGroupCode == 9)
            {
                if (0 == ::strcmpi(item.m_resString, "$EXTMIN"))
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 10) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointExtMin.x = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 20) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }
                    pDatabase->m_vars.m_pointExtMin.y = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 30) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }
                    pDatabase->m_vars.m_pointExtMin.z = item.m_resDouble;
                }
                else if (::strcmpi(item.m_resString, "$EXTMAX") == 0)
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 10) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointExtMax.x = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 20) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }
                       
                    pDatabase->m_vars.m_pointExtMax.y = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 30) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointExtMax.z = item.m_resDouble;
                }
                else if (::strcmpi(item.m_resString, "$LIMMAX") == 0)
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 10) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointLimMax.x = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 20){
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointLimMax.y = item.m_resDouble;
                }
                else if (::strcmpi(item.m_resString, "$LIMMIN") == 0)
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 10) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointLimMin.x = item.m_resDouble;

                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 20) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_pointLimMin.y = item.m_resDouble;
                }
                else if (::strcmpi(item.m_resString, "$ACADVER") == 0)
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 1) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    bool bVersionCorrect(false);
                    if (0 == ::strcmpi(item.m_resString, "AC1015"))
                    {
                        bVersionCorrect = true;// 2000 Minimal supported DXF file version
                    }
                    else if (0 == ::strcmpi(item.m_resString, "AC1018"))
                    {
                        bVersionCorrect = true;// 2004
                    }
                    else if (0 == ::strcmpi(item.m_resString, "AC1021"))
                    {
                        bVersionCorrect = true;// 2007
                    }
                    else if (0 == ::strcmpi(item.m_resString, "AC1024"))
                    {
                        bVersionCorrect = true;// 2010
                    }

                    if (!bVersionCorrect) {
                        return DXF_ERROR_UNSUPPORTED_VERSION;
                    }
                }
                else if (::strcmpi(item.m_resString, "$HANDSEED") == 0)
                {
                    reader.ReadItem(item);
                    pDatabase->m_vars.m_nHandSeed = item.m_resInt;
                }
                else if (::strcmpi(item.m_resString, "$INSUNITS") == 0)
                {
                    reader.ReadItem(item);
                    if (item.m_nGroupCode != 70) {
                        return DXF_ERROR_BAD_GROUPCODE;
                    }

                    pDatabase->m_vars.m_nInsUnits = item.m_resInt;
                }
            }

            reader.ReadItem(item);
        }

        return DXF_OK;
    }

    DXF_ERROR ReadTablesFromFile(CDXFReader &reader, CDXFDatabase *pDatabase)
    {
        char sTableName[64];
        CDXFObjectID tableID;
        CDXFReaderItem item;

        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "VPORT") != 0) {
            return DXF_ERROR_VPORT_TABLE_EXPECTED;
        }

        CDXFSymbolTable *pTable = new CDXFViewportTable;
        DXF_ERROR eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);
        if (eError != DXF_OK) {
            return eError;
        }

        pDatabase->SetViewPortTable(pTable->GetObjectID());

        // Read LTYPE table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "LTYPE")) {
            return DXF_ERROR_LTYPE_TABLE_EXPECTED;
        }

        pTable = new CDXFLineTypeTable;
        eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);

        if (eError != DXF_OK) {
            return eError;
        }

        pDatabase->SetLineTypeTable(pTable->GetObjectID());

        // Read LAYER table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "LAYER")) {
            return DXF_ERROR_LAYER_TABLE_EXPECTED;
        }

        pTable = new CDXFLayerTable;
        eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);

        if (eError != DXF_OK) {
            return eError;
        }

        pDatabase->SetLayerTable(pTable->GetObjectID());

        // Read STYLE table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "STYLE")) {
            return DXF_ERROR_STYLE_TABLE_EXPECTED;
        }

        pTable = new CDXFTextStyleTable;
        eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);

        if (eError != DXF_OK) {
            return eError;
        }

        pDatabase->SetTextStyleTable(pTable->GetObjectID());

        // Read VIEW table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "VIEW")) {
            return DXF_ERROR_VIEW_TABLE_EXPECTED;
        }
            
        pTable = new CDXFViewTable;
        eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);

        if (eError != DXF_OK) {
            return eError;
        }

        pDatabase->SetViewTable(pTable->GetObjectID());

        // Read UCS table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "UCS")) {
            // DXF_ERROR_UCS_TABLE_EXPECTED; //? is it mandadory?
        }
        else {
            pTable = new CDXFUCSTable;
            eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);
            if (eError != DXF_OK) {
                return eError;
            }

            pDatabase->SetUCSTable(pTable->GetObjectID());
        }

        // Read APPID table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "APPID")) {
            // DXF_ERROR_APPID_TABLE_EXPECTED; //? is it mandadory?
        }
        else {
            pTable = new CDXFAppIDTable;
            eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);
            
            if (eError != DXF_OK) {
                return eError;
            }

            pDatabase->SetAppIDTable(pTable->GetObjectID());
        }

        // Read DIMSTYLE table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "DIMSTYLE")) { //? is it mandadory?
                                                //DXF_ERROR_DIMSTYLE_TABLE_EXPECTED;
        }
        else {
            pTable = new CDXFDimStyleTable;
            eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);
            
            if (eError != DXF_OK) {
                return eError;
            }

            pDatabase->SetDimStyleTable(pTable->GetObjectID());
        }

        // Read BLOCK_RECORD table
        reader.GetTableName(sTableName);
        if (::strcmp(sTableName, "BLOCK_RECORD")) {
            return DXF_ERROR_BLOCK_RECORD_TABLE_EXPECTED;
        }

        pTable = new CDXFBlockTable;
        eError = dxf_symbol_table_file_util::ReadFromFileTable(reader, pTable);
        if (eError != DXF_OK) {
            return eError;
        }

        pDatabase->SetBlockTable(pTable->GetObjectID());

        // Check if *MODEL_SPACE block record was loaded
        CDXFObjectID recordID;

        pDatabase->GetBlockTable()->GetTableRecordId(DXF_MODEL_SPACE, recordID);
        if (recordID.IsNull()) {
            return DXF_ERROR_MODELSPACE_BLOCK_MISSING;
        }

        // Read ENDSEC item
        reader.ReadItem(item);
        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        if (0 != ::strcmpi(item.m_resString, "ENDSEC")) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        return DXF_OK;
    }

    void ReadClassesFromFile(CDXFReader &reader, CDXFDatabase *pDatabase)
    {
        CDXFReaderItem item;
        reader.ReadItem(item);

        while (::strcmp(item.m_resString, "ENDSEC") != 0) {
            reader.ReadItem(item);
        }
    }

    DXF_ERROR OpenDatabase(const wchar_t *sFileName, CDXFDatabase *pDatabase)
    {
        CDXFReader reader(pDatabase);

        CSetLocale locale; // Otherwise sscanf may fail to parse floats

        if ( !reader.Open(sFileName) ) {
            return DXF_ERROR_CAN_NOT_OPEN_FILE;
        }

        DXF_ERROR eError = DXF_OK;
        CDXFReaderItem item;
        // Read HEADER section
        reader.ReadItem(item);
        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "SECTION") != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        reader.ReadItem(item);
        if (item.m_nGroupCode != 2) {
            return DXF_ERROR_BAD_GROUPCODE;
        }

        if (::strcmp(item.m_resString, "HEADER") != 0) {
            return DXF_ERROR_HEADER_SECTION_MISSING;
        }
                
        eError = dxf_database_file_util::ReadHeaderFromFile(reader, pDatabase);
        
        if (eError != DXF_OK) {
            return eError;
        }

        // Read CLASSES section
        reader.ReadItem(item);
        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "SECTION") != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        reader.ReadItem(item);
        if (item.m_nGroupCode != 2) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "CLASSES") == 0)
        {
            dxf_database_file_util::ReadClassesFromFile(reader, pDatabase);
            reader.ReadItem(item);
            if (item.m_nGroupCode != 0) {
                return DXF_ERROR_BAD_GROUPCODE;
            }
            if (::strcmp(item.m_resString, "SECTION") != 0) {
                return DXF_ERROR_BAD_GROUPDATA;
            }

            reader.ReadItem(item);
            if (item.m_nGroupCode != 2)
                return DXF_ERROR_BAD_GROUPCODE;
        }

        if (::strcmp(item.m_resString, "TABLES") != 0) {
            return DXF_ERROR_TABLES_SECTION_MISSING;
        }

        eError = dxf_database_file_util::ReadTablesFromFile(reader, pDatabase);
        if ( eError != DXF_OK ) {
            return eError;
        }

        // Read BLOCKS section
        reader.ReadItem(item);
        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "SECTION") != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        reader.ReadItem(item);
        if (item.m_nGroupCode != 2) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "BLOCKS") != 0) {
            return DXF_ERROR_BLOCKS_SECTION_MISSING;
        }

        eError = dxf_block_table_file_util::ReadBlocksFromFile(reader, pDatabase->GetBlockTable());

        // Read ENTITIES section
        reader.ReadItem(item);
        if (item.m_nGroupCode != 0) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "SECTION") != 0) {
            return DXF_ERROR_BAD_GROUPDATA;
        }

        reader.ReadItem(item);
        if (item.m_nGroupCode != 2) {
            return DXF_ERROR_BAD_GROUPCODE;
        }
        if (::strcmp(item.m_resString, "ENTITIES") != 0) {
            return DXF_ERROR_ENTITIES_SECTION_MISSING;
        }

        eError = dxf_block_table_file_util::ReadEntitiesFromFile(reader, pDatabase->GetBlockTable());
        
        if (eError != DXF_OK) {
            return eError;
        }

        return DXF_OK;
    }

    bool SaveDatabase(const wchar_t *strFileName, CDXFDatabase *pDatabase)
    {
        CDXFFileWriter writer(strFileName);
        if ( !writer.IsOpened() ) {
            return false;
        }

        const std::string oldlocale = setlocale(LC_ALL, NULL);
        setlocale(LC_ALL, "C");        // Otherwise floats are printed incorrectly

        writer.Write("  0\nSECTION\n");
        writer.Write("  2\nHEADER\n");
        dxf_database_file_util::WriteHeaderToFile(writer, pDatabase);
        writer.Write("  0\nENDSEC\n");
        writer.Write("  0\nSECTION\n");
        writer.Write("  2\nTABLES\n");
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetViewPortTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetLineTypeTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetLayerTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetTextStyleTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetViewTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetUCSTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetAppIDTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetDimStyleTable(), pDatabase);
        dxf_symbol_table_file_util::PreWriteFile(writer, *pDatabase->GetBlockTable(), pDatabase);

        writer.Write("  0\nENDSEC\n");
        writer.Write("  0\nSECTION\n");
        writer.Write("  2\nBLOCKS\n");
        dxf_block_table_file_util::WriteBlocksToFile(writer, *pDatabase->GetBlockTable(), pDatabase);
        writer.Write("  0\nENDSEC\n");
        writer.Write("  0\nSECTION\n");
        writer.Write("  2\nENTITIES\n");
        dxf_block_table_file_util::WriteEntitiesToFile(writer, *pDatabase->GetBlockTable(), pDatabase);
        writer.Write("  0\nENDSEC\n");
        writer.Write("  0\nSECTION\n");
        writer.Write("  2\nOBJECTS\n");
        dxf_dict_file_util::PreWriteToFile(writer, *pDatabase->GetNamedObjectsDictionary(), pDatabase);
        writer.Write("  0\nENDSEC\n");

        writer.Write("  0\nEOF\n");

        setlocale(LC_ALL, oldlocale.c_str());

        return true;
    }
};