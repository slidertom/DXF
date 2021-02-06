#include "stdafx.h"
#include "DXFHatchPoly.h"

#include "../entity/DXFHatch.h"
#include "../base/DXFMatrix3D.h"
#include "DXFUtils.h"
#include "DXFMath.h"

#include "algorithm"
#include "execution"
#include "assert.h"

// windowdef.h defines these ones
#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif

namespace dxf_hatch_poly
{
    class CGetBoundryPoints : public CDXFHatchBoundaryVis,
                              public CDXFBoundaryEdgeVis
    {
        CHatchPoly::value_type Get3dPoint(const CDXFGePoint2D &pt) 
        {
            CHatchPoly::value_type point_3d;
            point_3d.x = pt.m_x;
            point_3d.y = pt.m_y;
            return point_3d;
        }
        
    // Construction
    public:
        CGetBoundryPoints(CHatchPoly *pPoly) { m_pPoly = pPoly; }

    // Operators
    public:
        void operator()(CDXFHatchBoundary *pBoundary) { pBoundary->Accept(*this); }
        void operator()(CDXFBoundaryEdgeBase *pBase) { pBase->Accept(*this); }
        void operator()(CDXF2DLineSegment &line) {
            CHatchPoly::value_type pointStart;
            CHatchPoly::value_type pointEnd;

            pointStart.x = line.m_x1;
            pointStart.y = line.m_y1;
            pointEnd.x   = line.m_x2;
            pointEnd.y   = line.m_y2;

            m_pPoly->push_back(pointStart);
            m_pPoly->push_back(pointEnd);
        }

    // Overrides CDXFHatchBoundaryVis
    public:
        virtual void OnVisitPolyLine(CDXFHatchPolylineBoundary *pBoundry) override {
            ASSERT(pBoundry);    
            std::vector<CDXFGeBulgePoint2D> vertices;
            pBoundry->GetVertices(vertices);
            typedef std::vector<CDXFGeBulgePoint2D>::const_iterator CIt;
            CIt end_it = vertices.end();
            CDXFGePoint2D point_2d;
            for (CIt it = vertices.begin(); it != end_it; ++it) {
                point_2d   = (*it).m_point;
                m_pPoly->push_back(Get3dPoint(point_2d));
            }
        }

        virtual void OnVisitNonPolyLine(CDXFHatchNonPolylineBoundary *pBoundry) override {
            ASSERT(pBoundry);
            CDXFNonPolylineBoundary *pBand = pBoundry->GetBand();
            std::for_each(pBand->begin(), pBand->end(), *this);
        }

    // Overrides CDXFBoundaryEdgeVis
    public:
        virtual void OnVisitEdgeArc(CDXFBoundaryEdgeArc &edge) override {
            CDXF2DLineVector lines;
            CArcGenerator::ConvertArcToLines(lines, edge.m_centerPoint.m_x, edge.m_centerPoint.m_y, edge.m_dStartAngle, edge.m_dEndAngle, edge.m_dRadius);   
            std::for_each(lines.begin(), lines.end(), *this);
        }

        virtual void OnVisitEdgeLine(CDXFBoundaryEdgeLine &edge) override {
            m_pPoly->push_back(Get3dPoint(edge.m_startPoint));
            m_pPoly->push_back(Get3dPoint(edge.m_endPoint));
        }

        virtual void OnVisitEdgeEllipse(CDXFBoundaryEdgeEllipse &edge) override {
            ASSERT(false); // ellipse is not supported
        }

        virtual void OnVisitEdgeSpline(CDXFBoundaryEdgeSpline &edge) override {
            for (const auto& controlPoint : edge.m_controlPointsArray) {
                m_pPoly->push_back(Get3dPoint(controlPoint.m_point));
            }
        }

    //  Attributes
    private:
        CHatchPoly *m_pPoly;
    };

    namespace geom
    {
        template <class TPoint>
        inline void CalcBoundRectangle(const TPoint points[], size_t nCount, TPoint &ptMin, TPoint &ptMax)
        {
            if (nCount < 1)  {
                return;
            }
            
            double dXMin = points[0].x, dXMax = points[0].x; 
            double dYMin = points[0].y, dYMax = points[0].y;

            for (size_t i = 1; i < nCount; ++i)
            {
                dXMin = std::min(points[i].x, dXMin);
                dXMax = std::max(points[i].x, dXMax);
                dYMin = std::min(points[i].y, dYMin);
                dYMax = std::max(points[i].y, dYMax);
            }

            ptMin.x = dXMin;
            ptMin.y = dYMin;
            ptMax.x = dXMax; 
            ptMax.y = dYMax;

        }

