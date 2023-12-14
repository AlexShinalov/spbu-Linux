#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h> 
 
#include <string> 
#include <iostream> 
 using namespace std;
class ByteBuffer { 
private: 
 void *array; 
 size_t size; 
 long fileDir; 
  
 template <typename T> 
 T checkDir(T ret) { 
  if (ret == T(-1))  
   throw system_error(errno, std::generic_category()); 
  return ret; 
 } 
public: 
 void createBuf(size_t s) { 
  size = s; 
  array = mmap(nullptr,  size,  PROT_READ|PROT_WRITE,  MAP_PRIVATE|MAP_ANONYMOUS, fileDir,  0); 
  clog << "Point: " << array << '\n'; 
 } 
  
 ByteBuffer() { 
  cout << "Create" << endl; 
 } 
  
 ByteBuffer(const std::string &filePth, size_t len) { 
  const char *tmp = &filePth[0]; 
   
  fileDir = checkDir(open(tmp, O_CREAT|O_RDWR, 0644)); 
  cout << "Open" << filePth << endl; 
   
  createBuf(len); 
 } 
  
 ~ByteBuffer() { 
  if (fileDir == -1) 
   return; 
  
  munmap(array, size); 
  checkDir(close(fileDir)); 
  cout << "Destroy" << endl; 
 } 
  
private: 
 void splitBuffer(ByteBuffer &buffer) { 
  fileDir = buffer.fileDir; 
  size = buffer.size; 
  array = buffer.array; 
  buffer.fileDir = -1; 
  buffer.size = 0; 
   
  cout << "Move buffer" << buffer.array << " in " << array <<endl;  
 } 
public:
 ByteBuffer &operator=(ByteBuffer &buffer) { 
  splitBuffer(buffer); 
  return *this; 
 } 
  
 ByteBuffer(ByteBuffer &buffer) { 
  splitBuffer(buffer); 
 } 
  
}; 
 
int main(void) { 
 size_t size = 4096*10; 
  
 ByteBuffer name1("hello.txt", size); 
 ByteBuffer name2; 
  
 name2 = name1; 
 ByteBuffer name3(name2);
 
  
 }  
