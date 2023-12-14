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
		void findFile(File &file) {
			filedir = file.filedir;
				file.filedir = -1;
		}
	public:
		File(File &file) {
			filedir = dup(file.filedir);
		}
		File(const string &path) {
			const char *buffer = &path[0];
			this->filedir = this->check(open(buffer, O_CREAT|O_RDWR, 0644));
			clog << "Open"<<endl;
		}
		~File() {
			this->check(close(this->filedir));
		}
		File& operator=(File &file) {
            filedir = dup2(file.filedir, filedir);
			findFile(file);
			return *this;
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
		int getPlace() {
			if (filedir == -1) return -1;
			return check(lseek(filedir, 0, SEEK_CUR));
		}

		void movePlace(int bias) {
			if (filedir == -1) return;
			check(lseek(filedir, bias, SEEK_SET));
		}		
};

void function() {
	File name("file.txt");
	name.writeFile("Vitya\n");
	cout<< name.getPlace() << endl;
	name.movePlace(0);
	cout << name.getPlace() << endl;
}

int main() {
	function();
	return 0;
}
