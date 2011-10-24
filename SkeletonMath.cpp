#include <stdio.h>

#include "SkeletonMath.h"
        
SkeletonVector::SkeletonVector()
{
}

SkeletonVector::SkeletonVector(const Point* p)
{

    point_.x_ = p->x_;
    point_.y_ = p->y_;
    point_.z_ = p->z_;
    point_.confidence_ = p->confidence_;

}

SkeletonVector::SkeletonVector(const Point& p)
{

    point_.x_ = p.x_;
    point_.y_ = p.y_;
    point_.z_ = p.z_;
    point_.confidence_ = p.confidence_;

}

SkeletonVector::SkeletonVector(const float x, const float y, const float z)
{
    point_.x_ = x;
    point_.y_ = y;
    point_.z_ = z;
}


SkeletonVector::~SkeletonVector()
{
}

SkeletonVector& SkeletonVector::operator=(const SkeletonVector& rhs)
{

    // they are pointing to the same location!
    if (this == &rhs)
        return *this;
        
    point_.x_ = rhs.getPoint().x_;
    point_.y_ = rhs.getPoint().y_;
    point_.z_ = rhs.getPoint().z_;
    point_.confidence_ = rhs.getPoint().confidence_;

}

SkeletonVector& SkeletonVector::operator+=(const SkeletonVector& rhs)
{
    point_.x_ = point_.x_ + rhs.getPoint().x_;
    point_.y_ = point_.y_ + rhs.getPoint().y_;
    point_.z_ = point_.z_ + rhs.getPoint().z_;
    
    return *this;

}

SkeletonVector& SkeletonVector::operator-=(const SkeletonVector& rhs)
{
    point_.x_ = point_.x_ - rhs.getPoint().x_;
    point_.y_ = point_.y_ - rhs.getPoint().y_;
    point_.z_ = point_.z_ - rhs.getPoint().z_;
    
    return *this;
}

SkeletonVector& SkeletonVector::operator*=(const SkeletonVector& rhs)
{
    point_.x_ = point_.x_ * rhs.getPoint().x_;
    point_.y_ = point_.y_ * rhs.getPoint().y_;
    point_.z_ = point_.z_ * rhs.getPoint().z_;
    
    return *this;
}

const SkeletonVector SkeletonVector::operator+(const SkeletonVector& rhs) const
{
    SkeletonVector result = *this;
    result += rhs;
    return result;
}

const SkeletonVector SkeletonVector::operator-(const SkeletonVector& rhs) const
{
    SkeletonVector result = *this;
    result -= rhs;
    return result;
}

const SkeletonVector SkeletonVector::operator*(const SkeletonVector& rhs) const
{
    SkeletonVector result = *this;
    result *= rhs;
    return result;
}

const SkeletonVector SkeletonVector::operator*(const float f) const
{
    SkeletonVector result = *this;
    result.setPoint(result.getPoint().x_ * f, result.getPoint().y_ * f, result.getPoint().z_ * f);
    return result;
}

void SkeletonVector::print() const
{
    printf("point_.x_:       %f\n", point_.x_);
    printf("point_.y_:       %f\n", point_.y_);
    printf("point_.z_:       %f\n", point_.z_);
}