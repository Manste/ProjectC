#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

typedef struct{
  int num;
  int temps1;
  int temps2;
  int temps3;
  int tt;//temps total
  int pit;//pit stop
  int out;//arrêt
} voiture;

/*void affichage(voiture voitures[20],int voitureCourse[20], int taille){
  srand(time(NULL));
  for(int i = 0; i < 20; i++){
      voitures[i].num = voitureCourse[i];
      voitures[i].temp1 = rand()%(taille - 1) + 1;
      voitures[i].temp2 = rand()%(taille-1) + 1;
      voitures[i].temp3 = rand()%(taille-1) + 1;
      voitures[i].tt = voitures[i].temp1 + voitures[i].temp2 + voitures[i].temp3;
      printf("%d\t\t|\t%d s\t|\t%d s\t|\t%d s\t|\t%d\t|\tPIT\t|\tOUT\t\n", voitures[i].num, voitures[i].temp1, voitures[i].temp2, voitures[i].temp3, voitures[i].tt);
  }
}
*/
int main(int argc, char *argv[]){
  printf("voitures\t|\tS1\t|\tS2\t|\tS3\t|\tTT\t|\tPIT\t|\tOUT\t\n\n");


  char tab[1024];
  int count ;
  int voituresCourse[] = {44,77,5,7,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};

  char etapes [7][2] = {"P1", "P2", "P3", "Q1", "Q2", "Q3", "C"};

  int fp[2];
  int rep = pipe(fp);
  srand(time(NULL));
  //for(int i= 0; i <= 20; i++){
    int f = fork();
    if (f == 0){
      voiture v;
      v.num = 44;
      v.temps1 = rand()%(1000 - 1) + 1;
      v.temps2 = rand()%(1000-1) + 1;
      v.temps3 = rand()%(1000-1) + 1;

      v.tt = v.temps1 + v.temps2 + v.temps3;
      dup2(fp[1],1);
      close(fp[0]);
      close(fp[1]);
      printf("%d\t\t|\t%d s\t|\t%d s\t|\t%d s\t|\t%d s\t|\tPIT\t|\tOUT\t\n", v.num, v.temps1, v.temps2, v.temps3, v.tt);
    }
    else{
      while (count = read(fp[0], tab, 1024)) {
             write(STDOUT_FILENO, tab, count);
      }
        close(fp[1]);
        close(fp[0]);
        wait(NULL);
  }

  //}


}
