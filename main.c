

#include<dirent.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include "sequence.h"
#include "readfile.h"
#define MY_PORT 3333
#define __weak __attribute__((weak))
struct write_handle
{
   int write_fd;
   int block_size;
};
struct http_request_msg
{
 char method[20];
 char path[256];
 char http_protocal[20];
 char boundary[256];
 int Content_Length;

};
void Create_download(char *buf, char *filename, int length)
{

    sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type:application/octet-stream\r\nContent-Length:%d\r\nAccept-Ranges: bytes\r\nContent-Disposition:attachment; filename=%s\r\n\r\n", length, filename);
}
// void SEND_FILE(int accept_fd, char *path)
// {
//    FILE *fp;
//    int buf[2000];
//    // char *buf;
//    int  size = 0;
//    fp = fopen(path, "rb");
//    fseek(fp, 0, SEEK_END);
//    size = ftell(fp);
//    //printf("\n%s",(path+2));
//    Create_download(buf,(path+2),size);
//    //printf("\n%s",buf);
//    write(accept_fd, buf, strlen(buf));
//   // printf("\n%d", size);
   
//    rewind(fp);

//    while (ftell(fp) < size - 1)
//    {

//        fread(buf, 1, 1999, fp);
//        write(accept_fd, buf, 1999);
//    }
//    fclose(fp);
// }
void SEND_FILE(int accept_fd, char *path)
{
   FILE *fp;
   int buf[2000];
   int fp_offset=0;
   int  size = 0;
   fp = fopen(path, "rb");
   fseek(fp, 0, SEEK_END);
   size = ftell(fp);
   //printf("\n%s",(path+2));
   Create_download(buf,(path+2),size);
   //printf("\n%s",buf);
   write(accept_fd, buf, strlen(buf));
   fclose(fp);
   struct write_handle write_handle;
   write_handle.write_fd=accept_fd;
   write_handle.block_size=size/16;
   struct Thread_Read_Result res;

   res=thread_read(&write_handle,path,50000,fp_offset);
      printf("complete:%d\r\n",res.complete_blocks);
   printf("retain:%d\r\n",res.retain_blocks);
   printf("fp_position%d\r\n",res.fp_position);
   while (res.retain_blocks!=0)
   {
      res=thread_read(&write_handle,path,50000,fp_offset+=res.fp_position);
   printf("complete:%d\r\n",res.complete_blocks);
   printf("retain:%d\r\n",res.retain_blocks);
   printf("fp_position%d\r\n",res.fp_position);
   }
   

}
void strcut(char *str, char *front, char *rear, char *re)
{
    char *p1, *p2;
    p1 = strstr(str, front);
    p1 += strlen(front);
    p2 = strstr(str, rear);

    while (p1 != p2)
    {
        *re++ = *p1++;
    }
    *re = '\0';
}
void Create_header(char *buf, int code, char *info, char *filetype)
{
    sprintf(buf, "HTTP/1.1 %d %s\r\nContent-Type:%s\r\n\r\n", code, info, filetype);
}


char *get_mime_type(char *name)
{
    char *dot;
    dot = strrchr(name, '.');
    if (dot == (char *)0)
        return "text/plain;charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".hml") == 0)
        return "text/html;charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".c") == 0)
        return "text/html;charset=utf-8\r\nContent-Disposition:attachment;\r\n";
}
void display(void *handle,char *data)
{

    write(handle,data,strlen(data));
    printf("%s\r\n",data);
}
struct sequeue HTMLTextGenerator(struct sequeue s,char *path)
{
    char *folder_name;
    char current_folder[100]="";
    DIR *dirp;

