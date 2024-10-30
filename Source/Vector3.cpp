#include "../Headers/Vector3.h"
#include "../Headers/Physics.h"
#include "../Headers/Precision.h"
#include <cmath>
#include <string>
#include <sstream>

namespace Physics {
	/// <summary>
	/// Default constructor creates zero vector.
	/// </summary>
	Vector3::Vector3() : x(0), y(0), z(0), pad(0) {};

	/// <summary>
	/// Explicit constructor creates vector of given components.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	Vector3::Vector3(const real x, const real y, const real z) : x(x), y(y), z(z), pad(0) {};

	/// <summary>
	/// Inverts components of the vector.
	/// </summary>
	void Vector3::invert() {
		this->x = -x;
		this->y = -y;
		this->z = -z;
	};

	real Vector3::magnitude() {
		return real_sqrt(x * x + y * y + z * z);
	};

	/// <summary>
	/// Ian Millington suggested this method avoids the call to sqrt, which can be slow.
	/// </summary>
	/// <returns></returns>
	real Vector3::magnitudeSquared() {
		return x * x + y * y + z * z;
	}

	void Vector3::clear() {
		x = 0; y = 0; z = 0;
	}

	void Vector3::normalize() {
		// TODO
		// two calls to magnitude because compiler puts into one reg anyways, or
		// put into variable only call once explicitly? 
		// does it matter?
		if (magnitude() > 0)
			(*this) *= ((real)1 / magnitude());
	}

	/// <summary>
	/// Adds other multiplied by scalar to this vector.
	/// </summary>
	/// <param name="other"></param>
	/// <param name="scalar"></param>
	void Vector3::addScaledVector(const Vector3& other, const real scalar) {
		x += other.x * scalar;
		y += other.y * scalar;
		z += other.z * scalar;
	}

	/// <summary>
	/// Return a new vector whose components are defined by
	/// each of this vectors components mulitplied by each of other's.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	Vector3 Vector3::componentWiseProduct(const Vector3& other) {
		return Vector3(x * other.x, y * other.y, z * other.z);
	}

	void Vector3::componentWiseProductUpdate(const Vector3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}

	/// <summary>
	/// Returns the dot product of this and other.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	real Vector3::dotProduct(const Vector3& other) {
		return x * other.x + y * other.y + z * other.z;
	}

	/// <summary>
	/// Returns the dot product of this and other.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	real Vector3::operator*(const Vector3& other) {
		return x * other.x + y * other.y + z * other.z;
	}

	/// <summary>
	/// Returns a vector defined by the cross product of 
	/// this and other.
	/// </summary>
	/// <param name="vector"></param>
	/// <returns></returns>
	Vector3 Vector3::crossProduct(const Vector3& vector) {
		return Vector3(y * vector.z - z * vector.y,
			z * vector.x - x * vector.z,
			x * vector.y - y * vector.x);
	}

	/// <summary>
	/// Returns the vector defined by the cross product of
	/// this and other.
	/// </summary>
	/// <param name="vector"></param>
	/// <returns></returns>
	Vector3 Vector3::operator%(const Vector3& vector) {
		return Vector3(y * vector.z - z * vector.y,
			z * vector.x - x * vector.z,
			x * vector.y - y * vector.x);
	}

	/// <summary>
	/// Transforms this vector into the cross product
	/// of this and other.
	/// </summary>
	/// <param name="other"></param>
	void Vector3::operator%=(const Vector3& other) {
		*this = crossProduct(other);
	}

	/// <summary>
	/// Adds other vector to this vector.
	/// </summary>
	/// <param name="other"></param>
	void Vector3::operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	/// <summary>
	/// Subtracts other vector from this vector.
	/// </summary>
	/// <param name="other"></param>
	void Vector3::operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	/// <summary>
	/// Returns a new vector = this + other
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	Vector3 Vector3::operator+(const Vector3& other) {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	/// <summary>
	/// Returns a new vector = this - other.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	Vector3 Vector3::operator-(const Vector3& other) {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	/// <summary>
	/// returns a copy of this vector mulitplied by given
	/// scalar.
	/// </summary>
	/// <param name="scalar"></param>
	/// <returns></returns>
	Vector3 Vector3::operator*(const real scalar) {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	/// <summary>
	/// Multiply this vector by given scalar.
	/// </summary>
	/// <param name="scalar"></param>
	void Vector3::operator*= (const real scalar) {
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
	}

	std::string Vector3::toString() {
		std::ostringstream stream;
		stream << "Vector3(" << x << ", " << y << ", " << z << ")";
		return stream.str();
	}
}