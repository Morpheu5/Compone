#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/app/TouchEvent.h"

#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <objc/message.h>

#include "Widgets/Widget.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ComponeApp : public App {
	CameraOrtho _cam;

	float _w, _h;
	float _zoom;
	vec2 _pan;

	mutex _widgetsMtx;
	list<shared_ptr<Widget>> _widgets;

	mutex _touchesMtx;
	map<int, TouchEvent::Touch> _touches;

public:
	void setup() override;
	void update() override;
	void draw() override;

	void touchesBegan(TouchEvent event) override;
	void touchesMoved(TouchEvent event) override;
	void touchesEnded(TouchEvent event) override;
};

void ComponeApp::setup() {
	_zoom = 1.0;
	_w = getWindowHeight()/(2.0f * _zoom);
	_h = getWindowWidth()/(2.0f * _zoom);
	_pan = vec2(0,0);

	_cam = CameraOrtho(-_w, _w, -_h, _h, 0, 2);
	_cam.lookAt(vec3(0,0,1), vec3(0));

	_widgets.push_back(shared_ptr<Widget>(new Widget));
	_widgets.push_back(shared_ptr<Widget>(new Widget(vec2(100, 100), 0)));
	_widgets.push_back(shared_ptr<Widget>(new Widget(vec2(100, 200), glm::radians(-45.0))));
}

void ComponeApp::update() {
	_cam.lookAt(vec3(_pan, 1), vec3(_pan, 0));
	_cam.setOrtho(-_w/_zoom, _w/_zoom, -_h/_zoom, _h/_zoom, 0, 2);
}

void ComponeApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::color(Color::white());

	gl::pushMatrices();
	gl::setMatrices(_cam);

	// Draw world (0,0) crosshair
	gl::color(0.6, 0.4, 0.0);
	gl::drawLine(vec3(0, -_h, 0), vec3(0, _h, 0));
	gl::drawLine(vec3(-_w, 0, 0), vec3(_w, 0, 0));

	_widgetsMtx.lock();
	int c = 1;
	for(auto w : _widgets) {
		gl::color(Color(ColorModel::CM_HSV, vec3(c*0.25, 1.0, 1.0)));
		gl::pushMatrices();
		auto t = glm::translate(vec3(w->position(), 0.0)) * glm::rotate(w->angle(), vec3(0,0,1));
		gl::setModelMatrix(t);
		w->draw();
		gl::popMatrices();
		c++;
	}
	_widgetsMtx.unlock();

	gl::color(0,0.5,1);
	_touchesMtx.lock();
	for(auto t : _touches) {
//		mat4 m = glm::translate(_cam.getPivotPoint());
		vec2 p(_pan*(1.0f/_zoom)*2.0f - vec2(_w, -_h) + vec2(_cam.getViewMatrix() * vec4(vec2(1,-1)*t.second.getPos(), 1, 1)));
		gl::drawSolidCircle(p, 10.0);
		console() << p << endl;
	}
	_touchesMtx.unlock();

	gl::popMatrices();
	// Draw window crosshair
	gl::color(1.0, 1.0, 0.0);
	gl::drawLine(vec2(getWindowWidth()/2, 0),  vec2(getWindowWidth()/2, getWindowHeight()));
	gl::drawLine(vec2(0, getWindowHeight()/2), vec2(getWindowWidth(), getWindowHeight()/2));
}

void ComponeApp::touchesBegan(cinder::app::TouchEvent event) {
	_touchesMtx.lock();
	for(auto &touch : event.getTouches()) {
		auto nt = touch;
		_touches[touch.getId()] = touch;
	}
	_touchesMtx.unlock();
}

void ComponeApp::touchesMoved(cinder::app::TouchEvent event) {
	_pan += vec2(-1,1)*(event.getTouches()[0].getPos() - event.getTouches()[0].getPrevPos());
	_touchesMtx.lock();
	for(auto &touch : event.getTouches()) {
		_touches[touch.getId()] = touch;
	}
	_touchesMtx.unlock();
}

void ComponeApp::touchesEnded(cinder::app::TouchEvent event) {
	_touchesMtx.lock();
	for(auto &touch : event.getTouches()) {
		_touches.erase(touch.getId());
	}
	_touchesMtx.unlock();
}

CINDER_APP(ComponeApp, RendererGl(RendererGl::Options().msaa(0)),
		   [](App::Settings *settings) {
			   settings->setHighDensityDisplayEnabled(true);
			   settings->setMultiTouchEnabled(true);
		   });