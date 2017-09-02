#ifndef __Search_H__
#define __Search_H__

#include "MusicItem.h"
#include <map>
#include <vector>

#ifdef __WIN32

    #define path_split "\\"

#else

	#define path_split "/"

#endif

	#include <stdlib.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>

int posix_dfs_find_suffix_file(std::multimap<std::string,MusicItem> &container,const std::string &dir,const std::vector<string> &suffix);

#endif
