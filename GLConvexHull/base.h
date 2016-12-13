#pragma once
#include "stdheaders.h"
#include "constants.h"
#include <glew\glew.h>

double drand(double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max());

template<class T>
class T2
{
public:

	// Constructors and destructors
	T2(const T& val = T{ 0 }) : m_x(val), m_y(val) {}
    T2(const T& x, const T& y) : m_x(x), m_y(y) {}
    T2(const T2& other) : m_x(other.m_x), m_y(other.m_y) {}
    virtual ~T2() {}

    // Access
	const T& x() const { return m_x; }
	T& x() { return m_x; }
	const T& y() const { return m_y; }
	T& y() { return m_y; }

    T& operator[](int index);
    const T& operator[](int index) const;

    T* begin() { return &m_x; }
    const T* begin() const { return &m_x; }
    T* end() { return &m_x + dim; }
    const T* end() const { return &m_x + dim; }

    // Unary operators
    T2 operator-() const { return T2{ -m_x, -m_y }; }

    // Assignment operators
    void operator+=(const T2& other);
    void operator-=(const T2& other);
    void operator*=(const T& factor);
    void operator/=(const T& factor);

    // Binary operators
    void operator=(const T2& other);

    T2 operator+(const T2& other) const;
    T2 operator-(const T2& other) const;
    T operator*(const T2& other) const;
    T2 operator*(const T& factor) const;
    T2 operator/(const T& factor) const;

    // Norms and lengths
    T norm() const;
    T norm2() const;
    T normalize();
    T2 normalized() const;

    // Input and output
    friend std::ostream& operator<<(std::ostream& os, const T2& val)
    {
        os << val.m_x << " " << val.m_y;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, T2& val)
    {
        is >> val.m_x >> val.m_y;
        return is;
    }

    // Auxiliary
    static T2 rand(const T& min = std::numeric_limits<T>::min(), const T& max = std::numeric_limits<T>::max()) { return T2{ 0 }; }

protected:
    T m_x;
    T m_y;

public:

	// Constants
	static const int dim = 2;
};

#pragma region T2 implementation

template<class T>
T& T2<T>::operator[](int index)
{
    assert(0 <= index && index < dim);
    return *(&m_x + index);
}

template<class T>
const T& T2<T>::operator[](int index) const
{
    assert(0 <= index && index < dim);
    return *(&m_x + index);
}

template<class T>
void T2<T>::operator+=(const T2& other)
{
    m_x += other.m_x;
    m_y += other.m_y;
}

template<class T>
void T2<T>::operator-=(const T2& other)
{
    m_x -= other.m_x;
    m_y -= other.m_y;
}

template<class T>
void T2<T>::operator*=(const T& factor)
{
    m_x *= factor;
    m_y *= factor;
}

template<class T>
void T2<T>::operator/=(const T& factor)
{
    m_x /= factor;
    m_y /= factor;
}

template<class T>
void T2<T>::operator=(const T2& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
}

template<class T>
T2<T> T2<T>::operator+(const T2& other) const
{
    return T2{ m_x + other.m_x, m_y + other.m_y };
}

template<class T>
T2<T> T2<T>::operator-(const T2& other) const
{
    return T2{ m_x - other.m_x, m_y - other.m_y };
}

template<class T>
T T2<T>::operator*(const T2& other) const
{
    return T{ m_x*other.m_x + m_y*other.m_y };
}

template<class T>
T2<T> T2<T>::operator*(const T& factor) const
{
    return T2{ m_x*factor, m_y*factor };
}

template<class T>
T2<T> T2<T>::operator/(const T& factor) const
{
    assert(abs(factor) > cts::eps);
    return T2{ m_x/factor, m_y/factor };
}

template<class T>
T T2<T>::norm() const
{
    return sqrt(norm2());
}

template<class T>
T T2<T>::norm2() const
{
    return m_x*m_x + m_y*m_y;
}

template<class T>
T T2<T>::normalize()
{
    const T n = norm();
    assert(n > cts::eps);
    m_x /= n;
    m_y /= n;

    return n;
}

template<class T>
T2<T> T2<T>::normalized() const
{
    T2 result{*this};
    result.normalize();
    return result;
}

using double2 = T2<double>;

template<>
inline double2 double2::rand(const double& min, const double& max)
{
    double2 result;
    for (int i = 0; i < T2<double>::dim; ++i)
        result[i] = drand(min, max);
    return result;
}

#pragma endregion

using int2 = T2<int>;
using float2 = T2<float>;
using glfloat2 = T2<GLfloat>;
using double2 = T2<double>;

template<class T>
class T3
{
public:

    // Constructors and destructors
    T3();
    T3(const T& val);
    T3(const T& x, const T& y, const T& z);
	T3(const T3& other) : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {}
    virtual ~T3() {}

    // Access
	const T& x() const { return m_x; }
	T& x() { return m_x; }
	const T& y() const { return m_y; }
	T& y() { return m_y; }
	const T& z() const { return m_z; }
	T& z() { return m_z; }

    T& operator[](int index);
    const T& operator[](int index) const;

    T* begin() { return &m_x; }
    const T* begin() const { return &m_x; }
    T* end() { return &m_x + dim; }
    const T* end() const { return &m_x + dim; }

