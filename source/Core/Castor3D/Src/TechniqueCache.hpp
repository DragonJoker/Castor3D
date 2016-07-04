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
	template<> struct CachedObjectNamer< RenderTechnique >
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
	class RenderTechniqueCache
		: public Cache< RenderTechnique, Castor::String, RenderTechniqueProducer >
	{
		using CacheType = Cache< RenderTechnique, Castor::String, RenderTechniqueProducer >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API RenderTechniqueCache( EngineGetter && p_get, RenderTechniqueProducer && p_produce );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderTechniqueCache();
		/**
		 *\~english
		 *\brief		Updates all techniques.
		 *\~french
		 *\brief		Met à jour toutes les techniques.
		 */
		C3D_API void Update();
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
	inline std::unique_ptr< RenderTechniqueCache >
	MakeCache( EngineGetter && p_get, RenderTechniqueProducer && p_produce )
	{
		return std::make_unique< RenderTechniqueCache >( std::move( p_get ), std::move( p_produce ) );
	}
}

#endif
