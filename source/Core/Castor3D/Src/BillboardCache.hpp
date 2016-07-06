/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_BILLBOARD_CACHE_H___
#define ___C3D_BILLBOARD_CACHE_H___

#include "Cache/ObjectCache.hpp"

#include "Scene/BillboardList.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<>
	struct CachedObjectNamer< BillboardList >
	{
		C3D_API static const Castor::String Name;
	};
	/**
	 *\~english
	 *\brief		Creates a BillboardList cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de BillboardList.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	inline std::unique_ptr< BillboardListCache >
	MakeObjectCache< BillboardList, Castor::String, BillboardListProducer, MovableAttacher, MovableDetacher, ElementInitialiser< BillboardList >, ElementCleaner< BillboardList >, ElementMerger< BillboardList, Castor::String > >
		( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, Engine & p_engine, Scene & p_scene, BillboardListProducer && p_produce )
	{
		return std::make_unique< BillboardListCache >( p_rootNode, p_rootCameraNode, p_rootObjectNode, p_engine, p_scene, std::move( p_produce ) );
	}
}

#endif
