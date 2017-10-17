/* TalkerOS Plugin                    for version 4.01 or higher
   -------------------------------------------------------------
   This code contains code taken from TalkerOS, the code for the
   numbering has been provided by "Mabs".
   Plugin-compatible modifications (and some further mutilations)
     by "Weaver".
   (Hopefully) 100% compatible with TalkerOS ver 4.x.

   The initialization line for this plugin is:

           if (tmp=plugin_00x001_init(cmd)) cmd=cmd+tmp;

   The command call for this plugin is:

           if (!strcmp(plugin->registration,"00-001")) { plugin_00x001_main(user
,str,comnum); return 1; }

    Unfortunately, this leaves you with xdmail, xrmail, and xfrom, cause
    with a plugin you can not overwrite existing commands. To try it out,
    it should be sufficient though.

    If you want to use the commands here under the name of "dmail", "rmail",
    and "xfrom", then do the following: 

    In function exec_com(user,inpstr)

    change the lines
                case RMAIL   : rmail(user);  break;
                case DMAIL   : dmail(user);  break;
                case FROM    : mail_from(user);  break;

    into
                case RMAIL   : plugin_00x001_xrmail(user); break;
                case DMAIL   : plugin_00x001_xdmail(user); break;
                case FROM    : plugin_00x001_xfrom(user); break;

    You can leave the original code in - a reasonably intelligent linker
    will not link it in anyway. So no need to make the additional change.

   ------------------------------------------------------------- */

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

plugin_00x001_init(cm)
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
strcpy(plugin->name,"extended mail");       /* Plugin Description   */
strcpy(plugin->author,"Mabs/Weaver");      /* Author's name        */
strcpy(plugin->registration,"00-001");          /* Plugin/Author ID     */
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
strcpy(com->command,"xrmail");                   /* Name of command */
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
strcpy(com->command,"xdmail");                   /* Name of command */
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
strcpy(com->command,"xfrom");                   /* Name of command */
com->id = plugin->id;                           /* Command reference ID */
com->req_lev = USER;                            /* Required level for cmd. */
com->comnum = i;
com->plugin = plugin;
/* end creating command - repeat as needed for more commands */

return i;
}

plugin_00x001_main(user,str,comid)
UR_OBJECT user;
char *str;
int comid;
{
switch (comid) {
        /* put case calls here for each command needed.
           remember that the commands will be in order, with the first
           command always being 1;
        */
        case  1: plugin_00x001_xrmail(user); return 1;
        case  2: plugin_00x001_xdmail(user); return 1;
        case  3: plugin_00x001_xfrom(user); return 1;
        default: return 0;
        }
}





/*** Read your mail ***/
plugin_00x001_xrmail(user)
UR_OBJECT user;
{
FILE *infp,*outfp;
int ret,cnt;
char c, filename[80], line[ARR_SIZE+1], line2[ARR_SIZE+1], 
     w1[ARR_SIZE+1], w2[ARR_SIZE+1],
     memoString[ARR_SIZE], fromString[ARR_SIZE];

/* see below why this is necessary */
sprintf(fromString,"%sFrom:",colors[CMAILHEAD]);
sprintf(memoString,"%s(MEMO)",colors[CMAILHEAD]);

sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(filename,"r"))) {
	write_user(user,"You have no mail.\n");  return;
	}
/* Update last read / new mail received time at head of file */
if (outfp=fopen("tempfile","w")) {
	fprintf(outfp,"%d\r",(int)(time(0)));
	/* skip first line of mail file */
	fgets(line,DNL,infp);

/***************************************************************/

cnt=0;
fgets(line,ARR_SIZE-1,infp);
while(!feof(infp)) {
      sscanf(line,"%s",w1);
      sscanf(remove_first(line),"%s",w2);
      strcpy(line2,line);
      if (!strcmp(w2,fromString) || !strcmp(w2,"From:") ||
         !strcmp(w2,memoString) || !strcmp(w2,"MEMO:"))
           {
           strcpy(line,remove_first(line2));
           }
      if (!strcmp(w2,fromString) || !strcmp(w2,"From:")
           || !strcmp(w1,fromString) || !strcmp(w1,"From:") 
           || !strcmp(w2,memoString) || !strcmp(w2,"MEMO:")
           || !strcmp(w1,memoString) || !strcmp(w1,"MEMO:"))
           {
            cnt++;
            sprintf(line2,"%s[%d]%s %s", colors[CMAILHEAD], cnt,
              colors[CDEFAULT],line);
            fputs(line2,outfp);
             }
            else fputs(line,outfp);                            
            w1[0]='\0'; /* if you dont do this, it messes the numbering*/
            w2[0]='\0'; /*not sure why this word clear is needed*/
        fgets(line,ARR_SIZE-1,infp);
        }

/***************************************************************/

	fclose(outfp);
	rename("tempfile",filename);
	}
