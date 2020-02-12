/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TECHNIQUE_CACHE_H___
#define ___C3D_TECHNIQUE_CACHE_H___

#include "CacheModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoModule.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for RenderTechnique.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour RenderTechnique.
	*/
	template< typename KeyType >
	struct CacheTraits< RenderTechnique, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function < std::shared_ptr< RenderTechnique >( KeyType const &
			, castor::String const &
			, RenderTarget &
			, Parameters const &
			, SsaoConfig const & ) >;
		using Merger = std::function< void( CacheBase< RenderTechnique, KeyType > const &
			, castor::Collection< RenderTechnique, KeyType > &
			, std::shared_ptr< RenderTechnique > ) >;
	};
}

// included after because it depends on CacheTraits
#include "Castor3D/Cache/Cache.hpp"

#endif
