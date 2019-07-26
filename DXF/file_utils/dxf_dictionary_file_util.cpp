#include "stdafx.h"
#include "dxf_dictionary_file_util.h"

#include "dxf_obj_file_util.h"
#include "dxf_mlinestyle_file_util.h"
#include "dxf_placeholder_file_util.h"

#include "DXFReader.h"
#include "DXFDatabase.h"
#include "DXFDictionary.h"

#include "DXFWriter.h"

namespace internal
{
    static void WriteToFile(CDXFWriter &writer, const CDXFDictionary &dict)
    {
        dxf_obj_file_util::WriteToFile(writer, dict);

        if (dict.m_ownerID.GetHandle() != 0)
        {
            writer.Write("  102\n{ACAD_REACTORS\n");
            writer.WriteHex(dict.m_ownerID.GetHandle(), 330);
            writer.Write("  102\n}\n");
        }

        writer.Write("  100\nAcDbDictionary\n");

        for (const auto &it : dict.m_map) 
        {
            ASSERT(it.second);
            writer.WriteText(it.first.c_str(), 3);
            writer.WriteHex(it.second->m_objectID.GetHandle(), 350);
        }
    }

    static void WriteToFileDictDefault(CDXFWriter &writer, const CDXFDictionaryWithDefault &dict)
    {
        WriteToFile(writer, dict);

        writer.Write("  100\nAcDbDictionaryWithDefault\n");
        writer.WriteHex(dict.GetDefaultID().GetHandle(), 340);
    }
};

class CDXFDictonaryWriterVisitor : public CDXFDictionaryVisitorAbs
{
// Construction/Destruction
public:
    CDXFDictonaryWriterVisitor(CDXFWriter &writer) : m_writer(writer) { }
    virtual ~CDXFDictonaryWriterVisitor() { }

// Overrides
public:
    virtual void VisitDictionary(CDXFDictionary &dict) override {
        internal::WriteToFile(m_writer, dict);
    }

    virtual void VisitDictionaryWithDefault(CDXFDictionaryWithDefault &dict) override {
        internal::WriteToFileDictDefault(m_writer, dict);
    }

// Attributes
private:
    CDXFWriter &m_writer;
};

class CDXFObjectWriterVisitor : public CDXFObjectVisitorAbs
{
public:
    CDXFObjectWriterVisitor(CDXFWriter &writer, const CDXFDatabase *pDB) : m_writer(writer), m_pDB(pDB) { }
    virtual ~CDXFObjectWriterVisitor() { }

public:
    virtual void VisitDictionary(CDXFDictionary &dict) override
    {
        dxf_dict_file_util::PreWriteToFile(m_writer, dict, m_pDB);
    }

    virtual void VisitDXFLineStyle(CDXFMlineStyle &line) override
    {
        dxf_mline_style_file_util::WriteToFile(m_writer, line);
    }

    virtual void VisitDXFPlaceHolder(CDXFPlaceHolder &placeholder) override
    {
        dxf_placeholder_file_util::WriteToFile(m_writer, placeholder);
    }

    virtual void VisitDXFEntity(CDXFEntity &entity) override
    {
        //should not go here
        ASSERT(FALSE);
    }

    virtual void VisitDXFSymbolTableRecord(CDXFSymbolTableRecord &record)
    {
        //should not go here
        ASSERT(FALSE);
    }

    virtual void VisitSymbolTable(CDXFSymbolTable &table)
    {
        //should not go here
        ASSERT(FALSE);
    }

// Attributes
private:
    CDXFWriter &m_writer;
    const CDXFDatabase *m_pDB;
};

namespace dxf_dict_file_util
{
    void PreReadFromFile(CDXFReader &reader, CDXFDictionary *pDict)
    {
        //unsigned long ulHandle;

        //ulHandle = pFile->ReadHandle();
        //pFile->GetDatabase()->AddObject(this, objectID, ulHandle);

        //dxf_dict_file_util::ReadFromFile(pFile, pDict);
    }

    void ReadFromFile(CDXFReader &reader, CDXFDictionary *pDict)
    {
        dxf_obj_file_util::ReadFromFile(reader, pDict);

        CDXFReaderItem item;
        reader.ReadItem(item);

        while (item.m_nGroupCode != 0)
        {
            switch (item.m_nGroupCode)
            {
            case 3:
                break;
            case 350:
                break;
            }

            reader.ReadItem(item);
        }
    }


    void PreWriteToFile(CDXFWriter &writer, CDXFDictionary &dict, const CDXFDatabase *pDB)
    {
        writer.WriteText(dict.GetObjectName(), 0);

        CDXFDictonaryWriterVisitor vis(writer);
        dict.Accept(vis);

        CDXFObjectWriterVisitor postvis(writer, pDB);

        for (const auto &it : dict.m_map) {
            ASSERT(it.second);
            it.second->Accept(postvis);
        }
    }
};