#ifndef ___GUI_ImagesLoader___
#define ___GUI_ImagesLoader___

namespace GuiCommon
{
	DECLARE_MAP( uint32_t, wxImage *, ImageId );
	typedef std::shared_ptr< std::thread > thread_sptr;
	DECLARE_VECTOR( thread_sptr, ThreadPtr );

	class ImagesLoader
	{
	private:
		static ImageIdMap		m_mapImages;
		static std::mutex		m_mutex;
		static ThreadPtrArray	m_arrayCurrentLoads;

	public:
		ImagesLoader();
		~ImagesLoader();

		static void			Cleanup			();
		static void			AddBitmap		( uint32_t p_uiID, char const * const * p_pBits );
		static wxImage *	GetBitmap		( uint32_t p_uiID );
		static void			WaitAsyncLoads	();
	};
}

#endif
