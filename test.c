#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define keysem 54321
#define shmkey 12345#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define keysem 54321
#define shmkey 12345

int semid;
union semun {//Structure de données du sémaphores
   int val;
};
struct sembuf sem_op;
typedef struct{
  int num;
  int mn1;
  int mn2;
  int mn3;
  int ss1;
  int ss2;
  int ss3;
  int ss;
  int tt;//temps total
  char *pit;//pit stop
  char *out;//arrêt
  pid_t pid;
  int echec;
} voiture;

voiture *voituresCourse;

int demandeAcces(int sem_id, int sem_num)
{
  //sem_num: index du semaphore dans l'array
  //SEM_UNDO comme flag pour que le noyau ajuste automatiquement la valeur
  //du semaphore si le processus exit.
	sem_op.sem_num=sem_num;
	sem_op.sem_op=-1;
	sem_op.sem_flg=SEM_UNDO;

	return semop(sem_id, &sem_op, 1);
}

int rendreAcces(int sem_id, int sem_num)
{
	sem_op.sem_num=sem_num;
	sem_op.sem_op=+1;
	sem_op.sem_flg=SEM_UNDO;

	return semop(sem_id, &sem_op, 1);
}

int initSemaphore(int sem_id, int sem_num, int init)
{
	union semun semunion;
	semunion.val=init;
	return semctl(sem_id,sem_num,SETVAL,semunion);
}

