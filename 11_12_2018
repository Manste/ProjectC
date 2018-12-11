#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#define key 12345
#define key_sem 54321

typedef struct{
    int num;
    int temps1;
    int temps2;
    int temps3;
    int tt;//temps total
    int pit;//pit stop
    int out;//arrêt
    pid_t pid;
} voiture;


voiture *voituresCourse;
voiture copieAffichage[20];


void courseTour() {
    int numVoiture[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
    for(int i = 0; i <= 19; i++){
        int f = fork();
        
        if( f == -1){
            perror("Impossible de créer un fils: Erreur fork.\n");
            exit(-1);
        }
        
        if(f == 0){
            srand(time(0) + getpid());
            int shmid2 = shmget(key, sizeof(voiture), IPC_CREAT | 0666);
            
            if(shmid2 == -1){
                perror("Création de segment impossible: Erreur shmget du fils.\n");
                exit(-1);
            }
            
            voituresCourse = (voiture *)shmat(shmid2, 0, 0);
            if(voituresCourse == (voiture*)-1){
                perror("Attachement impossible: Erreur shmat du fils.\n");
                exit(-1);
            }
            
            for(int i = 0; i < 100; i++){
                int a = rand()%(3300 - 2100) + 2100;
                int b = rand()%(3300 - 2100) + 2100;
                int c = rand()%(3300 - 2100) + 2100;
                int d = a + b + c;
                if(voituresCourse[i].tt == 0){
                    voituresCourse[i].num = numVoiture[i];
                    voituresCourse[i].temps1 = a;
                    voituresCourse[i].temps2 = b;
                    voituresCourse[i].temps3 = c;
                    voituresCourse[i].tt = d;
                    voituresCourse[i].pid = getpid();
                }
                else{
                    if(d < voituresCourse[i].tt){
                        voituresCourse[i].num = numVoiture[i];
                        voituresCourse[i].temps1 = a;
                        voituresCourse[i].temps2 = b;
                        voituresCourse[i].temps3 = c;
                        voituresCourse[i].tt = d;
                        voituresCourse[i].pid = getpid();
                    }
                }
            }
            //il est temps pour le fils de mourrir
            //mais avant il doit se détacher de la memoire partagee
            if(shmdt(voituresCourse) == -1){
                perror("détachement impossible: Erreur shmdt du fils.\n");
                exit(-1);
            }
            //le fils se suicide
            exit(1);
        }
    }
}


void initCourse(){
    int numVoiture[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
    for(int i = 0; i <= 19; i++){
        voituresCourse[i].num = numVoiture[i];
        voituresCourse[i].temps1 = 0;
        voituresCourse[i].temps2 = 0;
        voituresCourse[i].temps3 = 0;
        voituresCourse[i].tt = 0;
        voituresCourse[i].pid = 0;
    }
}

void trierTab(voiture voitureCopie[20]){
    for(int i = 0; i <= 19; i++){
        for(int j = 0; j <=18; j++){
            if(voitureCopie[j].tt > voitureCopie[j+1].tt){
                voiture voit = voitureCopie[j];
                voitureCopie[j]= voitureCopie[j+1];
                voitureCopie[j+1] = voit;
            }
        }
    }
}

void affichage(){
    system("clear");
    printf("COURSE P1 : première scéance d'essais !!!\n\n\n");
    printf("\t|\tS1\t\t|\tS2\t\t|\tS3\t\t|\tTT\t\t|\tPIT\t|\tOUT\t\n\n");
    voiture voitureCopie[20];
    
    // Semaphore
    int nsems = 1;
    int semflg = 0;
    
    if(int semid = semget(key_sem, nsems, semflg) == -1){
        perror("semget: semget failed");
        //exit(1);
    }
    
    typedef struct operation{
        short int ind_sem ;
        short op;
        short flag;
    };
    
    // opérations à réaliser atomiquement
    operation *ops;
    
    ops[0].ind_sem = 0;
    ops[0].op = +1;
    ops[0].flag = SEM_UNDO;
    
    ops[1].ind_sem = 0;
    ops[1].op = -1;
    ops[1].flag = SEM_UNDO;
    
    if(semop(semid, struct operation *ops, 0) == -1){
        perror("erruer de semop");
    };
    
    // copie de mémoire
    
    memcpy(&voitureCopie, voituresCourse, 20*sizeof(voiture));
    
    // tri
    
    trierTab(voitureCopie);
    
    // affichage
    for(int i = 0; i <= 19; i++){
        sleep(1);
        int div1 = voitureCopie[i].temps1 / 60;
        int res1 = voitureCopie[i].temps1 % 60;
        
        int div2 = voitureCopie[i].temps2 / 60;
        int res2 = voitureCopie[i].temps2 % 60;
        
        int div3 = voitureCopie[i].temps3 / 60;
        int res3 = voitureCopie[i].temps3 % 60;
        
        int div = voitureCopie[i].tt /3600;
        int res = (voitureCopie[i].tt % 3600) / 60;
        int mn = (voitureCopie[i].tt % 3600) % 60;
        // affichage
        printf("%d\t|\t00:%d:%d m\t|\t00:%d:%d m\t|\t00:%d:%d m\t|\t0%d:%d:%d m\t|\t(P)\t|\tIN\t\n", voituresCourse[i].num, div1, res1, div2, res2, div3, res3, div, res, mn);
    }
    printf("\n\nPremiere course terminee\n\n\n");
    
}

void courseSession(int trs){
    for(int i= 0; i<=trs; i++){
        courseTour();
        affichage();
    }
}

int main(int argc, char *argv[]){
    void initCourse();
    int shmid = shmget(key, sizeof(voiture), IPC_CREAT | 0666);
    if(shmid == -1){
        perror("Création de segment impossible: Erreur shmget du père.\n");
        exit(-1);
    }
    
    voituresCourse = (voiture *)shmat(shmid, 0, 0);
    if(voituresCourse == (voiture*)-1){
        perror("Attachement impossible: Erreur shmat du pére.\n");
        exit(-1);
    }
    
    courseSession(5);
    
    if(shmdt(voituresCourse) == -1){
        perror("détachement impossible: Erreur shmdt du père.\n");
        exit(-1);
    }
    
    if(shmctl(shmid, IPC_RMID,  (struct shmid_ds *) NULL) == -1){
        perror("destruction de la memoire partagee n'est pas faite: Erreur shmctl.\n");
        exit(-1);
    }
    exit(1);
    //le père attend la mort du fils
    wait(0);
    
    return 1;
}
