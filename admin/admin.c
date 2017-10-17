/* TalkerOS Plugin                    for version 4.01 or higher
   -------------------------------------------------------------
   All commands and code by "Weaver".
   (Hopefully) 100% compatible with TalkerOS ver 4.x.

   The initialization line for this plugin is:

           if (tmp=plugin_02x001_init(cmd)) cmd=cmd+tmp;

   The command call for this plugin is:

           if (!strcmp(plugin->registration,"02-001")) { plugin_02x001_main(user
,str,comnum); return 1; }


   ------------------------------------------------------------- */


#include <dirent.h>
#include <regex.h>

   extern UR_OBJECT create_user();   
   extern UR_OBJECT get_user(char[]);           
   extern CM_OBJECT create_cmd();             
   extern PL_OBJECT create_plugin();


  /* This is a duplicate forward declaration of function remove_first.
     It has to be here because the other forward declaration has not been
     parsed when this is parsed. Remove it, and you get a nice collection
     of warnings ;)
  */
char *remove_first();

plugin_02x001_init(cm)
int cm;
{
PL_OBJECT plugin;
CM_OBJECT com;
int i,verFail;
i=0; verFail=0;
/* create plugin */
if ((plugin=create_plugin())==NULL) {
        write_syslog("ERROR: Unable to create new registry entry!\n",0,SYSLOG);
        return 0;
        }
strcpy(plugin->name,"admin commands");          /* Plugin Description   */
strcpy(plugin->author,"Weaver");                /* Author's name        */
strcpy(plugin->registration,"02-001");          /* Plugin/Author ID     */
strcpy(plugin->ver,"1.0");                      /* Plugin version       */
strcpy(plugin->req_ver,"401");                  /* TOS version required */
plugin->id = cm;                                /* ID used as reference */
plugin->req_userfile = 0;                       /* Requires user data?  */
plugin->triggerable = 0;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/
strcpy(com->command,"usersfrom");               /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = WIZ;                             /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/
strcpy(com->command,"fullsamesite");            /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = WIZ;                             /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;


/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/

strcpy(com->command,"samesite");                /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = WIZ;                             /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;
/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/


/* end creating command - repeat as needed for more commands */

return i;
}

plugin_02x001_main(user,str,comid)
UR_OBJECT user;
char *str;
int comid;
{
switch (comid) {
        /* put case calls here for each command needed.
           remember that the commands will be in order, with the first
           command always being 1;
        */
        case  1: plugin_02x001_usersfrom(user, str); return 1;
        case  2: plugin_02x001_fullsamesite(user, str); return 1;
        case  3: plugin_02x001_samesite(user, str); return 1;
        default: return 0;
        }
}





plugin_02x001_usersfrom(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
  int i,j;
  FILE *fp;
  char line[82], filename[80], pattern[82], message[162], the_name[80];
  char c;
  DIR * myDir;
  struct dirent * myEntry;

  
  if (word_count < 2) {                                 
    write_user(user,"Usersfrom for which site?\n");  
    return;
  }

    /* compiling regexp pattern */
  sprintf(pattern, ".*%s.*", word[1]);
  if (re_comp(pattern) != 0) {
    write_user(user,"Usersfrom: rexex pattern could not be built\n");  
    return 0;
  };

    /* open directory */
  myDir = opendir(USERFILES);
  if (myDir == NULL) {
    write_user(user,"Usersfrom: could not open directory\n");  
    return 0;
  }

  while( (myEntry = readdir(myDir)) ) {                                  
    if ((myEntry->d_name[strlen(myEntry->d_name) - 2] == '.') &&
        (myEntry->d_name[strlen(myEntry->d_name) - 1] == 'D')) {

      sprintf(filename,"%s/%s",USERFILES,myEntry->d_name);
      if (!(fp=fopen(filename,"r")))
        return 0;

      i = 1;
      j = 0;
      c=getc(fp);
      while(!feof(fp)) {
        if (c == '\n') {  /* a newline is found */
          line[j] = 0;
          if (i == 7) {   /* the 7th line contains the host */
            if (re_exec(line) == 1) {  /* pattern found */
              strcpy(the_name, myEntry->d_name);
              the_name[strlen(myEntry->d_name) - 2] = 0;
              sprintf(message, "%-20s %s\n", the_name, line);
              write_user(user, message);  
            }
          }
          j = 0;
          i++;
        }
        else {  /* deal with non-newline chars - they are added to the string */
          if (j >= 82) {
            write_user(user,"Usersfrom: line in user file too long\n");  
            fclose(fp);
            return;
          }
          line[j] = c;
          j++;
        }
        c=getc(fp);
      }
      fclose(fp);
    }
  }

  if (closedir(myDir) != 0) {
    write_user(user,"Usersfrom: could not close directory\n");  
    return 0;
  }
}

plugin_02x001_fullsamesite(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
  int ret;
  int i,j;
  char line[82], filename[80];
  char c;
  FILE *fp;
  
  if (word_count < 2) {                                 
    write_user(user,"Fullsamesite for who?\n");  
    return;
  }

  sprintf(filename,"%s/%s.D",USERFILES, word[1]);
  if (!(fp=fopen(filename,"r"))) {
    write_user(user,"Fullsamesite: User not found!\n");  
    return 0;
  }

  i = 1;
  j = 0;
  c=getc(fp);
  while(!feof(fp)) {
    if (c == '\n') {  /* a newline is found */
      line[j] = 0;
      if (i == 7) {    /* the 7th line contains the host */
        strncpy(word[1], line, WORD_LEN - 1);  /* -1 so there is space for 
                                                  the 0 */
      }
      j = 0;
      i++;
    }
    else {  /* deal with non-newline chars - they are added to the string */
      if (j >= 82) {
        write_user(user,"Fullsamesite: line in user file too long\n");  
        fclose(fp);
        return;
      }
      line[j] = c;
      j++;
    }
    c=getc(fp);
  }
  fclose(fp);
  plugin_02x001_usersfrom(user,inpstr);

  return 0;
}


plugin_02x001_samesite(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
  int ret;
  int i,j;
  char line[82], filename[80], *shorthostname;
  char c;
  FILE *fp;
  
  if (word_count < 2) {                                 
    write_user(user,"Samesite for who?\n");  
    return;
  }

  sprintf(filename,"%s/%s.D",USERFILES, word[1]);
  if (!(fp=fopen(filename,"r"))) {
    write_user(user,"Samesite: User not found!\n");  
    return 0;
  }

  i = 1;
  j = 0;
  c=getc(fp);
  while(!feof(fp)) {
    if (c == '\n') {  /* a newline is found */
      line[j] = 0;
      if (i == 7) {    /* the 7th line contains the host */
        /* copy only hostname after the first . - if the name
           contains no . copy the whole hostname 
           the cast is only necessary cuz its not really 
           adviseable to properly include strings.h cuz 
           that would cause errors when compiling other files
           (though it would not be a bad idea to clean that up */
        shorthostname = (char *)strchr(line, '.');
        if (shorthostname == NULL)
          strncpy(word[1], line, WORD_LEN - 1);
        else
            /* ewww .... pointer arithmtic ... be careful ... */
          strncpy(word[1], (++shorthostname), WORD_LEN - 1);
      }
      j = 0;
      i++;
    }
    else {  /* deal with non-newline chars - they are added to the string */
      if (j >= 82) {
        write_user(user,"Samesite: line in user file too long\n");  
        fclose(fp);
        return;
      }
      line[j] = c;
      j++;
    }
    c=getc(fp);
  }
  fclose(fp);

  plugin_02x001_usersfrom(user,inpstr);

  return 0;
}

