
/**
	Loader 抽象类 加载器 被需要初始化的类继承 完成对类的初始化工作
*/


#ifndef __Loader_H__
#define __Loader_H__
#include <string>

	class Loader{

		public:
			Loader();
			Loader(Loader &);
			~Loader();
			virtual bool open() = 0;
			virtual bool open(const std::string &db_path)=0;
			virtual bool close() = 0;
			bool isalive();
		private:
			bool alive;
		protected:
			void set_alive(bool);

	};

	/*
		sqlite3 数据库 封装
	*/

	#ifndef __sqlite3_tools_H__
	#define __sqlite3_tools_H__

#ifdef __cplusplus  
extern "C" {  
#endif

	#include <sqlite3.h>
	
#ifdef  __cplusplus  
}  
#endif
  

	#include <vector>
	#include <string>
	#include <map>
	#include "MusicItem.h"

		// template <typename container,typename T>
		class sqlite3tools:public Loader
		{
			public:
				sqlite3tools();
				sqlite3tools(const std::string &db_path);
				~sqlite3tools();
				bool open(const std::string &db_path);
				bool close();
				bool open();
				bool execDDL(const std::string &ddl);
				bool update(const std::string &sql);
				bool insert(const std::string &sql);
				bool remove(const std::string &sql);
				bool update(std::vector<std::string> &sqls);
				bool insert(std::vector<std::string> &sqls);
				bool clear_table(const std::string &table);
				bool begin();
				bool commit();
				bool execDML(const std::string &dml);
				void syn_OFF();
				bool execDQL(const std::string &dql);
				void get_m_suffix(const std::string table,std::vector<std::string> &suffix);
				void get_m_lib(const std::string table,std::multimap<std::string, MusicItem> &lib);
				void * get_query_result();
			private:
				sqlite3 *db;
				std::string db_path;
				int count;
				// sqlite3_stmt* stmt;
				char *zerrorMsg;
				void* dql_result;
		};
	#endif

#endif



