/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_WRITER_H___
#define ___CASTOR_WRITER_H___

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

namespace castor
{
	template< class DataT, typename StreamT >
	class Writer
		: public castor::NonCopyable
	{
	protected:
		using Type = StreamT;

	public:
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
		virtual bool operator()( DataT const & CU_UnusedParam( object ), Type & CU_UnusedParam( file ) )
		{
			CU_LoaderError( "Export not supported by the loader registered for this type" );
		}
	};

	template< class DataT, FileType FileTypeT >
	class FileWriter
		: public Writer< DataT, typename FileTyper< FileTypeT >::Type >
	{
	protected:
		using Type = typename FileTyper< FileTypeT >::Type;
	};
}

#endif
