/*
#
# melbo @ https://x-plane.org
#
*/

#define VERSION "0.18.0"

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
#define XOBJECTS XROADS"/objects"
#define XROBJS XROADSDIR"/objects"
#define XLIB XROADSDIR"/library.txt"
#define XBLANKFAC XROBJS"/blank.fac"
#define XBLANKOBJ XROBJS"/blank.obj"
#define DEFROADS "Resources/default scenery/1000 roads"
#define XESCENE "simHeaven_X-Europe-4-scenery"

char *words[MAX_WRD];
int hasXE = 0;

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

         if ( words[w] == NULL ) words[w] = (char*) malloc(MAX_TXT);
         strcpy(words[w++], k);
         while(tmp[i] > '\0' && tmp[i] <= ' ' )
            i++;
         k = &tmp[i];
      } else {
         i++;
      }
   }
   if ( words[w] == NULL ) words[w] = (char*) malloc(MAX_TXT);
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

Create library.txt and add all found zOrtho* tiles

Also scan "Earth nav data" folder to support "ortho bundles"

Include "xroads.add" and append "xroad.opt" files if exist.

zOrtho4XP_+51-011    >  REGION_RECT -011 +51 -011 +51

*/

int genLibrary() {
   FILE *fp;
   FILE *opt;
   DIR *d,*e,*l;
   struct dirent *dir,*earth,*lonlat;

   char lon[8];
   char lat[8];
   char buf[MAX_TXT];
   char buf1[MAX_TXT];
   char buf2[MAX_TXT];

   if ( (fp = fopen(XLIB,"w")) != NULL ) {
      fputs("A\n800\nLIBRARY\n\nREGION_DEFINE Xroads\n",fp);

      d = opendir(XSCENERYDIR);
      if (d) {
         while ((dir = readdir(d)) != NULL) {
/*
            if( strstr(dir->d_name,"zOrtho4XP_") ) {
*/
            if( ! strncmp(dir->d_name,"zOrtho",strlen("zOrtho")) ) {
               /* ortho folder */
               strcpy(buf1,XSCENERYDIR);
               strcat(buf1,"/");
               strcat(buf1,dir->d_name);
               strcat(buf1,"/Earth nav data");
               /*
               printf("trying1 %s \n", buf1);
               */
               e = opendir(buf1);
               if (e) {
                  while ((earth = readdir(e)) != NULL) {     /* scan earth nav data */
                     if ( earth->d_name[0] != '.' ) {
                        strcpy(buf2,buf1);
                        strcat(buf2,"/");
                        strcat(buf2,earth->d_name);
                        /*
                        printf("trying2 %s \n", buf2);
                        */
                        l = opendir(buf2);
                        if (l) {
                           while ((lonlat = readdir(l)) != NULL) {
                              /* ignore backup files */ 
                              if( strstr(lonlat->d_name,".dsf") &&\
                                  strlen(lonlat->d_name) < 12 ) {
                                 printf("adding %s\n",lonlat->d_name);
                                 strncpy(lon,&lonlat->d_name[0],3);
                                 lon[3] = '\0';
                                 strncpy(lat,&lonlat->d_name[3],4);
                                 lat[4] = '\0';
                                 sprintf(buf,"REGION_RECT %s %s %s %s\n", lat,lon,lat,lon);
                                 fputs(buf,fp);
                              }
                           }
                           closedir(l);
                        }
                     }
                  }
                  closedir(e);
               } else {
                  if( strstr(dir->d_name,"zOrtho4XP_") ) {
                     strcpy(buf,dir->d_name);
                     strncpy(lon,&buf[10],3);
                     lon[3] = '\0';
                     strncpy(lat,&buf[13],4);
                     lat[4] = '\0';
                     if ( atoi(lon) != 0 && atoi(lat) != 0 ) {
                        printf("adding %s\n",dir->d_name);
                        sprintf(buf,"REGION_RECT %s %s %s %s\n", lat,lon,lat,lon);
                        fputs(buf,fp);
                     }
                  }
               }
            }
         }
         closedir(d);
      }

      /*  add optional tile coordinates to library */
      if ( (opt = fopen("xroads.add","r")) ) {
         while ( fgets(buf, MAX_TXT, opt) != NULL ) {
            if ( strlen(buf) > 1 ) { /* skip empty lines */
               fputs(buf,fp);
            }
         }
         fclose(opt);
      }

      /* add net file re-routes */
      fputs("\nREGION Xroads\nEXPORT_EXCLUDE lib/g10/roads.net 1000_roads/roads.net\nEXPORT_EXCLUDE lib/g10/roads_EU.net 1000_roads/roads_EU.net\n",fp);

      /* add object re-routes */
      if ( hasXE ) {
         fputs("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac    objects/Parking_Cars.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/Parking_Trucks.fac\n",fp);
      } else {
         fputs("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac    objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/blank.fac\n",fp);
      }
      fputs("EXPORT_EXCLUDE simheaven/ground/solar_panel.obj     objects/blank.obj\n",fp);

      /* add optional lines to the end of the library */
      if ( (opt = fopen("xroads.opt","r")) ) {
         while ( fgets(buf, MAX_TXT, opt) != NULL ) {
            if ( strlen(buf) > 1 ) { /* skip empty lines */
               fputs(buf,fp);
            }
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

int genNetFile(char *s) {

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
            if ( strstr(buf,"# Group: ") ) {
               if ( strstr(buf,"GRPHwyBYTs") || strstr(buf,"GRP_HIGHWAYS") ) {
                  hwy = 1;
               } else {
                  if ( strstr(buf,"GRP_RAIL") ) {
                     rail = 1;
                  } else {
                     hwy = 0;
                  }
               }
            } else {
               if ( ! hwy && ! rail && ( strstr(buf,"QUAD ") || strstr(buf,"TRI ") ) ) {
                  shift(buf);
                  buf[0] = '#';
               } else {
                  if ( ! rail && strstr(buf,"SEGMENT_DRAPED ") ) {
                     shift(buf);
                     buf[0] = '#';
                  } else {
                     if ( strstr(buf,"CAR_DRAPED") || strstr(buf,"CAR_GRADED") ) {
                        n = split(buf);
                        if ( n > 3 ) {
                           speed = atoi(words[3]) * defSpeed / 100;
                           sprintf(words[3],"%d",speed);
                           join(buf,(char **) words,n);
                           strip(buf);
                        }
                     } else {
                        /* ignore trees on roads */
                        if ( strstr(buf,"autogen_tree") ) {
                           shift(buf);
                           buf[0] = '#';
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
   return(0);
}

/*-----------------------------------------------------------------*/


int genFacFile(char *s) {

   FILE *in,*out;
   char infile[MAX_TXT];
   char outfile[MAX_TXT];
   char buf[MAX_TXT];
   int speed = 0;
   int rail = 0;
   int hwy = 0;
   int n = 0;

   sprintf(infile,"%s/%s/objects/ground/%s",XSCENERYDIR,XESCENE,s);
   sprintf(outfile,"%s/%s",XROBJS,s);
   if ( (in = fopen(infile,"r")) ) {
      if ( (out = fopen(outfile,"w")) ) {
         while ( fgets(buf, MAX_TXT, in) != NULL ) {
            strip(buf);

            if ( strstr(buf,"fencing.png") ) {
               sprintf(buf,"TEXTURE ../../%s/objects/ground/fencing.png",XESCENE);
            } else {
               if ( strstr(buf,"Roof_Asphalt.png") ) {
                  sprintf(buf,"TEXTURE ../../%s/objects/blank.dds",XESCENE);
               } else {
                  if ( strstr(buf,"Omni_Parking_Lit.obj") ) {
                     sprintf(buf,"OBJ ../../%s/objects/ground/Omni_Parking_Lit.obj",XESCENE);
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

   if ( ! isDir(XOBJECTS"/cars") ) {
#ifdef _WIN32
      sprintf(tmp,"mklink /j \"%s\" \""DEFROADS"/objects\"",XOBJECTS);
      /* printf("%s\n",tmp); */
      system(tmp);
#else
      symlink("../../../"DEFROADS"/objects",XOBJECTS);
#endif
   } else {
      printf("%s already exists\n",XOBJECTS);
   }

   genNetFile("roads.net");
   genNetFile("roads_EU.net");

   if ( ! isDir(XROBJS) ) {
      mkdir(XROBJS,0755);
   } else {
      printf("%s already exists\n",XROBJS);
   }

   genBlankFac();
   genBlankObj();

   if ( isDir(XSCENERYDIR"/"XESCENE"/objects") ) {
      hasXE = 1;
      printf("setting up X-Europe parking\n");
      genFacFile("Parking_Cars.fac");
      genFacFile("Parking_Trucks.fac");
   }

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
