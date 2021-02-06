#include "stdafx.h"
#include "DXFDictionary.h"

#include "DXFDatabase.h"

bool CDXFDictionary::SetAt(const char *sName, CDXFObject *pObject, CDXFDatabase *pDatabase)
{
    CDXFObjectID objectID;
    return SetAt(sName, pObject, objectID, pDatabase);
}

bool CDXFDictionary::SetAt(const char *sName, CDXFObject *pObject, CDXFObjectID &objectID, CDXFDatabase *pDatabase)
{
    pDatabase->AddObject(pObject, objectID);
    
    ((CDXFObject *)objectID.GetObject())->m_ownerID = m_objectID;
    ASSERT(m_map.find(sName) == m_map.end());
    m_map[sName] = pObject;

    return true;    
}