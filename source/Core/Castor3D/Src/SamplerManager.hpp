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
#ifndef ___C3D_SAMPLER_CACHE_H___
#define ___C3D_SAMPLER_CACHE_H___

#include "Manager/Manager.hpp"

#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"

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
	template<> struct CachedObjectNamer< Sampler >
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
	struct ElementProducer< Sampler, Castor::String >
	{
		using ElemPtr = std::shared_ptr< Sampler >;

		ElementProducer( Engine & p_engine )
			: m_engine{ p_engine }
		{
		}

		ElemPtr operator()( Castor::String const & p_key )
		{
			return m_engine.GetRenderSystem()->CreateSampler( p_key );
		}

		Engine & m_engine;
	};
	/**
	 *\~english
	 *\brief		Creates a Sampler cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de Sampler.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	std::unique_ptr< Cache< Sampler, Castor::String, ElementProducer< Sampler, Castor::String > > >
	MakeCache< Sampler, Castor::String, ElementProducer< Sampler, Castor::String > >( EngineGetter const & p_get, ElementProducer< Sampler, Castor::String > const & p_produce )
	{
		return std::make_unique< Cache< Sampler, Castor::String, ElementProducer< Sampler, Castor::String > > >( p_get, p_produce );
	}
}

#endif
