#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#include "cconio.h"

typedef struct _DHCP
{
    int ID;
    char * Date;
    char * Heure;
    char * Description;
    char * AdresseIP;
    char * HostName;
    char * AdrMac;
    char * UserName;
    char *  IdTransaction;
    char * Resultat;
    char * HeureEssai;
    char * IdCorr;
    char * DHCID;
}_DHCP;



_DHCP TabLogDHCP[100];
int n = 0;


void DeplacerDansFichier(FILE * f)
{
    char line[500];
    int ok = 0;
    char * pch;
    if ( f != NULL)
    {
        while(ok == 0)
        {
            fgets ( line, 500 * sizeof(char), f );
            pch = strstr(line,"ID, Date, Heure,");
            if(pch != NULL)
                ok = 1;
        }
        
    }
    
    
}

char * lireLigneDuFichier(FILE * f)
{
    char * line = NULL;
    if ( f != NULL)
    {
        line = malloc(500 * sizeof(char));
        fgets ( line, 500 * sizeof(char), f );
    }
    return line;
}


_DHCP split(FILE * f)
{
    char * str = lireLigneDuFichier(f);
    _DHCP dhcp;
      char * token = strtok(str, ",");
    if(token != NULL)
        dhcp.ID = atoi(token);
    else
    {
       dhcp.ID = -1;
    }
    dhcp.Date = strtok(NULL, ",");
    dhcp.Heure = strtok(NULL, ",");
    dhcp.Description = strtok(NULL, ",");
    dhcp.AdresseIP = strtok(NULL, ",");
    dhcp.HostName = strtok(NULL, ",");
    dhcp.AdrMac = strtok(NULL, ",");
    dhcp.UserName = strtok(NULL, ",");
    dhcp.IdTransaction = strtok(NULL, ",");
    dhcp.Resultat = strtok(NULL, ",");
    dhcp.HeureEssai = strtok(NULL, ",");
    dhcp.IdCorr = strtok(NULL, ",");
    dhcp.DHCID = strtok(NULL, ",");
    return dhcp;
}


void RemplirStructure(FILE * f)
{
    DeplacerDansFichier(f);
    
    while ( feof(f) == 0) {
        
        TabLogDHCP[n] = split(f);
        if(TabLogDHCP[n].ID != -1)
        n++;
        
    }
    
}
void print(i)
{
    
    printf("ID : %d\n",TabLogDHCP[i].ID);
    printf("Date et Heure  : %s %s\n",TabLogDHCP[i].Date,TabLogDHCP[i].Heure);
    printf("Adresse IP : %s\n",TabLogDHCP[i].AdresseIP);
    printf("Adresse MAC : %s\n",TabLogDHCP[i].AdrMac);
    printf("Nom d'hôte, : %s\n",TabLogDHCP[i].HostName);
    printf("Nom d'utilisateur :%s\n",TabLogDHCP[i].UserName);
    printf("Heure essai :%s\n",TabLogDHCP[i].HeureEssai);
    
}
void RechercheParID()
{
    system("cls");
    textcolor(14);
    printf("Signification des ID d'événement\n");
    printf("00	Le journal a démarré.\n");
    printf("01	Le journal a été arrêté.\n");
    printf("02	Le journal a été temporairement suspendu par manque d'espace disque.\n");
    printf("10	Une nouvelle adresse IP a été allouée par bail à un client.\n");
    printf("11	Un bail a été renouvelé par un client.\n");
    printf("12	Un bail a été libéré par un client.\n");
    printf("13	Une adresse IP a été trouvée en cours d'utilisation sur le réseau.\n");
     printf("14	Une demande de bail n'a pas pu être satisfaite, car le pool d'adresses de l'étendue a été épuisé.\n");
     printf("15	Un bail a été refusé.\n");
     printf("16	Un bail a été supprimé.\n");
     printf("17	Un bail a expiré et des enregistrements DNS pour un bail expiré n'ont pas été supprimés.\n");
     printf("18	Un bail a expiré et des enregistrements DNS ont été supprimés.\n");
     printf("20	Une adresse BOOTP a été allouée par bail à un client.\n");
     printf("21	Une adresse BOOTP dynamique a été allouée par bail à un client.\n");
     printf("22	Une demande BOOTP n'a pas pu être satisfaite, car le pool d'adresses de l'étendue pour BOOTP a été épuisé.\n");
     printf("23	Une adresse IP BOOTP a été supprimée après avoir vérifié qu'elle n'était pas utilisée.\n");
     printf("24	L'opération de nettoyage des adresses IP a commencé.\n");
     printf("25	Statistiques de nettoyage d'adresses IP.\n");
     printf("30	Demande de mise à jour DNS au serveur DNS nommé.\n");
     printf("31	Échec de la mise à jour DNS.\n");
     printf("32	Réussite de la mise à jour DNS.\n");
     printf("33	Paquet rejeté en raison de la stratégie NAP.\n");
     printf("34	Échec de la demande de mise à jour DNS en raison du dépassement du nombre maximal de demandes de mise à jour dans la file d'attente.\n");
     printf("50+	Les codes au-delà de 50 sont utilisés pour les informations de détection de serveur Rogue.");
    
     printf("\n\n\n\nSaisir Votre Choix :");
    int choix;
    scanf("%d",&choix);
    
    int i=0;
    textcolor(15);
    for (i=0; i<n; i++) {
        if (TabLogDHCP[i].ID == choix) {
            print(i);
        }
    }
    
}

