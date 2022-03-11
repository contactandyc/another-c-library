#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void sort(int *unsorted, int len) {
    int previous_num = unsorted[0];
    for (int i = 1; i < len; i++) {
        if (previous_num > unsorted[i]) {
            unsorted[i - 1] = unsorted[i];
            unsorted[i] = previous_num;
        }
        else {
            previous_num = unsorted[i];
        }
    }
}

void reverse_sort(int *unsorted, int len) {
    int previous_num = unsorted[len - 1];
    for (int i = len - 2; i > -1; i--) {
        if (previous_num < unsorted[i]) {
            unsorted[i + 1] = unsorted[i];
            unsorted[i] = previous_num;
        }
        else {
            previous_num = unsorted[i];
        }
    }
}

void brute_force(int *unsorted, int len) {
    int current_highest = 0;
    for (int i = 0; i < len; i++) 
      for (int j = i; j < len; j ++) {
        if (current_highest < unsorted[i]) {
            unsorted[j] = unsorted[i];
            unsorted[i] = current_highest;
        }
      }
}

int *fill_random(int len) {
    int *arr = (int *) malloc(len * sizeof(int));
    memset(arr, 0, sizeof(int)*len);
    for (int i = 0; i < len; i++)
        arr[i] = rand() % len;
    return arr;
}

double check(int *arr, int length) {
    int highest_val;
    int wrong;
    highest_val = wrong = 0;
    for (int i = 0; i < length; i++) {
        if (arr[i] < highest_val) 
            wrong++;
        else 
            highest_val = arr[i];
    }
    return 100.0f - (double)wrong / (double)length * 100.0f;
}

void print_arr(int *arr, int length) {
    for (int i = 0; i < length; i++)
        printf("%d, ", arr[i]);
}

int main(int argc, char *argv[]) {
    int length = 200;
    int accuracy = 2;
    int *nums = fill_random(length);
    int i = 0;
    while (check(nums, length) != 100.0f) {
        brute_force(nums, length);
        i++;
    }
    printf("\nSolved after %f comparisons. Brute force would have taken %d.\n", i * length * 2.0, (length * length - length) / 2);
    free(nums);
    return 0;
}