    struct dirent *dir_info;
char HTML_Text[]="<!DOCTYPE html>\
<html>\
<head>\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\
<title>Download</title>\
<style>\
    body {\
        width: 35em;\
        margin: 0 auto;\
    }\
</style>\
</head>\
<body>";
char upload_form[]="<form action=\"upload-file.php\" method=\"post\"\
enctype=\"multipart/form-data\"> \
<label for=\"file\">filename:</label>\
<input type=\"file\" name=\"file\" id=\"file\" />\
<br/>\
<input type=\"submit\" name=\"submit\" value=\"submit\"/>\
</form>";
s=init(s);
s=push(s,HTML_Text);
s=push(s,upload_form);
printf("%s\r\n",path);
dirp=opendir(path);
folder_name=strtok(path,"/");
while (folder_name!=NULL)
{
   folder_name=strtok(NULL,"/");
   if(folder_name!=NULL)
   {
        strcpy(current_folder,folder_name);
   }
}


while (dir_info=readdir(dirp))
    {
        char temp[256]="";
        // sprintf(temp, "<br><a href=\"%s\">%s</a></br>",dir_info->d_name, dir_info->d_name);
        // if(strcmp(path,"./")==0)
        // {   
        //     sprintf(temp, "<br><a href=\"%s\">%s</a></br>",dir_info->d_name, dir_info->d_name);

        // }else{
        sprintf(temp, "<br><a href=\"%s/%s\">%s</a></br>",current_folder,dir_info->d_name, dir_info->d_name);
        // }
        
        s=push(s,temp);

        
    }
    // s=push(s,upload_form);
    s=push(s,"</body>\n</html>\r\n");


    closedir(dirp);
    return s;
}
void write_socket(void *handle,char *buf)
{
    struct write_handle * write_handle=handle;
    // printf("%d",(int*)handle);
    printf("%d\r\n",sizeof(buf));
    write(write_handle->write_fd, buf,50000);
}
int main(int argc ,char **argv)
{
 
 init_param.output=write_socket;
//  test_write();
/*threadings all  worked,threadings which in order has exit,but other threadings has not exit until fork run
*/
 int listen_fd,accept_fd;
 struct sockaddr_in     client_addr;
 int n;
 printf("0x%x\r\n",listen_fd);
 if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
        printf("Socket Error:%s\n\a",strerror(errno));
        exit(1);
  }
 printf("0x%x\r\n",listen_fd);
 bzero(&client_addr,sizeof(struct sockaddr_in));
 client_addr.sin_family=AF_INET;
 client_addr.sin_port=htons(MY_PORT);
 client_addr.sin_addr.s_addr=htonl(INADDR_ANY);
 n=1;

 setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&n,sizeof(int));
 /*套接字级     
 默认情况下，当监听服务器在重启监听服务器通过调用socket，bind和listen重新启动时，
 由于他试图捆绑一个现有连接（即正由早先派生的那个子进程处理着的连接）上的端口，从而bind调用会失败*/
 printf("0x%x\r\n",listen_fd);
 if(bind(listen_fd,(struct sockaddr *)&client_addr,sizeof(client_addr))<0)
  {
        printf("Bind Error:%s\n\a",strerror(errno));
        exit(1);
  }
  listen(listen_fd,5);
  while(1)
  {
   accept_fd=accept(listen_fd,NULL,NULL);
   if((accept_fd<0)&&(errno==EINTR))
          continue;
   else if(accept_fd<0)
    {
        printf("Accept Error:%s\n\a",strerror(errno));
        continue;
    }
   if((n=fork())==0)
   {
        
        
        char buffer[1024];
        char *p;
        struct http_request_msg request_msg;
        struct sequeue s;void * handle;
        struct stat stat_buf; 
        char html_header[100]="";



        // while (*(p++)!=" ")
        // {
        //     printf("%s\n",*(p++));
        // }
        
        
        bzero(&request_msg,sizeof(struct http_request_msg));
        
        
        n=read(accept_fd,buffer,1024);
  

        printf("%s\r\n",buffer);
        p=strtok(buffer," ");
        strcpy(request_msg.method,p);
        printf("**%s**\r\n",request_msg.method);
        p=strtok(NULL," ");
        strcpy(request_msg.path,p);
        printf("**%s**\r\n",request_msg.path);
        p=strtok(NULL,"\r\n");
        strcpy(request_msg.http_protocal,p);
        printf("**%s**\r\n",request_msg.http_protocal);
        char temp[256]="";
        sprintf(temp,".%s",request_msg.path);
        stat(temp, &stat_buf);
        if(S_IFDIR & stat_buf.st_mode)
        {
            s.display=display;
            s=HTMLTextGenerator(s,temp);
            Create_header(html_header, 200, "OK", "text/html;charset=utf-8");
            write(accept_fd,html_header,strlen(html_header));
            Sequeue_display_from_rear(accept_fd,s);
        }
        else if(S_IFREG & stat_buf.st_mode){
   
            SEND_FILE(accept_fd,temp);
        }
        
        close(accept_fd);
        exit(0);
   }
   else if(n<0)
        printf("Fork Error:%s\n\a",strerror(errno));
   close(accept_fd);
  }
} 
