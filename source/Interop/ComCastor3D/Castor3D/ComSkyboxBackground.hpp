/* See LICENSE file in root folder */
#ifndef __COMC3D_ComSkyboxBackground_H__
#define __COMC3D_ComSkyboxBackground_H__

#include "ComAtlObject.hpp"

#include <Scene/Background/Skybox.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		29/06/2018
	\~english
	\brief		This class defines a CSkyboxBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CSkyboxBackground accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSkyboxBackground
		:	COM_ATL_OBJECT( SkyboxBackground )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSkyboxBackground();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSkyboxBackground();

		inline std::shared_ptr< castor3d::SkyboxBackground > getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( std::shared_ptr< castor3d::SkyboxBackground > internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Type
			, eBACKGROUND_TYPE
			, makeGetter( m_internal.get(), &castor3d::SceneBackground::getType ) );
		STDMETHOD( put_LeftImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_RightImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_TopImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_BottomImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_FrontImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_BackImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_CrossImage )( /* [in] */ BSTR filePath );
		STDMETHOD( LoadEquirectangularImage )( /* [in] */ BSTR filePath, /* [in] */ unsigned int size );

	private:
		std::shared_ptr< castor3d::SkyboxBackground > m_internal;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( SkyboxBackground ), CSkyboxBackground );

	DECLARE_VARIABLE_PTR_GETTER( SkyboxBackground, castor3d, SkyboxBackground );
	DECLARE_VARIABLE_PTR_PUTTER( SkyboxBackground, castor3d, SkyboxBackground );
}

#endif
