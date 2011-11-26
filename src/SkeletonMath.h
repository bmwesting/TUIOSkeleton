//SkeletonMath.h

#ifndef _SKELETON_MATH_H
#define _SKELETON_MATH_H

#include <math.h>

struct Point
{
    float x_, y_, z_, confidence_;
};

class Vector
{
    public:
        Vector();
        Vector(const Point* p);
        Vector(const Point& p);
        Vector(const float x, const float y, const float z);
        Vector(const int x, const int y, const int z);
        ~Vector();
        
        Vector& operator=(const Vector& rhs);
        
        Vector& operator+=(const Vector& rhs);
        Vector& operator-=(const Vector& rhs);
        Vector& operator*=(const Vector& rhs);
        
        const Vector operator+(const Vector& rhs) const;
        const Vector operator-(const Vector& rhs) const;
        const Vector operator*(const Vector& rhs) const;
        
        const Vector operator*(const float f) const;

        const Point& getPoint() const { return point_; }
        void setPoint(float x, float y, float z) { point_.x_ = x;
                                                   point_.y_ = y;
                                                   point_.z_ = z; }
        
        void print() const;
        Vector crossProduct(const Vector& rhs);
        float dotProduct(const Vector& rhs);
        inline float magnitude() {
                                return sqrt(point_.x_ * point_.x_ +
                                            point_.y_ * point_.y_ +
                                            point_.z_ * point_.z_); }
        
    private:
        Point point_;
        

};

#endif