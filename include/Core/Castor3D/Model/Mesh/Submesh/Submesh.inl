#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	inline void SubmeshComponentAdder< T >::add( std::shared_ptr< T > component
		, Submesh & submesh )
	{
		submesh.m_components.emplace( component->getID(), component );
	}

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< InstantiationComponent >
	{
		static inline void add( std::shared_ptr< InstantiationComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( component->getID(), component );

			if ( submesh.m_instantiation != component )
			{
				submesh.m_instantiation = component;
			}
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< IndexMapping >
	{
		[[noreturn]]
		static inline void add( std::shared_ptr< IndexMapping > component
			, Submesh & submesh )
		{
			CU_Exception( "Use setIndexMapping, to define the index mapping for a Submesh" );
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< TriFaceMapping >
	{
		[[noreturn]]
		static inline void add( std::shared_ptr< TriFaceMapping > component
			, Submesh & submesh )
		{
			CU_Exception( "Use setIndexMapping, to define the index mapping for a Submesh" );
		}
	};

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

	inline SkeletonSPtr Submesh::getSkeleton()const
	{
		return getParent().getSkeleton();
	}

	inline void Submesh::setDefaultMaterial( MaterialRPtr mat )
	{
		m_defaultMaterial = mat;
		setMaterial( {}, mat, false );
	}

	inline InterleavedVertex const & Submesh::operator[]( uint32_t index )const
	{
		CU_Require( index < m_points.size() );
		return m_points[index];
	}

	inline InterleavedVertex & Submesh::operator[]( uint32_t index )
	{
		CU_Require( index < m_points.size() );
		return m_points[index];
	}

	inline InterleavedVertex const & Submesh::getPoint( uint32_t index )const
	{
		CU_Require( index < m_points.size() );
		return m_points[index];
	}

	inline InterleavedVertex & Submesh::getPoint( uint32_t index )
	{
		CU_Require( index < m_points.size() );
		return m_points[index];
	}

	inline MaterialRPtr Submesh::getDefaultMaterial()const
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

	inline InterleavedVertexArray const & Submesh::getPoints()const
	{
		return m_points;
	}

	inline InterleavedVertexArray & Submesh::getPoints()
	{
		return m_points;
	}

	inline bool Submesh::hasBufferOffsets()const
	{
		return bool( m_bufferOffset );
	}

	inline ObjectBufferOffset const & Submesh::getBufferOffsets()const
	{
		CU_Require( hasBufferOffsets() );
		return m_bufferOffset;
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

	inline void Submesh::needsUpdate()
	{
		m_dynamic = true;
		m_dirty = true;
	}

	inline void Submesh::setIndexMapping( IndexMappingSPtr mapping )
	{
		if ( m_indexMapping
			&& m_indexMapping != mapping )
		{
			m_components.erase( m_indexMapping->getID() );
		}

		m_indexMapping = mapping;
		m_components.emplace( mapping->getID(), mapping );
	}

	inline IndexMappingSPtr Submesh::getIndexMapping()const
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

	inline void Submesh::addComponent( castor::String const & name
		, SubmeshComponentSPtr component )
	{
		m_components.emplace( component->getID(), component);
	}

	template< typename T >
	inline void Submesh::addComponent( std::shared_ptr< T > component )
	{
		SubmeshComponentAdder< T >::add( component, *this );
	}

	inline SubmeshComponentSPtr Submesh::getComponent( castor::String const & name )const
	{
		SubmeshComponentSPtr result;
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( SubmeshComponentIDMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );

		if ( it != m_components.end() )
		{
			result = it->second;
		}

		return result;
	}

	template< typename T >
	inline std::shared_ptr< T > Submesh::getComponent()const
	{
		return std::static_pointer_cast< T >( getComponent( T::Name ) );
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

	inline void Submesh::setTopology( VkPrimitiveTopology value )
	{
		m_topology = value;
	}

	inline void Submesh::disableSceneUpdate()
	{
		m_disableSceneUpdate = true;
	}

	//*********************************************************************************************
}
