#include "SubmeshComponent/BonesComponent.hpp"
#include "SubmeshComponent/BonesInstantiationComponent.hpp"
#include "SubmeshComponent/InstantiationComponent.hpp"
#include "SubmeshComponent/TriFaceMapping.hpp"

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	inline void SubmeshComponentAdder< T >::add( std::shared_ptr< T > component
		, Submesh & submesh )
	{
		submesh.m_components.emplace( T::Name, component );
	}

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< BonesInstantiationComponent >
	{
		static inline void add( std::shared_ptr< BonesInstantiationComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( BonesInstantiationComponent::Name, component );

			if ( submesh.m_instantiatedBones != component )
			{
				submesh.m_instantiatedBones = component;
			}
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< InstantiationComponent >
	{
		static inline void add( std::shared_ptr< InstantiationComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( InstantiationComponent::Name, component );

			if ( submesh.m_instantiation != component )
			{
				submesh.m_instantiation = component;
			}
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< BonesComponent >
	{
		static inline void add( std::shared_ptr< BonesComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( BonesComponent::Name, component );

			if ( !submesh.m_instantiatedBones )
			{
				submesh.addComponent( std::make_shared< BonesInstantiationComponent >( submesh
					, submesh.getInstantiation()
					, *component ) );
			}
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< IndexMapping >
	{
		static inline void add( std::shared_ptr< IndexMapping > component
			, Submesh & submesh )
		{
			CASTOR_EXCEPTION( "Use setIndexMapping, to define the index mapping for a Submesh" );
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< TriFaceMapping >
	{
		static inline void add( std::shared_ptr< TriFaceMapping > component
			, Submesh & submesh )
		{
			CASTOR_EXCEPTION( "Use setIndexMapping, to define the index mapping for a Submesh" );
		}
	};

	//*********************************************************************************************

	inline void Submesh::addPoints( std::vector< InterleavedVertex > const & p_vertices )
	{
		addPoints( p_vertices.data(), p_vertices.data() + p_vertices.size() );
	}

	template< size_t Count >
	inline void Submesh::addPoints( std::array< InterleavedVertex, Count > const & p_vertices )
	{
		addPoints( p_vertices.data(), p_vertices.data() + p_vertices.size() );
	}

	inline SkeletonSPtr Submesh::getSkeleton()const
	{
		return getParent().getSkeleton();
	}

	inline void Submesh::setDefaultMaterial( MaterialSPtr p_mat )
	{
		m_defaultMaterial = p_mat;
		setMaterial( nullptr, p_mat, false );
	}

	inline BufferElementGroupSPtr Submesh::operator[]( uint32_t p_index )const
	{
		REQUIRE( p_index < m_points.size() );
		return m_points[p_index];
	}

	inline BufferElementGroupSPtr Submesh::getPoint( uint32_t p_index )const
	{
		REQUIRE( p_index < m_points.size() );
		return m_points[p_index];
	}

	inline MaterialSPtr Submesh::getDefaultMaterial()const
	{
		return m_defaultMaterial.lock();
	}

	inline castor::CubeBox const & Submesh::getCollisionBox()const
	{
		return m_box;
	}

	inline castor::CubeBox & Submesh::getCollisionBox()
	{
		return m_box;
	}

	inline castor::SphereBox const & Submesh::getCollisionSphere()const
	{
		return m_sphere;
	}

	inline castor::SphereBox & Submesh::getCollisionSphere()
	{
		return m_sphere;
	}

	inline VertexPtrArray const & Submesh::getPoints()const
	{
		return m_points;
	}

	inline VertexPtrArray & Submesh::getPoints()
	{
		return m_points;
	}

	inline VertexBuffer const & Submesh::getVertexBuffer()const
	{
		return m_vertexBuffer;
	}

	inline VertexBuffer & Submesh::getVertexBuffer()
	{
		return m_vertexBuffer;
	}

	inline IndexBuffer const & Submesh::getIndexBuffer()const
	{
		return m_indexBuffer;
	}

	inline IndexBuffer & Submesh::getIndexBuffer()
	{
		return m_indexBuffer;
	}

	inline bool Submesh::isInitialised()const
	{
		return m_initialised;
	}

	inline Mesh const & Submesh::getParent()const
	{
		return m_parentMesh;
	}

	inline Mesh & Submesh::getParent()
	{
		return m_parentMesh;
	}

	inline uint32_t Submesh::getId()const
	{
		return m_id;
	}

	inline void Submesh::needsUpdate()
	{
		m_dirty = true;
	}

	inline void Submesh::setIndexMapping( IndexMappingSPtr mapping )
	{
		if ( m_indexMapping
			&& m_indexMapping != mapping )
		{
			m_components.erase( m_indexMapping->getType() );
		}

		m_indexMapping = mapping;
		m_components.emplace( mapping->getType(), mapping );
	}

	inline bool Submesh::hasComponent( castor::String const & name )
	{
		return m_components.find( name ) != m_components.end();
	}

	inline void Submesh::addComponent( castor::String const & name
		, SubmeshComponentSPtr component )
	{
		m_components.emplace( name, component );
	}

	template< typename T >
	inline void Submesh::addComponent( std::shared_ptr< T > component )
	{
		SubmeshComponentAdder< T >::add( component, *this );
	}

	inline SubmeshComponentSPtr Submesh::getComponent( castor::String const & name )
	{
		SubmeshComponentSPtr result;
		auto it = m_components.find( name );

		if ( it != m_components.end() )
		{
			result = it->second;
		}

		return result;
	}

	template< typename T >
	inline std::shared_ptr< T > Submesh::getComponent()
	{
		return std::static_pointer_cast< T >( getComponent( T::Name ) );
	}

	inline InstantiationComponent & Submesh::getInstantiation()
	{
		REQUIRE( m_instantiation );
		return *m_instantiation;
	}

	inline InstantiationComponent const & Submesh::getInstantiation()const
	{
		REQUIRE( m_instantiation );
		return *m_instantiation;
	}

	inline BonesInstantiationComponent & Submesh::getInstantiatedBones()
	{
		REQUIRE( m_instantiatedBones );
		return *m_instantiatedBones;
	}

	inline BonesInstantiationComponent const & Submesh::getInstantiatedBones()const
	{
		REQUIRE( m_instantiatedBones );
		return *m_instantiatedBones;
	}

	inline SubmeshComponentStrMap const & Submesh::getComponents()const
	{
		return m_components;
	}

	inline Topology Submesh::getTopology()const
	{
		return m_topology;
	}

	inline void Submesh::setTopology( Topology p_value )
	{
		m_topology = p_value;
	}

	//*********************************************************************************************
}
