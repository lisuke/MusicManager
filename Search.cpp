#include "./Search.h"
#include <algorithm>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>



int posix_dfs_find_suffix_file(std::multimap<std::string,MusicItem> &container,const std::string &dir,const std::vector<string> &suffix)
{//c++ /dir /suffix
    DIR *dp;
    struct dirent *ep;
    struct stat statbuf;

    dp = opendir(dir.c_str());
    if (dp != NULL)
    {
        while((ep = readdir(dp)) != NULL)
        {
            #ifndef _WIN32
            if (ep->d_type == 4)
            #else
            string temp = ep->d_name;
            if(temp.find(".")==string::npos)
            #endif
            {//is a dir
                if (ep->d_name[strlen(ep->d_name)-1] == '.')
                {//suffix avoid entry . current dir or .. parent dir 
                    continue;
                }
                //
                string soon_dir;
                soon_dir = dir + path_split + ep->d_name;
                //std::cout<<soon_dir;
                //recursive dfs
                // std::cout<<soon_dir;
                posix_dfs_find_suffix_file(container,soon_dir,suffix);
            }
            else
            {// is a file
                std::string m_name = ep->d_name;
                std::string m_path = dir+path_split+ep->d_name;

                for (int i = 0; i < suffix.size(); ++i)
                {//match
                    string suf = suffix[i];
                    int pos = m_name.rfind(".");
                    
                    transform(suf.begin(),suf.end(),suf.begin(),::tolower);
                    transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);

                    if (m_name.compare(pos+1,suf.length(),suf)==0)//compare pos n s
                    {//match succ
                        m_name = ep->d_name;
                        MusicItem m(container.size(),m_name,m_path);
                        container.insert(std::pair<std::string,MusicItem>(m_name,m));
                        std::cout<<m_path<<"\n";
                    }
                }
            }
        }
        (void) closedir (dp);
    }
    else
    {
        // std::cerr<<"Couldn't open the directory";
    }
    return 0;
}

/*
int posix_dfs_find_suffix_file(std::multimap<std::string,MusicItem> &container,const std::string &dir,const std::vector<string> &suffix)
{//c++ /dir /suffix
	DIR *dp;
	struct dirent *ep;
	struct stat statbuf;

	dp = opendir(dir.c_str());

	if ((dp = opendir(dir.c_str())) == NULL)
    {
		std::cerr<<"Couldn't open the directory"<<dir<<std::endl;
		return -1;
    }

    std::cout<<dp;

    while((ep = readdir(dp)) != NULL)
    {
        stat(ep->d_name,&statbuf);
        #ifndef _WIN
        if (S_ISDIR(statbuf.st_mode))
        #else
        string temp = ep->d_name;
        if(temp.find(".")==string::npos)
        #endif
        {//is a dir
            if (ep->d_name[strlen(ep->d_name)-1] == '.')
            {//suffix avoid entry . current dir or .. parent dir
                continue;
            }
            //
            string soon_dir;
            soon_dir = dir + path_split + ep->d_name;
            //std::cout<<soon_dir;
            //recursive dfs
            //std::cout<<soon_dir;
            posix_dfs_find_suffix_file(container,soon_dir,suffix);
        }
        else
        {// is a file
            std::string m_name = ep->d_name;
            std::string m_path = dir+path_split+ep->d_name;

            for (int i = 0; i < suffix.size(); ++i)
            {//match
                string suf = suffix[i];
                int pos = m_name.rfind(".");

                transform(suf.begin(),suf.end(),suf.begin(),::tolower);
                transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);

                if (m_name.compare(pos+1,suf.length(),suf)==0)//compare pos n s
                {//match succ
                    m_name = ep->d_name;
                    MusicItem m(container.size(),m_name,m_path);
                    container.insert(std::pair<std::string,MusicItem>(m_name,m));
                    std::cout<<111;
                }
            }
        }
    }
    (void) closedir (dp);

	return 0;
}
*/


/* c
int dfs_search_file (const char *dir)
{
  DIR *dp;
  struct dirent *ep;
  struct stat statbuf;

  dp = opendir (dir);
  if (dp != NULL)
    {
	    while ((ep = readdir (dp))!=NULL)
	    {
	        if(ep->d_type==4)
	        {//is dir
	      		if (strcmp(ep->d_name , ".")==0 || strcmp(ep->d_name , "..")==0)
	      		//if (ep->d_name[strlen(ep->d_name)-1]=='.')
	      		{
	      			continue;
	      		}
	      		//
	      		char * tmp = (char *)malloc(sizeof(char) * (strlen(ep->d_name) + strlen(dir)+2 ));
	      		strcpy(tmp,dir);
	      		strcat(tmp,"/");
	      		strcat(tmp,ep->d_name);
	      		//
		        //puts (ep->d_name);
	      		dfs_search_file(tmp);
	      		free(tmp);
	      	}
	      	else if(ep->d_type==8)
	      	{
	      		char * tmp = (char *)malloc(sizeof(char) * (strlen(ep->d_name) + strlen(dir)+2 ));
	      		strcpy(tmp,dir);
	      		strcat(tmp,"/");
	      		strcat(tmp,ep->d_name);
		        printf("%s\n", tmp);
		        free(tmp);
	      	}
      	}
      	(void) closedir (dp);
    }
    else
    {
    	perror ("Couldn't open the directory");
  	}

  return 0;
}
*/
