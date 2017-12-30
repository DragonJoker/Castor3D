/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SUBMESH_H__
#define __COMC3D_COM_SUBMESH_H__

#include "ComTextureLayout.hpp"

#include <Mesh/Submesh.hpp>
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
	class ATL_NO_VTABLE CSubmesh
		:	COM_ATL_OBJECT( Submesh )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSubmesh();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSubmesh();

		inline castor3d::SubmeshSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::SubmeshSPtr internal )
		{
			m_internal = internal;

			if ( m_internal->hasComponent( castor3d::TriFaceMapping::Name ) )
			{
				m_mapping = m_internal->getComponent< castor3d::TriFaceMapping >();
			}
			else
			{
				m_mapping = std::make_shared< castor3d::TriFaceMapping >( *m_internal );
				m_internal->addComponent( m_mapping );
			}
		}

		COM_PROPERTY( Topology, eTOPOLOGY, makeGetter( m_internal.get(), &castor3d::Submesh::getTopology ), makePutter( m_internal.get(), &castor3d::Submesh::setTopology ) );
		COM_PROPERTY_GET( FaceCount, unsigned int, makeGetter( m_internal.get(), &castor3d::Submesh::getFaceCount ) );
		COM_PROPERTY_GET( PointsCount, unsigned int, makeGetter( m_internal.get(), &castor3d::Submesh::getPointsCount ) );

		STDMETHOD( AddPoint )( /* [in] */ IVector3D * val );
		STDMETHOD( AddFace )( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z );

	private:
		castor3d::SubmeshSPtr m_internal;
		std::shared_ptr< castor3d::TriFaceMapping > m_mapping;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Submesh ), CSubmesh );

	DECLARE_VARIABLE_PTR_GETTER( Submesh, castor3d, Submesh );
	DECLARE_VARIABLE_PTR_PUTTER( Submesh, castor3d, Submesh );
}

#endif
