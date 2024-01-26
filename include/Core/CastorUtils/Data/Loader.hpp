/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LOADER_H___
#define ___CASTOR_LOADER_H___

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

namespace castor
{
	template< class T, FileType FT >
	class Loader
		: public NonMovable
	{
	protected:
		using Type = typename FileTyper< FT >::Type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Loader() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Loader()noexcept = default;
		/**
		 *\~english
		 *\brief			Reads a resource from a file.
		 *\param[in,out]	object	The object to read.
		 *\param[in,out]	file	The file where to read the object.
		 *\~french
		 *\brief			Lit une ressource à partir d'un fichier.
		 *\param[in,out]	object	L'objet à lire.
		 *\param[in,out]	file	Le fichier où lire l'objet.
		 */
		virtual bool operator()( CU_UnusedParam( T &, object ), CU_UnusedParam( Type &, file ) )
		{
			CU_LoaderError( "Import not supported by the loader registered for this type" );
		}
	};
}

#endif
