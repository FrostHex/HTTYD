#include "Control_Top.h"

#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Control_Top::SyncSkyTime(Node *time_of_day)
{
    if (!time_of_day)
    {
        return;
    }

    Time *time_singleton = Time::get_singleton();
    if (!time_singleton)
    {
        UtilityFunctions::printerr("Control_Top: Time singleton is unavailable");
        return;
    }

    Dictionary datetime_dict = time_singleton->get_datetime_dict_from_system(false);
    if (time_of_day->has_method("set_from_datetime_dict"))
    {
        time_of_day->call("set_from_datetime_dict", datetime_dict);
    }
    else
    {
        time_of_day->set("year", datetime_dict["year"]);
        time_of_day->set("month", datetime_dict["month"]);
        time_of_day->set("day", datetime_dict["day"]);
        if (time_of_day->has_method("set_time"))
        {
            time_of_day->call("set_time", datetime_dict["hour"], datetime_dict["minute"], datetime_dict["second"]);
        }
    }
}
