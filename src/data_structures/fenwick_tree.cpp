/**
 * Usage: FenwickTree (rsq/update/select), RUPQ (update de rango, consulta puntual), RURQ (update y suma de rango); indices desde 1
 * Complexity: O(log n) por operacion; build O(n); select O(log^2 n)
 * Source: Competitive Programming 4 (segun comentarios del codigo original)
 * Status: untested
 */
#define LSOne(S) ((S) & -(S))
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
