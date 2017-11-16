#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "Search.h"
#include "MusicItem.h"
#include "Loader.h"
#include <stdio.h>
#include <unistd.h>
// using namespace std;

std::multimap<std::string, MusicItem> lib;//音乐库
std::multimap<std::string, MusicItem> list;//播放列表
std::vector<string> suffix;//后缀
sqlite3tools database;//数据库
MusicItem current_music(0,"","");//当前选择播放的



void init()
{
	database.open("./database.db");
	database.execDDL("create table music_lib(m_path text primary key,m_name text)");
	// default suffix
	database.execDDL("create table music_suffix(format text primary key)");
	database.insert("insert into music_suffix values('mp3')");
	database.get_m_suffix("music_suffix",suffix);
	database.close();

	std::cout<< suffix.size()<<" format suffix:";
	for (int i = 0; i < suffix.size(); ++i)
	{
		std::cout<<" "<<suffix[i];
	}
	std::cout<<std::endl;
	
}

void add_suffix()
{
// 0
	std::string format;
	std::cout<<"输入音乐文件后缀如(mp3)以n或N结束."<<std::endl;
	
	while(1){
		std::cin>>format;
		if (format.size()==1&&(format[0]=='n' || format[0]=='N'))
		{
			break;
		}
		database.insert("insert into music_suffix values('"+format+"')");
		suffix.push_back(format);
	}
}
void save_to_lib(std::multimap<std::string, MusicItem> &lib)
{
	std::multimap<std::string, MusicItem>::iterator i;
	database.open();
	database.syn_OFF();
	database.begin();
    for (i=lib.begin(); i != lib.end();i++ )
	{
		database.insert("insert into music_lib values('"+(i->second.get_m_path())+"','"+(i->second.get_m_name())+"')");
	}
	database.commit();
	database.close();
}
void all_scan()
{
// 1
	lib.clear();
	#ifndef _Win32
		posix_dfs_find_suffix_file(lib,"/",suffix);
	#else
		posix_dfs_find_suffix_file(lib,"c:\\",suffix);
		posix_dfs_find_suffix_file(lib,"d:\\",suffix);
		posix_dfs_find_suffix_file(lib,"e:\\",suffix);
		posix_dfs_find_suffix_file(lib,"f:\\",suffix);
	#endif
	save_to_lib(lib);
	std::cout<<std::endl<<"scan succ. total: "<<lib.size()<<std::endl;
}

void read_lib(const std::string table,std::multimap<std::string, MusicItem> &lib)
{
	database.open();
	database.get_m_lib(table,lib);
	database.close();
}

void add_scan_path(){
// 2
	std::cout<<"请输入路径:";
	string path;
	std::cin>>path;
	// getline(std::cin,path);
	std::multimap<std::string, MusicItem> lib;
	posix_dfs_find_suffix_file(lib,path,suffix);

	std::cout<<std::endl<<"scan succ. total: "<<lib.size()<<std::endl;
	// database.insert();//保存记录
	database.open();
	database.execDDL("create table music_path(path text primary key)");
	database.insert("insert into music_path values('"+path+"')");
	database.close();
	save_to_lib(lib);
}

void add_music_file_to_lib()
{
// 3
	std::cout<<"请输入路径名:";
	string path;
	std::cin>>path;
	std::cout<<"请输入歌名:";
	string name;
	std::cin>>name;

	database.open();
	database.execDDL("create table music_lib(path text primary key)");
	database.insert("insert into music_lib values('"+(path)+"','"+(name)+"')");
	database.close();
	save_to_lib(lib);
}

