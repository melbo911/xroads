/*
#
# melbo @ x-plane.org
#
*/

#ifdef _WIN32
 #include <windows.h>
 #include <process.h>
 #include "dirent.h"
#else
 #include <libgen.h>
 #include <unistd.h>
 #include <dirent.h>
#endif

#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h>
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
 #define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
 #define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define defSpeed 70
#define MAX_TXT 1024
#define MAX_WRD 1024

char *words[MAX_WRD];

char xpDir[MAX_TXT];
char xSceneryDir[MAX_TXT];
char xRoadsDir[MAX_TXT];
char xRoads[MAX_TXT];
char xObjects[MAX_TXT];
char xLib[MAX_TXT];
char defRoads[MAX_TXT];
char xBlankFac[MAX_TXT];
char xBlankObj[MAX_TXT];

/*-----------------------------------------------------------------*/

int shift(char *s) {           /* shift string to the right */
   int i = strlen(s);
   while (i >= 0) {
      s[i+1] = s[i];
      i--;
   }
   s[i] = ' ';
   return(0);
}

/*-----------------------------------------------------------------*/

int strip(char *s) {           /* remove trailing whitespace */
   int i = strlen(s);
   while (i >= 0 && s[i] <= ' ') {
      s[i--] = '\0';
   }
   return(0);
}

/*-----------------------------------------------------------------*/

void join(char *s, char *w[], int n) {
  int i = 0;

  s[0]='\0';
  while (i<n) {
     strcat(s,w[i++]);  
     strcat(s," ");  
  }
}

/*-----------------------------------------------------------------*/

int split(char *s) {
  int w = 0;
  int i = 0;

  char tmp[MAX_TXT];
  char *k;

  k = tmp;

  strcpy(tmp,s);

  while(tmp[i] > '\0' && w < MAX_WRD ) {
     if (tmp[i] <= ' ' ) {
        tmp[i++] = '\0';

        if ( words[w] == NULL ) free(words[w]);

        words[w] = (char*) malloc(strlen(k));
        strcpy(words[w++], k);
        while(tmp[i] > '\0' && tmp[i] <= ' ' )
           i++;
        k = &tmp[i];
     } else {
        i++;
     }
  }
  if ( words[w] == NULL ) free(words[w]);
  words[w] = (char*) malloc(strlen(k));
  strcpy(words[w++], k);
  return(w);
}

/*-----------------------------------------------------------------*/

int isFile(char *s) {
   struct stat sb;

   if (stat(s, &sb) == 0 && S_ISREG(sb.st_mode)) {
      return(1); 
   } else {
      return(0); 
   }
}

/*-----------------------------------------------------------------*/

int isDir(char *s) {
   struct stat sb;

   if (stat(s, &sb) == 0 && S_ISDIR(sb.st_mode)) {
      return(1); 
   } else {
      return(0); 
   }
}

/*-----------------------------------------------------------------*/

int read_messages(char *f)        /* quick and dirty file read */
{
    FILE *fp;
    if ( (fp = fopen(f,"r")) ) {
/*
       fgets(end, MAX_TXT, fp);
       strip(end);
*/
       return(0);
    } else {
       printf("cannot open %s\n",f);
       return(9);
    }
}

/*-----------------------------------------------------------------*/

void initVars() {

   strcpy(xSceneryDir,xpDir);
   strcat(xSceneryDir,"/Custom Scenery");

   strcpy(xRoadsDir,xSceneryDir);
   strcat(xRoadsDir,"/zRoads");

   strcpy(xLib,xRoadsDir);
   strcat(xLib,"/library.txt");

   strcpy(xObjects,xRoadsDir);
   strcat(xObjects,"/objects");

   strcpy(xRoads,xRoadsDir);
   strcat(xRoads,"/1000_roads");

   strcpy(xBlankFac,xObjects);
   strcat(xBlankFac,"/blank.fac");

   strcpy(xBlankObj,xObjects);
   strcat(xBlankObj,"/blank.obj");

   strcpy(defRoads,xpDir);
   strcat(defRoads,"/Resources/default scenery/1000 roads");

}
/*-----------------------------------------------------------------*/

int genBlankFac() {
   FILE *fp;
   if ( (fp = fopen(xBlankFac,"w")) != NULL ) {
      fputs("A\n800\nFACADE\n",fp);
      fclose(fp);
      return(0);
   }
   return(1);
}

/*-----------------------------------------------------------------*/

int genBlankObj() {
   FILE *fp;
   if ( (fp = fopen(xBlankObj,"w")) != NULL ) {
      fputs("A\n800\nOBJ\n",fp);
      fclose(fp);
      return(0);
   }
   return(1);
}

/*-----------------------------------------------------------------*/

/*
zOrtho4XP_+51-011    >  REGION_RECT -011 +51 -011 +51
*/

/*-----------------------------------------------------------------*/

