#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>

using namespace std;

class File {
	private:
		int filedir;
		
		long check(long result) {
			if (result == long(-1)) {
				throw system_error(errno, generic_category());
			}
			return result;
		}
	public:
		File(const string &path) {
			const char *buffer = &path[0];
			this->filedir = this->check(open(buffer, O_CREAT|O_RDWR, 0644));
			clog << "Open"<<endl;
		}
		~File() {
			this->check(close(this->filedir));
		}
		void writeFile(const string &txt) {
			const char *buf = &txt[0];
			this->check(write(this->filedir, buf, sizeof(txt)));
		}
		string readFile(int size) {
			char buf[size+1];
			this->check(read(this->filedir, buf, size));
			buf[size] = '\0';
			return (string)buf;
		}
};

void function() {
	File name("file.txt");
	cout << name.readFile(5) << endl;
	name.writeFile("Vitya\n");
}

int main() {
	function();
	return 0;
}
