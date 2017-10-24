/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEXT_LOADER_H___
#define ___CASTOR_TEXT_LOADER_H___

#include "Loader.hpp"
#include "TextFile.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Partial castor::Loader specialisation for text files
	\~french
	\brief		Spécialisation partielle de castor::Loader, pour les fichiers texte
	*/
	template< class T >
	class TextLoader
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
		TextLoader() = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextLoader() = default;
	};
}

#endif
