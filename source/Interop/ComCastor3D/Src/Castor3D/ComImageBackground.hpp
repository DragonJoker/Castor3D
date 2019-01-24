/* See LICENSE file in root folder */
#ifndef __COMC3D_ComImageBackground_H__
#define __COMC3D_ComImageBackground_H__

#include "ComAtlObject.hpp"

#include <Scene/Background/Image.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		29/06/2018
	\~english
	\brief		This class defines a CImageBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CImageBackground accessible depuis COM.
	*/
	class ATL_NO_VTABLE CImageBackground
		:	COM_ATL_OBJECT( ImageBackground )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CImageBackground();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CImageBackground();

		inline std::shared_ptr< castor3d::ImageBackground > getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( std::shared_ptr< castor3d::ImageBackground > internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Type
			, eBACKGROUND_TYPE
			, makeGetter( m_internal.get(), &castor3d::SceneBackground::getType ) );
		STDMETHOD( put_Image )( /* [in] */ BSTR filePath );

	private:
		std::shared_ptr< castor3d::ImageBackground > m_internal;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( ImageBackground ), CImageBackground );

	DECLARE_VARIABLE_PTR_GETTER( ImageBackground, castor3d, ImageBackground );
	DECLARE_VARIABLE_PTR_PUTTER( ImageBackground, castor3d, ImageBackground );
}

#endif
