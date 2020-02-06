/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_WRITER_H___
#define ___CASTOR_WRITER_H___

#include "File.hpp"
#include "LoaderException.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Main resource writer class.
	\~french
	\brief		Classe de base pour les writers de ressource.
	*/
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
		 *\param[in]		p_object	The object to write.
		 *\param[in,out]	p_file		The file where to write the object.
		 *\~french
		 *\brief			Ecrit une ressource dans un fichier.
		 *\param[in]		p_object	L'objet à écrire.
		 *\param[in,out]	p_file		Le fichier où écrire l'objet.
		 */
		virtual bool operator()( T const & p_object, Type & p_file )
		{
			LOADER_ERROR( "Export not supported by the loader registered for this type" );
		}
	};
}

#endif
