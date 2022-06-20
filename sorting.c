#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void int_swap(int *a, int *b);
void insertion_sort(int *A, int size);
void print_array(int *A, int size);
void merge_sort(int A[], int size);
void merge(int A1[], int size1, int A2[], int size2);
void arr_cpy(int From[], int To[], int size);
int lomuto_partition(int A[], int size);
int hoare_partition(int A[], int size);
void quicksort(int A[], int size);

int main(int argc, char *argv[]) {
	int A[10] = {8, 4, 2, 9, 10, 3, 5, 7, 1, 6};
	quicksort(A, 10);
	print_array(A, 10);
	return 0;
}

void print_array(int *A, int size) {
	int i;
	printf("Printing array A: ");
	for (i = 0; i < size; i++) {
		printf("%d ", A[i]);
	}
	printf("\n");
}

void insertion_sort(int *A, int size) {
	int i, j;
	for (i = 1; i < size; i++) {
		j = i - 1;
		while (j >= 0 && A[j+1] < A[j]) {
			int_swap(&A[j+1], &A[j]);
			j--;
		}
	}
}

void merge_sort(int A[], int size) {
    if (size <= 1) return;
    int mid = size / 2;
    merge_sort(A, mid);
    merge_sort(A+mid, size - mid);
    merge(A, mid, A+mid, size - mid);
}

void merge(int A1[], int size1, int A2[], int size2) {
    int size = size1 + size2, numfilled = 0, index1=0, index2=0;
    int *temp_array;
    temp_array = malloc(sizeof(int)*size);
    while (numfilled < size) {
        if ((A1[index1] <= A2[index2] || index2 >= size2) && index1 < size1) {
            /* A1 gets to put its value down */
            temp_array[numfilled] = A1[index1];
            index1++;
        } else {
            /* A2 gets to put its value down */
            temp_array[numfilled] = A2[index2];
            index2++;
        }
        numfilled++;
    }
    assert(numfilled == size);
    arr_cpy(temp_array, A1, size);
}

void arr_cpy(int From[], int To[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        To[i] = From[i];
    }
}

void quicksort(int A[], int size) {
    if (size <= 1) {
        return;
    }
    int pivot = hoare_partition(A, size);
    quicksort(A, pivot);
    quicksort(&(A[pivot + 1]), size - pivot - 1);
}

int lomuto_partition(int A[], int size) {
    if (size <= 1) {
        return 0;
    }

    int pivot, i, last_swapped = 0, piv_index;
    /* use first element as pivot */
    pivot = A[0]; piv_index = 0;
    for (i = 0; i < size; i++) {
        if (A[i]<pivot) {
            /* swap required */
            last_swapped++;
            int_swap(&A[i], &A[last_swapped]);
        }
    }
    int_swap(&A[last_swapped], &A[piv_index]);
    return last_swapped;
}

int hoare_partition(int A[], int size) {
    int pivot = A[0], i = 0, j = size - 1;

    while (i < j) {
        while (A[i] <= pivot && i <= j) {
            i++;
        }
        while (A[j] > pivot && j >= i) {
            j--;
        }
        int_swap(&A[i], &A[j]);
    }
    int_swap(&A[i], &A[j]);
    int_swap(&A[0], &A[j]);
    return j;

}

void int_swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}