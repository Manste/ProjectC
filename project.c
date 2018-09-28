#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct{
  int num;
  int tour1;
  int tour2;//ici on compte le nombre de tours faits en 1h30 parr- exemple et non le temps
  int tour3;
} voiture;


/*double generate_time(){
  int x;
  //srand(time(NULL));
  x = rand();
  return x;
}*/

void trouverMax1(voiture voitures[], char c[2]){
  int max = 0;
  for(int i = 0; i < 20; i++){
    if(voitures[i].tour1 > max){
      max = voitures[i].num;
    }
  }
  printf("\n\t\t=>Le meilleur de %s est: %d.\n", c, max);
}

void trouverMax2(voiture voitures[], char c[2]){
  int max = 0;
  for(int i = 0; i < 20; i++){
    if(voitures[i].tour2 > max){
      max = voitures[i].num;
    }
  }
  printf("\t\t\t=>Le meilleur de %s est: %d\n", c, max);
}

void trouverMax3(voiture voitures[], char c[2]){
  int max = 0;
  for(int i = 0; i < 20; i++){
    if(voitures[i].tour3 > max){
      max = voitures[i].num;
    }
  }
  printf("\t\t\t\t=>Le meilleur de %s est: %d\n\n", c, max);
}

void affichage(voiture voitures[], char c1[2], char c2[2], char c3[2]){
  printf("voitures\t|\t%s\t|\t%s\t|\t%s\t|\n\n", c1, c2, c3);
  for(int i = 0; i < 20; i++){
    printf("%d\t\t|\t%d\t|\t%d\t|\t%d\t|\n", voitures[i].num, voitures[i].tour1, voitures[i].tour2, voitures[i].tour3);
  }
  trouverMax1(voitures, c1);
  trouverMax2(voitures, c2);
  trouverMax3(voitures, c3);
}

void essai(voiture voitures[20], int voitureCourse[20]){
  srand(time(NULL));
  for(int i = 0; i < 20; i++){
    voitures[i].num = voitureCourse[i];
    voitures[i].tour1 = rand()%(45-1) + 1;
    voitures[i].tour2 = rand()%(45-1) + 1;
    voitures[i].tour3 = rand()%(45-1) + 1;
  }
}

int main(){
  voiture voitures[20];
  int voitureCourse[] = {44,77,5,3,33,11,31,18,35,27,55,10,28,8,20,2,14,9,16};
  int classement[20];
  essai(voitures, voitureCourse);
  affichage(voitures, "P1", "P2", "P3");
}
