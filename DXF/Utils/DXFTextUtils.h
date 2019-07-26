#ifndef __DXF_TEXT_UTILS_H__
#define __DXF_TEXT_UTILS_H__
#pragma once

#include "string"

namespace dxf_text_utils
{
    std::string FormatDouble(double dValue, size_t nDecCount);
    void FindAndReplaceEURO(std::string &sSource);
};

#endif