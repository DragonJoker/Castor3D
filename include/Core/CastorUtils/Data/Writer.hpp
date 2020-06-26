/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_WRITER_H___
#define ___CASTOR_WRITER_H___

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Data/LoaderException.hpp"

namespace castor
{
	template< class T, FileType FT >
	class Writer
		: public castor::NonCopyable
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
		Writer() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Writer() = default;
		/**
		 *\~english
		 *\brief			Writes a resource to a file.
		 *\param[in]		object	The object to write.
		 *\param[in,out]	file	The file where to write the object.
		 *\~french
		 *\brief			Ecrit une ressource dans un fichier.
		 *\param[in]		object	L'objet à écrire.
		 *\param[in,out]	file	Le fichier où écrire l'objet.
		 */
		virtual bool operator()( T const & object, Type & file )
		{
			CU_LoaderError( "Export not supported by the loader registered for this type" );
		}
	};
}

#endif
