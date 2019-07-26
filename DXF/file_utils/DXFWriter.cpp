#include "stdafx.h"
#include "DXFWriter.h"

CDXFFileWriter::CDXFFileWriter(const wchar_t *sPath)
{
    m_pFile = _wfopen(sPath, L"wt");
    ASSERT(m_pFile);
}

CDXFFileWriter::~CDXFFileWriter()
{
    fclose(m_pFile);
}

void CDXFFileWriter::Write(const char *str)
{
    ASSERT(m_pFile);
    fprintf(m_pFile, str);
}