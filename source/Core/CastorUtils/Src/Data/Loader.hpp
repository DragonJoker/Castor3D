/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LOADER_H___
#define ___CASTOR_LOADER_H___

#include "File.hpp"
#include "LoaderException.hpp"
#include "Design/NonCopyable.hpp"

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
		 *\param[in,out]	p_object	The object to read.
		 *\param[in,out]	p_file		The file where to read the object.
		 *\~french
		 *\brief			Lit une ressource à partir d'un fichier.
		 *\param[in,out]	p_object	L'objet à lire.
		 *\param[in,out]	p_file		Le fichier où lire l'objet.
		 */
		virtual bool operator()( T & p_object, Type & p_file )
		{
			LOADER_ERROR( "Import not supported by the loader registered for this type" );
		}
	};
}

#endif
