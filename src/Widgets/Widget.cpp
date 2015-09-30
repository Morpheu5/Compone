#include "Widgets/Widget.h"

Widget::Widget(vec2 position, float angle) :
_position(position), _angle(angle) {

}

Widget::Widget() : Widget(vec2(0.0, 0.0), 0.0) {}

Widget::~Widget() {

}

void Widget::draw() {
	gl::drawSolidRect(Rectf(-50, -25, 50, 25));
	gl::color(1,0,0);
	gl::drawSolidCircle(hitPoint, 10);
	gl::color(1,1,1);
}

bool Widget::hit(vec2 position) {
//	console() << position << endl;
	hitPoint = position;

	return true;
}