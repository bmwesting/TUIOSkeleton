#include <stdio.h>

#include "SkeletonMath.h"
        
Vector::Vector()
{
}

Vector::Vector(const Point* p)
{

    point_.x_ = p->x_;
    point_.y_ = p->y_;
    point_.z_ = p->z_;
    point_.confidence_ = p->confidence_;

}

Vector::Vector(const Point& p)
{

    point_.x_ = p.x_;
    point_.y_ = p.y_;
    point_.z_ = p.z_;
    point_.confidence_ = p.confidence_;

}

Vector::Vector(const float x, const float y, const float z)
{
    point_.x_ = x;
    point_.y_ = y;
    point_.z_ = z;
}

Vector::Vector(const int x, const int y, const int z)
{
    point_.x_ = x;
    point_.y_ = y;
    point_.z_ = z;
}

Vector::~Vector()
{
}

Vector& Vector::operator=(const Vector& rhs)
{

    // they are pointing to the same location!
    if (this == &rhs)
        return *this;
        
    point_.x_ = rhs.getPoint().x_;
    point_.y_ = rhs.getPoint().y_;
    point_.z_ = rhs.getPoint().z_;
    point_.confidence_ = rhs.getPoint().confidence_;

}

Vector& Vector::operator+=(const Vector& rhs)
{
    point_.x_ = point_.x_ + rhs.getPoint().x_;
    point_.y_ = point_.y_ + rhs.getPoint().y_;
    point_.z_ = point_.z_ + rhs.getPoint().z_;
    
    return *this;

}

Vector& Vector::operator-=(const Vector& rhs)
{
    point_.x_ = point_.x_ - rhs.getPoint().x_;
    point_.y_ = point_.y_ - rhs.getPoint().y_;
    point_.z_ = point_.z_ - rhs.getPoint().z_;
    
    return *this;
}

Vector& Vector::operator*=(const Vector& rhs)
{
    point_.x_ = point_.x_ * rhs.getPoint().x_;
    point_.y_ = point_.y_ * rhs.getPoint().y_;
    point_.z_ = point_.z_ * rhs.getPoint().z_;
    
    return *this;
}

const Vector Vector::operator+(const Vector& rhs) const
{
    Vector result = *this;
    result += rhs;
    return result;
}

const Vector Vector::operator-(const Vector& rhs) const
{
    Vector result = *this;
    result -= rhs;
    return result;
}

const Vector Vector::operator*(const Vector& rhs) const
{
    Vector result = *this;
    result *= rhs;
    return result;
}

const Vector Vector::operator*(const float f) const
{
    Vector result = *this;
    result.setPoint(result.getPoint().x_ * f, result.getPoint().y_ * f, result.getPoint().z_ * f);
    return result;
}

void Vector::print() const
{
    printf("point_.x_:       %f\n", point_.x_);
    printf("point_.y_:       %f\n", point_.y_);
    printf("point_.z_:       %f\n", point_.z_);
}

Vector Vector::crossProduct(const Vector& rhs)
{
    Vector c = Vector(point_.y_*rhs.getPoint().z_ - point_.z_*rhs.getPoint().y_,
                point_.z_*rhs.getPoint().x_ - point_.x_*rhs.getPoint().z_,
                point_.x_*rhs.getPoint().y_ - point_.y_*rhs.getPoint().x_);
                
    return c;
}

float Vector::dotProduct(const Vector& rhs)
{
    return point_.x_*rhs.getPoint().x_ +
           point_.y_*rhs.getPoint().y_ + 
           point_.z_*rhs.getPoint().z_;
}