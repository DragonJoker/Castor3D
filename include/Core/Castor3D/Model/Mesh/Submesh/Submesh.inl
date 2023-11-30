#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

namespace castor3d
{
	inline void Submesh::addPoints( std::vector< InterleavedVertex > const & vertices )
	{
		addPoints( vertices.data(), vertices.data() + vertices.size() );
	}

	template< size_t Count >
	inline void Submesh::addPoints( std::array< InterleavedVertex, Count > const & vertices )
	{
		addPoints( vertices.data(), vertices.data() + vertices.size() );
	}

	inline void Submesh::setDefaultMaterial( MaterialObs mat )
	{
		m_defaultMaterial = mat;
		instantiate( {}, mat, false );
	}

	inline void Submesh::disableSceneUpdate()
	{
		m_disableSceneUpdate = true;
	}

	inline void Submesh::needsUpdate()
	{
		m_dirty = true;
	}

	inline void Submesh::setIndexMapping( IndexMappingUPtr mapping )
	{
		if ( m_indexMapping
			&& m_indexMapping != mapping.get() )
		{
			m_components.erase( m_indexMapping->getId() );
		}

		m_indexMapping = mapping.get();
		auto id = mapping->getId();
		m_components.emplace( id, castor::ptrRefCast< SubmeshComponent >( mapping ) );
	}

	inline IndexMappingRPtr Submesh::getIndexMapping()const
	{
		return m_indexMapping;
	}

	template< typename ComponentT, typename ... ParamsT >
	inline ComponentT * Submesh::createComponent( ParamsT && ... params )
	{
		auto component = castor::makeUnique< ComponentT >( *this
			, std::forward< ParamsT >( params )... );
		auto result = component.get();
		addComponent( castor::ptrRefCast< SubmeshComponent >( component ) );
		return result;
	}

	inline void Submesh::setTopology( VkPrimitiveTopology value )
	{
		m_topology = value;
	}

	inline SkeletonRPtr Submesh::getSkeleton()const noexcept
	{
		return getParent().getSkeleton();
	}

	inline MaterialObs Submesh::getDefaultMaterial()const noexcept
	{
		return m_defaultMaterial;
	}

	inline castor::BoundingBox const & Submesh::getBoundingBox()const noexcept
	{
		return m_box;
	}

	inline castor::BoundingBox & Submesh::getBoundingBox()noexcept
	{
		return m_box;
	}

	inline castor::BoundingSphere const & Submesh::getBoundingSphere()const noexcept
	{
		return m_sphere;
	}

	inline castor::BoundingSphere & Submesh::getBoundingSphere()noexcept
	{
		return m_sphere;
	}

	inline bool Submesh::isInitialised()const noexcept
	{
		return m_initialised;
	}

	inline Mesh const & Submesh::getParent()const noexcept
	{
		return *getOwner();
	}

	inline Mesh & Submesh::getParent()noexcept
	{
		return *getOwner();
	}

	inline uint32_t Submesh::getId()const noexcept
	{
		return m_id;
	}

	inline bool Submesh::hasComponent( castor::String const & name )const noexcept
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( SubmeshComponentIDMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );
		return it != m_components.end();
	}

	inline SubmeshComponentRPtr Submesh::getComponent( castor::String const & name )const noexcept
	{
		SubmeshComponentRPtr result{};
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( SubmeshComponentIDMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );

		if ( it != m_components.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	inline InstantiationComponent & Submesh::getInstantiation()noexcept
	{
		CU_Require( m_instantiation );
		return *m_instantiation;
	}

	inline InstantiationComponent const & Submesh::getInstantiation()const noexcept
	{
		CU_Require( m_instantiation );
		return *m_instantiation;
	}

	inline SubmeshComponentIDMap const & Submesh::getComponents()const noexcept
	{
		return m_components;
	}

	inline VkPrimitiveTopology Submesh::getTopology()const noexcept
	{
		return m_topology;
	}

	inline SubmeshComponentCombine Submesh::getComponentCombine()const noexcept
	{
		CU_Require( m_componentCombine.baseId != 0 );
		return m_componentCombine;
	}

	inline SubmeshComponentPlugin const & Submesh::getComponentPlugin( castor::String const & componentType )const
	{
		return getComponentPlugin( getComponentId( componentType ) );
	}

	template< typename ComponentT >
	inline ComponentT* Submesh::getComponent()const noexcept
	{
		return &static_cast< ComponentT & >( *getComponent( ComponentT::TypeName ) );
	}

	template< typename ComponentT >
	SubmeshComponentPlugin const & Submesh::getComponentPlugin()const
	{
		return getComponentPlugin( getComponentId( ComponentT::TypeName ) );
	}

	template< typename ComponentT >
	inline bool Submesh::hasComponent()const
	{
		return this->hasComponent( ComponentT::TypeName );
	}
}
