/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_Node___
#define ___C3D_Node___

#include "NodeBase.h"
#include "../light/Light.h"
#include "../camera/Camera.h"
#include "../camera/Ray.h"
#include "../geometry/primitives/Geometry.h"

namespace Castor3D
{
	template <typename T>
	class Node : public NodeBase
	{
	};

	template <> class Node<Light> : public NodeBase
	{
	public:
		Node<Light>( const String & p_strName = C3DEmptyString)
			:	NodeBase( p_strName)
		{
		}
		virtual void EndRender()
		{
			if (m_visible && m_displayable)
			{
				for (C3DMap( String, MovableObject *)::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
				{
					((Light *) l_it->second)->Disable();
				}

				for (C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
				{
					((Node<Light> *)l_it->second)->EndRender();
				}
			}
		}
		virtual eTYPE GetType()const { return eLight; }
	};

	template <> class Node<Camera> : public NodeBase
	{
	public:
		Node<Camera>( const String & p_strName = C3DEmptyString)
			:	NodeBase( p_strName)
		{
		}
		virtual void EndRender()
		{
			if (m_visible && m_displayable)
			{
				for (C3DMap( String, MovableObject *)::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
				{
					((Camera *) l_it->second)->EndRender();
				}

				for (C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
				{
					((Node<Camera> *)l_it->second)->EndRender();
				}
			}
		}
		virtual eTYPE GetType()const { return eCamera; }
	};

	template <> class Node<Geometry> : public NodeBase
	{
	public:
		Node<Geometry>( const String & p_strName = C3DEmptyString)
			:	NodeBase( p_strName)
		{
		}
		/**
		 * Creates the vertex buffer of attached geometries
		 *@param p_nm : [in] The normals mode (face or vertex)
		 *@param p_showNormals : [in] Tells if we show normals (of faces or vertex)
		 *@param p_nbFaces : [in/out] The faces number
		 *@param p_nbVertex : [in/out] The vertex number
		 */
		void CreateList( eNORMALS_MODE p_nm, bool p_showNormals, size_t & p_nbFaces, size_t & p_nbVertex)const
		{
			for (C3DMap( String, MovableObject *)::const_iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
			{
				((Geometry *)l_it->second)->CreateBuffers( p_nm, p_nbFaces, p_nbVertex);
			}

			for (C3DMap( String, NodeBase *)::const_iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
			{
				((Node<Geometry> *)l_it->second)->CreateList( p_nm, p_showNormals, p_nbFaces, p_nbVertex);
			}
		}
		/**
		 * Gets the nearest geometry held by this node or it's children nodes, which is hit by the ray
		 */
		Geometry * GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FacePtr * p_ppFace, SubmeshPtr * p_ppSubmesh)
		{
			Geometry * l_pReturn = NULL;
			real l_fDistance;

			for (C3DMap( String, MovableObject *)::iterator l_it = m_attachedObjects.begin() ; l_it != m_attachedObjects.end() ; ++l_it)
			{
				if ((l_fDistance = p_pRay->Intersects( (Geometry *)l_it->second, p_ppFace, p_ppSubmesh)) >= 0.0 && l_fDistance < p_fDistance)
				{
					p_fDistance = l_fDistance;
					l_pReturn = (Geometry *)l_it->second;
				}
			}

			Geometry * l_pTmp;

			for (C3DMap( String, NodeBase *)::iterator l_it = m_childs.begin() ; l_it != m_childs.end() ; ++l_it)
			{
				if ( ! (l_pTmp = ((Node<Geometry> *)l_it->second)->GetNearestGeometry( p_pRay, p_fDistance, p_ppFace, p_ppSubmesh)) == NULL)
				{
					l_pReturn = l_pTmp;
				}
			}

			return l_pReturn;
		}
		virtual eTYPE	GetType()const { return eGeometry; }
	};
}

#endif