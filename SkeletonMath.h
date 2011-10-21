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
        ~SkeletonVector();
        
        SkeletonVector& operator=(const SkeletonVector& rhs);
        
        SkeletonVector& operator+=(const SkeletonVector& rhs);
        SkeletonVector& operator-=(const SkeletonVector& rhs);
        SkeletonVector& operator*=(const SkeletonVector& rhs);
        
        const SkeletonVector operator+(const SkeletonVector& rhs) const;
        const SkeletonVector operator-(const SkeletonVector& rhs) const;
        const SkeletonVector operator*(const SkeletonVector& rhs) const;
        
        const SkeletonVector operator*(const float f) {}

        const Point& getPoint() const { return point_; }
        
        void print() const;
        
    private:
        Point point_;
        

};

#endif