
#include "Loader.h"

#ifdef __Loader_H__

	Loader::Loader()
	{
		alive = false;
	}
	Loader::~Loader()
	{

	}

	bool Loader::isalive()
	{
		return alive;
	}

	void Loader::set_alive(bool isalive)
	{
		this->alive = alive;
	}


#ifdef __sqlite3_tools_H__

	#include <string>
	#include <vector>
	#include <iostream>
	#include <map>
	#include "MusicItem.h"

#ifdef  __cplusplus  
extern "C" {  
#endif  

	#include <sqlite3.h>

#ifdef  __cplusplus  
}  
#endif

	sqlite3tools::sqlite3tools()
	{
		count=0;
		db = NULL;
	}

	sqlite3tools::sqlite3tools(const std::string &db_path):db_path(db_path)
	{
		set_alive(true);
		db = NULL;
		this->db_path = db_path;
		open(db_path);
		count=0;
	}

	sqlite3tools::~sqlite3tools()
	{
		close();
	}
	bool sqlite3tools::begin()
	{
		if (!db)
		{
			return false;
		}
		sqlite3_exec(db,"begin;",NULL,NULL,NULL);
		return true;
	}

	bool sqlite3tools::commit()
	{
		if (!db)
		{
			return false;
		}
		sqlite3_exec(db,"commit;",NULL,NULL,NULL);
		return true;
	}

	bool sqlite3tools::open(const std::string &db_path)
	{
		this->db_path = db_path;
		if (db) //alreay open
		{
			return true;
		}
		int result = sqlite3_open(db_path.c_str(),&db);
		if (result != SQLITE_OK)//open failure
		{
			std::cerr<<sqlite3_errmsg(db);
			close();
			return false;
		}
		return true;
	}
	bool sqlite3tools::open()
	{
		if (db) //alreay open
		{
			return true;
		}
		int result = sqlite3_open(db_path.c_str(),&db);
		if (result != SQLITE_OK)//open failure
		{
			std::cerr<<sqlite3_errmsg(db);
			close();
			return false;
		}
		return true;
	}

	bool sqlite3tools::close()
	{
		if (db)
		{
			if(sqlite3_close(db) != SQLITE_OK)
				return false;
			db = NULL;
		}
		return true;
	}
	void sqlite3tools::syn_OFF(){
		sqlite3_exec(db,"PRAGMA synchronous OFF;",NULL,NULL,NULL);
	}

	bool sqlite3tools::execDDL(const std::string &ddl)
	{
		if (!open())
		{
			if(!open())return false;
		}
		begin();
		sqlite3_exec(db,ddl.c_str(),NULL,NULL,NULL);
		commit();
		return true;
	}

	bool sqlite3tools::update(const std::string &sql)
	{
		if (!open())
		{
			if(!open())return false;
		}
		if(SQLITE_OK != sqlite3_exec(db,sql.c_str(),NULL,NULL,NULL))
			return false;

		return true;
	}

	bool sqlite3tools::insert(const std::string &sql)
	{
		if (!open())
		{
			if(!open())return false;
		}
		if(SQLITE_OK != sqlite3_exec(db,sql.c_str(),NULL,NULL,NULL))
			return false;
		return true;
	}

	bool sqlite3tools::remove(const std::string &sql)
	{
		if (!open())
		{
			if(!open())return false;
		}
		if(SQLITE_OK != sqlite3_exec(db,sql.c_str(),NULL,NULL,NULL))
			return false;
		return true;
	}

	bool sqlite3tools::update(std::vector<std::string> &sqls)
	{
		if (!open())
		{
			if(!open())return false;
		}
		begin();
		for (int i = 0; i < sqls.size(); ++i)
		{
			update(sqls[i]);
		}
		commit();
		return true;
	}

	bool sqlite3tools::insert(std::vector<std::string> &sqls)
	{
		if (!open())
		{
			if(!open())return false;
		}
		begin();
		for (int i = 0; i < sqls.size(); ++i)
		{
			insert(sqls[i]);
		}
		commit();
		return true;
	}

	bool sqlite3tools::clear_table(const std::string &table)
	{
		std::string sql = "delete from "+table;

		if (!open())
		{
			if(!open())return false;
		}
		begin();
		if(SQLITE_OK != sqlite3_exec(db,sql.c_str(),NULL,NULL,NULL))
			return false;
		commit();
		return true;
	}

	bool sqlite3tools::execDML(const std::string &dml)
	{
		if (!open())
		{
			if(!open())return false;
		}
		begin();
		if(SQLITE_OK != sqlite3_exec(db,dml.c_str(),NULL,NULL,NULL))
			return false;
		commit();
		return true;
	}
//////////////////////

class sql_test
{
public:
	static int call_back(void * data,int arg_count,char ** arg_values,char ** arg_names){
			std::cout<<arg_names[0]<<"\t"<<arg_names[1]<<std::endl;
		for (int i = 0; i < arg_count; ++i)
		{
			std::cout<<std::string("")+arg_values[i]<<std::endl;
		}
	}
};


bool sqlite3tools::execDQL(const std::string &dql)
{

	if (!open(db_path))
	{
		if(!open(db_path))return false;
	}
	// begin();
	// if(SQLITE_OK != sqlite3_exec(db,dql.c_str(),sql_test::call_back,(void *)(&dql_result),NULL))
	// 	return false;
	// commit();
	int ncols,nrows;
	char ** result;
	sqlite3_get_table(db,dql.c_str(),&result, &nrows, &ncols,  NULL);
	std::cout<<"行数: "<< nrows<<std::endl;
	std::cout<<"列数: "<< ncols<<std::endl;
	for (int i = 1; i <= nrows; ++i)
	{
		for (int j = 0; j < ncols; ++j)
		{
			// std::cout<< result[1] <<"\t";
			std::cout<< result[i*ncols+j] <<"\t";
		}
		std::cout<<std::endl;
	}
	return true;
}

void * sqlite3tools::get_query_result()
{

	return dql_result;
}
void sqlite3tools::get_m_suffix(const std::string table,std::vector<std::string> &suffix)
{

	if (!open(db_path))
	{
		if(!open(db_path))exit(-1) ;
	}
	int ncols,nrows;
	char ** result;
	std::string sql = "select * from ";
	sql += table;

	sqlite3_get_table(db,sql.c_str(),&result, &nrows, &ncols,  NULL);

	for (int i = 1; i <= nrows; ++i)
	{
		for (int j = 0; j < ncols; ++j)
		{
			suffix.push_back(result[i*ncols+j]);
		}
	}
}


void sqlite3tools::get_m_lib(const std::string table,std::multimap<std::string, MusicItem> &lib)
{

	if (!open(db_path))
	{
		if(!open(db_path))exit(-1) ;
	}
	int ncols,nrows;
	char ** result;
	std::string sql = "select * from ";
	sql += table;
	sqlite3_get_table(db,sql.c_str(),&result, &nrows, &ncols,  NULL);
	// std::cout<<sql<<std::endl;//
	lib.clear();
	//reload
	for (int i = 1; i <= nrows; ++i)
	{

		MusicItem m(lib.size(),result[i*ncols+1],result[i*ncols+0]);
        lib.insert(std::pair<std::string,MusicItem>(result[i*ncols+0],m));
	}
}


#endif


#endif