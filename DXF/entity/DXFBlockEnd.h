#ifndef __DXF_BLOCKEND_H__
#define __DXF_BLOCKEND_H__
#pragma once

#ifndef __DXF_ENTITY_H__
	#include "DXFEntity.h"
#endif

class CDXFBlockEnd : public CDXFEntity  
{
// Construction/Destruction
public:
    CDXFBlockEnd() : CDXFEntity("ENDBLK") { }
    virtual ~CDXFBlockEnd() { }

// Overrides
public:
    virtual void Accept(CDXFEntityVisitor &vis) override { }
};

#endif