        // calculates halfplane of vector v with respect to v1 (when they start in the same point)
        template <typename T>
        inline int32_t Halfplane(const T &cx1, const T &cy1, const T &cx2, const T &cy2) {
            const double diff = (cx1 * cy2 - cx2 * cy1);
            if (diff < 0) {
                return (diff < -dxf_math::sqeps)?(-1):(0);
            }
            else  {
                return (diff > dxf_math::sqeps)?(1):(0);
            }
        }

        // calculates halfplane of point pt with respect to line (pt1, pt2)
        template <class T2DPoint>
        inline int32_t Halfplane(const T2DPoint &pt, const T2DPoint &pt1, const T2DPoint &pt2) {
            const double cx1 = pt.x  - pt1.x;
            const double cy1 = pt.y  - pt1.y;
            const double cx2 = pt2.x - pt1.x;
            const double cy2 = pt2.y - pt1.y;
            return Halfplane(cx1, cy1, cx2, cy2);
        }

        // checks if three points lie in one line
        template <class T2DPoint>
        inline bool PointInLine(const T2DPoint& pt1, const T2DPoint& pt2, const T2DPoint& pt3) {
            return (Halfplane(pt1, pt2, pt3) == 0);
        }

        // use this function instead PointInSegment, when you know, when points are in one line
        // if pt == pt1 or pt == pt2 -> true will be returned also
        template <class T2DPoint>
        inline bool PointInSegment2(const T2DPoint& pt, const T2DPoint& pt1, const T2DPoint& pt2)
        {
            if (pt1.x > pt2.x + dxf_math::eps)
            {
                return (pt.x > pt2.x - dxf_math::eps && pt.x < pt1.x + dxf_math::eps);
            }
            else if (pt1.x < pt2.x - dxf_math::eps)
            {
                return (pt.x < pt2.x + dxf_math::eps && pt.x > pt1.x - dxf_math::eps);            
            }
            else
            {
                if (pt1.y > pt2.y)
                {
                    return (pt.y > pt2.y - dxf_math::eps && pt.y < pt1.y + dxf_math::eps);
                }
                else
                {
                    return (pt.y < pt2.y + dxf_math::eps && pt.y > pt1.y - dxf_math::eps);
                }
            }
        }

        // checks if the point is inside polygon
        template <class T2DPoint>
        inline bool PointInsidePolygon(const T2DPoint &pt, const T2DPoint poly[], size_t cnt)
        {
            if (cnt < 3)
            {
                return false;
            }
            
            T2DPoint pt1; 
            pt1.x = pt.x - 1.0;
            pt1.y = pt.y - 1.0;

            for (size_t i1 = 0; i1 < cnt; i1++)
            {
                if (poly[i1].x < pt1.x)
                    pt1.x = poly[i1].x - 1.0;
                if (poly[i1].y < pt1.y)
                    pt1.y = poly[i1].y - 1.0;
            }

            int32_t* hpl = new int32_t[cnt];
            for (size_t i3 = 0; i3 < cnt; i3++)
                hpl[i3] = Halfplane(poly[i3], pt, pt1);
            
            int32_t intcnt = 0;
            for (size_t i4 = 0; i4 < cnt; i4++)
            {
                if (hpl[i4] != 0)
                {
                    size_t i2 = (i4 == cnt-1)?(0):(i4+1);
                    if (hpl[i2] == 0)
                    {
                        while (hpl[i2] == 0 && PointInSegment2(poly[i2], pt, pt1))    
                            i2 = (i2 == cnt-1)?(0):(i2+1);

                        if (hpl[i2] != 0 && hpl[i2] != hpl[i4])
                            intcnt++;
                    }
                    else
                    {
                        if (hpl[i2] != hpl[i4] &&
                            abs(Halfplane(pt, poly[i4], poly[i2]) -
                            Halfplane(pt1, poly[i4], poly[i2])) == 2)
                            intcnt++;
                    }            
                }
            }

            delete[] hpl;

            // Check if intcnt is even number
            return ((intcnt & 1) == 1);
        }

