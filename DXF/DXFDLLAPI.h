#ifndef __DXF_DLLAPI_H__
#define __DXF_DLLAPI_H__
#pragma once

#ifdef _DXF_EXPORT_DLL
    #define DXFDLLAPI __declspec(dllexport)
#else
    #define DXFDLLAPI __declspec(dllimport)
#endif

#endif