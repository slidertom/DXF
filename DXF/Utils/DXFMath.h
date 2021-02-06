#ifndef __DXF_MATH_H__
#define __DXF_MATH_H__
#pragma once

#ifndef _INC_MATH
    #include "math.h"
#endif

// global macros

#define DXF_PI      3.1415926535897932384626433832795
#define DXF_EPSILON 0.000001

namespace dxf_math
{
    const double sqeps  = 0.00000001;
    const double eps    = 0.0001;

    inline double fabsx(double x) {
        return (x >= 0) ? (x) : (-x);
    }

    inline bool eqx(double x, double y)  {
        return (x < y)?(y - x < sqeps):(x - y < sqeps);
    }

    inline bool eq0(double x) {
        return (x < sqeps) && (x > -sqeps);
    }

    inline bool less(double x, double y) {
        return (x < y && !eqx(x, y));
    }

    inline bool lessOrEq(double x, double y) {
        return (x < y || eqx(x, y));
    }

    inline bool more(double x, double y) {
        return (x > y && !eqx(x, y));
    }

    inline int32_t sign(double x) {    
        return x < -sqeps ? -1 : (x > sqeps ? 1 : 0);        
    }

    inline int iround(double x) {
        if (x >= -0.5) {
            return (int)(x + 0.5);
        }
        else {
            return (int)(x - 0.5);
        }
    }
};

#endif