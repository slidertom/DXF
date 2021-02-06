#ifndef _DXF_WRITER_H__
#define _DXF_WRITER_H__
#pragma once

#include "vector"
#include "string"

class CDXFWriter
{
// Construction/Destruction
public:
    CDXFWriter() { }
    virtual ~CDXFWriter() { }

// Overrides
public:
    virtual void Write(const char *str) = 0;

// Implementations
public:
    template <typename... T>
    void Write(const char *sFormat, const T & ... args)  {
        const int32_t nCnt = std::snprintf(nullptr, 0, sFormat, args...);
        std::vector<char> buffer(nCnt + 1);
        std::sprintf(&buffer[0], sFormat, args...);
        Write(std::string(buffer.begin(), buffer.end()).c_str());
    }

    template <class TDXF3DPoint>
    void WritePoint(const TDXF3DPoint &pt, int32_t nCodeX, int32_t nCodeY, int32_t nCodeZ)  {
        Write("  %d\n%f\n  %d\n%f\n  %d\n%f\n", nCodeX, pt.x, nCodeY, pt.y, nCodeZ, pt.z);
    }

    template <class TDXF2DPoint>
    void Write2DPoint(const TDXF2DPoint &pt, int32_t nCodeX, int32_t nCodeY) {
        Write("  %d\n%f\n  %d\n%f\n", nCodeX, pt.x, nCodeY, pt.y);
    }

    void WriteFloatingPoint(const double dValue, int32_t nCode) {
        Write("  %d\n%f\n", nCode, dValue);
    }

    void WriteText(const char *sText, int32_t nCode) {
        Write("  %d\n%s\n", nCode, sText);
    }

    void WriteHeader(const char *str, int32_t nCode) {
        Write("  %d\n$%s\n", nCode, str);
    }

    void WriteNumber(const int32_t nValue, int32_t nCode) {
        Write("  %d\n%d\n", nCode, nValue);
    }

    void WriteHex(const unsigned long nValue, int32_t nCode) {
        Write("  %d\n%X\n", nCode, nValue);
    }
};

class CDXFFileWriter : public CDXFWriter
{
// Construction/Destruction
public:
    CDXFFileWriter(const wchar_t *sPath);
    virtual ~CDXFFileWriter();

// Overrides
public:
    virtual void Write(const char *str) override;

// Operations
public:
    bool IsOpened() const { return m_pFile; }

// Attributes
private:
    FILE *m_pFile { nullptr };
};

#endif
