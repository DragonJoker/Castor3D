#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	inline void SubmeshComponentAdder< T >::add( castor::UniquePtr< T > component
		, Submesh & submesh )
	{
		if constexpr ( std::is_base_of_v< IndexMapping, T > )
		{
			submesh.setIndexMapping( castor::ptrRefCast< IndexMapping >( component ) );
		}
		else
		{
			auto id = component->getID();
			auto comp = component.get();
			submesh.m_components.emplace( id
				, castor::ptrRefCast< SubmeshComponent >( component ) );

			if constexpr ( std::is_same_v< InstantiationComponent, T > )
			{
				if ( submesh.m_instantiation != comp )
				{
					submesh.m_instantiation = comp;
				}
			}
		}
	}

	//*********************************************************************************************

	inline void Submesh::addPoints( std::vector< InterleavedVertex > const & vertices )
	{
		addPoints( vertices.data(), vertices.data() + vertices.size() );
	}

	template< size_t Count >
	inline void Submesh::addPoints( std::array< InterleavedVertex, Count > const & vertices )
	{
		addPoints( vertices.data(), vertices.data() + vertices.size() );
	}

	inline SkeletonRPtr Submesh::getSkeleton()const
	{
		return getParent().getSkeleton();
	}

	inline void Submesh::setDefaultMaterial( MaterialObs mat )
	{
		m_defaultMaterial = mat;
		instantiate( nullptr, {}, mat, false );
	}

	inline MaterialObs Submesh::getDefaultMaterial()const
	{
		return m_defaultMaterial;
	}

	inline castor::BoundingBox const & Submesh::getBoundingBox()const
	{
		return m_box;
	}

	inline castor::BoundingBox & Submesh::getBoundingBox()
	{
		return m_box;
	}

	inline castor::BoundingSphere const & Submesh::getBoundingSphere()const
	{
		return m_sphere;
	}

	inline castor::BoundingSphere & Submesh::getBoundingSphere()
	{
		return m_sphere;
	}

	inline bool Submesh::isInitialised()const
	{
		return m_initialised;
	}

	inline Mesh const & Submesh::getParent()const
	{
		return *getOwner();
	}

	inline Mesh & Submesh::getParent()
	{
		return *getOwner();
	}

	inline uint32_t Submesh::getId()const
	{
		return m_id;
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
			m_components.erase( m_indexMapping->getID() );
		}

		m_indexMapping = mapping.get();
		auto id = mapping->getID();
		m_components.emplace( id, castor::ptrRefCast< SubmeshComponent >( mapping ) );
	}

	inline IndexMappingRPtr Submesh::getIndexMapping()const
	{
		return m_indexMapping;
	}

	inline bool Submesh::hasComponent( castor::String const & name )const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( SubmeshComponentIDMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );
		return it != m_components.end();
	}

	template< typename ComponentT, typename ... ParamsT >
	inline ComponentT * Submesh::createComponent( ParamsT && ... params )
	{
		auto component = castor::makeUnique< ComponentT >( *this
			, std::forward< ParamsT >( params )... );
		auto result = component.get();
		addComponent( std::move( component ) );
		return result;
	}

	inline void Submesh::addComponent( SubmeshComponentUPtr component )
	{
		auto id = component->getID();
		m_components.emplace( id, std::move( component ) );
	}

	template< typename T >
	inline void Submesh::addComponent( castor::UniquePtr< T > component )
	{
		SubmeshComponentAdder< T >::add( std::move( component ), *this );
	}

	inline SubmeshComponentRPtr Submesh::getComponent( castor::String const & name )const
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

	template< typename T >
	inline T * Submesh::getComponent()const
	{
		return &static_cast< T & >( *getComponent( T::Name ) );
	}

	inline InstantiationComponent & Submesh::getInstantiation()
	{
		CU_Require( m_instantiation );
		return *m_instantiation;
	}

	inline InstantiationComponent const & Submesh::getInstantiation()const
	{
		CU_Require( m_instantiation );
		return *m_instantiation;
	}

	inline SubmeshComponentIDMap const & Submesh::getComponents()const
	{
		return m_components;
	}

	inline VkPrimitiveTopology Submesh::getTopology()const
	{
		return m_topology;
	}

	inline SubmeshFlags Submesh::getFinalSubmeshFlags()const
	{
		auto result = m_submeshFlags;
		remFlag( result, SubmeshFlag::eIndex );

		if ( isDynamic() )
		{
			remFlag( result, SubmeshFlag::eSkin );
			addFlag( result, SubmeshFlag::eVelocity );
		}

		return result;
	}

	inline void Submesh::setTopology( VkPrimitiveTopology value )
	{
		m_topology = value;
	}

	//*********************************************************************************************
}
