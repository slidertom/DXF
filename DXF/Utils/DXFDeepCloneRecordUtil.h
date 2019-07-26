#ifndef __DXF_DEEP_CLONE_RECORD_UTIL_H__
#define __DXF_DEEP_CLONE_RECORD_UTIL_H__
#pragma once

class CDXFObject;
class CDXFLayerTableRecord;
class CDXFBlockTableRecord;
class CDXFDatabase;

#ifndef __DXF_DLLAPI_H__
    #include "DXFDLLAPI.h"
#endif

namespace deep_clone_util
{
    DXFDLLAPI CDXFObject *DeepCloneLayerTableRecord(CDXFLayerTableRecord *pRecord, CDXFDatabase *pDB);
    DXFDLLAPI CDXFObject *DeepCloneBlockTableRecord(CDXFBlockTableRecord *pRecord, CDXFDatabase *pSrcDB, CDXFDatabase *pDestDB);
}

#endif