        template <class T2DPoint, class TPoly>
        inline bool PointInsidePolygon(const T2DPoint &pt, TPoly &poly)
        {
            if (poly.empty()) {
                return false;
            }

            return PointInsidePolygon(pt, &poly[0], poly.size());
        }

        template <class TPoint, class TSegments>
        inline void GetSegmentsFromPoints(const TPoint points[], size_t nCnt, TSegments &segments)
        {
            segments.resize(nCnt);
            TSegments::value_type seg;
            for (size_t i = 0; i < nCnt; ++i)  
            {
                TPoint pt1 = points[i];
                TPoint pt2 = (i == nCnt - 1) ? points[0] : points[i + 1];
                seg.first.x  =  pt1.x;
                seg.first.y  =  pt1.y;
                seg.second.x =  pt2.x;
                seg.second.y =  pt2.y;
                segments[i] = seg;
            }
        }

        template <class T2DPoint>
        inline void MovePointOnLine(const T2DPoint &pt1, const T2DPoint &pt2, T2DPoint &point, double offs)
        {
            double b = pt2.x - pt1.x;
            double a = pt2.y - pt1.y;
            ASSERT( !dxf_math::eq0(a) || !dxf_math::eq0(b) );
            double ratio = offs / ::sqrt(a*a+b*b);
            
            point.x += b * ratio;
            point.y += a * ratio;
        }

        // vector is rotated by specified angle clockwise
        template <class TVector>
        inline void RotateVector(TVector &vect, double angle)
        {
            double sinx = sin(angle);
            double cosx = cos(angle);
            TVector old = vect;
            vect.x = old.x * cosx - old.y * sinx;
            vect.y = old.x * sinx + old.y * cosx;
        }

        template <class TPoint, class TVector>
        inline TPoint MovePointByVect(const TPoint &pt, const TVector &v)
        {
            TPoint pt1;
            pt1.x = pt.x + v.x;
            pt1.y = pt.y + v.y;
            return pt1;
        }

        inline double DegToRad(double deg) {
            return deg / 180.0 * DXF_PI;    
        }
    };

    template <class TPoint>
    inline void AddSegment(CHatchSegments &segments, const TPoint &pt1, const TPoint &pt2)
    {
        CHatchSegments::value_type seg;
        seg.first.x  = pt1.x;
        seg.first.y  = pt1.y;
        seg.second.x = pt2.x;
        seg.second.y = pt2.y;
        segments.push_back(seg);
    }

    template <class TPoint>
    inline void AddDashSegments(CHatchSegments &segments, const std::vector<double> &dashes, 
                                const TPoint &pt1, const TPoint &pt2)
    {
        if ( dashes.size() == 0 ) {
            AddSegment(segments, pt1, pt2);
            return;
        }

        TPoint ptD1 = pt1;
        
        while ( geom::PointInSegment2(ptD1, pt1, pt2) )
        {
            for ( auto dashIt = dashes.begin(); dashIt != dashes.end(); ++dashIt)
            {
                const double dVal    = *dashIt;
                const bool bDash     = dVal > 0 || dxf_math::eq0(dVal);
                const double dOffset = fabs(*dashIt);
               
                TPoint ptD2 = ptD1;
                geom::MovePointOnLine(pt1, pt2, ptD2, dOffset);
            
                if ( bDash )
                {
                    AddSegment(segments, ptD1, ptD2);
                }

                ptD1  = ptD2;
            }
        }
    }

    inline void AddSegment(CHatchSegments &segments, CDXFMatrix3D &trans_matrix, 
                           const std::vector<double> &dashes, double dx, double dy, const CHatchPoly::value_type &v)
    {              
        CDXF3DPoint p1;
        CDXF3DPoint p2;

        p1.x = dx;
        p1.y = dy;
        p1.z = 0.;

        p1 = MatrixPointMultiply(trans_matrix, p1);
        p2 = geom::MovePointByVect(p1, v);

        AddDashSegments(segments, dashes, p1, p2);  
    }

