#include <bits/stdc++.h>
using namespace std;
#define LSOne(S) ((S) & -(S))
typedef long long ll;
typedef vector<ll> vll;
typedef vector<int> vi;
class FenwickTree
{
private:
    vll ft;

public:
    FenwickTree(int m) { ft.assign(m + 1, 0); }
    void build(const vll &f)
    {
        int m = (int)f.size() - 1;
        ft.assign(m + 1, 0);
        for (int i = 1; i <= m; ++i)
        {
            ft[i] += f[i];
            if (i + LSOne(i) <= m)
                ft[i + LSOne(i)] += ft[i];
        }
    }
    FenwickTree(const vll &f) { build(f); }
    FenwickTree(int m, const vi &s)
    {
        vll f(m + 1, 0);
        for (int i = 0; i < (int)s.size(); ++i)
            ++f[s[i]];
        build(f);
    }
    ll rsq(int j)
    {
        ll sum = 0;
        for (; j; j -= LSOne(j))
            sum += ft[j];
        return sum;
    }
    ll rsq(int i, int j) { return rsq(j) - rsq(i - 1); } // inc/exclusion
    // updates value of the i-th element by v (v can be +ve/inc or-ve/dec)
    void update(int i, ll v)
    {
        for (; i < (int)ft.size(); i += LSOne(i))
            ft[i] += v;
    }
    int select(ll k)
    {
        int lo = 1, hi = ft.size() - 1;
        for (int i = 0; i < 30; ++i)
        {
            int mid = (lo + hi) / 2;
            // O(log^2 m)
            // 2^30 > 10^9; usually ok
            // BSTA
            (rsq(1, mid) < k) ? lo = mid : hi = mid; // See Section 3.3.1
        }
        return hi;
    }
};
class RUPQ
{
private:
    FenwickTree ft;

public:
    RUPQ(int m) : ft(FenwickTree(m)) {}
    void range_update(int ui, int uj, int v)
    {
        ft.update(ui, v);
        ft.update(uj + 1, -v);
    } //
    ll point_query(int i) { return ft.rsq(i); } 
};
class RURQ
{
private:
    RUPQ rupq;
    FenwickTree purq;

public:
    // RURQ variant
    // needs two helper FTs
    // one RUPQ and
    // one PURQ
    RURQ(int m) : rupq(RUPQ(m)), purq(FenwickTree(m)) {} // initialization
    void range_update(int ui, int uj, int v)
    {
        rupq.range_update(ui, uj, v);
        // [ui, ui+1, .., uj] +v
        purq.update(ui, v * (ui - 1));
        purq.update(uj + 1, -v * uj);
    }
    ll rsq(int j)
    {
        return rupq.point_query(j) * j -
               purq.rsq(j);
    }
    //-(ui-1)*v before ui
    // +(uj-ui+1)*v after uj
    // initial calculation
    // cancelation factor
    ll rsq(int i, int j) { return rsq(j) - rsq(i - 1); } // standard
};

int main()
{
    vll f = {0, 0, 1, 0, 1, 2, 3, 2, 1, 1, 0};
    // index 0 is always 0
    FenwickTree ft(f);
    printf("%lld\n", ft.rsq(1, 6));  // 7 => ft[6]+ft[4] = 5+2 = 7
    printf("%d\n", ft.select(7));    // index 6, rsq(1, 6) == 7, which is >= 7
    ft.update(5, 1);                 // update demo
    printf("%lld\n", ft.rsq(1, 10)); // now 12
    printf("=====\n");
    RUPQ rupq(10);
    RURQ rurq(10);
    rupq.range_update(2, 9, 7); // indices in [2, 3, .., 9] updated by +7
    rurq.range_update(2, 9, 7); // same as rupq above
    rupq.range_update(6, 7, 3); // indices 6&7 are further updated by +3 (10)
    rurq.range_update(6, 7, 3); // same as rupq above
    // idx = 0 (unused) | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10
    // val =- | 0 | 7 | 7 | 7 | 7 | 10 | 10 | 7 | 7 | 0 for (int i = 1; i <= 10; i++) printf("%d-> %lld\n", i, rupq.point_query(i));
    printf("RSQ(1, 10) = %lld\n", rurq.rsq(1, 10)); // 62
    printf("RSQ(6, 7) = %lld\n", rurq.rsq(6, 7));   // 20
    return 0;
}