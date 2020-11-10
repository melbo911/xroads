/*
#
# melbo @ https://x-plane.org
#
*/

#define VERSION "0.9.0"

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

#define defSpeed 70	/*  reduce speed of IA cars to 70 % */
#define MAX_TXT 1024
#define MAX_WRD 1024

#define XSCENERYDIR "./Custom Scenery"
#define XROADSDIR XSCENERYDIR"/Xroads"
#define XROADS XROADSDIR"/1000_roads"
#define XTEXTURES XROADS"/textures"
#define XOBJECTS XROADSDIR"/objects"
#define XLIB XROADSDIR"/library.txt"
#define XBLANKFAC XOBJECTS"/blank.fac"
#define XBLANKOBJ XOBJECTS"/blank.obj"
#define DEFROADS "Resources/default scenery/1000 roads"

char *words[MAX_WRD];

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

int genBlankFac() {
   FILE *fp;
   if ( ! isFile(XBLANKFAC) ) {
      if ( (fp = fopen(XBLANKFAC,"w")) != NULL ) {
         fputs("A\n800\nFACADE\n",fp);
         fclose(fp);
         return(0);
      }
   } else {
      printf("%s already exists\n",XBLANKFAC);
   }
   return(1);
}

/*-----------------------------------------------------------------*/

int genBlankObj() {
   FILE *fp;
   if ( ! isFile(XBLANKOBJ) ) {
      if ( (fp = fopen(XBLANKOBJ,"w")) != NULL ) {
         fputs("A\n800\nOBJ\n",fp);
         fclose(fp);
         return(0);
      }
   } else {
      printf("%s already exists\n",XBLANKOBJ);
   }
   return(1);
}

/*-----------------------------------------------------------------*/

/*

create library.txt and add all found Ortho4XP tiles

zOrtho4XP_+51-011    >  REGION_RECT -011 +51 -011 +51

*/

int genLibrary() {
   FILE *fp;
   FILE *opt;
   DIR *d;
   struct dirent *dir;

   char lon[8];
   char lat[8];
   char buf[MAX_TXT];

   if ( (fp = fopen(XLIB,"w")) != NULL ) {
      fputs("A\n800\nLIBRARY\n\nREGION_DEFINE Xroads\n",fp);

      d = opendir(XSCENERYDIR);
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

      /*  add optional tile coordinates to library */
      if ( (opt = fopen("xroads.add","r")) ) {
         while ( fgets(buf, MAX_TXT, opt) != NULL ) {
            fputs(buf,fp);
         }
         fclose(opt);
      }

      /* add net file re-routes */
      fputs("\nREGION Xroads\nEXPORT_EXCLUDE lib/g10/roads.net 1000_roads/roads.net\nEXPORT_EXCLUDE lib/g10/roads_EU.net 1000_roads/roads_EU.net\n",fp);

      /* add object re-routes */
      fputs("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac    objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/solar_panel.obj     objects/blank.obj\n",fp);

      /* add optional lines to the end of the library */
      if ( (opt = fopen("xroads.opt","r")) ) {
         while ( fgets(buf, MAX_TXT, opt) != NULL ) {
            fputs(buf,fp);
         }
         fclose(opt);
      }

      /* close library */
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
   int hwy = 0;
   int n = 0;

   sprintf(infile,"%s/%s",DEFROADS,s);
   sprintf(outfile,"%s/%s",XROADS,s);
   if ( (in = fopen(infile,"r")) ) {
      if ( (out = fopen(outfile,"w")) ) {
         while ( fgets(buf, MAX_TXT, in) != NULL ) {
            strip(buf);
            if ( strstr(buf,"# Group: ") != NULL ) {
               if ( strstr(buf,"GRPHwyBYTs") != NULL || strstr(buf,"GRP_HIGHWAYS") != NULL ) {
                  hwy = 1;
               } else {
                  if ( strstr(buf,"GRP_RAIL") != NULL ) {
                     rail = 1;
                  } else {
                     hwy = 0;
                  }
               }
            } else {
               if ( ! hwy && ! rail && ( strstr(buf,"QUAD ") != NULL || strstr(buf,"TRI ") != NULL ) ) {
                  shift(buf);
                  buf[0] = '#';
               } else {
                  if ( ! rail && strstr(buf,"SEGMENT_DRAPED ") != NULL ) {
                     shift(buf);
                     buf[0] = '#';
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
            fputs(buf, out);
            fputs("\n", out);
         }
         fclose(out);
      }
      fclose(in);
   } else {
      printf("cannot open %s\n",infile);
   }
   return(0);
}

/*-----------------------------------------------------------------*/

int main(int argc, char **argv) {

   char tmp[256];

   printf("Xroads - %s\n",VERSION);

#ifndef _WIN32
   strcpy(tmp,dirname(argv[0]));
   printf("changing to %s\n",tmp);
   chdir(tmp);
#endif

   if ( ! isDir(XROADSDIR) ) {
      if ( mkdir(XROADSDIR,0755) ) {
         printf("ERROR: cannot create %s\n",XROADSDIR);
         return(-1);
      } else {
         printf("%s created\n",XROADSDIR);
      }
   }

   if ( ! isDir(XROADS) ) {
      mkdir(XROADS,0755);
   } else {
      printf("%s already exists\n",XROADS);
   }

   if ( ! isFile(XTEXTURES"/bridges.dds") ) {
#ifdef _WIN32
      sprintf(tmp,"mklink /j \"%s\" \""DEFROADS"/textures\"",XTEXTURES);
      /* printf("%s\n",tmp); */
      system(tmp);
#else
      symlink("../../../"DEFROADS"/textures",XTEXTURES);
#endif
   } else {
      printf("%s already exists\n",XTEXTURES);
   }

   genFile("roads.net");
   genFile("roads_EU.net");

   if ( ! isDir(XOBJECTS) ) {
      mkdir(XOBJECTS,0755);
   } else {
      printf("%s already exists\n",XOBJECTS);
   }

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
