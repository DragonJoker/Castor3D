/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_RESOURCE_H___
#define ___CASTOR_RESOURCE_H___

#include "CastorUtils/Design/Collection.hpp"
#include "CastorUtils/Design/Named.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		External resource representation
	\remark		A resource is a collectionnable object. You can change it's name, the Collection is noticed of the change
	\~french
	\brief		Représentation d'une ressource externe
	\remark		Une ressource est un objet collectionnable. Si le nom de la ressource est changé, la Collection est notifiée du changement
	*/
	template< class ResType >
	class Resource
		: public Named
	{
	protected:
		typedef Collection< ResType, String > collection;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom
		 */
		explicit Resource( String const & name );
		/**
		*\~english
		*\brief		Destructor
		*\~french
		*\brief		Destructeur
		*/
		~Resource();

	public:
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	other	The Resource to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	other	La Resource à copier
		 */
		Resource( Resource const & other );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	other	The Resource to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	other	La Resource à déplacer
		 */
		Resource( Resource && other );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	other	The Resource to copy
		 *\return		A reference to this Resource
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	other	La Resource à copier
		 *\return		Une référence sur ce Resource
		 */
		Resource & operator=( Resource const & other );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	other	The Resource to move
		 *\return		A reference to this Resource
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	other	La Resource à déplacer
		 *\return		Une référence sur ce Resource
		 */
		Resource & operator=( Resource && other );
		/**
		 *\~english
		 *\brief		Defines the resource name
		 *\remarks		Also tells the collection the new resource name
		 *\param[in]	name	The new name
		 *\~french
		 *\brief		Définit le nom de la ressource
		 *\remarks		Dit aussi à la collection le nouveau de la ressource
		 *\param[in]	name	Le nouveau nom
		 */
		void changeName( String const & name );
	};

#	include "Resource.inl"
}

#endif
