

#include<dirent.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#define MY_PORT 3333
void Create_download(char *buf, char *filename, int length)
{

    sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type:application/octet-stream\r\nContent-Length:%d\r\nAccept-Ranges: bytes\r\nContent-Disposition:attachment; filename=%s\r\n\r\n", length, filename);
}
void SEND_FILE(int accept_fd, char *path)
{
   FILE *fp;
   int buf[2000];
   // char *buf;
   int  size = 0;
   fp = fopen(path, "rb");
   fseek(fp, 0, SEEK_END);
   size = ftell(fp);
   //printf("\n%s",(path+2));
   Create_download(buf,(path+2),size);
   //printf("\n%s",buf);
   write(accept_fd, buf, strlen(buf));
  // printf("\n%d", size);
   
   rewind(fp);

   while (ftell(fp) < size - 1)
   {

       fread(buf, 1, 1999, fp);
       write(accept_fd, buf, 1999);
   }
   fclose(fp);
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
void HTMLTextGenerator(char *re,char **dir_list)
{
    int i=0;
    DIR *dirp;
    char (*p)[100] = dir_list;
    struct dirent *dir_info;
    char HTML_Text[5000]="<!DOCTYPE html>\
<html>\
<head>\
<title>Download</title>\
<style>\
    body {\
        width: 35em;\
        margin: 0 auto;\
        font-family: Tahoma, Verdana, Arial, sans-serif;\
    }\
</style>\
</head>\
<body>";
    char List[1000]="";
    dirp=opendir(".");
    while (dir_info=readdir(dirp))
    {
        
        strcpy(*(p++),dir_info->d_name);
        sprintf(List, "%s<br><a href=\" %s\">%s</a></br>\n", List, dir_info->d_name, dir_info->d_name);
        // printf("%s\n",dir_info->d_name);
        
    }
    strcat(HTML_Text,List);
    strcat(HTML_Text,"</body>\n</html>\r\n");
    

    closedir(dirp);
    strcpy(re,HTML_Text);
}
int main(int argc ,char **argv)
{
 int listen_fd,accept_fd;
 struct sockaddr_in     client_addr;
 int n;
 
 if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
        printf("Socket Error:%s\n\a",strerror(errno));
        exit(1);
  }
 
 bzero(&client_addr,sizeof(struct sockaddr_in));
 client_addr.sin_family=AF_INET;
 client_addr.sin_port=htons(MY_PORT);
 client_addr.sin_addr.s_addr=htonl(INADDR_ANY);
 n=1;

 setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&n,sizeof(int));
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
        int flag=0;
        char dir_list[100][100];
        char (*p)[100];
        
        char buffer[1024];
        char path[100]="";
        char HTMLText[5000]="";
        char html_header[100]="";
        close(listen_fd);

        HTMLTextGenerator(HTMLText,dir_list);
        

        p=dir_list;
        // while (*(p++)!=" ")
        // {
        //     printf("%s\n",*(p++));
        // }
        
        
      
        
        
        n=read(accept_fd,buffer,1024);
        

         printf("%s\n",buffer);
        
        strcut(buffer,"GET "," HTTP", path);
        printf("\n*%s*\n",path);
        // for (int j = 0; j < 10; j++)
        // {
        //     if (/* condition */)
        //     {
        //         /* code */
        //     }
            
        //     if(strcmp((path+1),*(p++))==0)
        // }
        
        if(strcmp(path,"/")==0)
        {
            Create_header(html_header, 200, "OK", "text/html;charset=utf-8");
            write(accept_fd,html_header,strlen(html_header));
            write(accept_fd,HTMLText,strlen(HTMLText));
        }
        else{
            printf("send............\n");
            char temp[105]=".";
            SEND_FILE(accept_fd,strcat(temp,path));

        }
        
        // Create_header(html_header, 200, "OK", "text/html;charset=utf-8");
        
        // write(accept_fd,html_header,strlen(html_header));
        // write(accept_fd,HTMLText,strlen(HTMLText));
        close(accept_fd);
        exit(0);
   }
   else if(n<0)
        printf("Fork Error:%s\n\a",strerror(errno));
   close(accept_fd);
  }
} 
