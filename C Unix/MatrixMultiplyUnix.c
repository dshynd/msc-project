#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

#define SIZE 20

int A[SIZE][SIZE];
int B[SIZE][SIZE];
int C[SIZE][SIZE];

void initMatrix(int rows, int cols, int matrix[rows][cols]) {
	int count = 0;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			matrix[row][col] = count++;
		}
	}
}

void copyMatrix(int* src, int srcWidth, int srcStartPos, int* dest, int destWidth, int destStartPos, int numRows, int numCols) {
	int srcRowOffset = srcStartPos / srcWidth;
	int srcColOffset = srcStartPos % srcWidth;
	int destRowOffset = destStartPos / destWidth;
	int destColOffset = destStartPos % destWidth;

	for (int row = 0; row< numRows; row++) {
		for (int col = 0; col < numCols; col++) {
			int srcPos = (srcRowOffset + row)*srcWidth + (srcColOffset + col);
			int destPos = (destRowOffset + row)*destWidth + (destColOffset + col);
			dest[destPos] = src[srcPos];
		}
	}
}

void printMatrix(int rows, int cols, int matrix[rows][cols]) {
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			printf("%d ", matrix[row][col]);
		}
		printf("\n");
	}
	printf("----------------------------\n");
}

void addMatrices(int rows, int cols, int m1[rows][cols], int m2[rows][cols], int m3[rows][cols]) {
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			m3[row][col] = m1[row][col] + m2[row][col];
		}
	}
}

void* multiplyByEntry(void* entryID){
	int id = (int) entryID;
	int row = id / SIZE;
	int col = id % SIZE;

	C[row][col] = 0;
	for(int inner = 0; inner < SIZE; inner++){
		C[row][col] += A[row][inner] * B[inner][col];
	}

	return 0;
}

void multiplyMatricesPerEntry() {
	int entries = SIZE*SIZE;
	pthread_t* threads = malloc(entries * sizeof(pthread_t*));

	for(int i = 0; i < entries; i++){
		pthread_create(&threads[i], NULL, multiplyByEntry, (void*) i);
	}

	for(int i = 0; i < entries; i++){
		pthread_join(threads[i], NULL);
	}

	free(threads);
}

struct multiplyLocalArgs{
    int id;
	int localA[SIZE][SIZE];
	int localB[SIZE][SIZE];
};

void* multiplyByEntryLocal(void* args){
	struct multiplyLocalArgs* arguments = (struct multiplyLocalArgs*) args;
	int id = arguments->id;
	int row = id / SIZE;
	int col = id % SIZE;

	int result = 0;
	for(int inner = 0; inner < SIZE; inner++){
		result += arguments->localA[row][inner] * arguments->localB[inner][col];
	}

	free(arguments);

	int* ptr = malloc(sizeof(int));
    *ptr = result;
    return ptr;
}

void multiplyMatricesPerEntryLocal() {
	int entries = SIZE*SIZE;
	pthread_t* threads = malloc(entries * sizeof(pthread_t*));

	struct multiplyLocalArgs* args;
	for(int i = 0; i<entries; i++){
		args = malloc(sizeof(struct multiplyLocalArgs));
        args->id = i;

        memcpy(args->localA, A, entries * sizeof(int));
        memcpy(args->localB, B, entries * sizeof(int));

        pthread_create(&threads[i], NULL, multiplyByEntryLocal, args);
	}

	for(int i = 0; i<entries; i++){
        void* ptr;
        pthread_join(threads[i], &ptr);
        int result = *(int*) ptr;
        free(ptr);
        C[i / SIZE][i % SIZE] = result;
	}

	free(threads);
}


float timeWork(void (*f)(void)){
    struct timespec start, end;
    float elapsed = 0;

	for(int i = 0; i<=10; i++){
	    initMatrix(SIZE, SIZE, A);
		initMatrix(SIZE, SIZE, B);

	    clock_gettime(CLOCK_MONOTONIC, &start);
	    
	    for(int j = 0; j<1; j++)
	    	(*f)();

	    clock_gettime(CLOCK_MONOTONIC, &end);
	
		if(i!=0){
			float sample_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec) / 1000000000.0);
			printf("PREV: %f, SAMPLE: %f\n", elapsed, sample_time);
			elapsed += (1.0/(i)) * (sample_time - elapsed);
		}
	}

	printMatrix(SIZE, SIZE, C);

	return elapsed;
}

int main(int argc, char* argv[]) {
	float shared = timeWork(multiplyMatricesPerEntry);
	float local = timeWork(multiplyMatricesPerEntryLocal);

	return 0;
}
