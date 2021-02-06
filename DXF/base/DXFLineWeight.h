#ifndef __DXF_LINE_WEIGHT_H__
#define __DXF_LINE_WEIGHT_H__
#pragma once

// AcDb::LineWeight enumeration (from http://help.autodesk.com/view/OARXMAC/2017/ENU/?guid=OREFMAC-AcDb__LineWeight):
// This enumerated type provides the line weight (thickness) values used to specify how lines will be displayed and plotted.
// The lineweights are in 100ths of a millimeter, except for the negative values.
// The negative values denote the default indicated by their constant's name. 

enum class EDXFLineWeight : short int
{
    LW_000 = 0,
    LW_005 = 5,
    LW_009 = 9,
    LW_013 = 13,
    LW_015 = 15,
    LW_018 = 18,
    LW_020 = 20,
    LW_025 = 25,
    LW_030 = 30,
    LW_035 = 35,
    LW_040 = 40,
    LW_050 = 50,
    LW_053 = 53,
    LW_060 = 60,
    LW_070 = 70,
    LW_080 = 80,
    LW_090 = 90,
    LW_100 = 100,
    LW_106 = 106,
    LW_120 = 120,
    LW_140 = 140,
    LW_158 = 158,
    LW_200 = 200,
    LW_211 = 211,
    LW_ByLayer = -1,
    LW_ByBlock = -2,
    LW_Default = -3,
};

namespace dxf_line_width_utils
{
    namespace internal
    {
        inline float GetWidthMM(EDXFLineWeight eLineWeight)
        {
            switch (eLineWeight)
            {
            case EDXFLineWeight::LW_000:
                return 0.0f;
                break;
            case EDXFLineWeight::LW_005:
                return 5 * 0.01f;
                break;
            case EDXFLineWeight::LW_009:
                return 9 * 0.01f;
                break;
            case EDXFLineWeight::LW_013:
                return 13 * 0.01f;
                break;
            case EDXFLineWeight::LW_015:
                return 15 * 0.01f;
                break;
            case EDXFLineWeight::LW_018:
                return 18 * 0.01f;
                break;
            case EDXFLineWeight::LW_020:
                return 20 * 0.01f;
                break;
            case EDXFLineWeight::LW_025:
                return 25 * 0.01f;
                break;
            case EDXFLineWeight::LW_030:
                return 30 * 0.01f;
                break;
            case EDXFLineWeight::LW_035:
                return 35 * 0.01f;
                break;
            case EDXFLineWeight::LW_040:
                return 40 * 0.01f;
                break;
            case EDXFLineWeight::LW_050:
                return 50 * 0.01f;
                break;
            case EDXFLineWeight::LW_053:
                return 53 * 0.01f;
                break;
            case EDXFLineWeight::LW_060:
                return 60 * 0.01f;
                break;
            case EDXFLineWeight::LW_070:
                return 70 * 0.01f;
                break;
            case EDXFLineWeight::LW_080:
                return 80 * 0.01f;
                break;
            case EDXFLineWeight::LW_090:
                return 90 * 0.01f;
                break;
            case EDXFLineWeight::LW_100:
                return 100 * 0.01f;
                break;
            case EDXFLineWeight::LW_106:
                return 106 * 0.01f;
                break;
            case EDXFLineWeight::LW_120:
                return 120 * 0.01f;
                break;
            case EDXFLineWeight::LW_140:
                return 140 * 0.01f;
                break;
            case EDXFLineWeight::LW_158:
                return 158 * 0.01f;
                break;
            case EDXFLineWeight::LW_200:
                return 200 * 0.01f;
                break;
            case EDXFLineWeight::LW_211:
                return 211 * 0.01f;
                break;
            case EDXFLineWeight::LW_ByLayer:
            case EDXFLineWeight::LW_ByBlock:
            case EDXFLineWeight::LW_Default:
                return 100 * 0.01f;
                break;
            default:
                ASSERT(false);
                break;
            }
  
            return 100 * 0.01f;
        }
    }

    template<class TEntity>
    inline float GetPenWidthMM(const TEntity &entity, EDXFLineWeight eEntytiLineWeight)
    {
        if (EDXFLineWeight::LW_Default == eEntytiLineWeight) {
            return internal::GetWidthMM(EDXFLineWeight::LW_100);
        }
        else if (EDXFLineWeight::LW_ByBlock == eEntytiLineWeight) {
            const CDXFObjectID &id = entity.GetOwnerID();
            CDXFBlockTableRecord *pBlock = (CDXFBlockTableRecord *)id.GetObject();
            if (pBlock) {
                const EDXFLineWeight eLineWeightBlock = pBlock->GetLineWeight();
                return internal::GetWidthMM(eLineWeightBlock);
            }

            return internal::GetWidthMM(EDXFLineWeight::LW_100);
        }
        else if (EDXFLineWeight::LW_ByLayer == eEntytiLineWeight) {
            const CDXFObjectID &idLayer = entity.GetLayerID();
            if (!idLayer.IsNull()) {
                CDXFLayerTableRecord *pLayer = (CDXFLayerTableRecord *)idLayer.GetObject();
                if (pLayer) {
                    const EDXFLineWeight eLineWeightLayer = pLayer->GetLineWeight();
                    return internal::GetWidthMM(eLineWeightLayer);
                }
            }
            return internal::GetWidthMM(EDXFLineWeight::LW_100);
        }

        return internal::GetWidthMM(eEntytiLineWeight);
    }
}

#endif