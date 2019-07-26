#include "stdafx.h"
#include "DXFTextUtils.h"

#include "sstream"
#include "iomanip"

namespace dxf_text_utils
{
    std::string FormatDouble(double dValue, size_t nDecCount)
	{
		std::stringstream out;
        out << std::fixed;
        if ( nDecCount != 0 ) {
            out << std::showpoint;
        }
		out << std::setprecision(nDecCount) << dValue;
		return out.str();
	}

   	void FindAndReplaceEURO(std::string &sSource)
	{
		const std::string sFind = "€";
		const std::string sReplace("\\U+20AC");
		for ( std::string::size_type i = 0; (i = sSource.find(sFind, i)) != std::string::npos; ) {
			sSource.replace(i, sFind.length(), sReplace);
			i += sReplace.length();
		}
	}
}