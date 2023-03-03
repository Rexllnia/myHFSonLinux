#include "readfile.h"
int my_fsize(FILE *fp)
{
   int  size = 0;
   fseek(fp, 0, SEEK_END);
   size = ftell(fp); 
   rewind(fp);
   return size;
}
char *run(void * handle)
{
    char *buf;
    pthread_mutex_lock(&mutex);
    buf=malloc(target_file_info.block_size);//safe ?
    pthread_mutex_unlock(&mutex);
    struct create_thread_handle *thread_handle=handle;
    
    // printf("position:%d\r\n",thread_handle->position);
    FILE *fp;//create file point
    fp = fopen(target_file_info.r_path, "rb");
    fseek(fp,thread_handle->position,SEEK_SET);
    fread(buf, 1, target_file_info.block_size, fp);
    // printf("%s",buf);
    
    fclose(fp);
    pthread_exit(buf); 
}
void test_write()
{
   char* buf;
   strcpy(target_file_info.r_path,"../123.txt");
   pthread_t tid[Max_Thread_Num];//thread
   struct create_thread_handle create_thread_handle[Max_Thread_Num];
   FILE *fp;//create file point
   fp = fopen(target_file_info.r_path, "rb");
   target_file_info.size=my_fsize(fp);
   target_file_info.block_size=2;
   fclose(fp);
   // printf("%d\r\n",target_file_info.size);
   target_file_info.block=target_file_info.size/target_file_info.block_size;
   // printf("%d\r\n",target_file_info.block);
   for (int i = 0; i < target_file_info.block; i++)
   {
    create_thread_handle[i].position=i*target_file_info.block_size;
    pthread_create(&tid[i],NULL,run,&create_thread_handle[i]);
   }
    for (int i = 0; i < target_file_info.block; i++)
   {
    pthread_join(tid[i],&buf);
    printf("%s",buf);
   }
}

struct Thread_Read_Result thread_read(void* write_fd,char *read_path,int block_size,int fp_offset)
{
   char* buf;
   struct Thread_Read_Result res;
   bzero(&res,sizeof(struct Thread_Read_Result));
   strcpy(target_file_info.r_path,read_path);
   pthread_t tid[Max_Thread_Num];//thread
   pthread_mutex_init(&mutex,NULL);

   // pthread_mutex_init(&mutex,NULL);
   struct create_thread_handle create_thread_handle[Max_Thread_Num];
   FILE *fp;//create file point
   fp = fopen(target_file_info.r_path, "rb");
   target_file_info.size=my_fsize(fp)-fp_offset;
   target_file_info.block_size=block_size;
   fclose(fp);
   printf("%d\r\n",target_file_info.size);
    
   
   target_file_info.block=target_file_info.size/target_file_info.block_size;
   if (target_file_info.size%target_file_info.block_size!=0)
   {
    target_file_info.block+=1;
   }
   if(target_file_info.block>=Max_Thread_Num)//if the number of blocks bigger than Max_Thread_Num
   {
        res.retain_blocks=target_file_info.block-Max_Thread_Num;
        res.complete_blocks=Max_Thread_Num;
        res.fp_position=Max_Thread_Num*target_file_info.block_size;
        target_file_info.block=Max_Thread_Num;
   }
   else
   {
      res.complete_blocks=target_file_info.block;
   }
   printf("%d\r\n",target_file_info.block);
   for (int i = 0; i < target_file_info.block; i++)
   {
    create_thread_handle[i].position=i*target_file_info.block_size+fp_offset;
    pthread_create(&tid[i],NULL,run,&create_thread_handle[i]);
   }
    for (int i = 0; i < target_file_info.block; i++)
   {
    pthread_join(tid[i],&buf);//是否会free释放空间
    
    init_param.output(write_fd,buf);
   }
   return res;
}
