# Copyright (c) 2026 HTTYD Contributors

@tool
class_name CustomizedPatch
extends RefCounted

const MINUTES_PER_DAY: float = 1440.0
const HOURS_PER_DAY: float = 24.0
const HALFPI: float = PI * 0.5
const SUNSET_BRIDGE_CLASS: StringName = &"SunsetBridge"
const METHOD_CALC_SUNRISE: StringName = &"calc_sunrise"
const METHOD_CALC_SUNSET: StringName = &"calc_sunset"

static var _sunset_bridge: Object = null

static func calcSunrise(year: int, month: int, day: int, latitude_deg: float, longitude_deg: float, timezone: float) -> float:
	return _call_bridge(
		METHOD_CALC_SUNRISE,
		[year, month, day, latitude_deg, longitude_deg, timezone]
	)


static func calcSunset(year: int, month: int, day: int, latitude_deg: float, longitude_deg: float, timezone: float) -> float:
	return _call_bridge(
		METHOD_CALC_SUNSET,
		[year, month, day, latitude_deg, longitude_deg, timezone]
	)


static func compute_sun_altitude_from_sunrise_sunset(current_time: float, year: int, month: int, day: int, latitude_deg: float, longitude_deg: float, timezone: float, fallback_altitude: float) -> float:
	var sunrise_minutes: float = calcSunrise(year, month, day, latitude_deg, longitude_deg, timezone)
	var sunset_minutes: float = calcSunset(year, month, day, latitude_deg, longitude_deg, timezone)

	if _is_invalid_number(sunrise_minutes) or _is_invalid_number(sunset_minutes):
		return fallback_altitude

	sunrise_minutes = _normalize_minutes(sunrise_minutes)
	sunset_minutes = _normalize_minutes(sunset_minutes)
	if sunrise_minutes >= sunset_minutes:
		return fallback_altitude

	var sunrise_hour: float = sunrise_minutes / 60.0
	var sunset_hour: float = sunset_minutes / 60.0
	return _map_time_to_altitude(current_time, sunrise_hour, sunset_hour)


static func _map_time_to_altitude(current_time: float, sunrise_hour: float, sunset_hour: float) -> float:
	var t: float = fposmod(current_time, HOURS_PER_DAY)
	var day_duration: float = sunset_hour - sunrise_hour
	if day_duration <= 0.0:
		return HALFPI

	if t >= sunrise_hour and t <= sunset_hour:
		var day_phase: float = (t - sunrise_hour) / day_duration
		return lerpf(HALFPI, -HALFPI, clampf(day_phase, 0.0, 1.0))

	if t > sunset_hour:
		var night_after_sunset: float = HOURS_PER_DAY - sunset_hour
		if night_after_sunset <= 0.0:
			return -PI
		var evening_phase: float = (t - sunset_hour) / night_after_sunset
		return lerpf(-HALFPI, -PI, clampf(evening_phase, 0.0, 1.0))

	if sunrise_hour <= 0.0:
		return HALFPI
	var morning_phase: float = t / sunrise_hour
	return lerpf(PI, HALFPI, clampf(morning_phase, 0.0, 1.0))


static func _normalize_minutes(value: float) -> float:
	return fposmod(value, MINUTES_PER_DAY)


static func _is_invalid_number(value: float) -> bool:
	return value != value or absf(value) > 1e20

static func _get_bridge() -> Object:
	if _sunset_bridge and is_instance_valid(_sunset_bridge):
		return _sunset_bridge
	if not ClassDB.class_exists(SUNSET_BRIDGE_CLASS):
		return null
	_sunset_bridge = ClassDB.instantiate(SUNSET_BRIDGE_CLASS)
	return _sunset_bridge


static func _call_bridge(method_name: StringName, args: Array) -> float:
	var bridge: Object = _get_bridge()
	if bridge == null or not bridge.has_method(method_name):
		return NAN
	var result: Variant = bridge.callv(method_name, args)
	if result is float or result is int:
		return float(result)
	return NAN
