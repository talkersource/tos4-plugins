/* The TalkerOS "User Stuff" Plugin
   -------------------------------------------------------------
   INSTRUCTIONS FOR INSTALLING THIS PLUGIN INTO THE MAIN FILE:
   
   The initialization code for this plugin is:
   if (tmp=plugin_03x000_init(cmd)) cmd=cmd+tmp;

   The command call for this plugin is:
   if (!strcmp(plugin->registration,"03-000")) { plugin_03x000_main(user,str,comnum); return 1; }

   Be sure to #include "./plugins/userlist.c" at the top of the file
   where it indicates to install the plugin "#include" lines.

   No further modifications are necessary.
   ------------------------------------------------------------- */

#ifndef _DIRENT_H
#include <dirent.h>
#endif

extern UR_OBJECT get_user(char[]);
extern CM_OBJECT create_cmd();
extern PL_OBJECT create_plugin();

/* ------------------------------------------------------------- */

plugin_03x000_init(cm)
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
strcpy(plugin->name,"User DBase Functions") ;    /* Plugin Description   */
strcpy(plugin->author,"William Price");         /* Author's name        */
strcpy(plugin->registration,"03-000");          /* Plugin/Author ID     */
strcpy(plugin->ver,"1.2");                      /* Plugin version       */
strcpy(plugin->req_ver,"402");                  /* Runtime ver required */
plugin->id = cm;                                /* ID used as reference */
plugin->req_userfile = 1;                       /* Requires user data?  */
plugin->triggerable = 1;                        /* Can be triggered by
                                                   regular speech? */

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
}                                                               
i++;                                            /* Keep track of number created */
strcpy(com->command,"userlist");                /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = WIZ;                             /* Required level for cmd. */
com->comnum = i;                                /* Per-plugin command ID */
com->plugin = plugin;                           /* Link to parent plugin */
/* end creating command - repeat as needed for more commands */

/* create associated command */
if ((com=create_cmd())==NULL) {
        write_syslog("ERROR: Unable to add command to registry!\n",0,SYSLOG);
        return 0;
}                                                               
i++;                                            /* Keep track of number created */
strcpy(com->command,"wizlist");                /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = NEW;                             /* Required level for cmd. */
com->comnum = i;                                /* Per-plugin command ID */
com->plugin = plugin;                           /* Link to parent plugin */
/* end creating command - repeat as needed for more commands */

return i;
}                  

plugin_03x000_main(user,str,comid)
UR_OBJECT user;
char *str;
int comid;
{
        /* put case calls here for each command needed.
           remember that the commands will be in order, with the first
           command always being 1
        */
switch (comid) {
        case  1: return plugin_03x000_userlist(user);
        case  2: return plugin_03x000_wizlist(user);
        case -5: /* Trigger Heartbeat */
        case -6: /* Talker shutdown */
        case -7: /* 1st to save */
        case -8: /* save normal */
        case -9: /* load user data */
        default: return 0;
	}
}