void restart_scan_path()
{
// 4
	std::vector<string> path;
	database.open();
	database.remove("delete from music_lib");
	database.close();
	database.get_m_suffix("music_path",path);
	lib.clear();
	for (std::string p:path)
	{
		posix_dfs_find_suffix_file(lib,p,suffix);
	}
	save_to_lib(lib);
	std::cout<<std::endl<<"scan succ. total: "<<lib.size()<<std::endl;
}
void search_music()
{
// 5
	std::multimap<std::string, MusicItem> lib;
	std::string name;
	std::cout<<"请输入要搜索的歌曲:";
	std::cin>>name;
	read_lib("music_lib where m_path like '%"+name+"%'",lib);
	if (0==lib.size())
	{
		std::cerr<<"未找到."<<std::endl;
	}
	std::vector<MusicItem> tmp;
	std::multimap<std::string, MusicItem>::iterator i;
	int j=1;
	int n;
				
    for (i=lib.begin(); i != lib.end();i++ )
	{
		std::cout<< j++ << ".\t" <<i->second.get_m_name()<<std::endl;
		tmp.push_back(i->second);
		if ((j-1)%10 == 0)
		{
			bool test=true;
			std::string flag;
			while(1){
			std::cout<<"输入编号添加到列表.删除d n/D N,输入n/N继续输出.否则退出";
				std::cin>>flag;
				if (flag.size()==1&&(flag[0]=='n' || flag[0]=='N'))
				{//next show
					break;
				}

				if (flag.size()==1 && (flag[0]=='d'||flag[0]=='D'))
				{//delete 
					std::cin>>flag;
					for (auto c:flag)
					{
						if (!isdigit(c))
						{
							return;
						}
					}
					n=atoi(flag.c_str())-1;
					if (n>tmp.size())
					{
						std::cerr<<"error 不存在.";
						return;
					}
					std::cout<<"是否删除磁盘文件(y/n or Y/N)"<<tmp[n].get_m_path()<<std::endl;
						std::cin>>flag;

					database.remove("delete from music_lib where m_path='"+tmp[n].get_m_path()+"'");

					if (flag.size()==1 && (flag[0]=='y'||flag[0]=='Y'))
					{//remove local disk file
						remove(tmp[n].get_m_path().c_str());
					}
					else if (flag.size()==1 && (flag[0]=='n'||flag[0]=='N'))
					{
						//
					}
					continue;
					fflush(stdin);
				}
				for (auto c:flag)
				{
					if (!isdigit(c))
					{
						fflush(stdin);
						return;
					}
				}
				n=atoi(flag.c_str())-1;
				//put in list
				if (n>tmp.size())
				{
					std::cerr<<"error 不存在.";
					continue;
				}
				list.insert(std::pair<std::string,MusicItem>(tmp[n].get_m_path(),tmp[n]));
			}
		}
	}
			std::string flag;
			while(1){
			std::cout<<"输入编号添加到列表.删除d n/D N,否则退出.";
				std::cin>>flag;
				
				if (flag.size()==1 && (flag[0]=='d'||flag[0]=='D'))
				{//delete
					std::cin>>flag;
					for (auto c:flag)
					{
						if (!isdigit(c))
						{
							return;
						}
					}
					n=atoi(flag.c_str())-1;
					std::cout<<"是否删除磁盘文件(y/n or Y/N)"<<tmp[n].get_m_path()<<std::endl;
						std::cin>>flag;

					database.remove("delete from music_lib where m_path='"+tmp[n].get_m_path()+"'");

					if (flag.size()==1 && (flag[0]=='y'||flag[0]=='Y'))
					{//remove local disk file
						
					}
					else if (flag.size()==1 && (flag[0]=='n'||flag[0]=='N'))
					{
						//
					}
					continue;
					fflush(stdin);
				}
				for (auto c:flag)
				{
					if (!isdigit(c))
					{
						fflush(stdin);
						return;
					}
				}
				//add to list
				n=atoi(flag.c_str())-1;
				//put in list
				if (n>tmp.size())
				{
					std::cerr<<"error 不存在.";
				}
				list.insert(std::pair<std::string,MusicItem>(tmp[n].get_m_path(),tmp[n]));
			}
}

