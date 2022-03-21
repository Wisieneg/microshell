#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#define BUFFER_SIZE 2048

void mv(char **args,int size){
    if(size>3){
        printf("Too many arguments\n");
        return;
    }
    int des=link(args[1],args[2]);
    if(des==-1){
        printf("%s\n",strerror(errno));
    }else{
        unlink(args[1]);
    }
}

void ls(char **args,int size){
    struct dirent *des;
    DIR *dr;
    if(size==1) dr=opendir(".");	
    if(size==2) dr=opendir(args[1]);
    if(size>2){
        printf("Too many arguments\n");
        return;
    }
    if(dr==NULL){
    	printf("%s\n",strerror(errno));
        return;
    }
    while((des=readdir(dr))!=NULL){
        char *nazwa=des->d_name;
        if(nazwa[0]!='.') printf("%s\n",nazwa);
    }
    closedir(dr);
}

void mk_dir(char **args,int size){
    int i=0,w;
    if(size==1) printf("Missing operand\n");
    else{
        for(i=1;i<size;i++){
            w=mkdir(args[i],0777);
            if(w==-1) printf("%s (%s)\n",strerror(errno),args[i]);
        }
    }
}

void touch(char **args,int size){
    int i;
    for(i=1;i<size;i++){
    int des=open(args[i],O_CREAT,S_IWRITE|S_IREAD);
    if(des==(-1)){
        printf("%s\n",strerror(errno));
    }
    close(des);
    }
}

void help(){
    printf(" Microshell by: Wojciech Wiśniewski\n");
    printf("----------------------------------------------------------\n");
    printf(" cd *path*           | zmienia bieżący katalog na *path*\n");
    printf("                     |\n");
    printf(" ls                  | wyświetla pliki w bieżacym katalogu\n");
    printf("                     |\n");
    printf(" pwd                 | wyswietla katalog w którym obecnie znajduje się użytkownik\n");
    printf("                     |\n");
    printf(" rm *file*           | usuwa plik *file*\n");
    printf("                     |\n");
    printf(" mv *file1* *file2*  | przenosi plik1 do lokalizacji plik2\n");
    printf("                     |\n");
    printf(" mkdir *katalog*     | tworzy folder o nazwie/ścieżce *katalog*\n");
    printf("                     |\n");
    printf(" touch *file*        | wysyła sygnał do pliku *file*, jeżeli plik o tej nazwie nie istnieje to zostaje on stworzony\n");
    printf("                     |\n");
    printf(" *path* lub ./*name* | wykonuje program o ścieżce *path* lub *name* gdy znajduje sie w bieżącym folderze\n");
    printf("                     |\n");
    printf(" exit                | kończy działanie programu\n");
}

void cd(char **args,int size){
    if(size>2) printf("Too many arguments\n");
    else{
        int d=chdir(args[1]);
        if(d==-1){
            printf("%s\n",strerror(errno));
        }
    }
}

void rm(char **args,int size){
    if(size==1) printf("Too few arguments\n");
    else{
        int i,des;
        for(i=1;i<size;i++){
            des=unlink(args[i]);
            if(des==-1){
                printf("%s-(%s)\n",strerror(errno),args[i]);
            }
        }
    }
}

void read_order(char **args,int size){
    if(strcmp(args[0],"pwd")==0){
        char cwd[1024];
        printf("%s\n",getcwd(cwd,1024));        
    }
    else if(strcmp(args[0],"exit")==0){
        system("clear");
        exit(0);       
    }
    else if(strcmp(args[0],"help")==0){
        help();
    }
    else if(strcmp(args[0],"touch")==0){
        touch(args,size);   
    }
    else if(strcmp(args[0],"cd")==0){
        cd(args,size);
    }
    else if(strcmp(args[0],"mkdir")==0){
        mk_dir(args,size);
    }
    else if(strcmp(args[0],"ls")==0){
        ls(args,size);
    }
    else if(strcmp(args[0],"mv")==0){
        mv(args,size);
    }
    else if(strcmp(args[0],"clear")==0){
        system("clear");
    }
    else if(strcmp(args[0],"rm")==0){
        rm(args,size);
    }
    else if(args[0][0]=='/'||(args[0][0]=='.'&&args[0][1]=='/')){
        pid_t pid=fork();
        if(pid==0){
           int des=execvp(args[0],args);
           if(des==-1){
                printf("%s\n",strerror(errno));
                exit(EXIT_FAILURE);
           }
        }else{
            waitpid(pid,0,0);
        }
    }
    else{
        printf("Unknown command\n");
    }
}

void extract_args(char line[BUFFER_SIZE]){
    int i,size=0;
    for(i=0;i<BUFFER_SIZE;i++){
        if(line[i]==' '){
            size++;
        }
        if(line[i]=='\n'){
            size++;
            line[i]=' ';
            break;
        }
    }
    char **args=(char**) calloc((size+1),sizeof(char*));
    char *del=" ";
    args[0]=strtok(line,del);
    for(i=1;i<size;i++){
        args[i]=strtok(NULL,del);
    }
    args[size]=NULL;
    read_order(args,size);
}

int main(){
    system("clear");
    while(true)
    {
        char cwd[1024];
        char line[BUFFER_SIZE];
        char *login=getlogin();
        getcwd(cwd,1024);
        printf("\033[0;36m%s@\033[0m[%s]$ ",login,cwd);
        fgets(line,BUFFER_SIZE,stdin);
        if(strcmp(line,"\n")==0) continue;
        extract_args(line);
    }
    return 0;
}