plugin_03x000_userlist(user)
UR_OBJECT user;
{
	UR_OBJECT u;
	int err, ent, i, lev, count, abort;
	short int show_site, show_time, show_flags, show_files, show_lasttime, show_level, show_email;
	char start_alpha, end_alpha, c;
	struct dirent **userlist;

	FILE *fp, *tfp;
	char filename[80];
	
	i = 0; lev = 0; userlist = NULL;
	start_alpha = '\0'; end_alpha = '\0'; show_level = -1; abort = 0; count = 0;  err=0;
	show_site = 0; show_time = 0; show_flags = 0; show_files = 0; show_lasttime = 0; ent=0;
	show_email = 0;
	
	// Process display flags.
	for (i=0; i<word_count; i++) {
		if (!strcmp(word[i],"all")) {
			show_site++; show_time++; show_flags++; show_files++; show_lasttime++;
			if (show_email) show_site=0;
		}
		if (!strcmp(word[i],"time")) show_time++;
		if (!strcmp(word[i],"flags")) show_flags++;
		if (!strcmp(word[i],"files")) show_files++;
		if (!strcmp(word[i],"last")) show_lasttime++;
		if (!strcmp(word[i],"sites")) {
			if (show_email) show_email = 0;
			show_site++;
			}
		if (!strcmp(word[i],"email")) {
			if (show_site) show_site = 0;
			show_email++;
			}
		if (strlen(word[i]) < 3) {
			// Check for alpha or numeric characters.
			if (!isdigit(word[i][0])) 
			{	// Set beginning and/or ending alpha range.
				if (start_alpha && end_alpha) continue;
				else if (start_alpha) end_alpha = word[i][0];
				else start_alpha = word[i][0];
				
			} else {
				if (show_level != -1) continue;
				
				// Find maximum level on the system.
				for (lev=0; level_name[lev][0]!='*'; lev++);
				lev--;
								
				// Check validity and set which level to display only.
				if (atoi(word[i]) < 0) write_user("Invalid starting level specified - showing ALL instead.\n");
				else if (atoi(word[i]) > lev) write_user("Invalid starting level specified - showing ALL instead.\n");
				else show_level = atoi(word[i]);
			}
		}
	}
	
	// Check alpha validity.
	if (start_alpha!='\0') start_alpha = toupper(start_alpha);
	if (end_alpha!='\0') end_alpha = toupper(end_alpha);
	if (start_alpha > end_alpha && end_alpha != '\0') { c = end_alpha; end_alpha = start_alpha; start_alpha = c; }

	sprintf(filename,"./%s",USERFILES);
	err = scandir(filename, &userlist, 0, alphasort);
	if (err < 0) { 
		write_user(user,"USERLIST: Could not scan userfiles -- sorry.\n");
		return -1;
	}
	if (err == 0) {
		write_user(user,"USERLIST: No user files found in the current directory.\n");
		return -1;
	}
	write_user(user, "\nDirectory sort completed.\n");

	// Open output file.
	if (!(fp=fopen("userlist.txt","w"))) {
	        sprintf(text,"%s: Could not write to results file!\n",syserror);    
	        write_user(user,text);
	        sprintf(text,"PLUGIN ERROR: UserList - Could not write to results file!\n",user->name);
	        write_syslog(text,1,SYSLOG);
	        return -1;
	        }
	write_user(user,"File stream open.\nWriting header...");

	// Print file header based on user flags.
	fprintf(fp,"User list for %s %s\n",reg_sysinfo[TALKERNAME],long_date(1));
	if (show_level != -1 || start_alpha) { 
		fprintf(fp,"%s","Extra criteria: ");
		if (start_alpha != end_alpha && end_alpha) fprintf(fp,"Names '%c' to '%c'",start_alpha,end_alpha);
		else if (start_alpha) fprintf(fp,"Names starting with '%c'",start_alpha);
	}
	if (show_level != -1 && start_alpha) fprintf(fp,"%s"," + ");
	if (show_level != -1) fprintf(fp,"Level %d (%s)",show_level,level_name[show_level]);
	fprintf(fp,"%s","\n");
	if (show_level != -1 || start_alpha) fprintf(fp,"%s","\n");
	if (show_files || show_flags) {
		fprintf(fp,"%s","                          ");
		if (show_time) fprintf(fp,"%s","           ");
		if (show_lasttime) fprintf(fp,"%s","           ");
		if (show_files) fprintf(fp,"%s"," USER FILES  ");
		if (show_flags) fprintf(fp,"%s","USR FLAGS ");
		fprintf(fp,"%s","\n");
	}
	fprintf(fp,"%s","Username     Level        ");
	if (show_time) fprintf(fp,"%s","Login Time ");
	if (show_lasttime) fprintf(fp,"%s","Last Login ");
	if (show_files) fprintf(fp,"%s","    Mail     ");
	if (show_flags) fprintf(fp,"%s","Muz   Inv ");
	if (show_site) fprintf(fp,"%s","Site");
	if (show_email) fprintf(fp,"%s","E-Mail");
	fprintf(fp,"%s","\n");
	if (show_files || show_flags || show_time || show_lasttime) {
		fprintf(fp,"%s","                          ");
		if (show_time) fprintf(fp,"%s"," dd:hh:mm  ");
		if (show_lasttime) fprintf(fp,"%s","  (days)   ");
		if (show_files) fprintf(fp,"%s","Desc    Room ");
		if (show_flags) fprintf(fp,"%s","   Arr    ");
		fprintf(fp,"%s","\n");
	}
	fprintf(fp,"%s","\n");	
	write_user(user," done.\n");
	
	ent = err;
	while (err > 1 && !abort) {	// Read directory structure (sorted)
	
		err--;
//		if (!strcmp(userlist[ent-err]->d_name,".") || !strcmp(userlist[ent-err]->d_name,"..")) {
		if (userlist[ent-err]->d_name == NULL || userlist[ent-err] == NULL) {
			abort++;
//			err--;
			continue;
		}
		
		// Check the file extension for '*.D'
		if (userlist[ent-err]->d_name[strlen(userlist[ent-err]->d_name)-1] != 'D'
		||  userlist[ent-err]->d_name[strlen(userlist[ent-err]->d_name)-2] != '.') continue;
			
		// Userfile found -- do a prelim check to see if it's in our alpha range.
		if (   (start_alpha && userlist[ent-err]->d_name[0] < start_alpha)
		    || (end_alpha && userlist[ent-err]->d_name[0] > end_alpha)
		    || (start_alpha && !end_alpha && userlist[ent-err]->d_name[0]!=start_alpha))
		     continue;
		
		
		// Create a user object and load the file.
		if ((u=create_user())==NULL) {
		        sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	        	write_user(user,text);
	        	write_syslog("ERROR: Unable to create temporary user object in plugin 03x000.\n",0,SYSLOG);
	        	return -1;
	        }
		strncpy(u->name,userlist[ent-err]->d_name,strlen(userlist[ent-err]->d_name) - 2);
		u->name[strlen(userlist[ent-err]->d_name) - 2] = '\0';

		if (!(plugin_03x000_loaduser(u))) { destruct_user(u); destructed=0; continue; }

		// Check for level mismatch.
		if (show_level != -1 && u->level != show_level)
			{ destruct_user(u); destructed = 0; continue; }
		
		// Increment the (found) user count and start outputting data.
		count++;

		fprintf(fp,"%-12s %-12s ",u->name,level_name[u->level]);
		if (show_time)
			fprintf(fp,"%3d:%02d:%02d  ",(u->total_login/(3600*24)),
				((u->total_login%(3600*24))/3600),
				((u->total_login%(3600*24))%3600)/60);
		if (show_lasttime)
			fprintf(fp,"   %4d    ",((int)(time(0)) - u->last_login)/86400);
		if (show_files) {
			sprintf(filename,"%s/%s.P",USERFILES,u->name);
			if (tfp=fopen(filename,"r")) {
				fprintf(fp,"%s","  Y ");
				fclose(tfp);
			} else fprintf(fp,"%s","  N ");
			sprintf(filename,"%s/%s.M",USERFILES,u->name);
			if (tfp=fopen(filename,"r")) {
				fprintf(fp,"%s","  Y ");
				fclose(tfp);
			} else fprintf(fp,"%s","  N ");
			sprintf(filename,"%s/%s.R",USERFILES,u->name);
			if (tfp=fopen(filename,"r")) {
				fprintf(fp,"%s","  Y  ");
				fclose(tfp);
			} else fprintf(fp,"%s","  N  ");
		}
		if (show_flags) {
			if (u->muzzled) fprintf(fp,"%s"," Y ");
				else fprintf(fp,"%s"," N ");
			if (u->arrested) fprintf(fp,"%s"," Y ");
				else fprintf(fp,"%s"," N ");
			if (u->vis) fprintf(fp,"%s"," N  ");
				else fprintf(fp,"%s"," Y  ");
		}
		if (show_site) fprintf(fp,"%s",u->last_site);
		if (show_email) fprintf(fp,"%s",u->email);
		fprintf(fp,"%s","\n");
		
		destruct_user(u);
		destructed = 0;
		
	} // End while
	
	write_user(user,"User processing complete.\n");
	fprintf(fp,"\nTotal users matching this search: %d\n",count);
	fclose(fp);
	write_user(user,"File stream closed.\n\n");
        if (more(user,user->socket,"userlist.txt")==1) user->misc_op=2;
	return count;
}


