#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstdio>
#include <pthread.h>
#include <iostream>
#include <thread>
#include <vector>
void mythread(std::vector<std::vector<int>>& polynoms, int first, int last, std::vector<std::vector<int>>& results,int j) {
    std::vector<int> multiplying_result(polynoms[first].size());
    int counter = 0;
    while (first < last) {
        std::vector<int> multiplier(multiplying_result.size());
        if (counter == 0) {
            multiplier = polynoms[first];
        }
        else {
            multiplier = multiplying_result;
        }
        for (int i = 0; i < multiplying_result.size(); i++) {
            multiplying_result[i] = 0;
        }
        if (multiplying_result.size() < multiplying_result.size() + polynoms[first + 1].size()) {
            multiplying_result.resize(multiplying_result.size() + polynoms[first + 1].size() - 1, 0);
        }
        for (int i = 0; i < multiplier.size(); i++) {
            for (int k = 0; k < polynoms[first + 1].size(); k++) {
                multiplying_result[i + k] += multiplier[i] * polynoms[first + 1][k];
            }
        }
        first = first + 1;
        counter = counter + 1;
    }
    if (counter != 0) {
        results[j] = multiplying_result;
    }
    else {
        results[j] = polynoms[first];
    }
}
void print_polynom(std::vector<std::vector<int>>a) {
    for (int k = 0; k < a.size(); k++) {
        for (int l = 0; l < a[k].size(); l++) {
            std::cout << a[k][l] << "x^" << l;
            if (l != a[k].size() - 1) {
                std::cout << " + ";
            }
        }
        std::cout << "\n";
    }
}
int main(int argc, char* argv[]) {
    int number_of_threads;
    number_of_threads = atoi(argv[1]);
    std::cout << "number of threads\n";
    std::cout << number_of_threads<<"\n";
    int number_of_polynoms;
    int element_degree, element_coefficent, degree_of_polynom;
    std::cout << "input number of polynoms\n";
    std::cin >> number_of_polynoms;
    std::vector<std::vector<int>> polynoms;

    for (int i = 0; i < number_of_polynoms; i++) {
        std::cout << "input max stepen of polynom " << i+1 << "\n";
        std::cin >> degree_of_polynom;
        std::vector<int> polynom(degree_of_polynom + 1);
        for (int y = 0; y < degree_of_polynom + 1; y++) {
            std::cout << "input  degree of element " << y+1 << "\n";
            std::cin >> element_degree;
            std::cout << "input coefficent of element " << y+1 << "\n";
            std::cin >> element_coefficent;
            polynom[element_degree] += element_coefficent;
        }
        polynoms.push_back(polynom);
    }
    print_polynom(polynoms);

    std::thread* thread_array = new std::thread[number_of_threads];
    int i = 0;
    int size_of_set = 0;
    int step_size = number_of_polynoms / number_of_threads;
    if (step_size == 0) {
        step_size = number_of_threads % number_of_polynoms;
    }
    if (number_of_threads <= number_of_polynoms) {
        size_of_set =  number_of_threads;
    }
    else {
        size_of_set =  number_of_threads%number_of_polynoms/2;
        if (size_of_set == 0) {
            size_of_set += 1;
        }
    }
    int first = 0;
    std::vector<std::vector<int>> result_set(size_of_set);
    int result_set_insertion_index = 0;

    for (i = 0; i < number_of_threads; i++) {
        if (first >= number_of_polynoms) {
            break;
        }
        if (i != number_of_threads - 1) {
            thread_array[i] = std::thread(mythread, ref(polynoms), first, first + step_size - 1, ref(result_set), result_set_insertion_index);  // 1 - назв. ф-ии 2 - передаем вектор с полиномами 3
            first = first + step_size;
            result_set_insertion_index = result_set_insertion_index + 1;
        }
        if (i == number_of_threads - 1) {
            thread_array[i] = std::thread(mythread, ref(polynoms), first, number_of_polynoms - 1, ref(result_set), result_set_insertion_index);
        }
    }
    for (int j = 0; j < i; j++) {
        thread_array[j].join();
    }

    std::vector<std::vector<int>> main_result(1);
    mythread(std::ref(result_set), 0, result_set.size() - 1, std::ref(main_result), 0);
    std::cout << "result\n";
    print_polynom(main_result);
    return 0;
}











