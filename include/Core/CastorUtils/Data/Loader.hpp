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
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Main resource loader class.
	\remarks	Holds the two functions needed for a resource loader : Load and Save.
	\~french
	\brief		Classe de base pour les loaders de ressource.
	\remarks	Contient les 2 fonctions nécessaire a un loader : Load et Save.
	*/
	template< class T, FileType FT >
	class Loader
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
		Loader() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Loader() = default;
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
		virtual bool operator()( T & object, Type & file )
		{
			CU_LoaderError( "Import not supported by the loader registered for this type" );
		}
	};
}

#endif
