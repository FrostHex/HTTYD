#ifndef SUNSET_BRIDGE_H
#define SUNSET_BRIDGE_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class SunsetBridge : public RefCounted {
	GDCLASS(SunsetBridge, RefCounted);

public:
	SunsetBridge() = default;
	~SunsetBridge() = default;

	double calc_sunrise(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone) const;
	double calc_sunset(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone) const;
	double calc_custom_sunrise(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone, double angle) const;
	double calc_custom_sunset(int year, int month, int day, double latitude_deg, double longitude_deg, double timezone, double angle) const;

protected:
	static void _bind_methods();

private:
	static double _sanitize_timezone(double timezone);
	static double _clamp_latitude(double latitude_deg);
	static double _clamp_longitude(double longitude_deg);
};

} // namespace godot

#endif // SUNSET_BRIDGE_H
