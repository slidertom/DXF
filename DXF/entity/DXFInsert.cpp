#include "stdafx.h"
#include "DXFInsert.h"

void CDXFInsert::SetBlockTableRecordID(const CDXFObjectID &recordID)
{
    m_blockID = recordID;
    if (!m_objectID.IsNull() && !m_blockID.IsNull()) {
        ((CDXFBlockTableRecord *)m_blockID.GetObject())->m_insertEntityID = m_objectID;
    }
}

CDXFMatrix3D CDXFInsert::GetOCSToWCSMatrix() const
{
    CDXFMatrix3D result;
    CDXFMatrix3D tempMatrix;

    result.SetIdentity();

    // Translate to insert point
    tempMatrix.SetIdentity();
    tempMatrix.m14 = m_pointIns.x;
    tempMatrix.m24 = m_pointIns.y;
    tempMatrix.m34 = m_pointIns.z;
    result = MatrixMultiply(result, tempMatrix);

    // Apply rotation
    tempMatrix.SetRotationAboutZAxis(m_dAngle);
    result = MatrixMultiply(result, tempMatrix);

    // Apply all scalings
    tempMatrix.SetIdentity();
    tempMatrix.m11 = m_scale.x;
    tempMatrix.m22 = m_scale.y;
    tempMatrix.m33 = m_scale.z;
    result = MatrixMultiply(result, tempMatrix);

    return result;
}

CDXFInsert *CDXFInsert::CreateInsert(const CDXFObjectID &recordID)
{
    CDXFInsert *pInsert = new CDXFInsert;
    pInsert->m_blockID = recordID;
    if (!pInsert->m_objectID.IsNull() && !pInsert->m_blockID.IsNull()) {
        ((CDXFBlockTableRecord *)pInsert->m_blockID.GetObject())->m_insertEntityID = pInsert->m_objectID;
    }

    return pInsert;
}

CDXFInsert *CDXFInsert::CreateInsert(const CDXFInsert &src, const CDXFObjectID &recordID)
{
    CDXFInsert *pInsert = new CDXFInsert(src);
    pInsert->m_blockID = recordID;
    if (!pInsert->m_objectID.IsNull() && !pInsert->m_blockID.IsNull()) {
        ((CDXFBlockTableRecord *)pInsert->m_blockID.GetObject())->m_insertEntityID = pInsert->m_objectID;
    }

    return pInsert;
}