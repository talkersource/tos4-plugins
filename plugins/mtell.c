/* The TalkerOS Multi-Tell Plugin
   -------------------------------------------------------------
   The initialization code for this plugin is:

      if (tmp=plugin_02x000_init(cmd)) cmd=cmd+tmp;

   The command call for this plugin is:

      if (!strcmp(plugin->registration,"02-000")) { plugin_02x000_main(user,str,comnum); return 1; }

   Be sure to #include "./plugins/mtell.c" at the top of the file.

   No further modifications are necessary.
   ------------------------------------------------------------- */

   extern UR_OBJECT get_user(char[]);
   extern CM_OBJECT create_cmd();
   extern PL_OBJECT create_plugin();

/* ------------------------------------------------------------- */

plugin_02x000_init(cm)
int cm;
{
PL_OBJECT plugin;
CM_OBJECT com;
int i;
i=0;
/* create plugin */
if ((plugin=create_plugin())==NULL) {
        write_syslog("ERROR: Unable to create new registry entry!\n",0,SYSLOG);
        return 0;
        }
strcpy(plugin->name,"Multi-Tell (9-user)") ;    /* Plugin Description   */
strcpy(plugin->author,"William Price");         /* Author's name        */
strcpy(plugin->registration,"02-000");          /* Plugin/Author ID     */
strcpy(plugin->ver,"1.0");                      /* Plugin version       */
strcpy(plugin->req_ver,"400");                  /* Runtime ver required */
plugin->id = cm;                                /* ID used as reference */
plugin->req_userfile = 0;                       /* Requires user data?  */
plugin->triggerable = 0;                        /* Can be triggered by
                                                   regular speech? */

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
}                                                               
i++;                                            /* Keep track of number created */
strcpy(com->command,"mtell");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;                                /* Per-plugin command ID */
com->plugin = plugin;                           /* Link to parent plugin */
/* end creating command - repeat as needed for more commands */
return i;
}                  

plugin_02x000_main(user,str,comid)
UR_OBJECT user;
char *str;
int comid;
{
        /* put case calls here for each command needed.
           remember that the commands will be in order, with the first
           command always being 1
        */
switch (comid) {
        case  1: plugin_02x000_mtell(user,str); return 1;
        case -5: /* Trigger Heartbeat */
        case -6: /* Talker shutdown */
        case -7: /* 1st to save */
        case -8: /* save normal */
        case -9: /* load user data */
        default: return 0;
	}
}


