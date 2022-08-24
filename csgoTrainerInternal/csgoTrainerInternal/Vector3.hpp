#pragma once

class Vector3
{
public:
	float x, y, z;

	Vector3()
	{
		x = y = z = 0;
	}
	
	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3& operator+(Vector3 arg)
	{
		x += arg.x;
		y += arg.y;
		z += arg.z;
		return *this;
	}
	Vector3& operator-(Vector3 arg)
	{
		x -= arg.x;
		y -= arg.y;
		z -= arg.z;
		return *this;
	}
	Vector3& operator*(int arg)
	{
		x *= arg;
		y *= arg;
		z *= arg;
		return *this;
	} 

	void Normalize()
	{
		while (y < -180)
		{
			y += 360;
		}
		while (y > 180)
		{
			y -= 360;
		}
		if (x < -89)
		{
			x = -89;
		}
		if (x > 89)
		{
			x = 89;
		}
	}
};
