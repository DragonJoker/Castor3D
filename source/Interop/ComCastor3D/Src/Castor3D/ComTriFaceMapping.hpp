/* See LICENSE file in root folder */
#ifndef __COMC3D_ComTriFaceMapping_H__
#define __COMC3D_ComTriFaceMapping_H__

#include "ComTextureLayout.hpp"

#include <Mesh/SubmeshComponent/TriFaceMapping.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class ATL_NO_VTABLE CTriFaceMapping
		:	COM_ATL_OBJECT( TriFaceMapping )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTriFaceMapping();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTriFaceMapping();

		inline std::shared_ptr< castor3d::TriFaceMapping > getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( std::shared_ptr< castor3d::TriFaceMapping > internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( FacesCount, unsigned int, makeGetter( m_internal.get(), &castor3d::TriFaceMapping::getCount ) );

		STDMETHOD( get_Type )( /* [retval][out] */ eINDEX_MAPPING_TYPE * value );
		STDMETHOD( AddFace )( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z );

	private:
		std::shared_ptr< castor3d::TriFaceMapping > m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TriFaceMapping ), CTriFaceMapping );

	DECLARE_VARIABLE_PTR_GETTER( TriFaceMapping, castor3d, TriFaceMapping );
	DECLARE_VARIABLE_PTR_PUTTER( TriFaceMapping, castor3d, TriFaceMapping );
}

#endif
