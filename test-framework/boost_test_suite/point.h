#pragma once
#include <ostream>


class point3d
{
	int _x;
	int _y;
	int _z;

public:
	point3d(const int x = 0, const int y = 0, const int z = 0)
		: _x(x), _y(y), _z(z)
	{}

	int x() const { return _x; }
	point3d& x(const int x) { _x = x; return *this; }
	int y() const { return _y; }
	point3d& y(const int y) { _y = y; return *this; }
	int z() const { return _z; }
	point3d& z(const int z) { _z = z; return *this; }

	bool operator==(const point3d& pt) const
	{
		return _x == pt._x && _y == pt._y && _z == pt._z;
	}

	bool operator!=(const point3d& pt) const
	{
		return !(*this == pt);
	}

	bool operator<(const point3d& pt) const
	{
		return _x < pt._x || _y < pt._y || _z < pt._z;
	}

	friend std::ostream& operator<<(std::ostream& stream, const point3d& pt)
	{
		stream << "(" << pt._x << "," << pt._y << "," << pt._z << ")";
		return stream;
	}

	void offset(const int ox, const int oy, const int oz)
	{
		_x += ox;
		_y += oy;
		_z += oz;
	}

	static point3d origin() { return point3d{}; }
};