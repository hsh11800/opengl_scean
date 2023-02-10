/*
quatanion function for trackball


*/
#pragma once
#define _USE_MATH_DEFINES
#include"../InitShader.h"
#include<math.h>
VEC4 translate(VEC4 a, VEC3 d)
{
	glm::mat4x4 M = /*glm::transpose/**/(glm::translate(glm::mat4(1.0), d));
	return M * a;
}

bool trackingMouse = false;
bool redrawContinue = false;
bool trackballMove = false;

GLuint rquat_loc;//uniform variable rocation
GLuint translate_loc;//uniform variable translation
VEC4 rquat = VEC4(1.0, 0.0, 0.0, 0.0);
glm::mat4 trans = glm::mat4(1.0);
VEC3 axis;
GLfloat angle;
GLfloat x_d = 0.0;
VEC4 mulq(VEC4 a, VEC4 b)
{
	VEC3 aa = VEC3(a[1], a[2], a[3]);
	VEC3 bb = VEC3(b[1], b[2], b[3]);
	VEC3 cc = a[0] * bb + b[0] * aa + glm::cross(bb, aa);
	return VEC4(b[0] * a[0] - glm::dot(aa, bb), cc);
}
VEC4 invq(VEC4 a)
{
	return VEC4(a[0], -a[1], -a[2], -a[3]) / glm::dot(a, a);
}

VEC3 lastPos = VEC3(0.0, 0.0, 0.0);
int curx, cury;
int startX, startY;

VEC3 trackball_ptov(int x, int y, int width, int height)
{
	float d, a;
	VEC3 v;

	v[0] = (2.0 * x - width) / width;
	v[1] = (height - 2.0 * y) / height;

	d = glm::sqrt(v[0] * v[0] + v[1] * v[1]);
	if (d < 1.0)
	{
		v[2] = cos((M_PI / 2) * d);
	}
	else
	{
		v[2] = 0.0;
	}
	a = 1.0 / sqrt(glm::dot(v, v));
	v *= a;
	return v;
}
