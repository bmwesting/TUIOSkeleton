//SkeletonMath.h

#ifndef _SKELETON_MATH_H
#define _SKELETON_MATH_H

struct Point
{
    float x_, y_, z_, confidence_;
};

class SkeletonVector
{
    public:
        SkeletonVector();
        SkeletonVector(const Point* p);
        SkeletonVector(const Point& p);
        SkeletonVector(const float x, const float y, const float z);
        ~SkeletonVector();
        
        SkeletonVector& operator=(const SkeletonVector& rhs);
        
        SkeletonVector& operator+=(const SkeletonVector& rhs);
        SkeletonVector& operator-=(const SkeletonVector& rhs);
        SkeletonVector& operator*=(const SkeletonVector& rhs);
        
        const SkeletonVector operator+(const SkeletonVector& rhs) const;
        const SkeletonVector operator-(const SkeletonVector& rhs) const;
        const SkeletonVector operator*(const SkeletonVector& rhs) const;
        
        const SkeletonVector operator*(const float f) const;

        const Point& getPoint() const { return point_; }
        void setPoint(float x, float y, float z) { point_.x_ = x;
                                                   point_.y_ = y;
                                                   point_.z_ = z; }
        
        void print() const;
        
    private:
        Point point_;
        

};

#endif