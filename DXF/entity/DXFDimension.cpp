#include "stdafx.h"
#include "DXFDimension.h"

#include "DXFDatabase.h"
#include "DXFInsert.h"
#include "DXFLine.h"
#include "DXFText.h"
#include "DXFSolid.h"

#include "Utils/DXFTextUtils.h"

CDXFDimension::CDXFDimension(const CDXFDimension &dimension) 
: CDXFEntity(dimension, "DIMENSION")
{
    m_fFlags            = dimension.m_fFlags;
    m_defPoint          = dimension.m_defPoint;
    m_textMiddlePoint   = dimension.m_textMiddlePoint;
    m_eAttachmentPoint  = dimension.m_eAttachmentPoint;
    m_sDimensionText    = dimension.m_sDimensionText.c_str();
    m_bOverrideDimText  = dimension.m_bOverrideDimText;
    m_sOverridenDimText = dimension.m_sOverridenDimText.c_str();
    m_bDrawText         = dimension.m_bDrawText;
    m_dDimTextHeight    = dimension.m_dDimTextHeight;

    m_bExportTextAttachPoint = dimension.m_bExportTextAttachPoint;
    m_bEnableBlockRef        = dimension.m_bEnableBlockRef;
}

void CDXFDimension::SetDimStyle(const CDXFObjectID &dimStyleID)
{
    ASSERT(!dimStyleID.IsNull());
    m_dimStyleID = dimStyleID;
    delete m_pOverrideStyle;
    CDXFDimStyleTableRecord *pRecord = (CDXFDimStyleTableRecord *)m_dimStyleID.GetObject();
    m_pOverrideStyle = new CDXFDimStyleTableRecord(*pRecord);
}

void CDXFDimension::SetDimText(const char *sDimText)
{
    m_sDimensionText = sDimText;
    if (m_pDimText) {
        m_pDimText->SetText(sDimText);
    }
}

bool CDXFDimension::Init(CDXFDimension *pDimension, CDXFDatabase *pDB)
{
    ASSERT(pDB);

    if (pDimension->m_dimStyleID.IsNull()) {
        return false;
    }

    CDXFBlockTable *pBlockTable = pDB->GetBlockTable();
    pBlockTable->CreateUniqueTableRecord(pDimension->m_dimBlockID, pDB);
    return true;
}

void CDXFDimension::SetDimTextHeight(double dHeight)
{
    m_dDimTextHeight = dHeight;
    if (m_pDimText) {
        m_pDimText->SetTextHeight(dHeight);
    }
}

void CDXFDimension::SetDimTextEntity(CDXFText *pDimText)
{
    m_pDimText = pDimText;
    m_pDimText->SetInsertPoint(m_textMiddlePoint);
    m_pDimText->SetTextHeight(m_pOverrideStyle->m_dDimTxt * m_pOverrideStyle->m_dDimScale);
    m_pDimText->SetText(GetDimText());
}

void CDXFDimension::AddDefaultArrowBlock(CDXFDimension *pDim, CDXFBlockTableRecord *pDimBlock, CDXFDatabase *pDB, const CDXF3DPoint &dimPoint, double dAngle)
{
    CDXFSolid *pSolid;
    CDXFMatrix3D matrix;
    CDXF3DPoint points[3] =
    {
        CDXF3DPoint(0, 0, 0),
        CDXF3DPoint(1, 0.16667, 0),
        CDXF3DPoint(1, -0.16667, 0)
    };

    matrix.SetRotationAboutZAxis(dAngle);
    points[0] = MatrixPointMultiply(matrix, points[0]);
    points[1] = MatrixPointMultiply(matrix, points[1]);
    points[2] = MatrixPointMultiply(matrix, points[2]);

    points[0] *= pDim->GetDimScale() * pDim->GetDimASz();
    points[1] *= pDim->GetDimScale() * pDim->GetDimASz();
    points[2] *= pDim->GetDimScale() * pDim->GetDimASz();

    points[0] += dimPoint;
    points[1] += dimPoint;
    points[2] += dimPoint;

    pSolid = new CDXFSolid;
    pSolid->SetNumPoints(3);
    pSolid->SetPointAt(0, points[0]);
    pSolid->SetPointAt(1, points[1]);
    pSolid->SetPointAt(2, points[2]);

    pDimBlock->AddEntity(pSolid, pDB);
}

void CDXFDimension::AddArrowBlock(CDXFDimension *pDim, CDXFBlockTableRecord *pDimBlock, const CDXF3DPoint &dimPoint, const CDXFObjectID &arrowBlock, double dAngle, CDXFDatabase *pDB)
{
    CDXFInsert *pTick = CDXFInsert::CreateInsert(arrowBlock);
    pTick->SetInsertPoint(dimPoint);
    pTick->SetAngle(dAngle);
    pTick->SetScale(pDim->GetDimScale() * pDim->GetDimASz(),
                    pDim->GetDimScale() * pDim->GetDimASz(),
                    pDim->GetDimScale() * pDim->GetDimASz());
    pDimBlock->AddEntity(pTick, pDB);
}

void CDXFDimension::GetArrowBlocks(CDXFDimension *pDim, CDXFDatabase *pDB, CDXFObjectID &arrowBlock1, CDXFObjectID &arrowBlock2)
{
    if (pDim->m_pOverrideStyle->m_nDimSah == 0) {
        arrowBlock1 = pDim->m_pOverrideStyle->GetDimBlk0(pDB);
        arrowBlock2 = pDim->m_pOverrideStyle->GetDimBlk0(pDB);
    }
    else if (pDim->m_pOverrideStyle->m_nDimSah == 1) {
        arrowBlock1 = pDim->m_pOverrideStyle->GetDimBlk1(pDB);
        arrowBlock2 = pDim->m_pOverrideStyle->GetDimBlk2(pDB);
    }
}