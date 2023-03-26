#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cstdio>
#include <sys/mman.h>
#include <string.h>
#include <vector>
using namespace std;

void delenie(char* data_map, int* answer_map,int file_size) {
    int startline_number, inline_number;
    std::vector<int>inside_string;
    std::vector<vector<int>>all_numbers;
    string str;
    for (int i = 0; i < file_size; i++) {
        if (data_map[i] != ' ' && data_map[i] != '\n' && data_map[i] != '\0') {
            str += data_map[i];
        }
        if (data_map[i] == ' ') {
            int a = std::stoi(str);
            inside_string.push_back(a);
            str.clear();
        }
        if (data_map[i] == '\n') {
            int a = std::stoi(str);
            inside_string.push_back(a);
            all_numbers.push_back(inside_string);
            inside_string.clear();
            str.clear();
        }
       
    }
    vector<int>resulting_vector;
    int err = 0;
   for (int i = 0; i < all_numbers.size(); i++) {
       int result = all_numbers[i][0];
       for (int j = 0; j < all_numbers[i].size()-1; j++) {
           if (all_numbers[i][j + 1] != 0) {
               result = result / all_numbers[i][j + 1];
          }
           else {
               err += 1;
               break;
           }
        }
 
       if (err < 1) {
           if (all_numbers[i].size() - 1 == 0) {
               resulting_vector.push_back(result);
           }
           else {
               resulting_vector.push_back(result);
           }
       }
       if (err != 0) {
           break;
       }
    }
   answer_map[0] = resulting_vector.size();
   for (int i = 0; i < resulting_vector.size(); i++) {
       answer_map[i+1] = resulting_vector[i];
   }
   
}

int main(int argc, char* argv[]) {
    int file_descriptor;
    int status;
    int tmp;
    char file_path[100];
    cout << "Input filename\n";
    cin >> file_path;
    struct stat file_struct;
    long long file_size;
    file_descriptor = open(file_path, O_RDONLY);
    if (file_descriptor == -1) {
        std::cout << "Error file didn't open\n";
    }
    else
    {
        fstat(file_descriptor, &file_struct);
        file_size = file_struct.st_size;
    }
    char* buf = new char[file_size];
    read(file_descriptor,buf,file_size);
    int shared_mem_descriptor;
    shared_mem_descriptor = shm_open("a.back", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shared_mem_descriptor, file_size);
    char* data_map = static_cast<char*>(mmap(NULL, file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_mem_descriptor, 0));
    int counter=0; 
    int shared_mem_result = shm_open("result", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    memcpy(data_map, buf, file_size);
    msync(data_map, file_size, MS_SYNC);
    for (int i = 0; i < file_size; i++) {
        if (data_map[i] == ' ' || data_map[i] == '\n') {
            counter += 1; 
        }
    }
    ftruncate(shared_mem_result, (file_size-counter+1) * sizeof(int));
    int* result_map = static_cast<int*>(mmap(NULL, (file_size-counter+1) * sizeof(int), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_mem_result, 0));
    close(shared_mem_descriptor);
    int process_id = fork();

    if (process_id == 0) {
        
        delenie(data_map, result_map,file_size);
    }

    if (process_id > 0) {
        pid_t ended_process_id = wait(&status);
        if (ended_process_id == process_id) {
            close(shared_mem_descriptor);
        }
        if (WIFEXITED(status) != 0) {
            for (int i = 0; i < result_map[0]; i++) {
               std::cout << result_map[i+1] << '\n';
            }
        }
        munmap(data_map, file_size);
        munmap(result_map, (file_size -counter+1) * sizeof(int));
        shm_unlink("result");
       shm_unlink("a.back");
    }

    if (process_id == -1) {
        perror("Error: ");
    }

    return 0;
}
