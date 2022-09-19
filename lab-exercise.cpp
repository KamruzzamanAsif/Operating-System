#include <bits/stdc++.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void frequency_counter(int a[]){
    map<int, int> mp;
	
	for(int i=0; i<10; i++){
		int x = a[i];
        mp[x]++;
	}

    cout<<"frequency:\n";
    for (auto &e: mp) {
        cout << '{' << e.first << ", " << e.second << '}' << std::endl;
    }
}

void sum_even(int a[]){
    int sum = 0;
    for(int i=0; i<10; i++){
        if(a[i]%2 == 0)
            sum += a[i];
    }
    cout<<"Sum of even numbers:" <<sum<<endl;
}

void count_even_numbers(int a[]){
    int count = 0;
    for(int i=0; i<10; i++){
        if(a[i]%2 == 0)
            count++;
    }
    cout<<"Total even numbers:" <<count<<endl;
}

void sort_array(int a[]){
    cout<<"Sorted array\n";
    sort(a, a+10);
    for(int i=0; i<10; i++)
        cout<<a[i]<<" ";
}

int main(void){
	int a[10] = {5, 2, 7, 2, 1, 4, 2, 3, 1, 5};
	
    int pid = fork();
    if(pid == 0){
        frequency_counter(a);
    }
    else{
        int pid1 = fork();
        if(pid1 == 0){
            sum_even(a);
        }
        else{
            int pid2 = fork();
            if(pid2 == 0){
                count_even_numbers(a);
            }
            else{
                sort_array(a);
            }
        }
    }
}

