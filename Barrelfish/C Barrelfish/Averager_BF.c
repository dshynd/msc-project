#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <barrelfish/barrelfish.h>
#include <barrelfish/threads.h>
#include <barrelfish/deferred.h>
#include <barrelfish/sys_debug.h>

#include <pthread.h>
#include <posixcompat.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

float* globalArray;

unsigned int arraySize;
unsigned long loops;

static void initArray(int numElements, float array[numElements]) {
    int i;
    for(i = 0; i<numElements; i++){
            array[i] = ((float) rand() / (float) RAND_MAX);
    }
}

static void printArray(int numElements, float array[numElements]) {
    /*printf("[");
    int i;
    for(i = 0; i<numElements; i++){
            printf("%f ", array[i]);
    }
    printf("]\n");*/
}

int num_cores;
static int rrPlacement(void){
	static int current_core = -1;

	current_core = (current_core + 1) % num_cores;

	return current_core;
}


struct averageSharedArgs{
    int id;
    float left;
    float right;
};

static int averageSharedWorkBF(void* args){
    struct averageSharedArgs* arguments = (struct averageSharedArgs*) args;
    int id = arguments->id;

    float left = arguments->left;
    float right = arguments->right;

    float newValue = 0;
    long i;
    for(i = 0; i<loops;i++){
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

static void averageSharedBF(void) {
	struct thread** threads = malloc(arraySize * sizeof(struct thread*));

    struct averageSharedArgs* args;
    int i;
    for(i = 0; i<arraySize; i++){
        args = malloc(sizeof(struct averageSharedArgs));
        args->id = i;
        if(i > 0) { args->left = globalArray[i-1];}
        if(i < arraySize - 1) { args->right = globalArray[i+1];}

        domain_thread_create_on(rrPlacement(), averageSharedWorkBF, args, &threads[i]);
    }

    for(i = 0; i<arraySize; i++){
        domain_thread_join(threads[i], NULL);
    }

	free(threads);
}

struct averageLocalArgs{
    unsigned int arraySize;
    unsigned long loops;

    int id;
    float* array;
};

static int averageLocalWorkBF(void* args){
    struct averageLocalArgs* arguments = (struct averageLocalArgs*) args;
    int id = arguments->id;
    float* array = arguments->array;

    float newValue = 0;
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

    free(arguments->array);
    free(arguments);

    return newValue;
}

static void averageLocalBF(void) {
	struct thread** threads = malloc(arraySize * sizeof(struct thread*));

    struct averageLocalArgs* args;
    int i;
    for(i = 0; i<arraySize; i++){
        args = malloc(sizeof(struct averageLocalArgs));
        args->arraySize = arraySize;
        args->loops = loops;
        args->id = i;

        float* arrayCopy = malloc(arraySize * sizeof(float));
        memcpy(arrayCopy, globalArray, arraySize * sizeof(float));
        args->array = arrayCopy;

        domain_thread_create_on(rrPlacement(), averageLocalWorkBF, args, &threads[i]);
    }

    for(i = 0; i<arraySize; i++){
        int result;
        domain_thread_join(threads[i], &result);
        globalArray[i] = result;
    }

	free(threads);
}


static float performAveraging(void (*f)(void)){
    initArray(arraySize, globalArray);
    printArray(arraySize, globalArray);

 
	uint64_t start, end;
	uint64_t tsc_per_ms = 0;
	sys_debug_get_tsc_per_ms(&tsc_per_ms);
	printf("TSC PER MS = %" PRIu64 "\n", tsc_per_ms);

	int repeats = 0;
	float elapsed = 0;

    if(loops < 1000000){repeats = 10;}

	for(int i = 0; i<=repeats; i++){
		start = rdtsc();
		(*f)();
		end = rdtsc();
		
		uint64_t diff = (end - start) / tsc_per_ms;
		float sample = (diff / 1000) + ((diff % 1000) / 1000.0);
		elapsed += (1.0/(1+i)) * (sample - elapsed); 
	}

        printArray(arraySize, globalArray);
        return elapsed;
}

bool all_spanned = false;
static void span_cb(void *arg, errval_t err)
{
    if (err_is_fail(err)) {
        DEBUG_ERR(err, "span failed");
        return;
    }

    static int num_spanned = 1;

    num_spanned++;
	debug_printf("Span completed, num_spanned: %d\n", num_spanned);
    if (num_spanned >= num_cores) {
        all_spanned = true;
    }
}

int main(int argc, char* argv[]) {
        if (argc != 4) {
            printf("Incorrect number of arguments, must specify number of cores, array size, number of loops\n");
            exit(-1);
        }


        num_cores = atol(argv[1]);
        if(num_cores <= 0){
                printf("Invalid number of cores\n");
                exit(-1);
	}

        arraySize = atol(argv[2]);
        if(arraySize <= 0){
                printf("Invalid array size\n");
                exit(-1);
        }

        loops = atol(argv[3]);
        if(loops < 0){
                printf("Invalid number of loops\n");
                exit(-1);
	}

    srand(time(NULL));

	debug_printf("Spanning cores\n");
	
	errval_t err;
	for (int core = 1; core < num_cores; core++) {
		err = domain_new_dispatcher(core, span_cb, NULL);

		if (err_is_fail(err)) {
			DEBUG_ERR(err, "failed span %d", core);
		} 
	}

	while (num_cores != 1 && !all_spanned) {
		thread_yield();
	}
	debug_printf("Spanning complete!\n");	

    globalArray = (float*) malloc(arraySize * sizeof(float));

	unsigned long loops_[] = {1,100,10000,1000000,100000000,1000000000};
	for(int i = 0; i < 6; i++){	    
        loops = loops_[i];
	float shared = performAveraging(averageSharedBF);
	float local = performAveraging(averageLocalBF);
   	    printf("SHARED BF (%lu): %fs\n", loops, shared);
    	printf("LOCAL BF (%lu): %fs\n", loops, local);
        printf("\n");
	}

    free(globalArray);

    return 0;
}

