//based on opencv 3d point
//taken from https://github.com/opencv/opencv/blob/4.x/modules/core/include/opencv2/core/types.hpp

//////////////////////////////// Point3_ ////////////////////////////////

/** @brief Template class for 3D points specified by its coordinates `x`, `y` and `z`.

An instance of the class is interchangeable with the C structure CvPoint2D32f . Similarly to
Point_ , the coordinates of 3D points can be converted to another type. The vector arithmetic and
comparison operations are also supported.

The following Point3_\<\> aliases are available:
@code
    typedef Point3_<int> Point3i;
    typedef Point3_<float> Point3f;
    typedef Point3_<double> Point3d;
@endcode
@see cv::Point3i, cv::Point3f and cv::Point3d
*/

#include <vector>
using std::vector;
template<typename _Tp> class Point3_
{
public:
    typedef _Tp value_type;

    //! default constructor
    Point3_(){}
    Point3_(_Tp _x, _Tp _y, _Tp _z):x(_x), y(_y), z(_z)
    {

    }
    Point3_(Point3_&& pt) noexcept
    {
        x = std::move(pt.x);
        y = std::move(pt.y);
        z = std::move(pt.z);
    }
    explicit Point3_(const Point3_<_Tp>& pt)
    {
        x = pt.x;
        y = pt.y;
        z = pt.z;
    }
    Point3_(const vector<_Tp>& v){
        x = v[0];
        y = v[1];
        z = v[2];
    }

    Point3_& operator = (const Point3_& pt)
    {
        x = pt.x;
        y = pt.y;
        return *this;
    }

    Point3_& operator = (Point3_&& pt) noexcept
    {
        return *this;
    }
    //! conversion to another data type
    //template<typename _Tp2> operator Point3_<_Tp2>() const;
    //! conversion to cv::Vec<>
    //operator vector<_Tp>() const;

    _Tp &operator [](int idx){
        switch(idx)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                throw;
        }
    }
    //Point3_<_Tp> &operator = (Point3_ <_Tp> &rhs){
    //}

    //! dot product
    //_Tp dot(const Point3_& pt) const;
    //! dot product computed in double-precision arithmetics
    //double ddot(const Point3_& pt) const;
    //! cross product of the 2 3D points
    //Point3_ cross(const Point3_& pt) const;
    _Tp x; //!< x coordinate of the 3D point
    _Tp y; //!< y coordinate of the 3D point
    _Tp z; //!< z coordinate of the 3D point
    //vector<_Tp> vect;
};

typedef Point3_<int> Point3i;
typedef Point3_<float> Point3f;
typedef Point3_<double> Point3d;

