#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

float* globalArray;

unsigned int arraySize;
unsigned long loops;

void initArray(int numElements, float array[numElements]) {
    for(int i = 0; i<numElements; i++){
        array[i] = ((float) rand() / (float) RAND_MAX);
    }
}

void printArray(int numElements, float array[numElements]) {
	printf("[");
	for(int i = 0; i<numElements; i++){
		printf("%f ", array[i]);
	}
	printf("]\n");
}

struct averageSharedArgs{
	int id;
	float left;
	float right;
};

void* averageSharedWork(void* args){
	struct averageSharedArgs* arguments = (struct averageSharedArgs*) args;
	int id = arguments->id;

	float left = arguments->left;
	float right = arguments->right;

	float newValue;
	for(long i = 0; i<loops;i++){
		if(id == 0){
			newValue = (globalArray[0] + right) / 2;
		}
		else if(id == arraySize - 1){
			newValue = (left + globalArray[id]) / 2;
		}
		else{
			newValue = (left + globalArray[id] + right) / 3;
		}

		globalArray[id] = newValue;
	}

	free(arguments);

	return 0;
}

void averageShared() {
	pthread_t* threads = malloc(arraySize * sizeof(pthread_t));

	struct averageSharedArgs* args;
	for(int i = 0; i<arraySize; i++){
		args = malloc(sizeof(struct averageSharedArgs));
		args->id = i;
		if(i > 0) { args->left = globalArray[i-1];}
		if(i < arraySize - 1) { args->right = globalArray[i+1];}

		pthread_create(&threads[i], NULL, averageSharedWork, args);
	}

	for(int i = 0; i<arraySize; i++){
		pthread_join(threads[i], NULL);
	}

	free(threads);
}

struct averageLocalArgs{
	unsigned int arraySize;
	unsigned long loops;

	int id;
	float* array;
};

void* averageLocalWork(void* args){
	struct averageLocalArgs* arguments = (struct averageLocalArgs*) args;
	int id = arguments->id;
	float* array = arguments->array;

	float newValue;
	for(long i = 0; i < arguments->loops; i++){
		if(id == 0){
			newValue = (array[0] + array[1]) / 2;
		}
		else if(id == arguments->arraySize - 1){
			newValue = (array[id-1] + array[id]) / 2;
		}
		else{
			newValue = (array[id-1] + array[id] + array[id+1]) / 3;
		}

		array[id] = newValue;
	}

    float finalValue = array[id];

	free(arguments->array);
	free(arguments);

    float* ptr = malloc(sizeof(float));
    *ptr = finalValue;
    return ptr;
}

void averageLocal() {
	pthread_t* threads = malloc(arraySize * sizeof(pthread_t*));

	struct averageLocalArgs* args;
	for(int i = 0; i<arraySize; i++){
		args = malloc(sizeof(struct averageLocalArgs));
		args->arraySize = arraySize;
		args->loops = loops;
		args->id = i;

		float* arrayCopy = malloc(arraySize * sizeof(float));
		memcpy(arrayCopy, globalArray, arraySize * sizeof(float));
		args->array = arrayCopy;

		pthread_create(&threads[i], NULL, averageLocalWork, args);
	}

	for(int i = 0; i<arraySize; i++){
		void* ptr;
		pthread_join(threads[i], &ptr);
		float result = *(float*) ptr;
		free(ptr);
		globalArray[i] = result;
	}

	free(threads);
}

float performAveraging(void (*f)(void)){
    struct timespec start, end;
    float elapsed = 0;

	for(int i = 0; i<10; i++){
	    initArray(arraySize, globalArray);
	    //printArray(arraySize, globalArray);

	    clock_gettime(CLOCK_MONOTONIC, &start);

    	(*f)();

	    clock_gettime(CLOCK_MONOTONIC, &end);

		float sample_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec) / 1000000000.0);
		elapsed += (1.0/(i+1)) * (sample_time - elapsed);
	}

	return elapsed;
}

int main(int argc, char* argv[]) {
	srand(time(NULL));

	if (argc != 3) {
		printf("Incorrect number of arguments, must specify array size & number of loops\n");
		exit(-1);
	}

	arraySize = atol(argv[1]);
	if(arraySize <= 0){
		printf("Invalid array size\n");
		exit(-1);
	}

	loops = atol(argv[2]);
	if(loops < 0){
		printf("Invalid number of loops\n");
		exit(-1);
	}

	globalArray = (float*) malloc(arraySize * sizeof(float));

	printf("SHARED: %fs\n", performAveraging(averageShared));
	printf("LOCAL: %fs\n", performAveraging(averageLocal));

	free(globalArray);

	return 0;
}
