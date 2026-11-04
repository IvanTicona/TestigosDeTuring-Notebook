#include "template/template.cpp"
#include "src/data_structures/fenwick_tree.cpp"

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