    // Unary operators
	T3 operator-() const { return T3{ -m_x, -m_y, -m_z }; }

    // Assignment operators
    void operator+=(const T3& other);
    void operator-=(const T3& other);
    void operator*=(const T& factor);
    void operator/=(const T& factor);

    // Binary operators
    void operator=(const T3& other);

    T3 operator+(const T3& other) const;
    T3 operator-(const T3& other) const;
    T operator*(const T3& other) const;
    T3 operator*(const T& factor) const;
    T3 operator/(const T& factor) const;
    T3 operator%(const T3& other) const;

    // Norms and lengths
    T norm() const;
    T norm2() const;
    T normalize();
    T3 normalized() const;

    // Input and output
    friend std::ostream& operator<<(std::ostream& os, const T3& val)
    {
        os << val.x << " " << val.y << " " << val.z;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, T3& val)
    {
        is >> val.x >> val.y >> val.z;
        return is;
    }

    // Auxiliary
    static T3 rand(const T& min = std::numeric_limits<T>::min(), const T& max = std::numeric_limits<T>::max()) { return T3{ 0 }; }

public:

    // Constants
    static const int dim = 3;

protected:
	T m_x;
	T m_y;
	T m_z;
};

#pragma region T3 Implementation

template<class T>
T3<T>::T3()
{
	static_assert(std::_Is_numeric<T>::value, "Only numeric types are supported");
	m_x = T{ 0 };
	m_y = T{ 0 };
	m_z = T{ 0 };
}

template<class T>
T3<T>::T3(const T& val)
{
    static_assert(std::_Is_numeric<T>::value, "Only numeric types are supported");
	m_x = val;
	m_y = val;
	m_z = val;
}

template<class T>
T3<T>::T3(const T& x, const T& y, const T& z)
{
    static_assert(std::_Is_numeric<T>::value, "Only numeric types are supported");
    m_x = x;
    m_y = y;
    m_z = z;
}

template<class T>
T& T3<T>::operator[](int index)
{
    assert(0 <= index && index < dim);
    return *(&m_x + index);
}

template<class T>
const T& T3<T>::operator[](int index) const
{
    assert(0 <= index && index < dim);
	return *(&m_x + index);
}

template<class T>
void T3<T>::operator+=(const T3& other)
{
	m_x += other.m_x;
	m_y += other.m_y;
	m_z += other.m_z;
}

template<class T>
void T3<T>::operator-=(const T3& other)
{
	m_x -= other.m_x;
	m_y -= other.m_y;
	m_z -= other.m_z;
}

template<class T>
void T3<T>::operator*=(const T& factor)
{
	m_x *= factor;
	m_y *= factor;
	m_z *= factor;
}

template<class T>
void T3<T>::operator/=(const T& factor)
{
    assert(abs(factor) > cts::eps);
	m_x /= factor;
	m_y /= factor;
	m_z /= factor;
}

template<class T>
void T3<T>::operator=(const T3& other)
{
	m_x = other.m_x;
	m_y = other.m_y;
	m_z = other.m_z;
}

template<class T>
T3<T> T3<T>::operator+(const T3& other) const
{
	return T3{ m_x + other.m_x, m_y + other.m_y, m_z + other.m_z };
}

template<class T>
T3<T> T3<T>::operator-(const T3& other) const
{
	return T3{ m_x - other.m_x, m_y - other.m_y, m_z - other.m_z };
}

template<class T>
T T3<T>::operator*(const T3& other) const
{
	return T{ m_x*other.m_x + m_y*other.m_y + m_z*other.m_z };
}

template<class T>
T3<T> T3<T>::operator*(const T& factor) const
{
	return T3{ m_x*factor, m_y*factor, m_z*factor };
}

template<class T>
T3<T> T3<T>::operator/(const T& factor) const
{
    assert(abs(factor) > cts::eps);
	return T3{ m_x/factor, m_y/factor, m_z/factor };
}

template<class T>
T3<T> T3<T>::operator%(const T3& other) const
{
	return T3{ m_y*other.m_z - m_z*other.m_y, m_z*other.m_x - m_x*other.m_z, m_x*other.m_y - m_y*other.m_x };
}

template<class T>
inline T3<T> operator*(const T& factor, const T3<T>& vector)
{
    return vector*factor;
}

template<class T>
T T3<T>::norm() const
{
	return T{ sqrt(norm2()) };
}

template<class T>
T T3<T>::norm2() const
{
    const T3& self = *this;
    return self*self;
}

template<class T>
T T3<T>::normalize()
{
    const T n = norm();
    assert(n > cts::eps);
	m_x /= n;
	m_y /= n;
	m_z /= n;

    return n;
}

template<class T>
T3<T> T3<T>::normalized() const
{
    T3 result(*this);
    const T n = result.norm();
    assert(n > cts::eps);
    for (T& i : result)
        i /= n;

    return result;
}

#pragma endregion

using double3 = T3<double>;

template<>
inline double3 double3::rand(const double& min, const double& max)
{
    double3 result;
    for (int i = 0; i < T3<double>::dim; ++i)
        result[i] = drand(min, max);
    return result;
}

using int3 = T3<int>;
using float3 = T3<float>;
using glfloat3 = T3<GLfloat>;