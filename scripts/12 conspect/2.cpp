#include <bits/stdc++.h>
using namespace std;

int main() {
        int a = 5;
        char b = 'f';
        bool c = true;
        void* A = &a;
        void* B = &b;
        void* C = &c;
        cout << "address a: " << A << "\naddress b: " << B << "\naddress c: " << C << "\n\n";
        uintptr_t addressA = reinterpret_cast<uintptr_t>(A);
        uintptr_t addressB = reinterpret_cast<uintptr_t>(B);
        uintptr_t addressC = reinterpret_cast<uintptr_t>(C);
        cout << "address a: " << addressA << "\naddress b: " << addressB << "\naddress c: " << addressC << "\n\n";
        addressB -= addressA;
        addressC -= addressA;
        addressA -= addressA;
        cout << "address a: " << addressA << "\naddress b: " << addressB << "\naddress c: " << addressC << '\n';
        return 0;
}
