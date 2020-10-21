#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include<math.h>
#include <pthread.h>

#define NUM_THREADS 3
#define MAX_PRODUCT 20 
#define BUFFER_SIZE 20

typedef struct buf	
{		
	int rear;			
	int front;				
	int buffer[20];			
};

int exp_random(double lambda)
{
	double pV = 0.0;
    while(1)
    {
        pV = (double)rand()/(double)RAND_MAX;
        if (pV != 1)
        {
            break;
        }
    }
    pV = (-1.0/lambda)*log(1-pV);
    return (int)(pV * 1000000);
}

sem_t *full;
sem_t *empty;
sem_t *s_mutex ;//mutex for struct
pthread_t tid[NUM_THREADS];
void *ptr;


void *producer(void *param){
    double lambda = (* (int *) param) * 1.0;
    pthread_t tid = pthread_self();
    do{
        int sleep_time = exp_random(lambda);
        usleep(sleep_time);
        int item = rand() % 10;
        struct buf *shm_ptr = ((struct buf *) ptr);
        sem_wait(empty);
        sem_wait(s_mutex);
        printf("Producing the data %d to buffer[%d] by id %lu \n",item,shm_ptr->rear,(unsigned long)tid);
        shm_ptr->buffer[shm_ptr->rear] = item;
        shm_ptr->rear = (shm_ptr->rear+1) % BUFFER_SIZE;
        sem_post(s_mutex);//Unlock the binary-mutex
        sem_post(full);// Add a full buffer
    }while(1);
    pthread_exit(0);
}

//int argc, char** argv
int main(int argc, char** argv)
{
    struct buf shareMemory;
    memset(&shareMemory,0,sizeof(struct buf));
    //��ʼ���ڴ�ռ䣬������ȫ���ó�Ϊ0
    full = sem_open("full", O_CREAT,0666,0);
    //sem_t *sem_open(const char *name, int oflag,/*mode_t mode, unsigned int value*/);
    //name:�ź���������.  oflag: ʹ��O_CREAT ����ź����Ѿ����ڣ���ô��ʲôҲ����������Ҳ���ᱨ��
    //mode: ��ʾ˭���Է����ź����� value��ָ���ź����ĳ�ʼֵ��
    empty = sem_open("empty",O_CREAT,0666,0);
    s_mutex = sem_open("mutex",O_CREAT,0666,0);

    sem_init(full,1,0);
    //extern int sem_init __P ((sem_t *__sem, int __pshared, unsigned int __value));����
    // semΪָ���ź����ṹ��һ��ָ�룻
    // pshared��Ϊ��ʱ���ź����ڽ��̼乲������ֻ��Ϊ��ǰ���̵������̹߳���
    // value�������ź����ĳ�ʼֵ��
    sem_init(empty,1,BUFFER_SIZE);
    sem_init(s_mutex,1,1);

    int shm_fd = shm_open("buffer",O_CREAT|O_RDWR,0666);
    ftruncate(shm_fd,sizeof(struct buf));
    //���ٳ��ռ䣬fd�������ǳ�˵���ļ����
    ptr = mmap(0,sizeof(struct buf), PROT_WRITE, MAP_SHARED,shm_fd,0);
    //void* mmap(void* start,size_t length,int prot,int flags,int fd,off_t offset);
    //start:��Ҫӳ��ĵ��ڴ����ʼ���ʣ�ͨ������ΪNULL�� length: ���Ĳ���ӳ�䵽�ڴ�
    //prot ӳ������ı�����ʽ��PROT_EXEC ӳ������ɱ�ִ�� PROT_READ ӳ������ɱ���ȡPROT_WRITE ӳ������ɱ�д�� PROT_NONE ӳ�������ܴ�ȡ
    //falgs map_shared ��ӳ�������д�����ݻḴ�ƻ��ļ��ڣ�������������ӳ����ļ��Ľ��̹���
    //fd Ҫӳ�䵽�ڴ��е��ļ�������
    //offset ƫ����                        �ɹ��򷵻��ڴ���ʼ����
    int lambda = atoi(argv[1]);
    int lambda_1 = 1;
    int lambda_2 = 2;
    int lambda_3 = 3;
    pthread_t tid1,tid2,tid3;/*The thread identifier*/
    pthread_attr_t attr1,attr2,attr3;/*Set of thread attributes*/
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);
    pthread_create(&tid1,&attr1,producer,&lambda);
    pthread_create(&tid2,&attr2,producer,&lambda);
    pthread_create(&tid3,&attr3,producer,&lambda);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    return 0;
}