void show_all()
{
//6
	read_lib("music_lib",lib);
	if (0==lib.size())
	{
		std::cerr<<"库中为记录空.您可以添加音乐到库中."<<std::endl;
	}
	std::vector<MusicItem> tmp;
	std::multimap<std::string, MusicItem>::iterator i;
	int j=1;
	int n;
    			
    for (i=lib.begin(); i != lib.end();i++ )
	{
		std::cout<< j++ << ".\t" <<i->second.get_m_name()<<std::endl;
		tmp.push_back(i->second);
		if ((j-1)%10 == 0)
		{
			bool test=true;
			std::string flag;
			while(1){
			std::cout<<"输入编号添加到列表.删除d n/D N,输入n/N继续输出.否则退出";
				std::cin>>flag;
				if (flag.size()==1&&(flag[0]=='n' || flag[0]=='N'))
				{//next show
					break;
				}

				if (flag.size()==1 && (flag[0]=='d'||flag[0]=='D'))
				{//delete 
					std::cin>>flag;
					for (auto c:flag)
					{
						if (!isdigit(c))
						{
							return;
						}
					}
					n=atoi(flag.c_str())-1;
					if (n>tmp.size())
					{
						std::cerr<<"error 不存在.";
						return;
					}
					std::cout<<"是否删除磁盘文件(y/n or Y/N)"<<tmp[n].get_m_path()<<std::endl;
						std::cin>>flag;

					database.remove("delete from music_lib where m_path='"+tmp[n].get_m_path()+"'");

					if (flag.size()==1 && (flag[0]=='y'||flag[0]=='Y'))
					{//remove local disk file
						remove(tmp[n].get_m_path().c_str());
					}
					else if (flag.size()==1 && (flag[0]=='n'||flag[0]=='N'))
					{
						//
					}
					continue;
					fflush(stdin);
				}
				for (auto c:flag)
				{
					if (!isdigit(c))
					{
						fflush(stdin);
						return;
					}
				}
				n=atoi(flag.c_str())-1;
				//put in list
				if (n>tmp.size())
				{
					std::cerr<<"error 不存在.";
					continue;
				}
				list.insert(std::pair<std::string,MusicItem>(tmp[n].get_m_path(),tmp[n]));
			}
		}
	}
			std::string flag;
			while(1){
			std::cout<<"输入编号添加到列表.删除d n/D N,否则退出.";
				std::cin>>flag;
				
				if (flag.size()==1 && (flag[0]=='d'||flag[0]=='D'))
				{//delete
					std::cin>>flag;
					for (auto c:flag)
					{
						if (!isdigit(c))
						{
							return;
						}
					}
					n=atoi(flag.c_str())-1;
					std::cout<<"是否删除磁盘文件(y/n or Y/N)"<<tmp[n].get_m_path()<<std::endl;
						std::cin>>flag;

					database.remove("delete from music_lib where m_path='"+tmp[n].get_m_path()+"'");

					if (flag.size()==1 && (flag[0]=='y'||flag[0]=='Y'))
					{//remove local disk file
						
					}
					else if (flag.size()==1 && (flag[0]=='n'||flag[0]=='N'))
					{
						//
					}
					continue;
					fflush(stdin);
				}
				for (auto c:flag)
				{
					if (!isdigit(c))
					{
						fflush(stdin);
						return;
					}
				}
				//add to list
				n=atoi(flag.c_str())-1;
				//put in list
				if (n>tmp.size())
				{
					std::cerr<<"error 不存在.";
				}
				list.insert(std::pair<std::string,MusicItem>(tmp[n].get_m_path(),tmp[n]));
			}
}

void music_list()
{
// 7
	std::vector<MusicItem> tmp;
	std::multimap<std::string, MusicItem>::iterator i;
	int j=1;
	int n;
    for (i=list.begin(); i != list.end();i++ )
	{
		std::cout<< j++ << ".\t" <<i->second.get_m_name()<<std::endl;
		tmp.push_back(i->second);
		if ((j-1)%10 == 0)
		{
			std::string flag;
			while(1){
			std::cout<<"输入编号添加到列表，输入n/N继续输出.否则退出";
				std::cin>>flag;

				if (flag.size()==1 && (flag[0]=='d'||flag[0]=='D'))
				{
					std::cin>>flag;
					for (auto c:flag)
					{
						if (!isdigit(c))
						{
							return;
						}
					}
					n=atoi(flag.c_str())-1;
					if (n>tmp.size())
					{
						std::cerr<<"error 不存在.";
						continue;
					}
					std::cout<<"是否删除磁盘文件(y/n or Y/N)"<<tmp[n].get_m_path()<<std::endl;
						std::cin>>flag;

					database.remove("delete from music_lib where m_path='"+tmp[n].get_m_path()+"'");
					//正在播放 需要释放
					if (flag.size()==1 && (flag[0]=='y'||flag[0]=='Y'))
					{//remove local disk file
						remove(tmp[n].get_m_path().c_str());
					}
					else if (flag.size()==1 && (flag[0]=='n'||flag[0]=='N'))
					{
						//
					}
					continue;
					fflush(stdin);
				}
				if (flag.size()==1&&(flag[0]=='n' || flag[0]=='N'))
				{
					return;
				}
				
				for (auto c:flag)
				{
					if (!isdigit(c))
					{
						return;
					}
				}
				n=atoi(flag.c_str())-1;
				//put in list

				current_music = tmp[n];
			}
		}
	}
			std::string flag;
			while(1){
			std::cout<<"输入编号播放.否则退出.";
				std::cin>>flag;

				if (flag.size()==1 && (flag[0]=='d'||flag[0]=='D'))
				{//delete
					std::cin>>flag;
					for (auto c:flag)
					{
						if (!isdigit(c))
						{
							return;
						}
					}
					n=atoi(flag.c_str())-1;
					std::cout<<"是否删除磁盘文件(y/n or Y/N)"<<tmp[n].get_m_path()<<std::endl;
						std::cin>>flag;

					database.remove("delete from music_lib where m_path='"+tmp[n].get_m_path()+"'");

					if (flag.size()==1 && (flag[0]=='y'||flag[0]=='Y'))
					{//remove local disk file
						
					}
					else if (flag.size()==1 && (flag[0]=='n'||flag[0]=='N'))
					{
						//
					}
					continue;
					fflush(stdin);
				}

				for (auto c:flag)
				{
					if (!isdigit(c))
					{
						fflush(stdin);
						return;
					}
				}
				n=atoi(flag.c_str())-1;
				//put in list
				current_music = tmp[n];
			}

}

