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
#ifndef ___C3D_TECHNIQUE_CACHE_H___
#define ___C3D_TECHNIQUE_CACHE_H___

#include "Cache/Cache.hpp"

#include "Technique/TechniqueFactory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		21/02/2016
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<>
	struct CachedObjectNamer< RenderTechnique >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		21/02/2016
	\~english
	\brief		Render technique cache.
	\~french
	\brief		Le cache de techniques de rendu.
	*/
	template<>
	class Cache< RenderTechnique, Castor::String, RenderTechniqueProducer >
		: public CacheBase< RenderTechnique, Castor::String, RenderTechniqueProducer, ElementInitialiser< RenderTechnique >, ElementCleaner< RenderTechnique >, ElementMerger< RenderTechnique, Castor::String > >
	{
	public:
		using MyCacheType = CacheBase< RenderTechnique, Castor::String, RenderTechniqueProducer, ElementInitialiser< RenderTechnique >, ElementCleaner< RenderTechnique >, ElementMerger< RenderTechnique, Castor::String > >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API Cache( Engine & p_engine
					  , RenderTechniqueProducer && p_produce
					  , Initialiser && p_initialise = Initialiser{}
					  , Cleaner && p_clean = Cleaner{}
					  , Merger && p_merge = Merger{} )
			: MyCacheType( p_engine, std::move( p_produce ), std::move( p_initialise ), std::move( p_clean ), std::move( p_merge ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Cache();
		/**
		 *\~english
		 *\return		The RenderTechnique factory.
		 *\~french
		 *\return		La fabrique de RenderTechnique.
		 */
		inline TechniqueFactory const & GetFactory()const
		{
			return m_produce.m_factory;
		}
		/**
		 *\~english
		 *\return		The RenderTechnique factory.
		 *\~french
		 *\return		La fabrique de RenderTechnique.
		 */
		inline TechniqueFactory & GetFactory()
		{
			return m_produce.m_factory;
		}
	};
	using RenderTechniqueCache = Cache< RenderTechnique, Castor::String, RenderTechniqueProducer >;
	DECLARE_SMART_PTR (RenderTechniqueCache);
	/**
	 *\~english
	 *\brief		Creates a RenderTechnique cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de RenderTechnique.
	 *\param[in]	p_get		Le récupérateur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	inline std::unique_ptr< Cache< RenderTechnique, Castor::String, RenderTechniqueProducer > >
	MakeCache< RenderTechnique, Castor::String, RenderTechniqueProducer > ( Engine & p_engine, RenderTechniqueProducer && p_produce )
	{
		return std::make_unique< Cache< RenderTechnique, Castor::String, RenderTechniqueProducer > >( p_engine, std::move( p_produce ) );
	}
}

#endif
