// Stress test for src/misc/dates.cpp
// Oracle: walk the calendar day by day (naive increment using its own
// month-length table with the Gregorian leap rule) from 1 Jan -2400 to
// 31 Dec 2800, checking day_num consecutive, from_day inverse and the
// weekday cycling from the known anchor 1970-01-01 = Thursday.
#include "tests/stress/template_lib.h"
#include "src/misc/dates.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_dates, "src/misc/dates.cpp", "example_dates");
    auto isleap = [](ll y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; };
    int ml[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    ll y = -2400, d = 1;
    int m = 1;
    ll n0 = day_num(y, m, (int)d), k = 0, anchor = day_num(1970, 1, 1);
    CHECK(anchor == 0 && weekday(1970, 1, 1) == 3, "anchor");
    ll last = stress::SLOW ? 300 : 2800;
    while (y <= last) {
        ll n = day_num(y, m, (int)d);
        CHECK(n == n0 + k, "consecutive");
        auto r = from_day(n);
        CHECK(r[0] == y && r[1] == m && r[2] == d, "inverse");
        CHECK(weekday(y, m, (int)d) == (int)((((n - anchor) % 7 + 7) % 7 + 3) % 7), "weekday");
        int len = ml[m - 1] + (m == 2 && isleap(y));
        CHECK(days_in(y, m) == len && leap(y) == isleap(y), "month length");
        if (++d > len) d = 1, m++;
        if (m > 12) m = 1, y++;
        k++;
    }
    puts("OK");
}