int copy_file(const char * src,const char * dest)
{
	int c;
	FILE * psrc;
	FILE * pdest;
	if ((psrc=fopen(src,"rb"))==NULL)
	{
		printf("error open src failure. \n");
		return 1;
	}
	if ((pdest=fopen(dest,"wb"))==NULL)
	{
		printf("error open dest failure. \n");
		return 2;
	}
	while((c=fgetc(psrc))!=EOF){
		fputc(c,pdest);
	}
	fputc(EOF,pdest);
	fclose(psrc);
	fclose(pdest);
}

void load_play(char *envp[])
{//
	// system("gst-play-1.0 \"/home/lisuke/Downloads/让泪化作相思雨.mp3\"");
	
	string str;
	list.
	for (auto i = list.begin(); i != list.end(); ++i)
	{
		str+=" \""+i->first+"\"";
		// str=i->first;
	}
	std::cout<<str;

	if(fork()==0)
		if (execl("/bin/mpg123","mpg123",str.c_str(),(char *)NULL)<0)
		{
			
		}

}
void save_to_backup(std::multimap<std::string, string> &backup)
{
	std::multimap<std::string, std::string>::iterator i;
	database.open();
	database.syn_OFF();
	database.begin();
    for (i=backup.begin(); i != backup.end();i++ )
	{
		database.insert("insert into music_backup values('"+(i->first)+"','"+(i->second)+"')");
	}
	database.commit();
	database.close();
}
void backup_lib_music()
{
	std::string path;
	std::cin>>path;
	std::multimap<std::string, MusicItem> lib;
	std::multimap<std::string, string> backup;
	read_lib("music_lib",lib);
	std::multimap<std::string, MusicItem>::iterator i;
	int j=0;
	int n=0;
	char tmp[8];
    for (i=lib.begin(); i != lib.end();i++ ,j++)
	{
		sprintf(tmp,"//%d.%s",j,"mp3");
		n=copy_file(i->second.get_m_path().c_str(),(path+tmp).c_str());
		backup.insert(std::pair<std::string,std::string>(i->second.get_m_path(),std::string(path+tmp)));
		if (n==2)
		{
			std::cerr<<"路径不存在."<<std::endl;
			j--;
			return;
		}else if (n==1)
		{
			j--;
		}
	}
	std::cout<<"backup succ. start save log to db.";
	database.open();
	database.execDDL("create table music_backup(src_path text primary key,dest_path text)");
	save_to_backup(backup);
	database.close();
	//
	std::cout<<"loged. save total: "<<j<<std::endl;
}
void home_list()
{
	std::cout<<std::endl;
	std::cout<<"\e[31m\e[10m\e[1m\e[10m   0.  添加音乐文件后缀"<<std::endl;
	std::cout<<"   1.  全盘扫描"<<std::endl;
	std::cout<<"   2.  添加音乐文件路径 "<<std::endl;
	std::cout<<"   3.  添加音乐文件"<<std::endl;
	std::cout<<"   4.  重构音乐库"<<std::endl;
	std::cout<<"   5.  搜索歌曲"<<std::endl;
	std::cout<<"   6.  查看库"<<std::endl;
	std::cout<<"   7.  播放列表"<<std::endl;
	std::cout<<"   8.  加载播放"<<std::endl;
	std::cout<<"   9.  备份库中音乐到指定目录"<<std::endl;
	std::cout<<"   e. 退出\e[0m"<<std::endl;
}

int home_page(char *envp[])
{
	char flag;
	bool is=false;
	do
	{
		home_list();
		if (is)
		{
			std::cout<<"输入错误请重新输入:\t";
			is = false;
		}else{
			std::cout<<"请选择:\t\t\t";
		}
		std::cin>>flag;
		fflush(stdin);
		switch(flag)
		{
			case '0':add_suffix();				break;
			case '1':all_scan();				break;
			case '2':add_scan_path();			break;
			case '3':add_music_file_to_lib();	break;
			case '4':restart_scan_path();		break;
			case '5':search_music();			break;
			case '6':show_all();				break;
			case '7':music_list();				break;
			case '8':load_play(envp);			break;
			case '9':backup_lib_music();		break;
			case 'e':return 0; 				break;

			default: is = true;
				break;
		}
	} while (1);
	return 1;
}


int main(int argc, char const *argv[],char *envp[])
{
	std::cout<<"loading"<<std::endl;
	init();
	while(home_page(envp));
	std::cout<<"good bye!"<<std::endl;
	return 0;
}
