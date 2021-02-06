#include "stdafx.h"
#include "DXFBlockTableRecord.h"

#include "DXFDatabase.h"

#include "entity/DXFBlockBegin.h"
#include "entity/DXFBlockEnd.h"

static void InitEntity(CDXFEntity *pEntity, CDXFDatabase *pDB)
{
    // Create new entity in the layer of the block
    if (!pEntity->GetLayerID().IsNull()) {
        return;
    }

    CDXFObjectID layerID;
    pDB->GetLayerTable()->GetTableRecordId("0", layerID);
    pEntity->SetLayerID(layerID);
}

bool CDXFBlockTableRecord::AddEntity(CDXFEntity *pEntity, CDXFObjectID &objectID, CDXFDatabase *pDB)
{
    ::InitEntity(pEntity, pDB);

    pDB->AddObject(pEntity, objectID);
    m_entities.push_back(objectID);
    pEntity->m_ownerID = m_objectID;

    return true;
}

bool CDXFBlockTableRecord::AddEntity(CDXFEntity *pEntity, CDXFDatabase *pDB)
{
    CDXFObjectID objectID;
    return AddEntity(pEntity, objectID, pDB);
}

void CDXFBlockTableRecord::SetOrigin(const CDXF3DPoint &origin)
{
    m_pointOrigin = origin;
    if ( !m_blockBeginID.IsNull() ) {
        ((CDXFBlockBegin *)m_blockBeginID.GetObject())->SetBasePoint(m_pointOrigin);
    }
}

bool CDXFBlockTableRecord::SetName(const char *sName)
{
    if (::strcmpi(GetName(), DXF_MODEL_SPACE) == 0) {
        if (::strcmpi(GetName(), sName) != 0) {
            return false;
        }
    }

    CDXFSymbolTableRecord::SetName(sName);
    
    if (!m_blockBeginID.IsNull()) {
        ((CDXFBlockBegin *)m_blockBeginID.GetObject())->SetBlockName(GetName());
    }

    return true;
}

void CDXFBlockTableRecord::CreateBeginEndEntities(CDXFDatabase *pDB)
{
    ASSERT(pDB);

    if (m_blockBeginID.IsNull()) {
        CDXFBlockBegin *pBlockBegin = new CDXFBlockBegin(GetName(), m_pointOrigin);
        pDB->AddObject(pBlockBegin, m_blockBeginID);
        ::InitEntity(pBlockBegin, pDB);
    }

    if (m_blockEndID.IsNull()) {
        CDXFBlockEnd *pBlockEnd = new CDXFBlockEnd;
        pDB->AddObject(pBlockEnd, m_blockEndID);
        ::InitEntity(pBlockEnd, pDB);
    }

    if (::strcmpi(GetName(), DXF_PAPER_SPACE) == 0) {
        ((CDXFBlockBegin *)m_blockBeginID.GetObject())->SetInModelSpace(false);
        ((CDXFBlockEnd *)m_blockEndID.GetObject())->SetInModelSpace(false);
    }
}

bool CDXFBlockTableRecord::GetInsertEntityID(CDXFObjectID &objectID)
{
    objectID = m_insertEntityID;
    if (!m_insertEntityID.IsNull()) {
        return true;
    }
    else {
        return false;
    }
}

bool CDXFBlockTableRecord::DeleteEntity(CDXFBlockTableRecord *pRecord, const CDXFObjectID &objectID, CDXFDatabase *pDB)
{
    ASSERT(pDB);

    auto it_end = pRecord->m_entities.end();
    for (auto it = pRecord->m_entities.begin(); it != it_end; ++it) {
        const CDXFObjectID &id = *it;
        if (id == objectID) {
            if( pDB->DeleteObject(id) ) {
                pRecord->m_entities.erase(it);
                return true;
            }
            return false;
        }
    }
    
    return false;
}

const CDXFBlockEnd *CDXFBlockTableRecord::GetBlockEnd() const
{
    return (CDXFBlockEnd *)m_blockEndID.GetObject();
}

const CDXFBlockBegin *CDXFBlockTableRecord::GetBlockBegin() const
{
    return (CDXFBlockBegin *)m_blockBeginID.GetObject();
}

void CDXFBlockTableRecord::SetBlockBeginID(CDXFBlockTableRecord *pRecord, const CDXFObjectID &id, CDXFDatabase *pDB)
{
    ASSERT(!id.IsNull());

    if (!pRecord->m_blockBeginID.IsNull()) {
        pDB->DeleteObject(pRecord->m_blockBeginID);
    }

    pRecord->m_blockBeginID = id;
    CDXFObject *pObject = pRecord->m_blockBeginID.GetObject();
    if (pObject->GetOwnerID().IsNull()) {
        pObject->SetOwnerID(pRecord->m_objectID);
    }
}

void CDXFBlockTableRecord::SetBlockEndID(CDXFBlockTableRecord *pRecord, const CDXFObjectID &id, CDXFDatabase *pDB)
{
    ASSERT(!id.IsNull());

    if (!pRecord->m_blockEndID.IsNull()) {
        pDB->DeleteObject(pRecord->m_blockEndID);
    }

    pRecord->m_blockEndID = id;
    CDXFObject *pObject = pRecord->m_blockEndID.GetObject();
    if (pObject->GetOwnerID().IsNull()) {
        pObject->SetOwnerID(pRecord->m_objectID);
    }
}

void CDXFBlockTableRecord::VisitEntities(CDXFEntityVisitor &vis)
{
    CDXFEntity *pEntity = nullptr;
    for (CDXFObjectID &object_id : m_entities)
    {
        pEntity = (CDXFEntity *)object_id.GetObject();
        ASSERT(pEntity);
        pEntity->Accept(vis);
    }
}