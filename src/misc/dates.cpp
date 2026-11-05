/**
 * Usage: Gregorian calendar, any year (also negative, proleptic)
 *        day_num(y, m, d) -> consecutive day number (difference = days
 *                            between dates)
 *        from_day(n)      -> {y, m, d}
 *        weekday(y, m, d) -> 0 = Monday .. 6 = Sunday
 *        leap(y), days_in(y, m)
 * Complexity: O(1)
 * Source: UPB Dates (Julian day number formulas with int and divisions
 *         that truncate: wrong before 4800 BC) + UPB Dates.java + UTP
 *         dates; floor-based civil-from-days (Howard Hinnant): own
 * Note: add k days: from_day(day_num(...) + k). Day number 0 = 1970-01-01
 *       (a Thursday). Julian calendar: other leap rule (y % 4 == 0).
 * Status: ok
 */
bool leap(ll y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }
int days_in(ll y, int m) {
    static const int D[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return D[m - 1] + (m == 2 && leap(y));
}

ll day_num(ll y, int m, int d) {
    y -= m <= 2;  // the year starts in March (leap day at the end)
    ll era = (y >= 0 ? y : y - 399) / 400, yoe = y - era * 400;
    ll doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    ll doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

array<ll, 3> from_day(ll z) {
    z += 719468;
    ll era = (z >= 0 ? z : z - 146096) / 146097, doe = z - era * 146097;
    ll yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    ll y = yoe + era * 400, doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    ll mp = (5 * doy + 2) / 153, d = doy - (153 * mp + 2) / 5 + 1;
    ll m = mp < 10 ? mp + 3 : mp - 9;
    return {y + (m <= 2), m, d};
}

int weekday(ll y, int m, int d) {
    ll z = day_num(y, m, d) + 3;  // 1970-01-01 was a Thursday (= 3)
    return (int)(((z % 7) + 7) % 7);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_dates() {
    cout << day_num(2026, 11, 5) - day_num(2026, 1, 1) << ' '  // 308 3
         << weekday(2026, 11, 5) << '\n';
    auto [y, m, d] = from_day(day_num(2024, 2, 28) + 2);  // leap year
    int feb = days_in(1900, 2);  // 1900 is not leap (divisible by 100)
    cout << y << ' ' << m << ' ' << d << ' ' << feb << '\n';  // 2024 3 1 28
}
