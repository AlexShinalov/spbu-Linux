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
 void createBuffer(size_t sizeBuffer) { 
  size = sizeBuffer; 
  array = mmap(nullptr,  size,  PROT_READ|PROT_WRITE,  MAP_PRIVATE|MAP_ANONYMOUS,  fileDir,  0); 
  clog << "Point: " << array << endl; 
 } 
  
 ByteBuffer() { 
  clog << "Create buffer" << endl; 
 } 
  
 ByteBuffer(const std::string &filePth, size_t len) { 
  const char *tmp = &filePth[0]; 
   
  fileDir = checkDir(open(tmp, O_CREAT|O_RDWR, 0644)); 
  clog << "Open" << filePth << endl; 
   
  createBuffer(len); 
 } 
  
 ~ByteBuffer() { 
  munmap(array, size); 
  checkDir(close(fileDir)); 
  clog << "Destroy" << '\n'; 
 } 
  
  
  
}; 
 
int main(void) { 
 size_t size = 4096*10; 
  
 ByteBuffer name("hello.txt", size); 
 }
  
