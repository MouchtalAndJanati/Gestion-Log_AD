#include<stdio.h>

#include"split.c"



typedef struct _Log
{
    char * TypeEvent; // Type de l'événement
 
char * SourceEvent; // Source de l'événement

char * CategoEvent; // Catégorie de l'événement
    
int   IdEvent; // ID de l'événement
  
char * Date; // Date_de_Démarrage
    
char * Time; // Heure_de_Démarrage
    
char * User; // Utilisateur
    
char * Computer; // Ordinateur
    
char * Description;
}_Log;


_Log TabstrLog[100];

int n = 0;


void Print(int i)

{
    printf("-----------------------------------------\n");

     printf("Type de l'événement     : %s \n",TabstrLog[i].TypeEvent);
    
     printf("Source de l'événement   : %s \n",TabstrLog[i].SourceEvent);

     printf("Catégorie de l'événement: %s \n",TabstrLog[i].CategoEvent);
    
     printf("ID de l'événement       : %d \n",TabstrLog[i].IdEvent);
    
     printf("Date_de_Démarrage       : %s \n",TabstrLog[i].Date);
    
     printf("Heure_de_Démarrage      : %s \n",TabstrLog[i].Time);
    
     printf("Utilisateur             : %s \n",TabstrLog[i].User);
    
     printf("Ordinateur              : %s \n",TabstrLog[i].Computer);
    
     printf("Description             : %s \n",TabstrLog[i].Description);


}


void RemplirStructure(FILE * f)

{
    while ( feof(f) == 0) 
  {
        TabstrLog[n].TypeEvent = split(f);
   
         
         TabstrLog[n].SourceEvent = split(f);
         
         TabstrLog[n].CategoEvent = split(f);
         
         TabstrLog[n].IdEvent = atoi(split(f));
         
         TabstrLog[n].Date = split(f);
         
         TabstrLog[n].Time = split(f);
        
         TabstrLog[n].User = split(f);
         
         TabstrLog[n].Computer = split(f);
         
         TabstrLog[n].Description = split(f);

          n++;
    
  
}

   }



void ChercherParID(int Id)

{
    
     int i=0;

     for (i = 0; i < n; i++) 
    {
        
         if(TabstrLog[i].IdEvent == Id)
       
         {
            
           Print(i);
            
        return;

      }

    }

}

 
void ChercherTypeEvent(char * event)

{
    int i=0;

    for (i = 0; i < n; i++) 
    {
    if(strcmp(TabstrLog[i].TypeEvent,"Erreur") == 0)
        
        {
            
  		 Print(i);
           
        
   }
    }

   
 }



int main()

{
    FILE * f = ouvrirUnFichier("test.txt");

   
    RemplirStructure(f);
   // Print(0);

    ChercherTypeEvent("Erreur");
    
  
   return 0;

}