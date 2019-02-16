/* See LICENSE file in root folder */
#ifndef __COMC3D_ComIndexMapping_H__
#define __COMC3D_ComIndexMapping_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Mesh/SubmeshComponent/IndexMapping.hpp>
#include <Castor3D/Mesh/SubmeshComponent/LinesMapping.hpp>
#include <Castor3D/Mesh/SubmeshComponent/TriFaceMapping.hpp>

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
	class ATL_NO_VTABLE CIndexMapping
		:	COM_ATL_OBJECT( IndexMapping )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CIndexMapping();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CIndexMapping();

		inline castor3d::IndexMappingSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::IndexMappingSPtr internal )
		{
			m_internal = internal;
		}

		STDMETHOD( get_Type )( /* [retval][out] */ eINDEX_MAPPING_TYPE * value );

	private:
		castor3d::IndexMappingSPtr m_internal;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( IndexMapping ), CIndexMapping );

	DECLARE_VARIABLE_PTR_GETTER( IndexMapping, castor3d, IndexMapping );
	DECLARE_VARIABLE_PTR_PUTTER( IndexMapping, castor3d, IndexMapping );
}

#endif
