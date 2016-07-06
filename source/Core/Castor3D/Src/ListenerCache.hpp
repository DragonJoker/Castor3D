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
#ifndef ___C3D_LISTENER_CACHE_H___
#define ___C3D_LISTENER_CACHE_H___

#include "Cache/Cache.hpp"

#include "Event/Frame/FrameListener.hpp"

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
	struct CachedObjectNamer< FrameListener >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template<>
	struct ElementCleaner< FrameListener, FrameListener >
	{
		inline void operator()( Engine & p_engine, FrameListener & p_element )
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates a FrameListener cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de FrameListener.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	inline std::unique_ptr< Cache< FrameListener, Castor::String > >
	MakeCache< FrameListener, Castor::String > 
		( Engine & p_engine
		 , ElementProducer< FrameListener, Castor::String > && p_produce
		 , ElementInitialiser< FrameListener > && p_initialise = []( FrameListenerSPtr ){}
		 , ElementCleaner< FrameListener > && p_clean = []( FrameListenerSPtr p_element )
		 {
			 p_element->Flush();
		 }
		 , ElementMerger< FrameListener, Castor::String > && p_merge = []( Cache< FrameListener, Castor::String > const &, Castor::Collection< FrameListener, Castor::String > &, FrameListenerSPtr ){} )
	{
		return std::make_unique< Cache< FrameListener, Castor::String > >( std::move( p_engine ), std::move( p_produce ), std::move( p_initialise ), std::move( p_clean ), std::move(p_merge) );
	}
}

#undef DECLARE_MANAGED_MEMBER

#endif
