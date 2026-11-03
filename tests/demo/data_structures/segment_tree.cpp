#include "template/template.cpp"
#include "src/data_structures/segment_tree.cpp"

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
