#include "CronSchedule.h"

using namespace std::chrono;
using namespace date;

namespace libcron
{

    std::chrono::system_clock::time_point
    CronSchedule::calculate_from(const std::chrono::system_clock::time_point& from)
    {
        //auto time_part = from - date::floor<days>(from);
        auto curr = from;// - time_part;
        //auto dt = to_calendar_time(curr);

        bool done = false;

        while (!done)
        {
            year_month_day ymd = date::floor<days>(curr);

            // Add months until one of the allowed days are found, or stay at the current one.
            if (data.get_months().find(static_cast<Months>(unsigned(ymd.month()))) == data.get_months().end())
            {
                auto next_month = ymd + months{1};
                sys_days s = next_month.year() / next_month.month() / 1;
                curr = s;
                continue;
            }
                // If all days are allowed, then the 'day of week' takes precedence, which also means that
                // day of week only is ignored when specific days of months are specified.
            else if (data.get_day_of_month().size() != CronData::value_of(DayOfMonth::Last))
            {
                // Add days until one of the allowed days are found, or stay at the current one.
                if (data.get_day_of_month().find(static_cast<DayOfMonth>(unsigned(ymd.day()))) ==
                    data.get_day_of_month().end())
                {
                    sys_days s = ymd;
                    curr = s;
                    curr += days{1};
                    continue;
                }
            }
            else
            {
                //Add days until the current weekday is one of the allowed weekdays
                year_month_weekday ymw = date::floor<days>(curr);

                if (data.get_day_of_week().find(static_cast<DayOfWeek>(unsigned(ymw.weekday()))) ==
                    data.get_day_of_week().end())
                {
                    sys_days s = ymd;
                    curr = s;
                    curr += days{1};
                    continue;
                }
            }

            //curr += time_part;
            auto date_time = to_calendar_time(curr);
            if (data.get_hours().find(static_cast<Hours>(date_time.hour)) == data.get_hours().end())
            {
                curr += hours{1};
                curr -= minutes{date_time.min};
                curr -= seconds{date_time.sec};
            }
            else if (data.get_minutes().find(static_cast<Minutes >(date_time.min)) == data.get_minutes().end())
            {
                curr += minutes{1};
                curr -= seconds{date_time.sec};
            }
            else if (data.get_seconds().find(static_cast<Seconds>(date_time.sec)) == data.get_seconds().end())
            {
                curr += seconds{1};
            }
            else
            {
                done = true;
            }
        }

        return curr;
    }
}