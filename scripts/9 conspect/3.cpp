#include <iostream>

using namespace std;

bool checkL(char *str) {
	auto cnt = str;
	while (*cnt != '=') {
		if (*cnt == 'L')
		       	return 1;
		cnt++;
	}
	return 0;
}

int main() {
	extern char **environ;
	auto cnt = environ;

	while (*cnt) {
		if (checkL(*cnt))
			cout << *cnt << '\n';

		cnt++;
	}

	return 0;
}