user->read_mail=time(0);
fclose(infp);
write_user(user,"\n~BB*** Your mail ***\n\n");
ret=more(user,user->socket,filename);
if (ret==1) user->misc_op=2;
}




/*** Delete some or all of your mail. A problem here is once we have deleted
     some mail from the file do we mark the file as read? If not we could
     have a situation where the user deletes all his mail but still gets
     the YOU HAVE UNREAD MAIL message on logging on if the idiot forgot to 
     read it first. ***/
plugin_00x001_xdmail(user)
UR_OBJECT user;
{
FILE *infp,*outfp;
int num,num2,valid,cnt;
char tempfilename[80],filename[80],w1[ARR_SIZE],w2[ARR_SIZE],
     line[ARR_SIZE],line2[ARR_SIZE], temp1[10],temp2[10],
     memoString[ARR_SIZE], fromString[ARR_SIZE];

temp1[0]='\0';
temp2[0]='\0';
num=0;
num2=0;

if (word_count<2 || ((num=atoi(word[1]))<1 && strcmp(word[1],"all"))) {
        write_user(user,"Usage: dmail <message #>|<range>|all\n");
        return;
	}
if (!strstr(word[1],"-"))
    {
     if  ( ((num=atoi(word[1]))  < 1) && strcmp(word[1],"all"))
          {
            write_user(user,"Usage: dmail <message #>|<range>|all\n");
            return;
           }
      }
       else
           {
            strcpy(temp2,word[1]);
            valid=0;
            while (temp2[valid]!='-')
                 {
                  temp1[valid]=temp2[valid];
                  valid++;
                  }
            temp1[valid]='\0';
            valid=0;
            /*reduce array until we find the second number*/
            while (temp2[0]!='-')
                  {
                   while (temp2[valid]!='\0')
                         {
                         temp2[valid]=temp2[valid+1];
                         valid++;
                         }
                   temp2[valid]=temp2[valid+1];
                   valid=0;
                  }
           while (temp2[valid]!='\0')
                 {
                 temp2[valid]=temp2[valid+1];
                 valid++;
                  }
           temp2[valid]=temp2[valid+1];
           valid=0;
             /*hopefully here we will have two numbers in two strings*/
             num=atoi(temp1);
             num2=atoi(temp2);
             if ((num<1 || num2<1) || (num >num2))
                {
                 write_user(user,"Usage: dmail <message #>|<range>|all\n");
                 return;
                 }
              }

sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(filename,"r"))) {
	write_user(user,"You have no mail to delete.\n");  return;
	}
if (!strcmp(word[1],"all")) {
	fclose(infp);
	unlink(filename);
	write_user(user,"All mail deleted.\n");
	return;
	}
sprintf(tempfilename,"%s/%s.tempfile",USERFILES,user->name);
if (!(outfp=fopen(tempfilename,"w"))) {
        sprintf(text,"%s: couldn't open tempfile.\n",syserror);
        write_user(user,text);
        write_syslog("ERROR: Couldn't open tempfile in dmail().\n",0);
        fclose(infp);
        return;
        }
fprintf(outfp,"%d\r",(int)time(0));
user->read_mail=time(0);
cnt=0;  valid=1;
fgets(line,DNL,infp); /* Get header date */
fgets(line,ARR_SIZE-1,infp);

/* An error developed when counting messages because it looked for "~OLFrom:"
   but with the modular colorcodes, it wouldn't always be "~OL". */
sprintf(fromString,"%sFrom:",colors[CMAILHEAD]);
sprintf(memoString,"%s(MEMO)",colors[CMAILHEAD]);

