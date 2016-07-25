/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_GEOMETRY_H__
#define __COMC3D_COM_GEOMETRY_H__

#include "ComMesh.hpp"
#include "ComSubmesh.hpp"

#include <Scene/Geometry.hpp>

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

		inline Castor3D::GeometrySPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::GeometrySPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Mesh, IMesh *, make_getter( m_internal.get(), &Castor3D::Geometry::GetMesh ), make_putter( m_internal.get(), &Castor3D::Geometry::SetMesh ) );

		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &Castor3D::MovableObject::GetName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, make_getter( m_internal.get(), &Castor3D::MovableObject::GetType ) );
		COM_PROPERTY_GET( Scene, IScene *, make_getter( m_internal.get(), &Castor3D::MovableObject::GetScene ) );

		STDMETHOD( GetMaterial )( /* [in] */ ISubmesh * submesh, /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( SetMaterial )( /* [in] */ ISubmesh * submesh, /* [in] */ IMaterial * val );
		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();

	private:
		Castor3D::GeometrySPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Geometry ), CGeometry );

	DECLARE_VARIABLE_PTR_GETTER( Geometry, Castor3D, Geometry );
	DECLARE_VARIABLE_PTR_PUTTER( Geometry, Castor3D, Geometry );
}

#endif
