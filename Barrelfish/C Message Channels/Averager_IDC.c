#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <barrelfish/barrelfish.h>
#include <barrelfish/nameservice_client.h>
#include <barrelfish/spawn_client.h>
#include <barrelfish/sys_debug.h>

#include <if/idctest_defs.h>

#define LENGTH 80

const char *service_name = "idctest_service";

/* --------------------- Client ------------------------------ */
int id;
int result;

static void send_result_cb(void *a){
}

static void send_result(void *a){
    errval_t err;
    struct idctest_binding *binding = (struct idctest_binding*)a;

    struct event_closure txcont = MKCONT(send_result_cb, binding);
    err = idctest_result__tx(binding, txcont, id, result);

    if (err_is_fail(err)) {
        if (err_no(err) == FLOUNDER_ERR_TX_BUSY) {
            debug_printf("client: re-sending result\n");
            struct waitset *ws = get_default_waitset();
            txcont = MKCONT(send_result, binding);
            err = binding->register_send(binding, ws, txcont);
            if (err_is_fail(err)) {
                // note that only one continuation may be registered at a time
                DEBUG_ERR(err, "register_send on binding failed!");
            }
        } else {
            DEBUG_ERR(err, "error sending data message\n");
        }
    }
}

static void rx_data(struct idctest_binding *binding, uint64_t loops_, uint8_t *data, size_t arraysize_){
    uint8_t newValue = 0;
    for(long i = 0; i < loops_; i++){
        if(id == 0){
            newValue = (newValue + data[1]) / 2;
        }
        else if(id == arraysize_ - 1){
            newValue = (data[id-1] + newValue) / 2;
        }
        else{
            newValue = (data[id-1] + newValue + data[id+1]) / 3;
        }
    }
    result = newValue;

    send_result(binding);
}

static void send_ready_cb(void *a){
}

static void send_ready(void *a){
    struct idctest_binding *binding = (struct idctest_binding *)a;

    struct event_closure txcont = MKCONT(send_ready_cb, binding);

    errval_t err = idctest_ready__tx(binding, txcont, id);

    if (err_is_fail(err)) {
        if (err_no(err) == FLOUNDER_ERR_TX_BUSY) {
            debug_printf("client: re-sending ready\n");
            struct waitset *ws = get_default_waitset();
	       txcont = MKCONT(send_ready, binding);
            err = binding->register_send(binding, ws, txcont);
            if (err_is_fail(err)) {
                // note that only one continuation may be registered at a time
                DEBUG_ERR(err, "register_send on binding failed!");
            }
        } else {
            DEBUG_ERR(err, "error sending ready message\n");
        }
    }
}

struct idctest_rx_vtbl c_rx_vtbl = {
    .data = rx_data,
};

static void bind_cb(void *st, errval_t err, struct idctest_binding *binding){
    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "bind failed");
    }
    
    binding->rx_vtbl = c_rx_vtbl;

    send_ready(binding);
}


static void start_client(int id_){
    id = id_;

    iref_t iref;    
    errval_t err = nameservice_blocking_lookup(service_name, &iref);
    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "nameservice_blocking_lookup failed");
    }

    err = idctest_bind(iref, bind_cb, NULL, get_default_waitset(),IDC_BIND_FLAGS_DEFAULT);

    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "bind failed");
    }
}

/* --------------------- Server ------------------------------ */
int num_cores;
uint64_t loops;
size_t arraySize;

uint8_t* dataArray;

struct idctest_binding** bindings;

uint64_t start, end;

static void send_data_cb(void *a){
}

static void send_data(void *a)
{
    errval_t err;
    struct idctest_binding* binding = (struct idctest_binding*) a;

    struct event_closure txcont = MKCONT(send_data_cb, binding);
    err = idctest_data__tx(binding, txcont, loops, dataArray, arraySize);

    if (err_is_fail(err)) {
        if (err_no(err) == FLOUNDER_ERR_TX_BUSY) {
            debug_printf("server: re-sending data\n");
            struct waitset *ws = get_default_waitset();
            txcont = MKCONT(send_data, binding);
            err = binding->register_send(binding, ws, txcont);
            if (err_is_fail(err)) {
                // note that only one continuation may be registered at a time
                DEBUG_ERR(err, "register_send on binding failed!");
            }
        } else {
            DEBUG_ERR(err, "error sending data message\n");
        }
    }
    
}

static void rx_result(struct idctest_binding *b, int id_, int result_){
    dataArray[id_] = result_;

    static int num_results = 0;
    num_results++;
    if(num_results == num_cores){
        end = rdtsc();
        
        uint64_t tsc_per_ms = 0;
        sys_debug_get_tsc_per_ms(&tsc_per_ms);
        printf("TSC PER MS = %" PRIu64 "\n", tsc_per_ms);

        uint64_t diff = (end - start) / tsc_per_ms;
        float sample = (diff / 1000) + ((diff % 1000) / 1000.0);
        printf("TIME TAKEN (%lu): %f\n", loops, sample);
    }
}

static void rx_ready(struct idctest_binding *b, int id_){
    bindings[id_] = b;

    static int num_ready = 0;

    num_ready++;
    if(num_ready == num_cores){
        start = rdtsc();

        for(int i = 0; i < num_cores; i++){
            send_data(bindings[i]);
        }

        free(bindings);
	num_ready = 0;
    }
}

static struct idctest_rx_vtbl s_rx_vtbl = {
    .ready = rx_ready,
    .result = rx_result,
};

static errval_t connect_cb(void *st, struct idctest_binding *b){    
    b->rx_vtbl = s_rx_vtbl;

    return SYS_ERR_OK;
}

static void export_cb(void *st, errval_t err, iref_t iref){
    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "export failed");
    }

    err = nameservice_register(service_name, iref);
    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "nameservice_register failed");
    }
}

static void start_server(int num_cores_, int arraySize_, long loops_){
    num_cores = num_cores_;
    arraySize = arraySize_;
    loops = loops_;

    dataArray = malloc(arraySize * sizeof(uint8_t));

    bindings = malloc(num_cores * sizeof(struct idctest_binding*));

    for(int i = 0; i < arraySize; i++)
        dataArray[i] = i+5;

    errval_t err = idctest_export(NULL, export_cb, connect_cb, get_default_waitset(), IDC_EXPORT_FLAGS_DEFAULT);
    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "export failed");
    }

    for(int i = 0; i < num_cores; i++){
        char str[LENGTH];
        snprintf(str, LENGTH, "%d", i);
        char* args[] = {"Averager-MP", "client", str};

        printf("spawn_program %d\n",i); 
        spawn_program(i, args[0], args, NULL, 0, NULL);
    }
}

/* --------------------- Main ------------------------------ */
int main(int argc, char *argv[]){
    errval_t err;

    if ((argc >= 3) && (strcmp(argv[1], "client") == 0)) {
        start_client(atol(argv[2]));
    } else if ((argc >= 5) && (strcmp(argv[1], "server") == 0)) {
        start_server(atol(argv[2]), atol(argv[3]), atol(argv[4]));
    } else {
        printf("usage: %s (client id)|(server num_cores array_size loops)\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct waitset *ws = get_default_waitset();
    while (1) {
        err = event_dispatch(ws);
        if (err_is_fail(err)) {
            DEBUG_ERR(err, "in event_dispatch");
            break;
        }
    }

    return EXIT_FAILURE;
}
