/* TalkerOS Plugin                    for version 4.01 or higher
   -------------------------------------------------------------
   Plugin-compatible modifications (and some further mutilations)
     by "Weaver".
   (Hopefully) 100% compatible with TalkerOS ver 4.x.

   all commands except join, mtell, and here by "Weaver"
   join, mtell, here modified from code provided by "Spoila"

   WARNING - This plugin requires random numbers and therefore
   includes stdlib.h

   The initialization line for this plugin is:

           if (tmp=plugin_01x001_init(cmd)) cmd=cmd+tmp;

   The command call for this plugin is:

           if (!strcmp(plugin->registration,"01-001")) { plugin_01x001_main(user
,str,comnum); return 1; }


   ------------------------------------------------------------- */


   extern UR_OBJECT create_user();   
   extern UR_OBJECT get_user(char[]);           
   extern CM_OBJECT create_cmd();             
   extern PL_OBJECT create_plugin();

#include <stdlib.h>

  /* This is a duplicate forward declaration of function remove_first.
     It has to be here because the other forward declaration has not been
     parsed when this is parsed. Remove it, and you get a nice collection
     of warnings ;)
  */
char *remove_first();

plugin_01x001_init(cm)
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
strcpy(plugin->name,"misc commands");       /* Plugin Description   */
strcpy(plugin->author,"Weaver/Spoila");      /* Author's name        */
strcpy(plugin->registration,"01-001");          /* Plugin/Author ID     */
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
strcpy(com->command,"show");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/
strcpy(com->command,"join");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/
strcpy(com->command,"spin");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/
strcpy(com->command,"dice");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/

strcpy(com->command,"coin");                /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/

strcpy(com->command,"mtell");                /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
        }
i++;                                            /* Keep track of number created
*/

strcpy(com->command,"here");                /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
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

plugin_01x001_main(user,str,comid)
UR_OBJECT user;
char *str;
int comid;
{
switch (comid) {
        /* put case calls here for each command needed.
           remember that the commands will be in order, with the first
           command always being 1;
        */
        case  1: plugin_01x001_show(user, str); return 1;
        case  2: plugin_01x001_join(user); return 1;
        case  3: plugin_01x001_spin(user); return 1;
        case  4: plugin_01x001_dice(user); return 1;
        case  5: plugin_01x001_coin(user); return 1;
        case  6: plugin_01x001_mtell(user, str); return 1;
        case  7: plugin_01x001_here(user); return 1;
        default: return 0;
        }
}





plugin_01x001_show(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
  if (word_count<2) {
    write_user(user,"Usage: show <command>\n");  
    return;
  }

  if (user->muzzled) {
    write_user(user,"-=- You are muzzled... Action denied. -=-\n");
    return;
  }

  sprintf(text,"%s%sType -->%s %s\n", colors[CSELF], colors[CBOLD], 
          colors[CDEFAULT], inpstr);
  write_room(user->room,text);
}




