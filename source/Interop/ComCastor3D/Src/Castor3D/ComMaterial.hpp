/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MATERIAL_H__
#define __COMC3D_COM_MATERIAL_H__

#include "ComTextureLayout.hpp"

#include <Material/Material.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMaterial object accessible from COM.
	\~french
	\brief		Cette classe définit un CMaterial accessible depuis COM.
	*/
	class ATL_NO_VTABLE CMaterial
		:	COM_ATL_OBJECT( Material )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CMaterial();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CMaterial();

		inline castor3d::MaterialSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::MaterialSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( PassCount, unsigned int, make_getter( m_internal.get(), &castor3d::Material::getPassCount ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
		STDMETHOD( createPass )( /* [out, retval] */ IPass ** pVal );
		STDMETHOD( getPass )( /* [in] */ unsigned int val, /* [out, retval] */ IPass ** pVal );
		STDMETHOD( destroyPass )( /* [in] */ unsigned int val );

	private:
		castor3d::MaterialSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Material ), CMaterial );

	DECLARE_VARIABLE_PTR_GETTER( Material, castor3d, Material );
	DECLARE_VARIABLE_PTR_PUTTER( Material, castor3d, Material );
}

#endif
