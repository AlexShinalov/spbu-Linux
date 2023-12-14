#include <bits/stdc++.h>
using namespace std;

void func(int* X) {
        cout << *X << " # ";

        unsigned char* unit = reinterpret_cast<unsigned char*>(X);

        for (int i = 3; i >= 0; i--) cout << (unsigned int) * (unit + i) << " ";
        cout << '\n';
}

int main() {
        int x = INT_MAX;
        int* X = &x;.
        func(X);
        x = -x;
        func(X);
        return 0;
}
