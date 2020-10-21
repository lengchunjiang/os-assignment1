#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if( argc != 2){
        fprintf(2, "there is wrong about the argument \n");
        exit(1);
    }
    int x = atoi(argv[1]);
    fprintf(1, "to sleep %d\n" ,x);
    sleep(x);
    exit(0);
}