    void GenerateCarpet(const CDXFHatch &entity, const CHatchPoly &poly, CHatchSegments &segments)
    {
        const std::vector<CDXFHatch::CDXFHatchPattern> &patternLines = entity.m_patternLines;

        CHatchPoly::value_type  mi, ma;
        if (!poly.empty())
        {
            geom::CalcBoundRectangle(&poly[0], poly.size(), mi, ma);
        }

        for (auto it = patternLines.begin(); it != patternLines.end(); ++it) 
        { 
          //  if ( it == patternLines.begin() ) continue;

            const CDXFHatch::CDXFHatchPattern &pat = (*it);
            const std::vector<double> &dashes = pat.m_dashes;
            double dAngle = pat.m_dAngle;
          
            const double dRadAngle = geom::DegToRad(dAngle);
            
            const double dDeltaX   = pat.m_offset.m_x;
            const double dDeltaY   = pat.m_offset.m_y;  
            // dDeltaY - value gives the spacing between members of the family
            // dDeltaX - value gives the displacement between members of the family in the 
            // direction of the line!
            double dOriginX  = pat.m_basePoint.m_x;
            double dOriginY  = pat.m_basePoint.m_y;

            if ( dOriginX > ma.x )
            {   // pseudo logic, because we propagate hatching according to the axis directions
                dOriginX = mi.x - fabs(dOriginX);
            }

            if ( dOriginY > ma.y )
            {   // pseudo logic, because we propagate hatching according to the axis directions
                dOriginY = mi.y - fabs(dOriginY);
            }

            const double dStartX   = mi.x + dOriginX;
            const double dStartY   = mi.y + dOriginY;

            const double dResize = 2; // this is nonsense, but I use this to cover all polygon area....
            // pseudo logic, just trying to minimize possible carpet area
            const double dW1 = (ma.x - mi.x) * dResize;
            const double dH1 = (ma.y - mi.y) * dResize;
            const double dW2 = (ma.x - dStartX)  * dResize;
            const double dH2 = (ma.y - dStartY)  * dResize;
            const double dW  = dW1 > dW2 ? dW1 : dW2;
            const double dH  = dH1 > dH2 ? dH1 : dH2;
            const double dM  = dW > dH ? dW : dH;

            double dx = 0;
            double dy = 0.;
            bool bOffset = false;      
            
            CDXFMatrix3D trans_matrix;
            trans_matrix.SetIdentity();
            trans_matrix.m14 = dStartX;
            trans_matrix.m24 = dStartY;
            trans_matrix.m34 = 0.;    
            // we are going here along y axis and x axis -> this is not complete and fine
            // maybe algorithm should be based not on the axis, but on the line
            // and this line start point should be on the 0, 0
            // but in this case I can not manage dDeltaX.. correcly
            // another approach could be -> it should be used matrix based calculations:
            // generate hatch lines as here using y axis
            // and after that rotate, translate lines(segments) using matrix calculations
            if ( !dxf_math::eq0(dDeltaY) )
            {
                CHatchPoly::value_type v;
                v.x = dM;
                v.y = 0;

                if ( dDeltaY < 0 )
                {
                    v.x = -v.x;
                }
                geom::RotateVector(v, dRadAngle);

                for (dy = 0; dxf_math::lessOrEq(dy, dH); dy = dy + fabs(dDeltaY))
                {
                    dx = bOffset ? dDeltaX : 0.0; 
                    bOffset = !bOffset;
                    AddSegment(segments, trans_matrix, dashes, dx, dy, v);
                }
            }

         
            bOffset = false; // because we start again from 0, 0 but using other => "x" axis

            {
                double dShiftX = dDeltaX; // 90, 270, ... this is valid angles, but invalid for the tan
                double dShiftY = dDeltaY;
                bool bContinue = true;

                int32_t nRepeat = int32_t(dAngle / 360.);
                double dAngle1 = dAngle - 360.*nRepeat;

                if (  -90 < dAngle1 && dAngle1 > 90 ) 
                {
                    const double dTan = tan(dRadAngle);
                    if ( dxf_math::eq0(dTan) ) // we have angle 0 -> it will be covered by y axis filling algorithm
                    {
                        bContinue = false;
                    }
                    else
                    {
                        dShiftX = dDeltaY / dTan; // this is ok
                        dShiftY = dDeltaX * dTan; // this is ok
                    }
                }

                if ( dxf_math::eq0(dShiftX) ) {
                    bContinue = false;
                }

                if ( bContinue )
                {
                    CHatchPoly::value_type v;
                    v.x = dM;
                    v.y = 0;
                    geom::RotateVector(v, dRadAngle);

                    for (dx = 0.; dxf_math::lessOrEq(dx, dW); dx = dx + fabs(dShiftX))
                    {
                        dy = bOffset ? dShiftY : 0.0; 
                        bOffset = !bOffset;
                        AddSegment(segments, trans_matrix, dashes, dx, dy, v);
                    }
                }
            }
        }
    }
 
