#include "MusicItem.h"

MusicItem::MusicItem(int m_id,string m_name,string m_path):m_id(m_id),m_name(m_name),m_path(m_path)
{}

int MusicItem::get_m_id()
{
	return m_id;
}

std::string MusicItem::get_m_name()
{
	return m_name;
}

void MusicItem::set_m_name(std::string m_name)
{
	this->m_name = m_name;
}

std::string MusicItem::get_m_path()
{
	return m_path;
}

void MusicItem::set_m_path(std::string m_path)
{
	this->m_path = m_path;
}
