#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/ImagesLoader.hpp"

using namespace GuiCommon;

std::map <unsigned int, wxBitmap *> ImagesLoader :: m_mapBitmaps;

ImagesLoader :: ImagesLoader()
{
}

ImagesLoader :: ~ImagesLoader()
{
	for (std::map <unsigned int, wxBitmap *>::iterator l_it = m_mapBitmaps.begin() ; l_it != m_mapBitmaps.end() ; ++l_it)
	{
		delete l_it->second;
	}

	m_mapBitmaps.clear();
}

wxBitmap * ImagesLoader :: GetBitmap( unsigned int p_uiID)
{
	wxBitmap * l_pReturn = nullptr;
	std::map <unsigned int, wxBitmap *>::iterator l_it = m_mapBitmaps.find( p_uiID);

	if (l_it != m_mapBitmaps.end())
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

wxBitmap * ImagesLoader :: AddBitmap( unsigned int p_uiID, const char* const* p_bits)
{
	wxBitmap * l_pReturn = nullptr;
	std::map <unsigned int, wxBitmap *>::iterator l_it = m_mapBitmaps.find( p_uiID);

	if (l_it != m_mapBitmaps.end())
	{
		l_pReturn = l_it->second;
	}
	else
	{
		l_pReturn = new wxBitmap( p_bits);
		m_mapBitmaps.insert( std::pair<unsigned int, wxBitmap *>( p_uiID, l_pReturn));
	}

	return l_pReturn;
}
