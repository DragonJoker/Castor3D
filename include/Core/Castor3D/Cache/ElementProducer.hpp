/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheElementProducer_H___
#define ___C3D_CacheElementProducer_H___

#include "CacheModule.hpp"

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	/**@name Cache */
	//@{

	template< typename ElementType, typename KeyType >
	struct ElementProducer;

	template< typename KeyType >
	struct ElementProducer< RenderTarget, KeyType >
	{
		using Type = std::function< std::shared_ptr< RenderTarget >( KeyType const &, TargetType ) >;
	};

	template< typename KeyType >
	struct ElementProducer< ShaderProgram, KeyType >
	{
		using Type = std::function< std::shared_ptr< ShaderProgram >( KeyType const & ) >;
	};

	//@}
}

#endif
