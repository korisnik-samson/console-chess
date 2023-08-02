#pragma once

#include <cmath>

/// int32 2D vector struct
struct IVec2 {
	int x; 
    int y;
 
	// Default constructors and x,y constructor
	IVec2() : x(0), y(0) {};
	IVec2(int x, int y) : x(x), y(y) {};

	int lenSqr() const {
		return x * x + y * y;
	}

	float len() const {
		return sqrtf((float)(x * x + y * y));
	}

	int lenL1() const {
		return abs(x) + abs(y);
	}

	// True if a perfect multiple of other vector
	bool multipleOf(const IVec2& v) const {
		return x % v.x == 0 && y % v.y == 0;
	}

	// True if this Vec2 represents a position on a chess board 
	bool in88Square() const {
		return 0 <= x && x < 8 && 0 <= y && y < 8;
	}

	// OPERATORS
	// All operators are standard vectorial operators
	// which are applied element-wise. The non-assignment 
	// multiplication (operator*) defines the dot product.
	#pragma region Operators

	IVec2& operator=(const IVec2& a) {
		x = a.x;
		y = a.y;
		return *this;
	}

	IVec2 operator+(const IVec2& a) const {
		return IVec2(x + a.x, y + a.y);
	}

	IVec2& operator+= (const IVec2& a) {
		x += a.x;
		y += a.y;
		return *this;
	}

	IVec2 operator-() {
		return IVec2(-x, -y);
	}

	IVec2 operator-(const IVec2& a) const {
		return IVec2(x - a.x, y - a.y);
	}

	IVec2& operator-= (const IVec2& a) {
		x -= a.x;
		y -= a.y;
		return *this;
	}

	IVec2 operator*(int s) const {
		return IVec2(x * s, y * s);
	}

	IVec2& operator*= (int s) {
		x *= s;
		y *= s;
		return *this;
	}

	int operator*(const IVec2& a) const {
		return x * a.x + y * a.y;
	}

	IVec2& operator*= (const IVec2& a) {
		x *= a.x;
		y *= a.y;
		return *this;
	}

	IVec2 operator/(int s) const {
		return IVec2(x / s, y / s);
	}

	IVec2& operator/= (int s) {
		x /= s;
		y /= s;
		return *this;
	}

	IVec2 operator/(const IVec2& a) const {
		return IVec2(x / a.x, y / a.y);
	}

	IVec2& operator/= (const IVec2& a) {
		x /= a.x;
		y /= a.y;
		return *this;
	}

	IVec2 operator%(const IVec2& a) const {
		return IVec2(x % a.x, y % a.y);
	}

	IVec2& operator%= (const IVec2& a) {
		x %= a.x;
		y %= a.y;
		return *this;
	}

	bool operator== (const IVec2& a) const {
		return a.x == x && a.y == y;
	}

	bool operator!= (const IVec2& a) const {
		return a.x != x || a.y != y;
	}
	#pragma endregion
};

IVec2 operator*(int s, const IVec2& a) {
	return a.operator*(s);
}

IVec2 operator/(int s, const IVec2& a) {
	return a.operator/(s);
}