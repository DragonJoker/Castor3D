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
			CU_Exception( "Use setIndexMapping, to define the index mapping for a Submesh" );
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< TriFaceMapping >
	{
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

	inline void Submesh::setDefaultMaterial( MaterialSPtr mat )
	{
		m_defaultMaterial = mat;
		setMaterial( nullptr, mat, false );
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

	inline MaterialSPtr Submesh::getDefaultMaterial()const
	{
		return m_defaultMaterial.lock();
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

	inline ashes::VertexBuffer< InterleavedVertex > const & Submesh::getVertexBuffer()const
	{
		return *m_vertexBuffer;
	}

	inline ashes::VertexBuffer< InterleavedVertex > & Submesh::getVertexBuffer()
	{
		return *m_vertexBuffer;
	}

	inline ashes::VertexLayout const & Submesh::getVertexLayout()const
	{
		return *m_vertexLayout;
	}

	inline ashes::Buffer< uint32_t > const & Submesh::getIndexBuffer()const
	{
		return *m_indexBuffer;
	}

	inline ashes::Buffer< uint32_t > & Submesh::getIndexBuffer()
	{
		return *m_indexBuffer;
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

	inline IndexMappingSPtr Submesh::getIndexMapping()const
	{
		return m_indexMapping;
	}

	inline bool Submesh::hasComponent( castor::String const & name )const
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

	inline SubmeshComponentSPtr Submesh::getComponent( castor::String const & name )const
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

	inline BonesInstantiationComponent & Submesh::getInstantiatedBones()
	{
		CU_Require( m_instantiatedBones );
		return *m_instantiatedBones;
	}

	inline BonesInstantiationComponent const & Submesh::getInstantiatedBones()const
	{
		CU_Require( m_instantiatedBones );
		return *m_instantiatedBones;
	}

	inline SubmeshComponentStrMap const & Submesh::getComponents()const
	{
		return m_components;
	}

	inline ashes::PrimitiveTopology Submesh::getTopology()const
	{
		return m_topology;
	}

	inline void Submesh::setTopology( ashes::PrimitiveTopology value )
	{
		m_topology = value;
	}

	//*********************************************************************************************
}
