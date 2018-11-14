#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>

#include "fpga_ddr_rw_interface.h"
#include "memory_manager.h"

#define THREAD_NUM_MAX (64)

unsigned int result_flag[THREAD_NUM_MAX] = { 0 };

#define STR_PARSE_ARG   "t:p:m:s:l:f:r:w:oh"
unsigned int thread_num = 1;
unsigned int package_num = 1000;
unsigned int mode = 1;
unsigned int len = 64;
unsigned int slot_id = 0;
unsigned long long fpga_ddr_wr_addr = 0;
unsigned long long fpga_ddr_rd_addr = 0;
unsigned int success_thread_num = 0;
#define RETRY_TIME  (10000000)
static pthread_mutex_t g_result_lock = PTHREAD_MUTEX_INITIALIZER;
unsigned int read_flag1 = 0;
unsigned int read_flag2 = 0;
unsigned int write_flag1 =0;
unsigned int write_flag2 =0;

void rw_sleep_func(unsigned int length)
{
    if(MBUF_128B_SIZE >= length)
    {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 100;
        (void)nanosleep(&ts, NULL);
    }
}

void callback( unsigned int thread_id, unsigned int slotid, rw_ddr_data rw_data, int rw_flag)
{
    (void)slotid;
    if(rw_flag == 1) {
        (void)memory_manager_free_bulk((void*)(rw_data.cpu_vir_dst_addr));
    }
    else if(rw_flag == 2)
    {
        (void)memory_manager_free_bulk((void*)(rw_data.cpu_vir_src_addr));
    }
    else
    {
        (void)memory_manager_free_bulk((void*)(rw_data.cpu_vir_dst_addr));
        (void)memory_manager_free_bulk((void*)(rw_data.cpu_vir_src_addr));
    }
   
    (void)pthread_mutex_lock(&g_result_lock);
    result_flag[thread_id]++;
    if(result_flag[thread_id] > package_num)
        printf("Some errors occurred, Please reduce the bd delivery rate! \n");
    (void)pthread_mutex_unlock(&g_result_lock);
    return;
}

void *rw_fpga_ddr_func(void *para) {

    int ret;
    unsigned int thread_id = 0;
    void *src_addr = NULL;
    void *src_addr1= NULL;
    rw_ddr_data write_data;
    unsigned int idx = 0;
    unsigned int retry_time = 0;
    unsigned int th_mode = *(unsigned int *)para;

    ret = alloc_thread_id(&thread_id);
    if(ret) {
        printf("call alloc_thread_id fail .\n");
        return NULL;
    }
    printf("alloc thread id %d success.\n", thread_id);
    
    for(idx = 0; idx < package_num; idx++) {

        for(retry_time = 0; retry_time < RETRY_TIME; retry_time++) {
            src_addr = memory_manager_alloc_bulk(len);
            if(NULL == src_addr) {
                (void)usleep(1);
                continue;
            }
            else {
                break;
            }
        }
        if(retry_time == RETRY_TIME) {
            (void)free_thread_id(thread_id);
            printf(" memory alloc failed . %d\n", idx );
            return NULL;
        }

        if(th_mode == 1)   /*read*/
        {
            write_data.cpu_vir_dst_addr = (unsigned long long)src_addr;
            write_data.fpga_ddr_rd_addr = fpga_ddr_rd_addr;
        }
        else if(th_mode == 2)  /*write*/
        {
            write_data.cpu_vir_src_addr = (unsigned long long)src_addr;
            write_data.fpga_ddr_wr_addr = fpga_ddr_wr_addr;
        }
        else
        {
            write_data.fpga_ddr_rd_addr = fpga_ddr_rd_addr;
            write_data.fpga_ddr_wr_addr = fpga_ddr_wr_addr;
            write_data.cpu_vir_src_addr = (unsigned long long)src_addr;
            for(retry_time = 0; retry_time < RETRY_TIME; retry_time++) {
                src_addr1 = memory_manager_alloc_bulk(len);
                if(NULL == src_addr1) {
                    (void)usleep(1);
                    continue;
                }
                else {
                    break;
                }
            }
            if(retry_time == RETRY_TIME) {
                (void)free_thread_id(thread_id);
                printf("memory alloc fail2 . %d\n", idx);
                return NULL;
            }
            write_data.cpu_vir_dst_addr = (unsigned long long)src_addr1;
            
        }
        
        write_data.length = len;
        for(retry_time = 0; retry_time < RETRY_TIME; retry_time++) {
            rw_sleep_func(len);
            if(th_mode == 1)
            {
                ret = read_data_from_fddr(thread_id, slot_id, write_data);
            }
            else if(th_mode == 2)
            {
                ret = write_data_to_fddr(thread_id, slot_id, write_data);
            }
            else
            {
                ret = process_data_with_fpga(thread_id, slot_id, write_data);
            }
            if(ret) {
                (void)usleep(10);
                continue;
            }
            else {
                break;
            }
        }
        if(retry_time == RETRY_TIME) {
            (void)free_thread_id(thread_id);
            (void)memory_manager_free_bulk(src_addr);

            if(th_mode == 0)
            {
                (void)memory_manager_free_bulk(src_addr1);
            }
            return NULL;
        }
    }  

    while(1) {
        if(result_flag[thread_id] == package_num) {
            (void)pthread_mutex_lock(&g_result_lock);
            success_thread_num++;
            result_flag[thread_id] = 0;	
            (void)pthread_mutex_unlock(&g_result_lock);
            printf("thread id %d, result num %d, all come back, quit it .\n", thread_id, result_flag[thread_id]);
            break;
        }
        (void)usleep(1);
    }

    ret = free_thread_id(thread_id);
    if(ret) {
        printf("call free_thread_id fail .\n");
        return NULL;
    }
    printf("free thread_id success, %d\n", thread_id);

    return NULL;
}

