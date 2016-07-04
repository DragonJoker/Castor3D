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
#ifndef ___C3D_WINDOW_CACHE_H___
#define ___C3D_WINDOW_CACHE_H___

#include "Cache/ResourceCache.hpp"

#include "Render/RenderWindow.hpp"

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
	template<> struct CachedObjectNamer< RenderWindow >
	{
		C3D_API static const Castor::String Name;
	};
	/**
	 *\~english
	 *\brief		Creates a RenderWindow cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de RenderWindow.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	inline std::unique_ptr< Cache< RenderWindow, Castor::String, WindowProducer > >
	MakeCache< RenderWindow, Castor::String, WindowProducer >( EngineGetter && p_get, WindowProducer && p_produce )
	{
		return std::make_unique< Cache< RenderWindow, Castor::String, WindowProducer > >( std::move( p_get ), std::move( p_produce ) );
	}
	/**
	 *\~english
	 *\brief		Renders one frame for each window.
	 *\param[in]	p_force		Forces the rendering.
	 *\~english
	 *\brief		Dessine une image de chaque fenêtre.
	 *\param[in]	p_force		Dit si on force le rendu.
	 */
	C3D_API void Render( Cache< RenderWindow, Castor::String, WindowProducer > & p_cache, bool p_force );
}

#endif