plugin_01x001_join(user)
UR_OBJECT user;
{
  UR_OBJECT u;
  RM_OBJECT rm;

  if (word_count<2){
    write_user(user,"Join who?\n"); 
    return;
  }

  if (!(u=get_user(word[1]))){
    write_user(user,notloggedon); 
    return;
  }

  if (u==user){
    write_user(user,"You are already with yourself!\n"); 
    return;
  }

  rm=u->room;

  if (!has_room_access(user,rm)){
    write_user(user,"That room is currently private, you must be invited fir
st\n");
    return;
   }

  if (user->room==rm){
    write_user(user,"You are already there!\n"); 
    return;
  }

  sprintf(text,"%s appears to join %s\n",user->name, u->name);
  write_room(rm,text);

  sprintf(text,"%s vanishes to join %s\n",user->name, u->name);
  write_room_except(user->room,text,user);

  reset_access(user->room);
  user->room=rm;
  look(user);
}


plugin_01x001_spin(user)
UR_OBJECT user;
{
  UR_OBJECT u;
  RM_OBJECT rm;
  int users, choice, i;

  rm = user->room;
  
  /* count users in room */
  users = 0;
  u = user_first;
  while (u != NULL) {
   if ((u->room == rm) && (u->vis))
     users++;
   u = u->next;
  }

  if (users<=1) {
    write_room(user->room, "This command only works if there are at least 2 people in the room!\n");
    return;
  }

  sprintf(text,"%s spins the bottle\n", user->name);
  write_room(user->room, text);

  u = user;
  while (u == user) {
    /* that strange looking recipe was taken from the rand manpage 
       I'm not sure the 1.0 is necessary, but leave it in so everything
       _is_ converted to real. Or whatever 1.0 is. */
    choice = 1 + (int) (1.0 * users * rand() / (RAND_MAX + 1.0));

    i = 0;
    u = user_first;
    while (i < choice) {
     if ((u->room == rm) && (u->vis)) 
       i++;
       /* fetch no next user if _the_ user has been found */
     if (i < choice)
       u = u->next;
    }
  }

  sprintf(text,"and the bottle points to ... %s%s%s\n",
          colors[CBOLD], u->name, colors[CDEFAULT]);
  write_room(user->room, text);
}


plugin_01x001_dice(user)
UR_OBJECT user;
{
  int dice;
  
  dice = 1 + (int) (6.0 * rand() / (RAND_MAX + 1.0));
  sprintf(text,"%s throws the dice and rolls a %s%d%s\n", user->name,
          colors[CBOLD], dice, colors[CDEFAULT]);
  write_room(user->room, text);
}

plugin_01x001_coin(user)
UR_OBJECT user;
{
  int coin;
  
  coin = 1 + (int) (2.0 * rand() / (RAND_MAX + 1.0));
  if (coin == 1)
    sprintf(text,"%s flips a coin ... the coin shows %sheads%s\n", user->name,
            colors[CBOLD], colors[CDEFAULT]);
  else
    sprintf(text,"%s flips a coin ... the coin shows %stails%s\n", user->name,
            colors[CBOLD], colors[CDEFAULT]);

  write_room(user->room, text);
}


plugin_01x001_mtell(user, inpstr)
UR_OBJECT user;
char *inpstr;
{
  int i, j;
  char message[201];
  char temp_s[201];

  if (word_count<4) {
    write_user(user,"Usage: mtell <user><user1> : <message>\n"); 
    return;
  }

  for (i = 0; inpstr[i] != ':' && i < strlen(inpstr); i++)
    ;
  i++;

  if (i > strlen(inpstr)) {
    write_user(user,"Usage: mtell <user><user1> : <message>\n"); 
    return;
  }

  if ((inpstr[i-2] != ' ') || (inpstr[i] != ' ')) {
    write_user(user,"mtell: You have to type a blank before and after the ':'!\n"); 
    return;
  }

  strcpy(message, &inpstr[i]);
  for (i = 1; strcmp(word[i], ":")!=0; i++) {
    strcpy(temp_s,word[i]);
    strcat(temp_s, " ");
    strcat(temp_s, message);
    strcpy(word[1], word[i]);
    tell(user,temp_s);
  }
}


plugin_01x001_here(user)
UR_OBJECT user;
{
  RM_OBJECT rm;
  UR_OBJECT u;
  int users;
  int  mins,idle;
  char line[USER_NAME_LEN+USER_DESC_LEN*2];


  rm=user->room;
  users=0;
  sprintf(text,"\n%s~OL%s-=-   People who are here with you                      Time On/Idle  -=-\n\n",
    colors[CPEOPLEHI], colors[CTEXT]);
  write_user(user,text);

  for(u=user_first;u!=NULL;u=u->next) {
    if (u->room!=rm || (!u->vis && u->level>user->level))
      continue;
    mins=(int)(time(0) - u->last_login)/60;
    idle=(int)(time(0) - u->last_input)/60;
    sprintf(line,"%s %-30.30s%s", u->name, u->desc, colors[CDEFAULT]);
    if (!u->vis) 
      sprintf(text,"%s*%s%-61.61s %3d/%02d\n", colors[CSYSTEM], 
        colors[CDEFAULT], line, mins, idle);
    else 
      sprintf(text," %-61.61s %3d/%02d\n",line,mins,idle);
    write_user(user,text);
  }
}
