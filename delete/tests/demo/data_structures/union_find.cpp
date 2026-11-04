#include "template/template.cpp"
#include "src/data_structures/union_find.cpp"

int main()
{
    UnionFind UF(5);                      // create 5 disjoint sets
    printf("%d\n", UF.numDisjointSets()); // 5
    UF.unionSet(0, 1);
    printf("%d\n", UF.numDisjointSets()); // 4
    UF.unionSet(2, 3);
    printf("%d\n", UF.numDisjointSets()); // 3
    UF.unionSet(4, 3);
    printf("%d\n", UF.numDisjointSets());                 // 2
    printf("isSameSet(0, 3) = %d\n", UF.isSameSet(0, 3)); // 0 (false)
    printf("isSameSet(4, 3) = %d\n", UF.isSameSet(4, 3)); // 1 (true)
    for (int i = 0; i < 5; ++i)                           // 1 for {0, 1} and 3 for {2, 3, 4}
        printf("findSet(%d) = %d, sizeOfSet(%d) = %d\n",
               i, UF.findSet(i), i, UF.sizeOfSet(i));
    UF.unionSet(0, 3);
    printf("%d\n", UF.numDisjointSets()); // 1
    for (int i = 0; i < 5; ++i)           // 3 for {0, 1, 2, 3, 4}
        printf("findSet(%d) = %d, sizeOfSet(%d) = %d\n",
               i, UF.findSet(i), i, UF.sizeOfSet(i));
    return 0;
}