int genLibrary() {
   FILE *fp;
   DIR *d;
   struct dirent *dir;

   char lon[8];
   char lat[8];
   char buf[100];

   if ( (fp = fopen(xLib,"w")) != NULL ) {
      fputs("A\n800\nLIBRARY\n\nREGION_DEFINE Xroads\n",fp);

      d = opendir(xSceneryDir);
      if (d) {
         while ((dir = readdir(d)) != NULL) {
            if( strstr(dir->d_name,"zOrtho4XP_") ) {
               strcpy(buf,dir->d_name);
               strncpy(lon,&buf[10],3);
               lon[3] = '\0';
               strncpy(lat,&buf[13],4);
               lat[4] = '\0';
               sprintf(buf,"REGION_RECT %s %s %s %s\n", lat,lon,lat,lon);
               fputs(buf,fp);
            }
         }
         closedir(d);
      }

      fputs("\nREGION Xroads\nEXPORT_EXCLUDE lib/g10/roads.net 1000_roads/roads.net\nEXPORT_EXCLUDE lib/g10/roads_EU.net 1000_roads/roads_EU.net\n",fp);

      fputs("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac    objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/solar_panel.obj     objects/blank.obj\n",fp);
      fclose(fp);
      return(0);
   }
   return(1);
}

/*-----------------------------------------------------------------*/

int genFile(char *s) {

    FILE *in,*out;
    char infile[MAX_TXT];
    char outfile[MAX_TXT];
    char buf[MAX_TXT];
    int speed = 0;
    int rail = 0;
    int keep = 0;
    int n = 0;

    sprintf(infile,"%s/%s",defRoads,s);
    sprintf(outfile,"%s/%s",xRoads,s);
    if ( ! isFile(outfile) ) {
       if ( (in = fopen(infile,"r")) ) {
          if ( (out = fopen(outfile,"w")) ) {
             while ( fgets(buf, MAX_TXT, in) != NULL ) {
                strip(buf);
                if (strstr(buf," Junction BYT") != NULL ) {
                   if (strstr(buf,"BYT1000") != NULL || \
                       strstr(buf,"BYT1100") != NULL || \
                       strstr(buf,"BYT1200") != NULL) {
                      keep = 1;
                   } else {
                      keep = 0;
                   }
                } else {
                   if ( ! keep && strstr(buf,"QUAD ") != NULL ) {
                      shift(buf);
                      buf[0] = '#';
                   } else {
   
                      if ( strstr(buf,"TRI ") != NULL || (rail == 0 && strstr(buf,"SEGMENT_DRAPED") != NULL) ) {
                         shift(buf);
                         buf[0] = '#';
                      } else {
                         if ( strstr(buf,"#rail_") != NULL ) {
                            rail = 1;
                         } else {
                            if ( strstr(buf,"CAR_DRAPED") != NULL || strstr(buf,"CAR_GRADED") != NULL ) {
                               n = split(buf);               
                               speed = atoi(words[3]) * defSpeed / 100;
                               sprintf(words[3],"%d",speed);
                               join(buf,(char **) words,n);
                               strip(buf);
                            }
                         }
                      }
                   }
                }
                fputs(buf, out);
                fputs("\n", out);
             }
             fclose(out);
          }
          fclose(in);
       } else {
          printf("cannot open %s\n",infile);
       }
       return(1);
    } else {
       printf("%s already exists\n",outfile);
       return(0);
    }
}


/*-----------------------------------------------------------------*/

int main(int argc, char **argv) {

#ifdef _WIN32
   GetCurrentDirectory(MAX_TXT,xpDir);
#else
   strcpy(xpDir,dirname(argv[0]));
#endif

   initVars();

   printf("xRoadsDir = %s\n",xRoadsDir);

   if ( ! isDir(xRoadsDir) ) {
      if ( mkdir(xRoadsDir,0755) ) {
          printf("cannot create %s in Custom Scenery\n",xRoadsDir);
      } else {
         printf("%s created \n",xRoadsDir);
      }
   }
   mkdir(xRoads,0755);
   mkdir(xObjects,0755);

   genFile("roads.net");
   genFile("roads_EU.net");

   genBlankFac();
   genBlankObj();

   genLibrary();

}


/*
     library.txt

A
800
LIBRARY

REGION_DEFINE Xroads
REGION_RECT +008 +43 +008 +43
REGION_RECT +006 +43 +006 +43
.
.
REGION_RECT +007 +43 +007 +43
REGION_RECT +011 +48 +011 +48
REGION_RECT +010 +43 +010 +43

REGION Xroads
EXPORT_EXCLUDE lib/g10/roads.net 1000_roads/roads.net
EXPORT_EXCLUDE lib/g10/roads_EU.net 1000_roads/roads_EU.net

EXPORT_EXCLUDE simheaven/ground/parking_cars.fac    objects/blank.fac
EXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/blank.fac
EXPORT_EXCLUDE simheaven/ground/solar_panel.obj     objects/blank.obj

*/
