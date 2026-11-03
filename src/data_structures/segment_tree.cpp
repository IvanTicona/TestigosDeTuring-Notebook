#include <bits/stdc++.h>
using namespace std;
typedef vector<int> vi;
class SegmentTree
{
private:
    int n;
    vi A, st, lazy;
    int l(int p) { return p << 1; }
    int r(int p) { return (p << 1) + 1; }
    int conquer(int a, int b)
    {
        if (a == -1)
            return b;
        if (b == -1)
            return a;
        return min(a, b);
    }
    void build(int p, int L, int R)
    {
        if (L == R)
            st[p] = A[L];
        else
        {
            int m = (L + R) / 2;
            build(l(p), L, m);
            build(r(p), m + 1, R);
            st[p] = conquer(st[l(p)], st[r(p)]);
        }
    }
    void propagate(int p, int L, int R)
    {
        if (lazy[p] != -1)
        {
            st[p] = lazy[p];
            if (L != R)
                lazy[l(p)] = lazy[r(p)] = lazy[p];
            else
                A[L] = lazy[p];
            lazy[p] = -1;
        }
    }
    int RMQ(int p, int L, int R, int i, int j)
    {
        propagate(p, L, R);
        if (i > j)
            return -1;
        if ((L >= i) && (R <= j))
            return st[p];
        int m = (L + R) / 2;
        return conquer(RMQ(l(p), L, m, i, min(m, j)),
                       RMQ(r(p), m + 1, R, max(i, m + 1), j));
    }

    void update(int p, int L, int R, int i, int j, int val)
    {
        propagate(p, L, R);
        if (i > j)
            return;
        if ((L >= i) && (R <= j))
        {
            lazy[p] = val;
            propagate(p, L, R);
        }
        else
        {
            int m = (L + R) / 2;
            update(l(p), L, m, i, min(m, j), val);
            update(r(p), m + 1, R, max(i, m + 1), j, val);
            int lsubtree = (lazy[l(p)] != -1) ? lazy[l(p)] : st[l(p)];
            int rsubtree = (lazy[r(p)] != -1) ? lazy[r(p)] : st[r(p)];
            st[p] = (lsubtree <= rsubtree) ? st[l(p)] : st[r(p)];
        }
    }

public:
    SegmentTree(int sz) : n(sz), st(4 * n), lazy(4 * n, -1) {}
    SegmentTree(const vi &initialA) : SegmentTree((int)initialA.size())
    {
        A = initialA;
        build(1, 0, n - 1);
    }
    void update(int i, int j, int val) { update(1, 0, n - 1, i, j, val); }
    int RMQ(int i, int j) { return RMQ(1, 0, n - 1, i, j); }
};
int main()
{
    vi A = {18, 17, 13, 19, 15, 11, 20, 99};
    SegmentTree st(A);

    cout << "0, 1, 2, 3, 4, 5, 6, 7\n";
    cout << "A is {18,17,13,19,15,11,20,99}\n";

    cout << "RMQ(1, 3) = " << st.RMQ(1, 3) << "\n"; // 13
    cout << "RMQ(4, 7) = " << st.RMQ(4, 7) << "\n";
    cout << "RMQ(3, 4) = " << st.RMQ(3, 4) << "\n";

    st.update(5, 5, 77);
    cout << "0, 1, 2, 3, 4, 5, 6, 7\n";
    cout << "Now, modify A into {18,17,13,19,15,77,20,99}\n";
    cout << "RMQ(1, 3) = " << st.RMQ(1, 3) << "\n"; // remains 13
    cout << "RMQ(4, 7) = " << st.RMQ(4, 7) << "\n";
    cout << "RMQ(3, 4) = " << st.RMQ(3, 4) << "\n"; // now 15

    st.update(0, 3, 30); // update A[0..3] to 30
    cout << "0, 1, 2, 3, 4, 5, 6, 7\n";
    cout << "Now, modify A into {30,30,30,30,15,77,20,99}\n";
    cout << "RMQ(1, 3) = " << st.RMQ(1, 3) << "\n"; // now 30
    cout << "RMQ(4, 7) = " << st.RMQ(4, 7) << "\n";
    cout << "RMQ(3, 4) = " << st.RMQ(3, 4) << "\n"; // remains 15

    st.update(3, 3, 7); // update A[3] to 7
    cout << "0, 1, 2, 3, 4, 5, 6, 7\n";
    cout << "Now, modify A into {30,30,30,7,15,77,20,99}\n";
    cout << "RMQ(1, 3) = " << st.RMQ(1, 3) << "\n"; // now 7
    cout << "RMQ(4, 7) = " << st.RMQ(4, 7) << "\n";
    cout << "RMQ(3, 4) = " << st.RMQ(3, 4) << "\n";

    return 0;
}