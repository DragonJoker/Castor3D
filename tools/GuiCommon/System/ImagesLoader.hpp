/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_IMAGES_LOADER_H___
#define ___GUICOMMON_IMAGES_LOADER_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <CastorUtils/Design/UnicityException.hpp>

#include <thread>

#include <wx/image.h>

namespace GuiCommon
{
	class ImagesLoader
	{
	public:
		ImagesLoader();
		~ImagesLoader();

		static void cleanup();
		static void addBitmap( uint32_t id, char const * const * pBits );
		static wxImage * getBitmap( uint32_t id );
		static void waitAsyncLoads();

		static ImageIdMap const & getBitmaps()noexcept
		{
			return doGetInstance()->m_mapImages;
		}

	private:
		void doCleanup();
		void doAddBitmap( uint32_t id, char const * const * pBits );
		wxImage * doGetBitmap( uint32_t id );
		void doWaitAsyncLoads();

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Throws an exception if the instance is already initialised.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		Lance une exception si l'instance est déjà initialisée.
		 */
		explicit ImagesLoader( ImagesLoader * pThis )
		{
			if ( !doGetInstance() )
			{
				doGetInstance() = pThis;
			}
			else
			{
				CU_UnicityError( castor::UnicityError::eAnInstance, typeid( ImagesLoader ).name() );
			}
		}
		/**
		 *\~english
		 *\return		The unique instance, nullptr if none.
		 *\~french
		 *\return		L'instance unique, nullptr s'il n'y en a pas.
		 */
		static inline ImagesLoader *& doGetInstance()
		{
			static ImagesLoader * instance = nullptr;
			return instance;
		}

	private:
		ImageIdMap m_mapImages;
		std::mutex m_mutex;
		std::vector< std::thread > m_arrayCurrentLoads;
	};
}

#endif
