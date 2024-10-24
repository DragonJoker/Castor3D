/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEXT_LOADER_H___
#define ___CASTOR_TEXT_LOADER_H___

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Data/Loader.hpp"
#include "CastorUtils/Data/TextFile.hpp"

namespace castor
{
	template< class T >
	class TextLoaderT
		: public Loader< T, FileType::eText >
	{
	protected:
		using Type = typename Loader< T, FileType::eText >::Type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TextLoaderT() = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextLoaderT()noexcept = default;
	};
}

#endif