plugin_03x000_wizlist(user)
UR_OBJECT user;
{
	UR_OBJECT u;
	int err, ent, i, lev, count, abort;
	struct dirent **userlist;

	FILE *fp, *tfp;
	char filename[80];
	
	i = 0; lev = 0; userlist = NULL;
	abort = 0; count = 0;  err=0; ent=0;
	
	sprintf(filename,"./%s",USERFILES);
	err = scandir(filename, &userlist, 0, alphasort);
	if (err < 0) { 
		write_user(user,"WIZLIST: Could not scan userfiles -- sorry.\n");
		return -1;
	}
	if (err == 0) {
		write_user(user,"WIZLIST: No user files found in the current directory.\n");
		return -1;
	}

	// Open output file.
	if (!(fp=fopen("wizlist.txt","w"))) {
	        sprintf(text,"%s: Could not write to results file!\n",syserror);    
	        write_user(user,text);
	        sprintf(text,"PLUGIN ERROR: WizList - Could not write to results file!\n",user->name);
	        write_syslog(text,1,SYSLOG);
	        return -1;
	        }

	// Print file header based on user flags.
	fprintf(fp,"\nWizard (administrators) list for %s %s\n",reg_sysinfo[TALKERNAME],long_date(1));
	fprintf(fp,"%s","\n~OL~FYUSERNAME     LEVEL        ");
	fprintf(fp,"%s","\n\n");
	
	ent = err;
	while (err > 1 && !abort) {	// Read directory structure (sorted)
	
		err--;
//		if (!strcmp(userlist[ent-err]->d_name,".") || !strcmp(userlist[ent-err]->d_name,"..")) {
		if (userlist[ent-err]->d_name == NULL || userlist[ent-err] == NULL) {
			abort++;
//			err--;
			continue;
		}
		
		// Check the file extension for '*.D'
		if (userlist[ent-err]->d_name[strlen(userlist[ent-err]->d_name)-1] != 'D'
		||  userlist[ent-err]->d_name[strlen(userlist[ent-err]->d_name)-2] != '.') continue;
			
		// Create a user object and load the file.
		if ((u=create_user())==NULL) {
		        sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	        	write_user(user,text);
	        	write_syslog("ERROR: Unable to create temporary user object in plugin 03x000.\n",0,SYSLOG);
	        	return -1;
	        }
		strncpy(u->name,userlist[ent-err]->d_name,strlen(userlist[ent-err]->d_name) - 2);
		u->name[strlen(userlist[ent-err]->d_name) - 2] = '\0';

		if (!(plugin_03x000_loaduser(u))) { destruct_user(u); destructed=0; continue; }

		if (u->level < WIZ) {
			destruct_user(u); destructed = 0;
			continue;
		}

		// Increment the (found) user count and start outputting data.
		count++;

		fprintf(fp,"%-12s %-12s ",u->name,level_name[u->level]);
		fprintf(fp,"%s","\n");
		
		destruct_user(u);
		destructed = 0;
		
	} // End while
	
	fprintf(fp,"\nTotal wizards at %s: %d\n",reg_sysinfo[TALKERNAME],count);
	fclose(fp);
        if (more(user,user->socket,"wizlist.txt")==1) user->misc_op=2;
	return count;
}


