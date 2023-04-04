#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/stat.h> 
 
#include <string> 
#include <iostream> 
#include <system_error> 
 
class ByteBuffer { 
private: 
 void *array; 
 size_t size; 
 long fileDir; 
  
 template <typename T> 
 T checkDir(T ret) { 
  if (ret == T(-1))  
   throw std::system_error(errno, std::generic_category()); 
  return ret; 
 } 
public: 
 void alloc(size_t len) { 
  size = len; 
  array = mmap(nullptr,  
    size,  
    PROT_READ|PROT_WRITE,  
    MAP_SHARED,  
    fileDir,  
    0); 
  std::clog << "Point: " << array << '\n'; 
 } 
  
 ByteBuffer() { 
  std::cout << "Create" << '\n'; 
 } 
  
 ByteBuffer(const std::string &filePth) { 
  const char *tmp = &filePth[0]; 
   
  struct stat status; 
  checkDir(stat(tmp, &status)); 
   
  fileDir = checkDir(open(tmp, O_CREAT|O_RDWR, 0644)); 
  std::cout << "Open " << filePth << "\n"; 
   
  size = status.st_size; 
   
  alloc(size); 
 } 
  
 ~ByteBuffer() { 
  if (fileDir == -1) 
   return; 
   
  checkDir(msync(array, size, MS_SYNC)); 
  checkDir(munmap(array, size)); 
  checkDir(close(fileDir)); 
  std::cout << "Destroy" << '\n'; 
 } 
  
 void read() { 
  char *chars = static_cast<char*>(array); 
  for (size_t i = 0; i < size; i++) { 
   chars[i] = std::toupper(chars[i]); 
   std::clog << "chars[" << i << "]: " << chars[i] << '\n'; 
  } 
 } 
   
  
private: 
 void move(ByteBuffer &buffer) { 
  fileDir = buffer.fileDir; 
  size = buffer.size; 
  array = buffer.array; 
  buffer.fileDir = -1; 
  buffer.size = 0; 
   
  std::cout << "Move:" << buffer.array << " to " << array << '\n';  
 } 
 
public: 
 ByteBuffer &operator=(ByteBuffer &buffer) { 
  move(buffer); 
  return *this; 
 } 
  
 ByteBuffer(ByteBuffer &buffer) { 
  move(buffer); 
 } 
  
 void resize(size_t newSize) { 
  array = mremap( array, 
    size, 
    newSize, 
    MREMAP_MAYMOVE); 
  size = newSize; 
  madvise(array, size, MADV_SEQUENTIAL); 
   
  std::cout << "new size: " << size << '\n'; 
 } 
  
}; 
 
int main(void) { 
 size_t size = 4096*10; 
  
 ByteBuffer name("hello.txt"); 
 name.read(); 
  
  
}
