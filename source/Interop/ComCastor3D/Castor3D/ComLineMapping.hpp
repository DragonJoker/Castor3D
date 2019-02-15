/* See LICENSE file in root folder */
#ifndef __COMC3D_ComLinesMapping_H__
#define __COMC3D_ComLinesMapping_H__

#include "ComTextureLayout.hpp"

#include <Mesh/Submesh.hpp>
#include <Mesh/SubmeshComponent/LinesMapping.hpp>

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
	class ATL_NO_VTABLE CLinesMapping
		:	COM_ATL_OBJECT( LinesMapping )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CLinesMapping();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CLinesMapping();

		inline std::shared_ptr< castor3d::LinesMapping > getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( std::shared_ptr< castor3d::LinesMapping > internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( LinesCount, unsigned int, makeGetter( m_internal.get(), &castor3d::Submesh::getFaceCount ) );

		STDMETHOD( get_Type )( /* [retval][out] */ eINDEX_MAPPING_TYPE * value );
		STDMETHOD( AddLine )( /* [in] */ unsigned int x, /* [in] */ unsigned int y );

	private:
		std::shared_ptr< castor3d::LinesMapping > m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LinesMapping ), CLinesMapping );

	DECLARE_VARIABLE_PTR_GETTER( LinesMapping, castor3d, LinesMapping );
	DECLARE_VARIABLE_PTR_PUTTER( LinesMapping, castor3d, LinesMapping );
}

#endif
