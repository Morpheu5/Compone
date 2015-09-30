#pragma once

#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class Widget {
	vec2 _position;
	float _angle;

public:
	vec2 hitPoint;
	
	Widget();
	Widget(vec2 position, float angle);
	virtual ~Widget();
	
	virtual vec2 &position() { return _position; }
	virtual float angle() { return _angle; }

	virtual void draw();

	virtual bool hit(vec2 position);
};
