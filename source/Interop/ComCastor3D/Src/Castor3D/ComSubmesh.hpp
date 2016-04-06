/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef __COMC3D_COM_SUBMESH_H__
#define __COMC3D_COM_SUBMESH_H__

#include "ComTexture.hpp"
#include "ComColour.hpp"

#include <Submesh.hpp>

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

		inline Castor3D::SubmeshSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::SubmeshSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Topology, eTOPOLOGY, make_getter( m_internal.get(), &Castor3D::Submesh::GetTopology ), make_putter( m_internal.get(), &Castor3D::Submesh::SetTopology ) );
		COM_PROPERTY_GET( FaceCount, unsigned int, make_getter( m_internal.get(), &Castor3D::Submesh::GetFaceCount ) );
		COM_PROPERTY_GET( PointsCount, unsigned int, make_getter( m_internal.get(), &Castor3D::Submesh::GetPointsCount ) );

		STDMETHOD( AddPoint )( /* [in] */ IVector3D * val );
		STDMETHOD( AddFace )( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z );

	private:
		Castor3D::SubmeshSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Submesh ), CSubmesh );

	DECLARE_VARIABLE_PTR_GETTER( Submesh, Castor3D, Submesh );
	DECLARE_VARIABLE_PTR_PUTTER( Submesh, Castor3D, Submesh );
}

#endif
