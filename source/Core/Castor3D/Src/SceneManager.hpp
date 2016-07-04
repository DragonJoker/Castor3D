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
#ifndef ___C3D_SCENE_MANAGER_H___
#define ___C3D_SCENE_MANAGER_H___
#include "Manager/Manager.hpp"

#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"

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
	template<> struct CachedObjectNamer< Scene >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template<>
	struct ElementProducer< Scene, Castor::String, Engine & >
	{
		using ElemPtr = std::shared_ptr< Scene >;

		ElemPtr operator()( Castor::String const & p_key, Engine & p_engine )
		{
			return std::make_shared< Scene >( p_key, p_engine );
		}
	};
	/**
	 *\~english
	 *\brief		Creates a Scene cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de Scene.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	std::unique_ptr< Cache< Scene, Castor::String, ElementProducer< Scene, Castor::String, Engine & > > >
	MakeCache< Scene, Castor::String, ElementProducer< Scene, Castor::String, Engine & > >( EngineGetter const & p_get, ElementProducer< Scene, Castor::String, Engine & > const & p_produce )
	{
		return std::make_unique< Cache< Scene, Castor::String, ElementProducer< Scene, Castor::String, Engine & > > >( p_get, p_produce );
	}
	/**
	 *\~english
	 *\brief		Updates all scenes of the given cache.
	 *\param[in]	p_cache	The cache.
	 *\~french
	 *\brief		Met à jour toutes les scènes du cache donné.
	 *\param[in]	p_cache	Le cache.
	 */
	inline void Update( Cache< Scene, Castor::String, ElementProducer< Scene, Castor::String, Engine & > > & p_cache )
	{
		auto l_lock = make_unique_lock( p_cache );

		for ( auto l_it : p_cache )
		{
			l_it.second->Update();
		}
	}
}

#endif
