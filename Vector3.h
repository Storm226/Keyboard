#ifndef VECTOR3_H
#define VECTOR3_H

#include "Precision.h"
#include "Physics.h"
#include <string>

namespace Physics {



	/// <summary>
	/// Class used to define 3-Dimensional vectors and operate with them and
	/// on them for use in describing our physics engine.
	/// </summary>
	class Vector3
	{

	public:
		Vector3();
		Vector3(const real x, const real y, const real z);

		// Functions
		void invert();
		real magnitude();
		real magnitudeSquared();
		void normalize();
		void addScaledVector(const Vector3& other, const real scalar);
		void clear();


		// Vector multiplication operations
		Vector3 componentWiseProduct(const Vector3& other);
		void componentWiseProductUpdate(const Vector3& other);

		real dotProduct(const Vector3& other);
		real operator*(const Vector3& other);

		Vector3 crossProduct(const Vector3& other);
		Vector3 operator%(const Vector3& vector);
		void operator%=(const Vector3& other);


		// operators
		void operator+=(const Vector3& other);
		void operator-=(const Vector3& other);
		Vector3 operator+(const Vector3& other);
		Vector3 operator-(const Vector3& other);
		Vector3 operator*(const real scalar);
		void operator*=(const real scalar);

		std::string toString();


		real x;
		real y;
		real z;

	private:
		real pad;
	};
}

#endif