/*** Load the users details ***/
plugin_03x000_loaduser(user)
UR_OBJECT user;
{
FILE *fp;
char line[81],filename[80];
int temp1,temp2,temp3,temp4;

sprintf(filename,"%s/%s.D",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) return 0;

fscanf(fp,"%s\n",user->pass); /* datafile version identification and/or pwd */

if (!strncmp(user->pass,"4.0",3)
        || !strncmp(user->pass,"401",3)
        || !strncmp(user->pass,"402",3)
        || !strncmp(user->pass,"403",3)) {      /* Userfile is in lastest format. */

                fscanf(fp,"%s\n",user->pass); /* NOW get the password */

                /* TIME DATA */
                fscanf(fp,"%d %d %d %d %d\n",&temp1,&temp2,&user->last_login_len,&temp3,&temp4);
                user->last_login=(time_t)temp1;
                user->total_login=(time_t)temp2;
                user->read_mail=(time_t)temp3;
                user->created=(time_t)temp4;

                /* USER PRIVLIDGES */
                fscanf(fp,"%d %d %d %d %d\n",&user->level,&user->muzzled,&user->arrested,&user->reverse,&user->can_edit_rooms);
                user->orig_level=user->level;
                user->cloaklev = user->orig_level;

                /* USER SESSION VARIABLES */
                fscanf(fp,"%d %d %d %d %d %d\n",&user->prompt,&user->charmode_echo,&user->command_mode,&user->colour,&user->duty,&user->vis);

                /* USER SETTINGS and STATS */
                fscanf(fp,"%d %d %d %d %d %d\n",&user->gender,&user->age,&user->pstats,&user->pueblo_mm,&user->pueblo_pg,&user->voiceprompt);

                /* last site */
                fscanf(fp,"%s\n",user->last_site);
        
                /* description */
                fgets(line,USER_DESC_LEN+2,fp);
                line[strlen(line)-1]=0;
                strcpy(user->desc,line); 

                /* enter prhase */
                fgets(line,PHRASE_LEN+2,fp);
                line[strlen(line)-1]=0;
                strcpy(user->in_phrase,line); 

                /* exit phrase */
                fgets(line,PHRASE_LEN+2,fp);
                line[strlen(line)-1]=0;
                strcpy(user->out_phrase,line); 

                /* personal room topic */
                fgets(line,TOPIC_LEN+2,fp);
                line[strlen(line)-1]=0;
                strcpy(user->room_topic,line); 

                /* email */
                fgets(line,58,fp);
                line[strlen(line)-1]=0;
                strcpy(user->email,line); 

                /* http address */
                fgets(line,58,fp);
                line[strlen(line)-1]=0;
                strcpy(user->http,line); 

        fclose(fp);
        return 1;
        }
else {
        /* backwards compatibility */
/*        fscanf(fp,"%s",user->pass);  Not needed anymore. */
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d",
                &temp1,&temp2,&user->last_login_len,&temp3,&user->level,&user->prompt,&user->muzzled,&user->charmode_echo,&user->command_mode,&user->colour);
        user->orig_level = user->level;
        user->cloaklev = user->orig_level;
        user->last_login=(time_t)temp1;
        user->total_login=(time_t)temp2;
        user->read_mail=(time_t)temp3;
        fscanf(fp,"%s\n",user->last_site);
        
        /* Need to do the rest like this 'cos they may be more than 1 word each */
        fgets(line,USER_DESC_LEN+2,fp);
        line[strlen(line)-1]=0;
        strcpy(user->desc,line); 
        fgets(line,PHRASE_LEN+2,fp);
        line[strlen(line)-1]=0;
        strcpy(user->in_phrase,line); 
        fgets(line,PHRASE_LEN+2,fp);
        line[strlen(line)-1]=0;
        strcpy(user->out_phrase,line); 
        fclose(fp);
        return 1;
        }
}