/*** Tell MULTIPLE users something ***/
plugin_02x000_mtell(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
UR_OBJECT u;
char type[5],*name;
int i,num,rx,done;
int sent[MAX_WORDS];

for (i=0; i<MAX_WORDS; i++) sent[i] = 0;

if (user->muzzled) {
        write_user(user,"-=- You are muzzled... Action denied. -=-\n");  
        return;
        }
if (word_count<3) {
        write_user(user,"Multi-Tell which people and what?\nUsage: .mtell <user1> [<user2>...<user9>] <message>\n");  return;
        }
done=0; rx=0;
for (num=1; num < MAX_WORDS && !done; num++) {
	if (!(u=get_user(word[num]))) { done=1; continue; }
	else rx++;
	}
if (!rx) {
        write_user(user,"Couldn't find a recipient name to send the message to.\nThis command quits looking for user names after the first non-user it finds.\n");  return;
        }
if (word_count < rx + 2) { 
	write_user(user,"Multi-Tell which people and what?\nUsage: .mtell <user1> [<user2>...<user9>] <message>\n");  return;
}

// Check for Duplicates
for (num = 1; num <= rx; num++) {
	if (get_user(word[num]) == user) {
		write_user(user,"Do not include yourself on a multi-tell recipient list.\n");
		return;
	}
	if (rx<2) continue;
	for (i = 1; i<=rx; i++) {
		if (i == num) continue;
		if (get_user(word[i]) == get_user(word[num])) {
			sprintf(text,"Duplicate usernames(s) found in multi-tell recipient list: %s %s\n",word[i],word[num]);
			write_user(user,text);
			write_user(user,"Be sure that you clairify any similar names if you abbreviated and that\nthe first word of your MESSAGE is *not* a user name or an abbreviation of a user name.\n");
			return;
		}
	}
}
	

if (user->vis) name=user->name; else name=invisname;

// Remove the recipient names.
for (num = 1; num <= rx; num++) inpstr=remove_first(inpstr);

// Do the send.
for (num = 1; num <= rx; num++) {
u=get_user(word[num]);

if (u->afk) {
        if (u->afk_mesg[0])
                sprintf(text,"%s is AFK, message is: %s\n",u->name,u->afk_mesg);
        else sprintf(text,"%s is AFK at the moment.\n",u->name);
        write_user(user,text);
        write_user(user,"Your message has been stored in the user's .revtell buffer.\n");
//        inpstr=remove_first(inpstr);
        if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
        else strcpy(type,"tell");
        sprintf(text,"(AFK) [MULTI] ~RS~OL%s%s %ss you:~RS%s %s\n",colors[CTELLUSER],name,type,colors[CTELL],inpstr);
        record_tell(u,text);
        u->primsg++;
	sent[num]++;
        continue;
        }
if ((u->misc_op >= 3 && u->misc_op <= 5) || (u->misc_op >= 8 && u->misc_op <= 10)) {
        sprintf(text,"%s is using the editor.\n",u->name);
        write_user(user,text);
        write_user(user,"Your message has been stored in the user's .revtell buffer.\n");
//        inpstr=remove_first(inpstr);
        if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
        else strcpy(type,"tell");
        sprintf(text,"(EDIT) [MULTI] ~RS~OL%s%s %ss you:~RS%s %s\n",colors[CTELLUSER],name,type,colors[CTELL],inpstr);
        record_tell(u,text);
        u->primsg++;
	sent[num]++;
        return;
        }

if (u->ignall && (user->level<WIZ || u->level>user->level)) {
        if (u->malloc_start!=NULL) 
                sprintf(text,"%s is using the editor at the moment.\n",u->name);
        else sprintf(text,"%s is ignoring everyone at the moment.\n",u->name);
        write_user(user,text);  
        continue;
        }
if (!strcmp(u->ignuser,user->name) || (u->igntell && (user->level<WIZ || u->level>user->level))) {
        sprintf(text,"%s is ignoring tells at the moment.\n",u->name);
        write_user(user,text);
        continue;
        }
if (u->room==NULL) {
        sprintf(text,"%s is offsite and would not be able to reply to you.\n",u->name);
        write_user(user,text);
        continue;
        }
//inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"tell");
if (!user->vis && u->level >= user->level) { sprintf(text,"%s[ %s ] ",colors[CSYSTEM],user->name); write_user(u,text); }
sprintf(text,"[MULTI] ~RS~OL%s%s %ss you:~RS%s %s\n",colors[CTELLUSER],name,type,colors[CTELL],inpstr);
write_user(u,text);
record_tell(u,text);
if (u->type==BOT_TYPE) {
        sprintf(text,"%s[ %s tells the bot: %s ~RS%s]\n",colors[CSYSTEM],user->name,inpstr,colors[CSYSTEM]);
        write_duty(WIZ,text,NULL,user,0);
        }
sent[num]++;
}

sprintf(text,"~OL%sYou multi-%s",colors[CTELLSELF],type); write_user(user,text);
for (i=0; i<=rx; i++) {
	if (sent[i]) { 
		u=get_user(word[i]);
		sprintf(text," ~OL%s%s",colors[CTELLSELF],u->name);
		write_user(user,text);
	}
}

sprintf(text,"~OL%s: ~RS%s%s\n",colors[CTELLSELF],colors[CTELL],inpstr);
write_user(user,text);
record_tell(user,text);
}
