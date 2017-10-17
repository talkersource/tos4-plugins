/* TalkerOS Plugin                    for version 4.01 or higher
   -------------------------------------------------------------
   Originally written by Rob Melhuish aka 'Squirt'.
   Converted from RaMTITS to Amnuts 2.2.1 by Mags
   Plugin-compatible modifications by "William".
   100% compatible with TalkerOS ver 4.x.

   The initialization line for this plugin is:

           if (tmp=plugin_04x000_init(cmd)) cmd=cmd+tmp;

   The command call for this plugin is:

           if (!strcmp(plugin->registration,"04-000")) { plugin_04x000_main(user,str,comnum); return 1; }

   The following lines MUST be added INSIDE the user structure in
   the tos401.h file:

  	int plugin_04x000_first;
	char plugin_04x000_array[10];
	struct user_struct *plugin_04x000_opponent;
	
   Don't forget to #include "./plugins/tictac.c" in the main .c file!
   ------------------------------------------------------------- */

   extern UR_OBJECT create_user();
   extern UR_OBJECT get_user(char[]);
   extern CM_OBJECT create_cmd();
   extern PL_OBJECT create_plugin();
   extern char* remove_first(char*);

/* ------------------------------------------------------------- */

void plugin_04x000_reset_tictac(UR_OBJECT);
void plugin_04x000_print_tic(UR_OBJECT);
int plugin_04x000_legal_tic(char*, int, int);
int plugin_04x000_win_tic(char*);
void plugin_04x000_tictac(UR_OBJECT, char*);

#define PLUGIN_TICTAC_LEV XUSR

plugin_04x000_init(cm)
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
strcpy(plugin->name,"2-Player TicTacToe");       /* Plugin Description   */
strcpy(plugin->author,"'Squirt'/W.Price");      /* Author's name        */
strcpy(plugin->registration,"04-000");          /* Plugin/Author ID     */
strcpy(plugin->ver,"1.0");                      /* Plugin version       */
strcpy(plugin->req_ver,"401");                  /* TOS version required */
plugin->id = cm;                                /* ID used as reference */
plugin->req_userfile = 1;                       /* Requires user data?  */
                                                /* (no separate file required
                                                    since it keeps its data
                                                    in a central file, but
                                                    we need to do housekeeping
                                                    procedures when the user
                                                    leaves, so we set this to
                                                    1 so that we are notified
                                                    when a user leaves.) */
plugin->triggerable = 0;                        
/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
	}
i++;                                            /* Keep track of number created */
strcpy(com->command,"tictac");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = PLUGIN_TICTAC_LEV;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;
/* end creating command - repeat as needed for more commands */

write_syslog("PLUGIN: TicTacToe is installed.\n",0,SYSLOG);
return i;
}


plugin_04x000_main(user,str,comid)
UR_OBJECT user;
char *str;
int comid;
{
char filename[80];
sprintf(filename,"%s/%s.X",USERFILES,user->name);
switch (comid) {
        /* put case calls here for each command needed.
           remember that the commands will be in order, with the first
           command always being 1;
        */
        case -5: return 1;
        case -6: return 1;
        case -7: plugin_04x000_reset_tictac(user);
        	 unlink(filename);
        	 return 1;
        case -8: plugin_04x000_reset_tictac(user);
                 return 1;
        case -9: strcpy(user->plugin_04x000_array, "000000000");
		 user->plugin_04x000_first=0;
		 return 1;
        case  1: plugin_04x000_tictac(user,str); return 1;
        default: return 0;
        }
}


int plugin_04x000_legal_tic(char *array,int move,int plugin_04x000_first) {          
int count1=0,count2=0;
int i;
       
if (array[move-1]==1||array[move-1]==2) return 0;
for (i=0;i<9;i++) {
 if (array[i]==1) count1++;
 else if (array[i]==2) count2++;
 }
if (count1>count2) return 0;
if (plugin_04x000_first!=1) if (count1==count2) return 0; 
return 1;
}

int plugin_04x000_win_tic(char *array) {
int i,j;
int person;
    
for (person=1;person<3;person++) {        
 for (i=0;i<3;i++) 
  for (j=0;j<3;j++) {
         if (array[i*3+j]!=person) break;
            if (j==2) return person;
           }
 for (i=0;i<3;i++) 
  for (j=0;j<3;j++) {
   if (array[j*3+i]!=person) break;
   if (j==2) return person;
   }  
 if (array[0]==person&&array[4]==person&&array[8]==person)
  return person;
 if (array[2]==person&&array[4]==person&&array[6]==person)
  return person;
 }
for (i=0,j=0;i<9;i++) {
 if (array[i]==1||array[i]==2) j++;
 }
if (j==9) return 3;
return 0;
}

