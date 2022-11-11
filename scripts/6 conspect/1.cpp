#include <iostream>

using namespace std;

int main() {
    #if defined(USE_GPU) && defined(USE_FLOAT)
    cout << "USE_GPU USE_FLOAT" << endl;
    #elif defined(USE_GPU) && !defined(USE_FLOAT)
    cout << "USE_GPU" << endl;
    #elif !defined(USE_GPU) && defined(USE_FLOAT)
    cout << "USE_FLOAT" << endl;
    #elif !defined(USE_GPU) && !defined(USE_FLOAT)
    cout << "None" << endl;
    #endif
}
