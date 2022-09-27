/*
    compile: g++ -o 1082414_prog1 1082414_prog1.cpp -lrt
    execute: ./1082414_prog1
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

void error_and_die(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int cof(int n){
	if(n%2==0){
		return n/2;
	}else{
		return 3*n+1;
	}
}

int main(int argc, char *argv[]) {
  
	const char *name = "test";
	const size_t region_size = 16;

	int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	if (fd == -1)
	  error_and_die("shm_open");

	int r = ftruncate(fd, region_size);
	if (r != 0)
	  error_and_die("ftruncate");

	int *ptr =(int*)mmap(0, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
	  error_and_die("mmap");
	  
	close(fd);
	
	std::cin >> ptr[0];//用來存考拉茲猜想的值
	ptr[1] = 0;//用來存第幾次執行考拉茲猜想
	ptr[2] = 0;//用來存序列中最大的數
	ptr[3] = 0;//用來存最大的數發生在第幾次

	while(ptr[0]!=1){
	  	pid_t pid = fork();

	  	if (pid == 0) {
				ptr[0] = cof(ptr[0]);
				ptr[1]++;
				if(ptr[0]>ptr[2]){
					ptr[2] = ptr[0];
					ptr[3] = ptr[1];
				}
				std::cout << "["<<getpid()<<" Child]  : "<<ptr[0] << std::endl;
	    	exit(0);
	  	}
	  	else {
	    		int status;
	    		waitpid(pid, &status, 0);
			if(ptr[0] == 1){
				std::cout << "["<<getpid()<<" Parent] : "<<ptr[0] << std::endl;
				std::cout << "Max : " << ptr[2] << std::endl;
				std::cout << "Order : " << ptr[3] << std::endl;
				break;
			}
			ptr[0] = cof(ptr[0]);
			ptr[1]++;
			if(ptr[0]>ptr[2]){
				ptr[2] = ptr[0];
				ptr[3] = ptr[1];
			}
			if(ptr[0] == 1){
				std::cout << "["<<getpid()<<" Parent] : "<<ptr[0] << std::endl;
				std::cout << "Max : " << ptr[2] << std::endl;
				std::cout << "Order : " << ptr[3] << std::endl;
				break;
			}	
			std::cout << "["<<getpid()<<" Parent] : "<<ptr[0] << std::endl;
	  	}
	}

 	r = munmap(ptr, region_size);
  	if (r != 0)error_and_die("munmap");
 	r = shm_unlink(name);
 	if (r != 0)error_and_die("shm_unlink");
	
  return 0;
}
