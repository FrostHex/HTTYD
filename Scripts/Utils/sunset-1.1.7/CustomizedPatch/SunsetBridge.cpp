#include "SunsetBridge.h"

#include "sunset.h"

#include <algorithm>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SunsetBridge::_bind_methods() {
	ClassDB::bind_method(D_METHOD("calc_sunrise", "year", "month", "day", "latitude_deg", "longitude_deg", "timezone"), &SunsetBridge::calc_sunrise);
	ClassDB::bind_method(D_METHOD("calc_sunset", "year", "month", "day", "latitude_deg", "longitude_deg", "timezone"), &SunsetBridge::calc_sunset);
	ClassDB::bind_method(D_METHOD("calc_custom_sunrise", "year", "month", "day", "latitude_deg", "longitude_deg", "timezone", "angle"), &SunsetBridge::calc_custom_sunrise);
	ClassDB::bind_method(D_METHOD("calc_custom_sunset", "year", "month", "day", "latitude_deg", "longitude_deg", "timezone", "angle"), &SunsetBridge::calc_custom_sunset);
}

double SunsetBridge::_sanitize_timezone(double timezone) {
	if (timezone < -12.0 || timezone > 14.0) {
		return 0.0;
	}
	return timezone;
}

double SunsetBridge::_clamp_latitude(double latitude_deg) {
	return std::clamp(latitude_deg, -89.9999, 89.9999);
}

double SunsetBridge::_clamp_longitude(double longitude_deg) {
	return std::clamp(longitude_deg, -180.0, 180.0);
}

double SunsetBridge::calc_sunrise(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone) const {
	SunSet sun;
	sun.setPosition(_clamp_latitude(latitude_deg), _clamp_longitude(longitude_deg), _sanitize_timezone(timezone));
	sun.setCurrentDate(year, month, day);
	return sun.calcSunrise();
}

double SunsetBridge::calc_sunset(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone) const {
	SunSet sun;
	sun.setPosition(_clamp_latitude(latitude_deg), _clamp_longitude(longitude_deg), _sanitize_timezone(timezone));
	sun.setCurrentDate(year, month, day);
	return sun.calcSunset();
}

double SunsetBridge::calc_custom_sunrise(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone, double angle) const {
	SunSet sun;
	sun.setPosition(_clamp_latitude(latitude_deg), _clamp_longitude(longitude_deg), _sanitize_timezone(timezone));
	sun.setCurrentDate(year, month, day);
	return sun.calcCustomSunrise(angle);
}

double SunsetBridge::calc_custom_sunset(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone, double angle) const {
	SunSet sun;
	sun.setPosition(_clamp_latitude(latitude_deg), _clamp_longitude(longitude_deg), _sanitize_timezone(timezone));
	sun.setCurrentDate(year, month, day);
	return sun.calcCustomSunset(angle);
}