void RechercheParIP()
{
    system("cls");
    char ip[20];
     printf("Saisir l'adresse IP \n");
    scanf("%s",ip);
    int i=0;
    textcolor(15);
    for (i=0; i<n; i++) {
        if (strcmp(TabLogDHCP[i].AdresseIP,ip)==0) {
            print(i);
        }
    }
    
}

void RechercheParMac()
{
    system("cls");
    char mac[20];
    printf("Saisir l'adresse Mac \n");
    scanf("%s",mac);
    int i=0;
    textcolor(15);
    for (i=0; i<n; i++) {
        if (strcmp(TabLogDHCP[i].AdrMac,mac)==0) {
            print(i);
        }
    }
    
}

void RechercheParHote()
{
    system("cls");
    char hote[20];
    printf("Saisir Nom d'hôte \n");
    scanf("%s",hote);
    int i=0;
    textcolor(15);
    for (i=0; i<n; i++) {
        if (strcmp(TabLogDHCP[i].HostName,hote)==0) {
            print(i);
        }
    }
    
}

void menu()
{
    
    system("cls");
    textcolor(14);
    gotoxy(10,2); printf("******************************************************");
    gotoxy(10,3);printf("*          Analyse des Logs DHCP                      *");
    gotoxy(10,4);printf("*******************************************************");
    
    gotoxy(10,7); printf("*******************************************************");
    gotoxy(10,8); printf("*                                                     *");
    gotoxy(10,9); printf("*  1.         Filtrer par ID                          *");
    gotoxy(10,10);printf("*                                                     *");
    gotoxy(10,11);printf("*  2.         Filtrer par Adresse IP                  *");
    gotoxy(10,12);printf("*                                                     *");
    gotoxy(10,13);printf("*  3.         Filtrer par Adresse Mac                 *");
    gotoxy(10,14);printf("*                                                     *");
    gotoxy(10,15);printf("*  4.         Filtrer par Nom d'hôte                  *");
    gotoxy(10,16);printf("*                                                     *");
    gotoxy(10,17);printf("*  5.         Exit                                    *");
    gotoxy(10,18);printf("*******************************************************");
    
    gotoxy(10,22);printf("Saisir Votre Choix :");
    
    
}

int main()
{
    
    FILE * f = fopen("DhcpSrvLog-Mar.log","r");
    RemplirStructure(f);
    menu();
    int choix;
    scanf("%d",&choix);
    switch(choix)
    {
        case 1:RechercheParID();
            break;
        case 2:RechercheParIP();
            break;
        case 3:RechercheParMac();
            break;
        case 4:RechercheParHote();
            break;
        case 5:
            exit(0);
    }
    return 0;
}
