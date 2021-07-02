#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>    
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
    int rf;
    if ((rf = open(argv[1], O_RDONLY)) < 0) {
    perror("open");
    exit(1);
    }
    int wf1;
    if ((wf1 = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT)) < 0) {
        perror("open1");
        exit(1);
    }
    int wf2;
    if ((wf2 = open(argv[3], O_WRONLY|O_TRUNC|O_CREAT)) < 0) {
        perror("open2");
        exit(1);
    }
    int split_n = atoi(argv[4]);
    char c[1]={'!'};
    ssize_t error;
    error = read(rf, c, sizeof(char));
    for(int i=0;i<split_n && error != -1;i++){
        write (wf1,c, 1);
        error = read(rf, c, sizeof(c[0]));
    }

    off_t ret = -1;
    off_t pre_ret = 100;
    error = read(rf, c, sizeof(c[0]));
    for(; pre_ret!=ret ;){
        write (wf2,c, 1);
        error = read(rf, c, sizeof(c[0]));
        pre_ret = ret;
        ret = lseek (rf, (off_t) 0, SEEK_CUR);
    }
    close(rf);
    close(wf1);
    close(wf2);
    return 0;
}