    namespace geom_segment
    {
        // returns true if segments intersect: returns intersection point
        template<typename T>
        inline bool intersect(const T& x1, const T& y1,
                              const T& x2, const T& y2,
                              const T& x3, const T& y3,
                              const T& x4, const T& y4,
                                    T& ix,       T& iy)
        {
          T ax = x2 - x1;
          T bx = x3 - x4;

          T lowerx;
          T upperx;
          T uppery;
          T lowery;

          if ( ax < T(0.0) )
          {
             lowerx = x2;
             upperx = x1;
          }
          else
          {
             upperx = x2;
             lowerx = x1;
          }

          if ( bx > T(0.0) )
          {
             if ((upperx < x4) || (x3 < lowerx))
             return false;
          }
          else if ((upperx < x3) || (x4 < lowerx))
          {
             return false;
          }

          T ay = y2 - y1;
          T by = y3 - y4;

          if (ay < T(0.0))
          {
             lowery = y2;
             uppery = y1;
          }
          else
          {
             uppery = y2;
             lowery = y1;
          }

          if (by > T(0.0))
          {
             if ((uppery < y4) || (y3 < lowery))
                return false;
          }
          else if ((uppery < y3) || (y4 < lowery))
             return false;

          T cx = x1 - x3;
          T cy = y1 - y3;
          T d  = (by * cx) - (bx * cy);
          T f  = (ay * bx) - (ax * by);

          if (f > T(0.0))
          {
             if ((d < T(0.0)) || (d > f))
                return false;
          }
          else if ((d > T(0.0)) || (d < f))
             return false;

          T e = (ax * cy) - (ay * cx);

          if (f > T(0.0))
          {
             if ((e < T(0.0)) || (e > f))
                return false;
          }
          else if ((e > T(0.0)) || (e < f))
             return false;

          T ratio = (ax * -by) - (ay * -bx);

          if ( !dxf_math::eq0(ratio) )
          {
             ratio = ((cy * -bx) - (cx * -by)) / ratio;
             ix    = x1 + (ratio * ax);
             iy    = y1 + (ratio * ay);
          }
          else
          {
             if ( dxf_math::eqx((ax * -cy),(-cx * ay)) )
             {
                ix = x3;
                iy = y3;
             }
             else
             {
                ix = x4;
                iy = y4;
             }
          }

          return true;
        }


        template <typename TPoint>
        inline bool intersect(const TPoint &pt1, const TPoint &pt2,
                              const TPoint &pt3, const TPoint &pt4,
                                    TPoint &pt)
        {
            return intersect(pt1.x, pt1.y, 
                             pt2.x, pt2.y,
                             pt3.x, pt3.y,
                             pt4.x, pt4.y,
                              pt.x, pt.y);
        }

        template <typename TSegment, typename TPoint>
        inline bool intersect(const TSegment &seg1, 
                              const TSegment &seg2, 
                                    TPoint &pt)
        {
            return intersect(seg1.first, seg1.second,
                             seg2.first, seg2.second, 
                             pt);
        }
    };

    // checks if two colinear vectors have the same direction
    template <typename T>
    inline bool SameDirection(const T &x1, const T &y1, const T &x2, const T &y2)  {
        return ((dxf_math::sign(x1) == dxf_math::sign(x2)) && (dxf_math::sign(y2) == dxf_math::sign(y1)));        
    }

    class CSortByLine
    {
    public:
        template <class TPoint>
        bool operator()(const TPoint &pt1, const TPoint &pt2)
        {
            const double dx = pt2.x - pt1.x;
            const double dy = pt2.y - pt1.y;
            if ( SameDirection(dx, dy, m_dx, m_dy) )
            {
                return true;
            }

            return false;
        }

    // Attributes
    public:
        double m_dx;
        double m_dy;
    };

