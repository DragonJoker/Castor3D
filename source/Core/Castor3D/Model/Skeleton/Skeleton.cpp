#include "Castor3D/Model/Skeleton/Skeleton.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	Skeleton::Skeleton( Scene & scene )
		: Animable{ *scene.getEngine() }
		, m_scene{ &scene }
		, m_globalInverse{ 1 }
	{
	}

	Skeleton::~Skeleton()
	{
		Animable::cleanupAnimations();
	}

	SkeletonNode * Skeleton::createNode( castor::String name )
	{
		auto node = std::make_unique< SkeletonNode >( std::move( name ), *this );
		auto result = node.get();
		m_nodes.emplace_back( std::move( node ) );
		return result;
	}

	BoneNode * Skeleton::createBone( castor::String name
		, castor::Matrix4x4f const & offset )
	{
		auto node = std::make_unique< BoneNode >( std::move( name ), *this, offset, uint32_t( m_bones.size() ) );
		auto result = node.get();
		m_nodes.emplace_back( std::move( node ) );
		m_bones.emplace_back( result );
		return result;
	}

	SkeletonNode * Skeleton::findNode( castor::String const & name )const
	{
		auto it = std::find_if( m_nodes.begin()
			, m_nodes.end()
			, [&name]( SkeletonNodeUPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		SkeletonNode * result{};

		if ( it != m_nodes.end() )
		{
			result = it->get();
		}

		return result;
	}

	SkeletonNode * Skeleton::getRootNode()const
	{
		auto it = std::find_if( m_nodes.begin()
			, m_nodes.end()
			, []( SkeletonNodeUPtr const & lookup )
			{
				return lookup->getParent() == nullptr;
			} );
		return it == m_nodes.end()
			? nullptr
			: it->get();
	}

	void Skeleton::setNodeParent( SkeletonNode & node, SkeletonNode & parent )
	{
		if ( m_nodes.end() == std::find_if( m_nodes.begin()
			, m_nodes.end()
			, [&node]( SkeletonNodeUPtr const & lookup )
			{
				return lookup.get() == &node;
			} ) )
		{
			CU_Exception( "Skeleton::setBoneParent - Child bone is not in the Skeleton's nodes" );
		}

			if ( m_nodes.end() == std::find_if( m_nodes.begin()
				, m_nodes.end()
				, [&parent]( SkeletonNodeUPtr const & lookup )
				{
					return lookup.get() == &parent;
				} ) )
		{
			CU_Exception( "Skeleton::setBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		parent.addChild( node );
		node.setParent( parent );
	}

	SkeletonAnimation & Skeleton::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			addAnimation( std::make_unique< SkeletonAnimation >( *this, name ) );
		}

		return doGetAnimation< SkeletonAnimation >( name );
	}

	void Skeleton::removeAnimation( castor::String const & name )
	{
		if ( hasAnimation( name ) )
		{
			doRemoveAnimation( name );
		}
	}

	void Skeleton::computeContainers( Mesh & mesh )
	{
		auto it = m_boxes.emplace( &mesh, std::vector< castor::BoundingBox >{} ).first;

		if ( it == m_boxes.end() )
		{
			auto & boxes = it->second;
			boxes.reserve( m_bones.size() );

			for ( auto & bone : m_bones )
			{
				boxes.emplace_back( bone->computeBoundingBox( mesh
					, bone->getId() ) );
			}
		}
	}
}
