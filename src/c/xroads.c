/*
#
# melbo @ https://x-plane.org
#
# 
# compile:   cl xroads.c /D "NODEBUG" /O2 /link Ole32.lib
# 
*/

#define VERSION "0.37"

#ifdef _WIN32
  #include <windows.h>
  #include <process.h>
  #include <shlobj.h>
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

#define defSpeed         70	/*  reduce speed of IA cars to 70 % */
#define MAX_TXT          1024
#define MAX_WRD          256
#define OPT_NO_HWY_LTS   0x01
#define OPT_LHT          0x02

#define XSCENERYDIR  "./Custom Scenery"
#define XROADSDIR    XSCENERYDIR"/Xroads"
#define XROADS       XROADSDIR"/1000_roads"
#define XTEXTURES    XROADS"/textures"
#define XOBJECTS     XROADS"/objects"
#define XROBJS       XROADSDIR"/objects"
#define XLIB         XROADSDIR"/library.txt"
#define XBLANKFAC    XROBJS"/blank.fac"
#define XBLANKOBJ    XROBJS"/blank.obj"
#define DEFROADS     "Resources/default scenery/1000 roads"

char *XSCENES[100] = {
  "simHeaven_X-Europe-4-scenery",
  "simHeaven_X-America-4-scenery",
  "simHeaven_X-Europe-6-scenery",
  "simHeaven_X-America-6-scenery",
  "simHeaven_X-World_Europe-6-scenery",
  "simHeaven_X-World_America-6-scenery",
  NULL
};

int  hasXE           = 0;
char XSCENE[MAX_TXT] = "";
char XTEST[MAX_TXT]  = "";
char words[MAX_WRD][MAX_TXT];

#ifdef _WIN32

// special code for Shortcuts
 
char targetPath[MAX_PATH];
wchar_t wshortcut[MAX_PATH];

int GetShortcutTargetPath(const char* shortcutFile)
{
    // Initialize the COM library
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        return 1;
    }

    // Create an instance of the IShellLink interface
    IShellLink* pShellLink;
    hr = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (void**)&pShellLink);
    if (FAILED(hr))
    {
        CoUninitialize();
        return 2;
    }

    // Get a pointer to the IPersistFile interface
    IPersistFile* pPersistFile;
    hr = pShellLink->lpVtbl->QueryInterface(pShellLink, &IID_IPersistFile, (void**)&pPersistFile);
    if (FAILED(hr))
    {
        pShellLink->lpVtbl->Release(pShellLink);
        CoUninitialize();
        return 3;
    }

    // Load the shortcut file
    hr = pPersistFile->lpVtbl->Load(pPersistFile, shortcutFile, STGM_READ);
    if (FAILED(hr))
    {
        pPersistFile->lpVtbl->Release(pPersistFile);
        pShellLink->lpVtbl->Release(pShellLink);
        CoUninitialize();
        return 4;
    }

    // Get the target path of the shortcut
    //wchar_t szTargetPath[MAX_PATH];
    hr = pShellLink->lpVtbl->GetPath(pShellLink, targetPath, MAX_PATH, NULL, SLGP_RAWPATH);
    if (FAILED(hr))
    {
        pPersistFile->lpVtbl->Release(pPersistFile);
        pShellLink->lpVtbl->Release(pShellLink);
        CoUninitialize();
        return 5;
    }

    // Convert the target path to a multi-byte string
    if (!targetPath)
    {
        pPersistFile->lpVtbl->Release(pPersistFile);
        pShellLink->lpVtbl->Release(pShellLink);
        CoUninitialize();
        return 6;
    }

    // Clean up and return the target path
    pPersistFile->lpVtbl->Release(pPersistFile);
    pShellLink->lpVtbl->Release(pShellLink);
    CoUninitialize();
    return 0;
}

#endif

/*-----------------------------------------------------------------*/

int strip(char *s) {        /* remove trailing whitespace */
  int i = strlen(s);
  while (i >= 0 && s[i] <= ' ') {
    s[i--] = '\0';
  }
  return(0);
}

/*-----------------------------------------------------------------*/

int join(char *s, int n) {
  int i = 0;

  s[0]='\0';
  while (i<n) {
    strcat(s,words[i++]);  
    strcat(s," ");  
  }
  return(0);
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

      strcpy(words[w++], k);
      while(tmp[i] > '\0' && tmp[i] <= ' ' )
        i++;
      k = &tmp[i];
    } else {
      i++;
    }
  }
 
  strcpy(words[w++], k);
  return(w);
}
/*-----------------------------------------------------------------*/