    template <class T2DPointArray>
    void RemoveDuplicatePoints(T2DPointArray &poly)
    {
        auto new_end = std::end(poly);
        for (auto it1 = std::begin(poly); it1 != new_end; ++it1) {
            new_end = std::remove_if(std::execution::seq, std::next(it1), new_end, [&](const auto &val2) {
                return dxf_math::eqx(it1->x, val2.x) && dxf_math::eqx(it1->y, val2.y);
            });
        }
        poly.erase(new_end, poly.end());
    }

    // removes unnecessary point from polygon, leaves only corner points
    template <class T2DPointArray>
    inline void RemovePointsOnEdges(T2DPointArray &poly)
    {
        int32_t cnt = poly.size();

        if (cnt < 3) {
            return;
        }

        T2DPointArray poly2;    
        poly2.resize(cnt);
        int32_t detcnt = 0;
        for (auto i1 = 0; i1 < cnt; ++i1)
        {
            int32_t i2 = (i1 == 0)?(cnt-1):(i1-1);
            int32_t i3 = (i1 == cnt-1)?(0):(i1+1);
            if (!PointInLine(poly[i1], poly[i2], poly[i3]))
            {
                poly2[detcnt] = poly[i1];
                detcnt++;
            }
        }

        poly = poly2;
        poly.resize(detcnt);
    }

    void GetPoly(const CDXFHatch &entity, CHatchPoly &points)
    {
        CGetBoundryPoints get_poly(&points);
        std::for_each(entity.m_boundaryPtrs.begin(), entity.m_boundaryPtrs.end(), get_poly);
        RemoveDuplicatePoints(points);    
        //RemovePointsOnEdges(points);          
    }

    void GetHatch(const CDXFHatch &entity, CHatchSegments &result)
    {
        CHatchPoly points;
        GetPoly(entity, points);

        CHatchSegments segments;
        GenerateCarpet(entity, points, segments);

        CHatchSegments poly_segments;
        if (!points.empty())
        {
            geom::GetSegmentsFromPoints(&points[0], points.size(), poly_segments);
        }

        {
            typedef CHatchSegments::const_iterator CIt;
            CIt seg_end_it  = segments.end(); 
            CIt poly_end_it = poly_segments.end();
            CIt poly_it;
            CHatchPoint pt;
            std::vector<CHatchPoint> inter_points;
            bool bIntersects = false;
            for (CIt seg_it = segments.begin(); seg_it != seg_end_it; ++seg_it)
            {      
                bIntersects = false;
                inter_points.clear();

                const bool bFirst  = geom::PointInsidePolygon((*seg_it).first,  points);
                const bool bSecond = geom::PointInsidePolygon((*seg_it).second, points);
                
                for (poly_it = poly_segments.begin(); poly_it != poly_end_it; ++poly_it)
                {
                    if ( geom_segment::intersect(*seg_it, *poly_it, pt) )
                    {
                        bIntersects = true;
                        inter_points.push_back(pt);
                    }      
                }
                //bIntersects = false;
                if ( bIntersects )
                {
                    CSortByLine sort_by_line;
                    sort_by_line.m_dx = (*seg_it).second.x - (*seg_it).first.x;
                    sort_by_line.m_dy = (*seg_it).second.y - (*seg_it).first.y;

                    if ( bFirst ) 
                    {
                        inter_points.insert(inter_points.begin(), (*seg_it).first);
                    }

                    std::sort(inter_points.begin(), inter_points.end(), sort_by_line);

                    if ( bSecond ) 
                    {
                        inter_points.push_back((*seg_it).second);
                    }

                    typedef std::vector<CHatchPoint>::const_iterator CPointIt;
                    CHatchSegment new_seg;
                    CPointIt beg_it = inter_points.begin();
                    CPointIt end_it = inter_points.end();
                    CPointIt it2;
                    
                    for (CPointIt it1 = beg_it; it1 != end_it; ++it1)
                    {
                       ASSERT(it1 != end_it);
                       new_seg.first = *it1;
                       it2 = it1; ++it2;
                    
                       if ( it2 == end_it )
                            break;

                       new_seg.second = *it2;
                       result.push_back(new_seg);
                   
                       it1 = it2;
                    }
                }
                else
                {
                    if ( !bFirst && !bSecond )
                    {
                        continue; // skip => segment is not in the poly
                    }

                    result.push_back(*seg_it);
                }
            }
        }
    }
};
