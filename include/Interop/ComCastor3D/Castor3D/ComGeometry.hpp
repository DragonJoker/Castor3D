/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_GEOMETRY_H__
#define __COMC3D_COM_GEOMETRY_H__

#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComSubmesh.hpp"

#include <Castor3D/Scene/Geometry.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CGeometry object accessible from COM.
	\~french
	\brief		Cette classe définit un CGeometry accessible depuis COM.
	*/
	class ATL_NO_VTABLE CGeometry
		:	COM_ATL_OBJECT( Geometry )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CGeometry();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CGeometry();

		inline castor3d::GeometrySPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::GeometrySPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Mesh, IMesh *, makeGetter( m_internal.get(), &castor3d::Geometry::getMesh ), makePutter( m_internal.get(), &castor3d::Geometry::setMesh ) );

		COM_PROPERTY_GET( Name, BSTR, makeGetter( m_internal.get(), &castor3d::MovableObject::getName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, makeGetter( m_internal.get(), &castor3d::MovableObject::getType ) );
		COM_PROPERTY_GET( Scene, IScene *, makeGetter( m_internal.get(), &castor3d::MovableObject::getScene ) );

		STDMETHOD( GetMaterial )( /* [in] */ ISubmesh * submesh, /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( SetMaterial )( /* [in] */ ISubmesh * submesh, /* [in] */ IMaterial * val );
		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();

	private:
		castor3d::GeometrySPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Geometry ), CGeometry );

	DECLARE_VARIABLE_PTR_GETTER( Geometry, castor3d, Geometry );
	DECLARE_VARIABLE_PTR_PUTTER( Geometry, castor3d, Geometry );
}

#endif