int shift(char *s) {        /* shift string to the right */
  int i = strlen(s);
  while (i >= 0) {
    s[i+1] = s[i];
    i--;
  }
  return(0);
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

int isFile(char *s) {
  struct stat sb;

  if (stat(s, &sb) == 0 && S_ISREG(sb.st_mode)) {
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
    printf("%s exists\n",XBLANKFAC);
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
    printf("%s exists\n",XBLANKOBJ);
  }
  return(1);
}

/*-----------------------------------------------------------------*/

/*

Create library.txt and add all found zOrtho* tiles

Also scan "Earth nav data" folder to support "ortho bundles"

Include "xroads.add" and append "xroad.opt" files if exist.

zOrtho4XP_+51-011   >  REGION_RECT -011 +51 -011 +51

*/

int genLibrary() {
  FILE *fp;
  FILE *opt;
  DIR *d,*e,*l;
  struct dirent *dir,*earth,*lonlat;
  int rc = 0;

  char lon[8];
  char lat[8];
  char buf[MAX_TXT];
  char buf1[MAX_TXT];
  char buf2[MAX_TXT];

  if ( (fp = fopen(XLIB,"w")) != NULL ) {
    printf("creating library.txt\n");
    fputs("A\n800\nLIBRARY\n",fp);

    /*  add optional tile coordinates to library */
    if ( (opt = fopen("xroads.pre","r")) ) {
      printf("prepending xroads.pre\n");
      while ( fgets(buf, MAX_TXT, opt) != NULL ) {
        fputs(buf,fp);
      }
      fclose(opt);
    }
    fputs("\nREGION_DEFINE Xroads\n",fp);

    d = opendir(XSCENERYDIR);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if( ! strncmp(dir->d_name,"zOrtho",strlen("zOrtho")) || 
            ! strncmp(dir->d_name,"zPhoto",strlen("zPhoto")) ||
            ! strncmp(dir->d_name,"zVStates",strlen("zVStates")) ||
            ! strncmp(dir->d_name,"z_",strlen("z_")) ) {

          /* ortho folder */
          strcpy(buf1,XSCENERYDIR);
          strcat(buf1,"/");
          strcat(buf1,dir->d_name);
#ifdef _WIN32
          if ( strstr(dir->d_name,".lnk") ) {       //  if shortcut, resolve target          
            // printf("trying shortcut %s ...\n",dir->d_name);
            mbstowcs(wshortcut,buf1,MAX_PATH);
            rc = GetShortcutTargetPath(wshortcut);
            if ( ! rc ) {
              printf("following shortcut: %s -> %s\n",dir->d_name,targetPath);
              strcpy(buf1,targetPath);
            } else {
              printf("could not follow shortcut: %s (rc=%d)\n",dir->d_name,rc);
            }
          }
#endif
          strcat(buf1,"/Earth nav data");  
          // printf("trying1 %s \n", buf1);
          e = opendir(buf1);
          if (e) {
            while ((earth = readdir(e)) != NULL) {    /* scan earth nav data */
              if ( earth->d_name[0] != '.' ) {
                strcpy(buf2,buf1);
                strcat(buf2,"/");
                strcat(buf2,earth->d_name);
                // printf("trying2 %s \n", buf2);
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
          } else if( strstr(dir->d_name,"zOrtho4XP_") ) {
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
      closedir(d);
    }

    /*  add optional tile coordinates to library */
    if ( (opt = fopen("xroads.add","r")) ) {
      printf("appending xroads.add\n");
      while ( fgets(buf, MAX_TXT, opt) != NULL ) {
        fputs(buf,fp);
      }
      fclose(opt);
    }

    /* add net file re-routes */
    fputs("\nREGION Xroads\nEXPORT_EXCLUDE lib/g10/roads.net 1000_roads/roads.net\nEXPORT_EXCLUDE lib/g10/roads_EU.net 1000_roads/roads_EU.net\n",fp);

    /* add object re-routes */
    if ( hasXE ) {
      fputs("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac   objects/Parking_Cars.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/Parking_Trucks.fac\n",fp);
    } else {
      fputs("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac   objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/blank.fac\n",fp);
    }
    fputs("EXPORT_EXCLUDE simheaven/ground/solar_panel.obj    objects/blank.obj\n",fp);

    /* add optional lines to the end of the library */
    if ( (opt = fopen("xroads.opt","r")) ) {
      printf("appending xroads.opt\n");
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

int genNetFile(char *s_in,char *s_out, int opts) {

  FILE *in,*out;
  char infile[MAX_TXT];
  char outfile[MAX_TXT];
  char buf[MAX_TXT];
  char *ch;
  unsigned int speed = 0;
  unsigned int n = 0;
  int rail = 0;
  int hwy = 0;
  int lht = 0;
  int skipNext = 0;
  

  sprintf(infile,"%s/%s",DEFROADS,s_in);
  sprintf(outfile,"%s/%s",XROADS,s_out);
  if ( (in = fopen(infile,"r")) ) {
    if ( (out = fopen(outfile,"w")) ) {
      printf("creating %s\n",s_out);
      while ( fgets(buf, MAX_TXT, in) ) {
        strip(buf);
        if ( strstr(buf,"# Group: ") ) {
          if ( strstr(buf,"GRPHwyBYTs") || strstr(buf,"GRP_HIGHWAYS") ) {
            hwy = 1;
          } else if ( (strstr(buf,"GRPLocal") || strstr(buf,"GRPPrimary") || 
                      strstr(buf,"GRP_rail") || strstr(buf,"GRPSecondary")) && 
                      ! strstr(buf,"OneW") ) {
            lht = 1;
          } else if ( strstr(buf,"GRP_RAIL") ) {
            rail = 1;
          } else {
            hwy = 0;
            lht = 0;
          }
        } else if ( ! hwy && ! rail && ( strstr(buf,"QUAD ") || strstr(buf,"TRI ") ) ) {
          shift(buf);
          buf[0] = '#';
        } else if ( ! rail && strstr(buf,"SEGMENT_DRAPED ") ) {
          shift(buf); 
          buf[0] = '#';
        } else if ( strstr(buf,"CAR_DRAPED") || strstr(buf,"CAR_GRADED") ) {
          ch = strtok(buf, " \t");
          n = 0;
          while (ch != NULL) {
            strcpy(words[n++],ch);
            ch = strtok(NULL, " \t");
          }
          if ( n > 3 ) {
            if ( lht && (opts&OPT_LHT) ) {
              if ( words[1][0] == '0' )  // swap RH/LH driving
                words[1][0] = '1';
              else
                words[1][0] = '0';
            }
            speed = atoi(words[3]) * defSpeed / 100;
            sprintf(words[3],"%d",speed);
            n = join(buf,n);
            strip(buf);
          }
        } else if ( strstr(buf,"autogen_tree") ) {   /* ignore trees on roads */
          shift(buf);
          buf[0] = '#';
        } else if ( (opts&OPT_NO_HWY_LTS) && (strstr(buf,"HwyLt") || strstr(buf,"RmpLt")) ) {
          shift(buf);
          buf[0] = '#';
          skipNext = 1;
        } else if ( skipNext ) {
          shift(buf);
          buf[0] = '#';
          skipNext = 0;
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
  
  sprintf(infile,"%s/%s/objects/ground/%s",XSCENERYDIR,XSCENE,s);
  sprintf(outfile,"%s/%s",XROBJS,s);
  if ( (in = fopen(infile,"r")) ) {
    if ( (out = fopen(outfile,"w")) ) {
      while ( fgets(buf, MAX_TXT, in) != NULL ) {
        strip(buf);  
        if ( strstr(buf,"fencing.png") ) {
          sprintf(buf,"TEXTURE ../../%s/objects/ground/fencing.png",XSCENE);
        } else if ( strstr(buf,"Roof_Asphalt.png") ) {
          sprintf(buf,"TEXTURE ../../%s/objects/blank.dds",XSCENE);
        } else if ( strstr(buf,"Omni_Parking_Lit.obj") ) {
          sprintf(buf,"OBJ ../../%s/objects/ground/Omni_Parking_Lit.obj",XSCENE);
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
  int i;

  printf("Xroads - %s - melbo @x-plane.org\n",VERSION);

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
  } else {
    printf("%s exists\n",XROADSDIR);
  }

  if ( ! isDir(XROADS) ) {
    if ( mkdir(XROADS,0755) ) {
      printf("ERROR: cannot create %s\n",XROADS);
      return(-1);
    } else {
      printf("%s created\n",XROADS);
    }
  } else {
    printf("%s exists\n",XROADS);
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
    printf("%s exists\n",XTEXTURES);
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
    printf("%s exists\n",XOBJECTS);
  }

  genNetFile("roads.net",   "roads.net",0);
  genNetFile("roads_EU.net","roads_EU.net",OPT_NO_HWY_LTS);
  genNetFile("roads.net",   "roads_LH.net",OPT_LHT);
  genNetFile("roads_EU.net","roads_UK.net",OPT_NO_HWY_LTS|OPT_LHT);

  if ( ! isDir(XROBJS) ) {
    if ( mkdir(XROBJS,0755) ) {
      printf("ERROR: cannot create %s\n",XROBJS);
      return(-1);
    } else {
      printf("%s created\n",XROBJS);
    }
  } else {
    printf("%s exists\n",XROBJS);
  }

  genBlankFac();
  genBlankObj();


  // identify installed SimHeaven scenery packs

  for ( i=0 ; XSCENES[i] != NULL ; i++ ) {
    sprintf(XTEST,"%s/%s",XSCENERYDIR,XSCENES[i]);
    if ( isDir(XTEST) ) {
     strcpy(XSCENE,XSCENES[i]);
  }
 
  }
  if ( strlen(XSCENE) > 0 ) {
    hasXE = 1;
    printf("setting up %s parking\n",XSCENE);
    genFacFile("Parking_Cars.fac");
    genFacFile("Parking_Trucks.fac");
  } else {
    printf("info: no supported SimHeaven scenery found\n");
  }

  genLibrary();

}


/*
   example library.txt

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

EXPORT_EXCLUDE simheaven/ground/parking_cars.fac   objects/Parking_Cars.fac
EXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/Parking_Trucks.fac
EXPORT_EXCLUDE simheaven/ground/solar_panel.obj    objects/blank.obj

*/