static void help() {
    printf(
        "-----------------------------------------------------------------------------------\r\n"
        "argument format:\n"
        "\t-t xxx   xxx: thread num, 1 as default\r\n"
        "\t-s xxx   xxx: slot id, 0 as default\r\n"
        "\t-p xxx   xxx: package num, 1000 as default;\r\n"
        "\t-m xxx   xxx: mode, should be [0, 1, 2], 1 as default;\r\n"
        "\t-l xxx   xxx: len, 64 as default;\r\n"
        "\t-r xxx   xxx: fpga ddr read addr, should be [0, 64*1024*1024*1024], 0 as default;\r\n"
        "\t-w xxx   xxx: fpga ddr write addr, should be [0, 64*1024*1024*1024], 0 as default;\r\n"
        "\t-h: print help\n"
        "-----------------------------------------------------------------------------------\r\n");
}

static int parse_arg(int argc, char* argv[]) {
    char*   arg_val = NULL;
    int     ch = 0;
    unsigned long int value = 0;
    while ((ch=getopt(argc, argv, STR_PARSE_ARG)) != -1) {
        switch (ch) {
            case 't': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    if(value ==  0 || value > THREAD_NUM_MAX) {
                        printf("-t param input unvalid.\n");
                    }
                    else {
                        thread_num = value;
                    }
                }
                break;
            }
            case 'p': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    if(value == 0) {
                        printf("-p param input unvalid.\n");
                    }
                    else {
                        package_num = value;
                    }
                }
                break;
            }
            case 's': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    slot_id = value;
                }
                break;
            }
            case 'm': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    if(value != 0x0 && value != 0x1 && value != 0x2) {
                        printf("-m param input unvalid.\n");
                    }
                    else {
                        mode = value;
                    }
                }
                break;
            }
            case 'l': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    if(value == 0x0) {
                        printf("-l param input unvalid.\n");
                    }
                    else {
                        len = value;
                    }
                }
                break;
            }
            case 'r': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    fpga_ddr_rd_addr = value;
                }
                break;
            }
            case 'w': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    fpga_ddr_wr_addr = value;
                }
                break;
            }
            case 'h':
            default:
                goto parse_error;
        }
    }
    
    return 0;
    
parse_error:
    help();
    return -1;
}

int main(int argc, char* argv[]) {
    int ret;
    pthread_t thread_id[THREAD_NUM_MAX];
    unsigned int i = 0;
    unsigned long long diff;
    struct timeval  tx_start_time, tx_end_time;

    if (0 != parse_arg(argc, argv)) {
        return -1;
    }
    
    ret = fddr_access_mode_init(callback);
    if(ret) {
        printf("call fpga_ddr_rw_module_init fail .\n");
        return ret;
    }

    (void)gettimeofday(&tx_start_time, NULL);
    for(i = 0; i < thread_num; i++) {
        ret = pthread_create(&thread_id[i], NULL, rw_fpga_ddr_func, &mode);

        if (0 != ret)
        {
            printf("call pthread_create 0x%x failed.\n", ret);
        }
    }

    for (i = 0; i < thread_num; i++)
    {
        (void)pthread_join(thread_id[i],NULL);
    }

    printf("\n------------\n");
    if(success_thread_num == thread_num) {
        printf("Test Success.\n");
    }
    else
    {
        printf("Test Fail.\n");
    }
    (void)gettimeofday(&tx_end_time, NULL);
    diff = 1000000*(tx_end_time.tv_sec-tx_start_time.tv_sec) + tx_end_time.tv_usec - tx_start_time.tv_usec;
    printf("Speed %f Gbps\n", ((float)len * thread_num * package_num * 8/1000/1000/1000*1000*1000/diff));
    printf("Speed %f Mpps\n", ((float) thread_num * package_num /1000/1000*1000*1000/diff));

    info_collect_mem_manager();

    (void)fddr_access_mode_uninit();
    
    return 0;
}