void plugin_04x000_print_tic(UR_OBJECT user)
{          
char temp_s[ARR_SIZE];
char array[10];
int i;
          
for(i=0;i<9;i++) {
     if (user->plugin_04x000_array[i]==1) array[i]='X';
     else if (user->plugin_04x000_array[i]==2) array[i]='O';
     else array[i]=' ';
     }
     write_user(user,"\n");
     write_user(user,"~OL~FB.=============================.~RS\n");
     write_user(user,"~OL~FB!   ~FM<~FG1~FM>~FB   |   ~FM<~FG2~FM>~FB   |   ~FM<~FG3~FM>~FB   !~RS\n");
     write_user(user,"~OL~FB!         |         |         !~RS\n");
     sprintf(temp_s, "~OL~FB!    ~FY%c~FB    |    ~FY%c~FB    |    ~OL~FY%c~FB    !~RS    ~OL~FTYour Opponent Is~FW: ~FY%s ~FM(~FYO~FM)\n",array[0],array[1],array[2],user->plugin_04x000_opponent->name);
     write_user(user,temp_s);
     sprintf(temp_s, "~OL~FB!         |         |         !~RS\n");
     write_user(user,temp_s);
     sprintf(temp_s, "~OL~FB!---------+---------+---------!~RS\n");
     write_user(user,temp_s);
     write_user(user,"~OL~FB!   ~FM<~FG4~FM>~FB   |   ~FM<~FG5~FM>~FB   |   ~FM<~FG6~FM>~FB   !~RS\n");
     write_user(user,"~OL~FB!         |         |         !~RS\n");
     sprintf(temp_s, "~OL~FB!    ~FY%c~FB    |    ~FY%c~FB    |    ~FY%c~FB    !~RS\n",array[3],array[4],array[5]);
     write_user(user,temp_s);
     write_user(user,"~OL~FB!         |         |         !~RS\n");
     write_user(user,"~OL~FB!---------+---------+---------!~RS\n");
     write_user(user,"~OL~FB!   ~FM<~FG7~FM>~FB   |   ~FM<~FG8~FM>~FB   |   ~FM<~FG9~FM>~FB   !~RS\n");
     write_user(user,"~OL~FB!         |         |         !~RS\n");
     sprintf(temp_s, "~OL~FB!    ~FY%c~FB    |    ~FY%c~FB    |    ~FY%c~FB    !~RS\n",array[6],array[7],array[8]);
     write_user(user,temp_s);
     write_user(user,"~OL~FB!         |         |         !~RS
");
     write_user(user,"~OL~FB+=============================+~RS\n");
     write_user(user,"
");
}

void plugin_04x000_reset_tictac(user)
UR_OBJECT user;
{          
user->plugin_04x000_opponent=0;
strcpy(user->plugin_04x000_array,"000000000");
user->plugin_04x000_first=0;
}

void plugin_04x000_tictac(UR_OBJECT user,char *inpstr)
{

UR_OBJECT u;
char temp_s[ARR_SIZE];
char *remove_first();
int move;

if (word_count<2) {
 write_user(user,"Usage   :  tictac [<user>|<#>|reset|show|say]\n");
 write_user(user,"Examples:  tictac <user>   =  Challenge a user to Tic Tac Toe\n");
 write_user(user,"           tictac <#>      =  Place an 'X' in spot '#'\n");
 write_user(user,"           tictac reset    =  Reset Tic Tac Toe\n");
 write_user(user,"           tictac show     =  Display the game board\n");
 write_user(user,"           tictac say      =  Speak to your opponent.\n");
 return;
 }
if (strstr(word[1],"reset")) {
 write_user(user,"~OLResetting The Current Tic Tac Toe Game...\n");
 plugin_04x000_reset_tictac(user);
 if (user->plugin_04x000_opponent!=NULL) plugin_04x000_reset_tictac(user->plugin_04x000_opponent);
 return;
 }
if (strstr(word[1],"show")) {
 if (!user->plugin_04x000_opponent) {
  write_user(user,"You are not currently playing Tic Tac Toe!\n");
  return;
  }
 plugin_04x000_print_tic(user);
 return;
 }
if (strstr(word[1],"say")) {
 if (!user->plugin_04x000_opponent) {
  write_user(user,"~OLYou have to be playing a game of Tic Tac Toe!\n");
  return;
  }
 inpstr=remove_first(inpstr);
 if (!inpstr[0]) { write_user(user,"Say what to your opponent?\n"); return; }
 sprintf(text,"~OL-> You say to %s: ~RS\"%s\"\n",user->plugin_04x000_opponent->name,inpstr);
 write_user(user,text);
 sprintf(text,"~OL-> %s says to you: ~RS\"%s\"\n",user->plugin_04x000_opponent->name,inpstr);
 write_user(user->plugin_04x000_opponent,text);
 return;
 }
if (atoi(word[1])>9) {
 write_user(user,"~OLThere are only nine spots on the Tic Tac Toe board!\n");
 return;
 }
if (!isdigit(word[1][0])) {
        u=get_user(word[1]);
 if (u==NULL) {
         write_user(user,notloggedon);
                return;
        }
 if (u==user) {
  write_user(user,"~OLYou cannot play Tic Tac Toe With yourself!\n");
  return;
  }
        plugin_04x000_reset_tictac(user);
     user->plugin_04x000_opponent=u;
     if (user->plugin_04x000_opponent->plugin_04x000_opponent) {
         if (user->plugin_04x000_opponent->plugin_04x000_opponent!=user) {
                        write_user(user, "Sorry, that person is already playing Tic Tac Toe.\n");
             return;
            }
               if (user->plugin_04x000_opponent->level<PLUGIN_TICTAC_LEV) {
                    write_user(user,"~OLThat user doesn't have Tic Tac Toe!\n");
             return;
            }
               sprintf(temp_s, "~OL%s agrees to play Tic Tac Toe with you!\n",user->name);
               write_user(user->plugin_04x000_opponent,temp_s);
               sprintf(temp_s, "~OLYou agree to a game of Tic Tac Toe with %s.\n",user->plugin_04x000_opponent->name);
         write_user(user,temp_s);
               sprintf(temp_s,"~OL%s starts playing Tic Tac Toe with %s.\n",user->name,user->plugin_04x000_opponent->name);
         write_room(user->room,temp_s);
         if (user->plugin_04x000_opponent->room != user->room) write_room(user->plugin_04x000_opponent->room, temp_s);
         plugin_04x000_print_tic(user); plugin_04x000_print_tic(user->plugin_04x000_opponent);
         return;
        }
     else {
               sprintf(temp_s, "~OL%s wants to play a game of Tic Tac Toe with you!\nYou can use '.tictac %s' to accept the game!\n",user->name,user->name);
         write_user(user->plugin_04x000_opponent,temp_s);
               sprintf(temp_s,"~OLYou ask %s to play a game of Tic Tac Toe.\n",user->plugin_04x000_opponent->name);
         write_user(user,temp_s);
         return;
        }
    }   
if (!user->plugin_04x000_opponent) {
 write_user(user,"~OLYou are not playing Tic Tac Toe with anyone!\n");
     return;
 }
if (user->plugin_04x000_opponent->plugin_04x000_opponent!=user) {
     write_user(user,"~OLYour opponent has not accepted yet.\n");
     return;
    }
if (!strcmp(user->plugin_04x000_array,"000000000") && !user->plugin_04x000_opponent->plugin_04x000_first) {
     user->plugin_04x000_first=1;
    }
move=word[1][0]-'0';
if (plugin_04x000_legal_tic(user->plugin_04x000_array,move,user->plugin_04x000_first)) {
     user->plugin_04x000_array[move-1] = 1;
     user->plugin_04x000_opponent->plugin_04x000_array[move-1] = 2;
     plugin_04x000_print_tic(user);
     plugin_04x000_print_tic(user->plugin_04x000_opponent);
    }
else {
     write_user(user,"~OL~FRThat is an illegal move!\n");
 write_user(user,"~OLIf this is your first move, try '.tictac reset' and then re-start the game.\n");
     return;
    }   
if (!plugin_04x000_win_tic(user->plugin_04x000_array)) return;
if (plugin_04x000_win_tic(user->plugin_04x000_array)==1) {
        sprintf(temp_s, "~OL%s has beaten %s at Tic Tac Toe!\n",user->name,user->plugin_04x000_opponent->name);
        }
else if (plugin_04x000_win_tic(user->plugin_04x000_array) == 2) {
        sprintf(temp_s, "~OL%s has beaten %s at Tic Tac Toe!\n",user->plugin_04x000_opponent->name, user->name);
        }
else {
        sprintf(temp_s,"~OLIt's a draw between %s and %s.\n",user->name,user->plugin_04x000_opponent->name);
    }
write_room(user->room, temp_s);
if (user->room != user->plugin_04x000_opponent->room) write_room(user->plugin_04x000_opponent->room, temp_s);
strcpy(user->plugin_04x000_array, "000000000");
strcpy(user->plugin_04x000_opponent->plugin_04x000_array,"000000000");
user->plugin_04x000_first=0;
user->plugin_04x000_opponent->plugin_04x000_first=0;
user->plugin_04x000_opponent->plugin_04x000_opponent=NULL;
user->plugin_04x000_opponent=NULL;
}
