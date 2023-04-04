#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>

using namespace std;

class MyFile {
	private:
		int fd = -1;

		template <typename T>
		T check(T res) {
			if (res == T(-1)) {
				throw system_error(errno, generic_category());
			}
			return res;
		}

		void captureFile(MyFile &file) {
			fd = file.fd;
			file.fd = -1;
			cout << "Capture file\n";
		}

	public:
		MyFile(MyFile &file) {
			captureFile(file);
		}

		MyFile(const string &path) {
			const char *buf = &path[0];
			fd = check(open(buf, O_CREAT|O_RDWR, 0644));
			cout << "Open file\n";
		}

		~MyFile() {
			if (fd == -1) return;
			check(close(fd));
			cout << "Close file\n";
		}

		MyFile& operator=(MyFile &file) {
			captureFile(file);
			return *this;
		}

		void writeFile(const string &txt) {
			if (fd == -1) return;
			const char *buf = &txt[0];
			check(write(fd, buf, sizeof(txt)));
		}

		string readFile(int size) {
			if (fd == -1) return "";
			char buf[size+1];
			check(read(fd, buf, size));
			buf[size] = '\0';
			return (string)buf;
		}

		int getPos() {
			if (fd == -1) return -1;
			return check(lseek(fd, 0, SEEK_CUR));
		}

		void movePos(int bias) {
			if (fd == -1) return;
			check(lseek(fd, bias, SEEK_SET));
		}
};

void f() {
	MyFile f1("file.txt");
	f1.writeFile("hello");
	cout << "Write 'aboba', current position = " << f1.getPos() << '\n';
	f1.movePos(0);
	cout << "Current position = " << f1.getPos() << ", text = " << f1.readFile(5) << '\n';
}

int main() {
	f();
	return 0;
}

