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
#ifndef __COMC3D_COM_SUBMESH_H__
#define __COMC3D_COM_SUBMESH_H__

#include "ComTextureLayout.hpp"
#include "ComColour.hpp"

#include <Mesh/Submesh.hpp>

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
		}

		COM_PROPERTY( Topology, eTOPOLOGY, make_getter( m_internal.get(), &castor3d::Submesh::getTopology ), make_putter( m_internal.get(), &castor3d::Submesh::setTopology ) );
		COM_PROPERTY_GET( FaceCount, unsigned int, make_getter( m_internal.get(), &castor3d::Submesh::getFaceCount ) );
		COM_PROPERTY_GET( PointsCount, unsigned int, make_getter( m_internal.get(), &castor3d::Submesh::getPointsCount ) );

		STDMETHOD( addPoint )( /* [in] */ IVector3D * val );
		STDMETHOD( addFace )( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z );

	private:
		castor3d::SubmeshSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Submesh ), CSubmesh );

	DECLARE_VARIABLE_PTR_GETTER( Submesh, castor3d, Submesh );
	DECLARE_VARIABLE_PTR_PUTTER( Submesh, castor3d, Submesh );
}

#endif