while(!feof(infp)) {
      sscanf(line,"%s",w1);
      sscanf(remove_first(line),"%s",w2);
      strcpy(line2,line);
      if (!strcmp(w2,fromString) || !strcmp(w2,"From:") ||
         !strcmp(w2,memoString) || !strcmp(w2,"(MEMO)"))
          {
          strcpy(line,remove_first(line2));
          }
      if (!strcmp(w1,fromString) || !strcmp(w1,"From:")
          || !strcmp(w2,fromString) || !strcmp(w2,"From:")
          || !strcmp(w1,memoString) || !strcmp(w1,"(MEMO)")
          || !strcmp(w2,memoString) || !strcmp(w2,"(MEMO)"))
          {
          cnt++;
          sprintf(line2,"~BR[%d]~RS %s",cnt,line);
          strcpy(line,line2);
          }
        if (((cnt==num)&&(num2==0))||((cnt>=num)&&(cnt<=num2)))
              {
                while (*line!='\n')
                      {
                       fgets(line,ARR_SIZE-1,infp);
                       }
                }
           else
            {
             fputs(line,outfp);
             }
             w1[0]='\0';/*see comment on rmail for explanation on this*/
             w2[0]='\0';
        fgets(line,ARR_SIZE-1,infp);
        }

fclose(infp);
fclose(outfp);
if (cnt<num) {
	unlink("tempfile");
	sprintf(text,"There were only %d messages in your mailbox, all now deleted.\n",cnt);
	write_user(user,text);
	return;
	}
if ((num==1)&&(cnt==num2)) {
        unlink(filename);
        unlink(tempfilename); /* cos it'll be empty anyway */
        write_user(user,"All messages deleted.\n");
        user->room->mesg_cnt=0;
        }
else {
        unlink(filename);
        rename(tempfilename,filename);
        if (num2>num)
            {
             valid=num2-num+1;
             sprintf(text,"%d messages deleted.(%d-%d)\n",valid,num,num2);
             }
             else
             sprintf(text,"Message %d deleted.\n",num);
        write_user(user,text);
        }
}


/*** Show list of people your unread mail is from without seeing the whole lot ***/
plugin_00x001_xfrom(user)
UR_OBJECT user;
{
FILE *fp;
int valid,cnt,memo,total;
char w1[ARR_SIZE], w2[ARR_SIZE], line[ARR_SIZE], filename[80], line2[ARR_SIZE],
     memoString[ARR_SIZE], fromString[ARR_SIZE], nrString[ARR_SIZE];

sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"You have no mail.\n");  return;
	}
sprintf(text,"\n%s%s-=- Mail Contents -=-\n\n",colors[CHIGHLIGHT],colors[CTEXT]);
write_user(user,text);
valid=1;  cnt=0;  memo=0; total=0;
fgets(line,DNL,fp); 
fgets(line,ARR_SIZE-1,fp);

/* An error developed when counting messages because it looked for "~OLFrom:"
   but with the modular colorcodes, it wouldn't always be "~OL". */
sprintf(fromString,"%sFrom:",colors[CMAILHEAD]);
sprintf(memoString,"%s(MEMO)",colors[CMAILHEAD]);
sprintf(nrString,"%s[",colors[CMAILHEAD]);


while(!feof(fp)) {
	if (*line=='\n') valid=1;
	sscanf(line,"%s",w1);

            /* found an already read message */
        if (valid && (!strncmp(w1, nrString, strlen(nrString)) || 
                      !strncmp(w1,"[", 1))) {
          printf("%s %d %d \n", w1, strncmp(w1, nrString, strlen(nrString)),
                      strncmp(w1,"[", 1));
          total++;
        }

           /* processes only unread messages, cause read messages have a
              number in front of them */
        if (valid && (!strcmp(w1,fromString) || !strcmp(w1,memoString) || 
                      !strcmp(w1,"(MEMO)") || !strcmp(w1,"From:"))) {
                if (!strcmp(w1,fromString) || !strcmp(w1,"From:")) {
                sprintf(text,"%s[%d]%s %s", colors[CMAILHEAD],
                    ++total, colors[CDEFAULT],
                    remove_first(line));
                  cnt++;  valid=0; 
                }
                else { 
                  sprintf(text,"%s[%d]%s (MEMO)        %s", colors[CMAILHEAD],
                    ++total, colors[CDEFAULT],
                    remove_first(line));
                  memo++;  valid=0; 
                }

                write_user(user,text);
	}
        w1[0]='\0'; /* why? check dmail and rmail */
        w2[0]='\0';
	fgets(line,ARR_SIZE-1,fp);
	}
fclose(fp);
sprintf(text,"\nTotal of %d new messages, %d new personal memos.\n\n",cnt,memo);
write_user(user,text);
}
