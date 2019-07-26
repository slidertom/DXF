#include "stdafx.h"

#ifdef _DXF_EXPORT_DLL
	
#else
	#ifdef _DEBUG
		#define _DXFDLL_LIB "MxDXF20D.lib"
	#else
		#define _DXFDLL_LIB "MxDXF20.lib"
	#endif

    #pragma message("MxDXF will automatically link with " _DXFDLL_LIB)
	#pragma comment(lib, _DXFDLL_LIB)
#endif
