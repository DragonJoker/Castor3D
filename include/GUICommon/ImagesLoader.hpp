#ifndef ___GUI_ImagesLoader___
#define ___GUI_ImagesLoader___

namespace GuiCommon
{
	class ImagesLoader
	{
	private:
		static std::map <unsigned int, wxBitmap *> m_mapBitmaps;

	public:
		ImagesLoader();
		~ImagesLoader();

		static wxBitmap * AddBitmap( unsigned int p_uiID, const char* const* p_bits);
		static wxBitmap * GetBitmap( unsigned int p_uiID);
	};
}

#endif