void courseTour(int nbVoitures, int km) {
  int numVoiture[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
  int i;
  for(i = 0; i < 20; i++){
    int f = fork();

    if( f == -1){
      perror("Impossible de créer un fils: Erreur fork.\n");
      exit(-1);
    }

    if(f == 0){

      srand(time(0) + getpid());
      int shmid2 = shmget(shmkey, sizeof(voiture), IPC_CREAT | 0666);

      if(shmid2 == -1){
        perror("Création de segment impossible: Erreur shmget du fils.\n");
        exit(-1);
      }

      voituresCourse = (voiture *)shmat(shmid2, 0, 0);
      if(voituresCourse == (voiture*)-1){
        perror("Attachement impossible: Erreur shmat du fils.\n");
        exit(-1);
      }
      int j;

      if(voituresCourse[i].echec == 0 || voituresCourse[i].out != "O"){
        voituresCourse = (voiture *)shmat(shmid2, 0, 0);
        if(voituresCourse == (voiture*)-1){
          perror("Attachement impossible: Erreur shmat du fils.\n");
          exit(-1);
        }
        int j;
        voituresCourse[i].num = numVoiture[i];
        for(j = 0; j <= km; j++){
          int a = rand()%(60 - 35) + 35;
          int b = rand()%(60 - 35) + 35;
          int c = rand()%(60 - 35) + 35;

          int somme = 0;
          somme += a;//pour avoir des resultats atomiques
          somme += b;
          somme += c;

          int d = rand()%(60 - 35) + 35;
          int e = rand()%(60 - 35) + 35;
          int f = rand()%(60 - 35) + 35;

          int g = d + e + f;
          int h = g%60;
          int l = g/60;
          somme += l;
          if(voituresCourse[i].tt == 0){
            voituresCourse[i].mn1 = a;
            voituresCourse[i].mn2 = b;
            voituresCourse[i].mn3 = c;

            voituresCourse[i].ss1 = d;
            voituresCourse[i].ss2 = e;
            voituresCourse[i].ss3 = f;

            voituresCourse[i].ss = h;
            voituresCourse[i].tt = somme;
            voituresCourse[i].pid = getpid();
          }
          else{
            if(somme < voituresCourse[i].tt && voituresCourse[i].out != "O" && voituresCourse[i].pit != "P"){
              voituresCourse[i].mn1 = a;
              voituresCourse[i].mn2 = b;
              voituresCourse[i].mn3 = c;

              voituresCourse[i].ss1 = d;
              voituresCourse[i].ss2 = e;
              voituresCourse[i].ss3 = f;

              voituresCourse[i].ss = h;
              voituresCourse[i].tt = somme;

              voituresCourse[i].pid = getpid();

              if(voituresCourse[i].pit == "P"){
                voituresCourse[i].pit = " ";
              }
              if(voituresCourse[i].ss2 == 59){
                voituresCourse[i].out = "O";
              }
              if(voituresCourse[i].ss3 == 59){
                voituresCourse[i].pit = "P";
              }
            }
            sleep(1);
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
  int i;
  int numVoiture[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
  for(i = 0; i < 20; i++){
    voituresCourse[i].num = numVoiture[i];
    voituresCourse[i].mn1 = 60;
    voituresCourse[i].ss1 = 60;
    voituresCourse[i].mn2 = 60;
    voituresCourse[i].ss2 = 60;
    voituresCourse[i].mn3 = 60;
    voituresCourse[i].ss3 = 60;
    voituresCourse[i].tt = 500;
    voituresCourse[i].ss = 0;
    voituresCourse[i].pid = 0;
    voituresCourse[i].out = " ";
    voituresCourse[i].pit = " ";
  }
}

void initVoituresCourse(){
  int i;
  for(i = 0; i < 20; i++){
    if(voituresCourse[i].echec == 0){
      voituresCourse[i].mn1 = 0;
      voituresCourse[i].ss1 = 0;
      voituresCourse[i].mn2 = 0;
      voituresCourse[i].ss2 = 0;
      voituresCourse[i].mn3 = 0;
      voituresCourse[i].ss3 = 0;
      voituresCourse[i].tt = 0;
      voituresCourse[i].ss = 0;
      voituresCourse[i].pid = 0;
      voituresCourse[i].out = " ";
      voituresCourse[i].pit = " ";
    }
  }
}

void trierTab(voiture *voitureCopie){
  int i;
  for(i = 0; i <= 19; i++){
    int j;
    for(j = 0; j <= 18; j++){
      if(voitureCopie[j].tt > voitureCopie[j+1].tt){
        voiture voit = voitureCopie[j];
        voitureCopie[j]= voitureCopie[j+1];
        voitureCopie[j+1] = voit;
      }
      else if (voitureCopie[j].tt == voitureCopie[j+1].tt){
        if (voitureCopie[j].ss > voitureCopie[j+1].ss){
          voiture voit = voitureCopie[j];
          voitureCopie[j]= voitureCopie[j+1];
          voitureCopie[j+1] = voit;
        }

      }
    }
  }
}

void trieFinale(voiture *voitureCopie){
  int i;
  trierTab(voitureCopie);
  wait(NULL);
  for(i = 0; i <= 19; i++){
    int j;
    for(j = 0; j <= 18; j++){
      if(voitureCopie[j].out == " " ){
        if(voitureCopie[j].tt > voitureCopie[j+1].tt){
          voiture voit = voitureCopie[j];
          voitureCopie[j]= voitureCopie[j+1];
          voitureCopie[j+1] = voit;
        }
        else if (voitureCopie[j].tt == voitureCopie[j+1].tt){
          if (voitureCopie[j].ss > voitureCopie[j+1].ss){
            voiture voit = voitureCopie[j];
            voitureCopie[j]= voitureCopie[j+1];
            voitureCopie[j+1] = voit;
          }
        }
      }
      else{
        /*for(int a = j+1; a <= 19; a++){
          voiture voit = voitureCopie[j];
          voitureCopie[j]= voitureCopie[j+1];
          voitureCopie[j+1] = voit;
        }*/

      voiture voit = voitureCopie[j];
      voitureCopie[j]= voitureCopie[19];
      voitureCopie[19] = voit;
      }
    }
  }
}

int trouverVoitMn1(voiture *voitureCopie, int mn, int ss){
  int i;
  for(i = 0; i < 20; i++){
    if(voitureCopie[i].mn1 == mn && voitureCopie[i].ss1 == ss ){
      return voitureCopie[i].num;
    }
  }
  //return 0;
}

int trouverVoitMn2(voiture *voitureCopie, int mn, int ss){
  int i;
  for(i = 0; i < 20; i++){
    if(voitureCopie[i].mn2 == mn && voitureCopie[i].ss2 == ss ){
      return voitureCopie[i].num;
    }
  }
  //return 0;
}

int trouverVoitMn3(voiture *voitureCopie, int mn, int ss){
  int i;
  for(i = 0; i < 20; i++){
    if(voitureCopie[i].mn3 == mn && voitureCopie[i].ss3 == ss ){
      return voitureCopie[i].num;
    }
  }
  //return 0;
}

int minS1(voiture *voitureCopie){
  int i;
  int mnMin = voitureCopie[0].mn1;
  int ssMin = voitureCopie[0].ss1;
  for(i = 1; i < 20; i++){
    if(voitureCopie[i].mn1 < mnMin){
      mnMin = voitureCopie[i].mn1;
      ssMin = voitureCopie[i].ss1;
    }
    else if(voitureCopie[i].mn1 == mnMin){
      if(ssMin > voitureCopie[i].ss1){
        ssMin = voitureCopie[i].ss1;
      }
    }
  }
  return trouverVoitMn1(voitureCopie, mnMin, ssMin);
}

int minS2(voiture *voitureCopie){
  int i;
  int mnMin = voitureCopie[0].mn2;
  int ssMin = voitureCopie[0].ss2;
  for(i = 1; i < 20; i++){
    if(voitureCopie[i].mn2 < mnMin){
      mnMin = voitureCopie[i].mn2;
    }
    else if(voitureCopie[i].mn2 == mnMin){
      if(ssMin > voitureCopie[i].ss2){
        ssMin = voitureCopie[i].ss2;
      }
    }
  }
  return trouverVoitMn2(voitureCopie, mnMin, ssMin);
}

int minS3(voiture *voitureCopie){
  int i;
  int mnMin = voitureCopie[0].mn3;
  int ssMin = voitureCopie[0].ss3;
  for(i = 1; i < 20; i++){
    if(voitureCopie[i].mn3 < mnMin){
      mnMin = voitureCopie[i].mn3;
    }
    else if(voitureCopie[i].mn3 == mnMin){
      if(ssMin > voitureCopie[i].ss3){
        ssMin = voitureCopie[i].ss3;
      }
    }
  }
  return trouverVoitMn3(voitureCopie, mnMin, ssMin);
}

void affichage(char str1[], int nbVoitures, int finale){
  system("clear");
  printf("%s", str1);
  printf("\t\tRang\t|\tNumeros\t\t|\tS1\t\t|\tS2\t\t|\tS3\t\t|\tTT\t\t|\tPIT\t|\tOUT\t\n\n");
  voiture voitureCopie[20];

  if(demandeAcces(semid,0)==-1){		// Demande l'acces a la memoire partagee
		perror("Erreur : Demande d'accès à la mémoire impossible");
		exit(-1);
	}

  memcpy(&voitureCopie, voituresCourse, 20*sizeof(voiture));

  if(rendreAcces(semid,0)==-1){		// Rend l'acces a la memoire partagee
		perror("Erreur: Rendre l'accès à la mémoire impossible");
		exit(-1);
	}

  if(finale == 0){
    trierTab(voitureCopie);
  }
  else{
    trieFinale(voitureCopie);
  }

  int i;
  for(i = 0; i < nbVoitures; i++){
    int div = voitureCopie[i].tt / 60;
    int res = voitureCopie[i].tt % 60;

    if(voituresCourse[i].tt != 600){
      printf("\t\t%d\t|\t%d\t\t|\t00:%d:%d mn\t|\t00:%d:%d mn\t|\t00:%d:%d mn\t|\t%d:%d:%d mn\t|\t%s\t|\t\%s\t\n",i+1, voitureCopie[i].num, voitureCopie[i].mn1, voitureCopie[i].ss1,
              voitureCopie[i].mn2, voitureCopie[i].ss2, voitureCopie[i].mn3, voitureCopie[i].ss3, div, res, voitureCopie[i].ss, voituresCourse[i].pit, voituresCourse[i].out);
    }

  }
  for(i = 0; i < (20 - nbVoitures); i++){
    printf("\n");
  }
  printf("\n\n\n\tMeilleur S1: \t%d\n", minS1(voitureCopie));
  printf("\tMeilleur S2: \t%d\n", minS2(voitureCopie));
  printf("\tMeilleur S3: \t%d\n", minS3(voitureCopie));
  printf("\tMeilleur de la course en elle-même: \t%d\n\n\n", voitureCopie[0].num);

  sleep(1);
}

void courseSession(char str1[], int trs, int km, int nbVoitures, int finale){
  int i;
  for(i= 0; i<trs; i++){
    courseTour(nbVoitures, km);
    affichage(str1, nbVoitures, finale);
  }
}

void mettreAjourVoitCourse(int nb){
  voiture voitureCopie[20];
  memcpy(&voitureCopie, voituresCourse, 20*sizeof(voiture));
  trierTab(voituresCourse);
  initCourse();
  voiture copie[nb];
  int j, i;
  for( i = 0; i < nb; i++){
    copie[i] = voitureCopie[i];
  }

  for( i = 0; i < nb; i++){
    for(j = 0; j <= 20; j++){
      if(copie[i].num == voituresCourse[i].num){
        voituresCourse[i] = copie[i];
      }
    }
  }
  for(i = 0; i < 20; i++){
    if(voituresCourse[i].pid == 0 && voituresCourse[i].tt == 500){
      voituresCourse[i].echec = 1;
    }
  }

}

int main(int argc, char *argv[]){

  int shmid = shmget(shmkey, sizeof(voiture), IPC_CREAT | 0666);
  if(shmid == -1){
    perror("Création de segment impossible: Erreur shmget du père.\n");
    exit(-1);
  }

  semid = semget(keysem, 1, IPC_CREAT|0666);

  if(initSemaphore(semid,0,1)==-1){				// Initialisation a 1 du 1er semaphore du groupe
		perror("initSemaphore :");
		exit(-1);
	}

  voituresCourse = (voiture *)shmat(shmid, 0, 0);

  if(voituresCourse == (voiture*)-1){
    perror("Attachement impossible: Erreur shmat du pére.\n");
    exit(-1);
  }

  initCourse();
  initVoituresCourse();
  sleep(1);
  courseSession("\n\tCOURSE P1 : Premiere scéance d'essais !!!\n\n\n", 30, 20, 20, 0);
  printf("%s\n", "\tPremière course terminee\n\n\n" );
  sleep(1);
  initCourse();
  initVoituresCourse();
  sleep(1);
  courseSession("\n\tCOURSE P2 : Deuxième scéance d'essais !!!\n\n\n", 30, 20, 20, 0);
  printf("%s\n", "\tDeuxieme course terminee\n\n\n" );
  sleep(1);
  initCourse();
  initVoituresCourse();
  sleep(1);
  courseSession("\n\tCOURSE P3 : Troisième scéance d'essais !!!\n\n\n", 10, 20, 20, 0);
  printf("%s\n", "\tTroisième course terminee\n\n\n" );
  sleep(1);

  initCourse();
  initVoituresCourse();
  courseSession("\n\tDébut de la séance de qualification\n\tCOURSE Q1 : Premiere scéance de qualification !!!\n\n\n", 18, 20, 20, 0);
  printf("%s\n", "\tPremière course terminee\n\n\n" );
  sleep(1);

  mettreAjourVoitCourse(15);
  initVoituresCourse();
  courseSession("\n\n\n\tCOURSE Q2 : Deuxième scéance de qualification !!!\n\n\n", 15, 20, 15, 0);
  printf("%s\n", "\tDeuxieme course terminee\n\n\n" );
  sleep(1);

  mettreAjourVoitCourse(10);
  initVoituresCourse();
  courseSession("\n\n\n\tCOURSE Q3 : Troisième scéance de qualification !!!\n\n\n", 12, 20, 10, 0);
  printf("%s\n", "\tTroisième course terminee\n\n\n" );
  sleep(1);

  printf("%s\n", "\tFin de qualification! \n");
  affichage("\n\n\n\tLe classement pour la finale est:\n\n\n", 10, 0);
  sleep(2);

  /**finale**/
  mettreAjourVoitCourse(10);
  initVoituresCourse();
  courseSession("\n\n\n\tLA FINALE\n\n\n", 20, 20, 10, 1);
  printf("%s\n", "\tFin De La Finale\n\n\n" );
  sleep(1);
  affichage("\n\n\n\n\t Classement Finale\n\n\n", 10, 1);

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

union semun {
             int val;
             struct semid_ds *buf;
             unsigned short *array;
             struct seminfo *__buf;
};

typedef struct{
    int num;
    int mn1;
    int mn2;
    int mn3;
    int ss1;
    int ss2;
    int ss3;
    int ss;
    int tt;//temps total
    char *pit;//pit stop
    char *out;//arrêt
    pid_t pid;
    int echec;
} voiture;

voiture *voituresCourse;

int down(int sem_id, int sem_num)
{
	struct sembuf sem_op;
	sem_op.sem_num=sem_num;
	sem_op.sem_op=-1;
	sem_op.sem_flg=0;

	return semop(sem_id, &sem_op, 1);
}

int up(int sem_id, int sem_num)
{
	struct sembuf sem_op;
	sem_op.sem_num=sem_num;
	sem_op.sem_op=+1;
	sem_op.sem_flg=0;

	return semop(sem_id, &sem_op, 1);
}

int initialize(int sem_id, int sem_num, int init)
{
	union semun semunion;
	semunion.val=init;
	return semctl(sem_id,sem_num,SETVAL,semunion);
}

void courseTour(int nbVoitures, int km) {
  int numVoiture[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
  int i;
  for(i = 0; i < 20; i++){
    int f = fork();

    if( f == -1){
      perror("Impossible de créer un fils: Erreur fork.\n");
      exit(-1);
    }

    if(f == 0){
      srand(time(0) + getpid());
      int shmid2 = shmget(shmkey, sizeof(voiture), IPC_CREAT | 0666);

      if(shmid2 == -1){
        perror("Création de segment impossible: Erreur shmget du fils.\n");
        exit(-1);
      }

      voituresCourse = (voiture *)shmat(shmid2, 0, 0);
      if(voituresCourse == (voiture*)-1){
        perror("Attachement impossible: Erreur shmat du fils.\n");
        exit(-1);
      }
      int j;

      if(voituresCourse[i].echec == 0 || voituresCourse[i].out != "O"){
        voituresCourse = (voiture *)shmat(shmid2, 0, 0);
      if(voituresCourse == (voiture*)-1){
        perror("Attachement impossible: Erreur shmat du fils.\n");
        exit(-1);
      }
      int j;
      voituresCourse[i].num = numVoiture[i];
      for(j = 0; j <= km; j++){
        int a = rand()%(60 - 35) + 35;
        int b = rand()%(60 - 35) + 35;
        int c = rand()%(60 - 35) + 35;

        int somme = 0;
        somme += a;//pour avoir des resultats atomiques
        somme += b;
        somme += c;

        int d = rand()%(60 - 35) + 35;
        int e = rand()%(60 - 35) + 35;
        int f = rand()%(60 - 35) + 35;

        int g = d + e + f;
        int h = g%60;
        int l = g/60;
        somme += l;
        if(voituresCourse[i].tt == 0){
          voituresCourse[i].mn1 = a;
          voituresCourse[i].mn2 = b;
          voituresCourse[i].mn3 = c;

          voituresCourse[i].ss1 = d;
          voituresCourse[i].ss2 = e;
          voituresCourse[i].ss3 = f;

          voituresCourse[i].ss = h;
          voituresCourse[i].tt = somme;
          voituresCourse[i].pid = getpid();
        }
        else{
          if(somme < voituresCourse[i].tt && voituresCourse[i].out != "O" && voituresCourse[i].pit != "P"){
            voituresCourse[i].mn1 = a;
            voituresCourse[i].mn2 = b;
            voituresCourse[i].mn3 = c;

            voituresCourse[i].ss1 = d;
            voituresCourse[i].ss2 = e;
            voituresCourse[i].ss3 = f;

            voituresCourse[i].ss = h;
            voituresCourse[i].tt = somme;
            voituresCourse[i].out = "   ";
            voituresCourse[i].pit = "   ";

            voituresCourse[i].pid = getpid();

            if(voituresCourse[i].pit == "P"){
              voituresCourse[i].pit = "   ";
            }
            if(voituresCourse[i].ss2 == 58){
              voituresCourse[i].out = "O";
            }
            if(voituresCourse[i].ss3 == 59){
              voituresCourse[i].pit = "P";
            }
          }
          sleep(1);
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
  int i;
  int numVoiture[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
  for(i = 0; i < 20; i++){
    voituresCourse[i].num = numVoiture[i];
    voituresCourse[i].mn1 = 60;
    voituresCourse[i].ss1 = 60;
    voituresCourse[i].mn2 = 60;
    voituresCourse[i].ss2 = 60;
    voituresCourse[i].mn3 = 60;
    voituresCourse[i].ss3 = 60;
    voituresCourse[i].tt = 500;
    voituresCourse[i].ss = 0;
    voituresCourse[i].pid = 0;
    voituresCourse[i].out = "   ";
    voituresCourse[i].pit = "   ";
  }
}

void initVoituresCourse(){
  int i;
  for(i = 0; i < 20; i++){
    if(voituresCourse[i].echec == 0){
      voituresCourse[i].mn1 = 0;
      voituresCourse[i].ss1 = 0;
      voituresCourse[i].mn2 = 0;
      voituresCourse[i].ss2 = 0;
      voituresCourse[i].mn3 = 0;
      voituresCourse[i].ss3 = 0;
      voituresCourse[i].tt = 0;
      voituresCourse[i].ss = 0;
      voituresCourse[i].pid = 0;
      voituresCourse[i].out = "   ";
      voituresCourse[i].pit = "   ";
    }
  }
}

void trierTab(voiture *voitureCopie){
  int i;
  for(i = 0; i <= 19; i++){
    int j;
    for(j = 0; j <= 18; j++){
      if(voitureCopie[j].tt > voitureCopie[j+1].tt){
        voiture voit = voitureCopie[j];
        voitureCopie[j]= voitureCopie[j+1];
        voitureCopie[j+1] = voit;
      }
      else if (voitureCopie[j].tt == voitureCopie[j+1].tt){
        if (voitureCopie[j].ss > voitureCopie[j+1].ss){
          voiture voit = voitureCopie[j];
          voitureCopie[j]= voitureCopie[j+1];
          voitureCopie[j+1] = voit;
        }

      }
    }
  }
}

void trieFinale(voiture *voitureCopie){
  int i;
  trierTab(voitureCopie);
  wait(NULL);
  for(i = 0; i <= 19; i++){
    int j;
    for(j = 0; j <= 18; j++){
      if(voitureCopie[j].out == "   " ){
        if(voitureCopie[j].tt > voitureCopie[j+1].tt){
          voiture voit = voitureCopie[j];
          voitureCopie[j]= voitureCopie[j+1];
          voitureCopie[j+1] = voit;
        }
        else if (voitureCopie[j].tt == voitureCopie[j+1].tt){
          if (voitureCopie[j].ss > voitureCopie[j+1].ss){
            voiture voit = voitureCopie[j];
            voitureCopie[j]= voitureCopie[j+1];
            voitureCopie[j+1] = voit;
          }
        }
      }
    }
  }
}

int trouverVoitMn1(voiture *voitureCopie, int mn, int ss){
  int i;
  for(i = 0; i < 20; i++){
    if(voitureCopie[i].mn1 == mn && voitureCopie[i].ss1 == ss ){
      return voitureCopie[i].num;
    }
  }
  //return 0;
}

int trouverVoitMn2(voiture *voitureCopie, int mn, int ss){
  int i;
  for(i = 0; i < 20; i++){
    if(voitureCopie[i].mn2 == mn && voitureCopie[i].ss2 == ss ){
      return voitureCopie[i].num;
    }
  }
  //return 0;
}

int trouverVoitMn3(voiture *voitureCopie, int mn, int ss){
  int i;
  for(i = 0; i < 20; i++){
    if(voitureCopie[i].mn3 == mn && voitureCopie[i].ss3 == ss ){
      return voitureCopie[i].num;
    }
  }
  //return 0;
}

int minS1(voiture *voitureCopie){
  int i;
  int mnMin = voitureCopie[0].mn1;
  int ssMin = voitureCopie[0].ss1;
  for(i = 1; i < 20; i++){
    if(voitureCopie[i].mn1 < mnMin){
      mnMin = voitureCopie[i].mn1;
      ssMin = voitureCopie[i].ss1;
    }
    else if(voitureCopie[i].mn1 == mnMin){
      if(ssMin > voitureCopie[i].ss1){
        ssMin = voitureCopie[i].ss1;
      }
    }
  }
  return trouverVoitMn1(voitureCopie, mnMin, ssMin);
}

int minS2(voiture *voitureCopie){
  int i;
  int mnMin = voitureCopie[0].mn2;
  int ssMin = voitureCopie[0].ss2;
  for(i = 1; i < 20; i++){
    if(voitureCopie[i].mn2 < mnMin){
      mnMin = voitureCopie[i].mn2;
    }
    else if(voitureCopie[i].mn2 == mnMin){
      if(ssMin > voitureCopie[i].ss2){
        ssMin = voitureCopie[i].ss2;
      }
    }
  }
  return trouverVoitMn2(voitureCopie, mnMin, ssMin);
}

int minS3(voiture *voitureCopie){
  int i;
  int mnMin = voitureCopie[0].mn3;
  int ssMin = voitureCopie[0].ss3;
  for(i = 1; i < 20; i++){
    if(voitureCopie[i].mn3 < mnMin){
      mnMin = voitureCopie[i].mn3;
    }
    else if(voitureCopie[i].mn3 == mnMin){
      if(ssMin > voitureCopie[i].ss3){
        ssMin = voitureCopie[i].ss3;
      }
    }
  }
  return trouverVoitMn3(voitureCopie, mnMin, ssMin);
}

void affichage(char str1[], int nbVoitures, int finale){
  system("clear");
  printf("%s", str1);
  printf("\t\tRang\t|\tNumeros\t\t|\tS1\t\t|\tS2\t\t|\tS3\t\t|\tTT\t\t|\tPIT\t|\tOUT\t\n\n");
  voiture voitureCopie[20];

  int semid=semget(keysem,1,IPC_CREAT|0666);
  initialize(semid,0,1);
  down(semid,0);

  memcpy(&voitureCopie, voituresCourse, 20*sizeof(voiture));

  up(semid, 0);

  if(finale == 0){
    trierTab(voitureCopie);
  }
  else{
    trieFinale(voitureCopie);
  }

  int i;
  for(i = 0; i < nbVoitures; i++){
    int div = voitureCopie[i].tt / 60;
    int res = voitureCopie[i].tt % 60;

    if(voituresCourse[i].tt != 600){
      printf("\t\t%d\t|\t%d\t\t|\t00:%d:%d mn\t|\t00:%d:%d mn\t|\t00:%d:%d mn\t|\t%d:%d:%d mn\t|\t%s\t|\t\%s\t\n",i+1, voitureCopie[i].num, voitureCopie[i].mn1, voitureCopie[i].ss1,
              voitureCopie[i].mn2, voitureCopie[i].ss2, voitureCopie[i].mn3, voitureCopie[i].ss3, div, res, voitureCopie[i].ss, voituresCourse[i].pit, voituresCourse[i].out);
    }

  }
  for(i = 0; i < (20 - nbVoitures); i++){
    printf("\n");
  }
  printf("\n\n\n\tMeilleur S1: \t%d\n", minS1(voitureCopie));
  printf("\tMeilleur S2: \t%d\n", minS2(voitureCopie));
  printf("\tMeilleur S3: \t%d\n", minS3(voitureCopie));
  printf("\tMeilleur de la course en elle-même: \t%d\n\n\n", voitureCopie[0].num);

  sleep(1);
}

void courseSession(char str1[], int trs, int km, int nbVoitures, int finale){
  int i;
  for(i= 0; i<trs; i++){
    courseTour(nbVoitures, km);
    affichage(str1, nbVoitures, finale);
  }
}

void mettreAjourVoitCourse(int nb){
  voiture voitureCopie[20];
  memcpy(&voitureCopie, voituresCourse, 20*sizeof(voiture));
  trierTab(voituresCourse);
  initCourse();
  voiture copie[nb];
  int j, i;
  for( i = 0; i < nb; i++){
    copie[i] = voitureCopie[i];
  }

  for( i = 0; i < nb; i++){
    for(j = 0; j <= 20; j++){
      if(copie[i].num == voituresCourse[i].num){
        voituresCourse[i] = copie[i];
      }
    }
  }
  for(i = 0; i < 20; i++){
    if(voituresCourse[i].pid == 0 && voituresCourse[i].tt == 500){
      voituresCourse[i].echec = 1;
    }
  }

}

int main(int argc, char *argv[]){
  int shmid = shmget(shmkey, sizeof(voiture), IPC_CREAT | 0666);
  if(shmid == -1){
    perror("Création de segment impossible: Erreur shmget du père.\n");
    exit(-1);
  }

  voituresCourse = (voiture *)shmat(shmid, 0, 0);
  if(voituresCourse == (voiture*)-1){
    perror("Attachement impossible: Erreur shmat du pére.\n");
    exit(-1);
  }

  initCourse();
  initVoituresCourse();
  sleep(1);
  courseSession("\n\tCOURSE P1 : Premiere scéance d'essais !!!\n\n\n", 30, 20, 20, 0);
  printf("%s\n", "\tPremière course terminee\n\n\n" );
  sleep(1);
  initCourse();
  initVoituresCourse();
  sleep(1);
  courseSession("\n\tCOURSE P2 : Deuxième scéance d'essais !!!\n\n\n", 30, 20, 20, 0);
  printf("%s\n", "\tDeuxieme course terminee\n\n\n" );
  sleep(1);
  initCourse();
  initVoituresCourse();
  sleep(1);
  courseSession("\n\tCOURSE P3 : Troisième scéance d'essais !!!\n\n\n", 10, 20, 20, 0);
  printf("%s\n", "\tTroisième course terminee\n\n\n" );
  sleep(1);

  initCourse();
  initVoituresCourse();
  courseSession("\n\tDébut de la séance de qualification\n\tCOURSE Q1 : Premiere scéance de qualification !!!\n\n\n", 18, 20, 20, 0);
  printf("%s\n", "\tPremière course terminee\n\n\n" );
  sleep(1);

  mettreAjourVoitCourse(15);
  initVoituresCourse();
  courseSession("\n\n\n\tCOURSE Q2 : Deuxième scéance de qualification !!!\n\n\n", 15, 20, 15, 0);
  printf("%s\n", "\tDeuxieme course terminee\n\n\n" );
  sleep(1);

  mettreAjourVoitCourse(10);
  initVoituresCourse();
  courseSession("\n\n\n\tCOURSE Q3 : Troisième scéance de qualification !!!\n\n\n", 12, 20, 10, 0);
  printf("%s\n", "\tTroisième course terminee\n\n\n" );
  sleep(1);

  printf("%s\n", "\tFin de qualification! \n");
  affichage("\n\n\tLe classement pour la finale est:\n\n\n", 10, 0);
  sleep(2);

  /**finale**/
  mettreAjourVoitCourse(10);
  initVoituresCourse();
  courseSession("\n\n\n\tLA FINALE\n\n\n", 20, 20, 10, 1);
  printf("%s\n", "\tFin De La Finale\n\n\n" );
  sleep(1);
  affichage("\n\n\tClassement Finale\n\n\n", 10, 1);

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
