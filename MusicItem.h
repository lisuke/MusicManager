#ifndef __Music_Item_H__
#define __Music_Item_H__

#include <string>

using std::string;

class MusicItem
{
	private:
		int m_id;
		string m_name;
		string m_path;
		
	public:
		MusicItem(int m_id,string m_name,string m_path);
		int get_m_id();
		std::string get_m_name();
		void set_m_name(std::string m_name);
		std::string get_m_path();
		void set_m_path(std::string m_path);
};

#endif
