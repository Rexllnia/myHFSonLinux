#include<dirent.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#define Max_Thread_Num 4096
struct Thread_Read_Result
{
    int fp_position;
    int retain_blocks;
    int complete_blocks;
};
struct create_thread_handle
{
    int position;
};
struct init_param
{
    void (*output)(void *handle,char * buf);
};
struct target_file_info
{
    char r_path[100];
    int size;
    int block;
    int block_size;
};

pthread_mutex_t mutex;
struct init_param init_param;
struct target_file_info target_file_info;
int my_fsize(FILE *fp);
void test_write();
struct Thread_Read_Result thread_read(void* write_fd,char *read_path,int block_size,int fp_offset);