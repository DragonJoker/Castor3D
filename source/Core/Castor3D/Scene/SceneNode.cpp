#include "Castor3D/Scene/SceneNode.hpp"

#include "Castor3D/Binary/BinarySceneNodeAnimation.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, SceneNode )

namespace castor3d
{
	namespace node
	{
		static CU_ImplementAttributeParserBlock( parserStatic, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [static] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->isStatic );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserParent, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [parent] parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				SceneNodeRPtr parent;

				if ( name == Scene::ObjectRootNode )
				{
					parent = blockContext->scene->scene->getObjectRootNode();
				}
				else if ( name == Scene::CameraRootNode )
				{
					parent = blockContext->scene->scene->getCameraRootNode();
				}
				else if ( name == Scene::RootNode )
				{
					parent = blockContext->scene->scene->getRootNode();
				}
				else
				{
					parent = blockContext->scene->scene->findSceneNode( name );
				}

				if ( parent )
				{
					blockContext->parentNode = parent;
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVisible, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [visible] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->isVisible );

				if ( blockContext->currentNode )
				{
					blockContext->currentNode->setVisible( blockContext->isVisible );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPosition, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [position] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->position );

				if ( blockContext->currentNode )
				{
					blockContext->currentNode->setPosition( blockContext->position );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOrientation, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [orientation] parameter." ) );
			}
			else
			{
				blockContext->orientation = castor::Quaternion::fromAxisAngle( params[0]->get< castor::Point3f >()
					, castor::Angle::fromDegrees( params[1]->get< float >() ) );

				if ( blockContext->currentNode )
				{
					blockContext->currentNode->setOrientation( blockContext->orientation );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRotate, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [orientation] parameter." ) );
			}
			else
			{
				blockContext->orientation *= castor::Quaternion::fromAxisAngle( params[0]->get< castor::Point3f >()
					, castor::Angle::fromDegrees( params[1]->get< float >() ) );

				if ( blockContext->currentNode )
				{
					blockContext->currentNode->setOrientation( blockContext->orientation );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDirection, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [direction] parameter." ) );
			}
			else
			{
				castor::Point3f direction;
				params[0]->get( direction );
				castor::Point3f up{ 0, 1, 0 };
				castor::Point3f right{ castor::point::cross( direction, up ) };
				blockContext->orientation = castor::Quaternion::fromAxes( right, up, direction );

				if ( blockContext->currentNode )
				{
					blockContext->currentNode->setOrientation( blockContext->orientation );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserScale, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [direction] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->scale );

				if ( blockContext->currentNode )
				{
					blockContext->currentNode->setScale( blockContext->scale );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, NodeContext )
		{
			if ( !blockContext->currentNode )
			{
				SceneNodeUPtr sceneNode = blockContext->scene->scene->createSceneNode( blockContext->name
					, *blockContext->scene->scene
					, blockContext->parentNode
					, blockContext->position
					, blockContext->orientation
					, blockContext->scale
					, blockContext->isStatic );
				sceneNode->setVisible( blockContext->isVisible );
				auto name = sceneNode->getName();
				auto node = blockContext->scene->scene->addSceneNode( name, sceneNode, true );
				sceneNode.reset();

				if ( !blockContext->isStatic )
				{
					for ( auto const & fileName : blockContext->scene->root->csnaFiles )
					{
						auto fName = fileName.getFileName();

						if ( auto pos = fName.find( name );
							pos == 0u && fName[name.size()] == '-' )
						{
							if ( auto animName = fName.substr( name.size() + 1u );
								!animName.empty() )
							{
								auto & animation = node->createAnimation( animName );
								BinaryParser< SceneNodeAnimation > parser;
								castor::BinaryFile animFile{ fileName, castor::File::OpenMode::eRead };
								parser.parse( animation, animFile );
							}
						}
					}
				}

				log::info << "Loaded scene node [" << name << "]" << std::endl;
			}
		}
		CU_EndAttributePop()
	}

	uint64_t SceneNode::Count = 0;
	uint64_t SceneNode::CurrentId = 0;

	SceneNode::SceneNode( castor::String const & name
		, SceneNodeCreateInfo const & createInfo )
		: SceneNode{ name
			, *createInfo.scene
			, createInfo.parentNode
			, createInfo.position
			, createInfo.orientation
			, createInfo.scale
			, createInfo.isStatic }
	{
	}

	SceneNode::SceneNode( castor::String const & name
		, Scene & scene
		, SceneNode * parent
		, castor::Point3f position
		, castor::Quaternion orientation
		, castor::Point3f scale
		, bool isStatic )
		: Animable{ *scene.getEngine() }
		, castor::Named{ name }
		, m_scene{ &scene }
		, m_static{ isStatic }
		, m_displayable{ name == Scene::RootNode }
		, m_orientation{ castor::move( orientation ) }
		, m_position{ castor::move( position ) }
		, m_scale{ castor::move( scale ) }
	{
		if ( m_name.empty() )
		{
			m_name = cuT( "SceneNode_" );
			m_name += castor::string::toString( Count );
		}

		if ( parent )
		{
			doAttachTo( *parent );
		}

		++CurrentId;
		++Count;
	}

	SceneNode::SceneNode( castor::String const & name
		, Scene & scene )
		: SceneNode{ name
			, scene
			, nullptr
			, castor::Point3f{}
			, castor::Quaternion::identity()
			, castor::Point3f{ 1.0f, 1.0f, 1.0f }
			, false }
	{
	}

	SceneNode::~SceneNode()
	{
		Count--;
		auto parent = getParent();

		if ( parent )
		{
			parent->doDetachChild( getName() );
		}

		doDetachChildren( true );
		cleanupAnimations();
	}

	void SceneNode::update()
	{
		doComputeMatrix();
	}

	void SceneNode::cloneInto( SceneNode & output )const
	{
		output.m_static = m_static;
		output.m_displayable = m_displayable;
		output.m_visible = m_visible;
		output.m_serialisable = m_serialisable;
		output.m_orientation = m_orientation;
		output.m_position = m_position;
		output.m_scale = m_scale;
		output.m_mtxSet = m_mtxSet;
		output.m_transform = m_transform;
		output.m_mtxChanged = m_mtxChanged;
		output.m_derivedTransform = m_derivedTransform;
		output.m_derivedMtxChanged = m_derivedMtxChanged;
	}

	void SceneNode::attachObject( MovableObject & object )
	{
		object.detach();
		m_objects.push_back( object );
		object.attachTo( *this );
	}

	void SceneNode::detachObject( MovableObject const & object )
	{
		auto it = std::find_if( m_objects.begin()
			, m_objects.end()
			, [&object]( castor::ReferenceWrapper< MovableObject > obj )
			{
				return &obj.get() == &object;
			} );

		if ( it != m_objects.end() )
		{
			m_objects.erase( it );
		}
	}

	void SceneNode::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< NodeContext > context{ result, CSCNSection::eNode, CSCNSection::eScene };
		context.addParser( cuT( "static" ), node::parserStatic, { makeParameter< ParameterType::eBool >() } );
		context.addParser( cuT( "visible" ), node::parserVisible, { makeParameter< ParameterType::eBool >() } );
		context.addParser( cuT( "parent" ), node::parserParent, { makeParameter< ParameterType::eName >() } );
		context.addParser( cuT( "position" ), node::parserPosition, { makeParameter< ParameterType::ePoint3F >() } );
		context.addParser( cuT( "orientation" ), node::parserOrientation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "rotate" ), node::parserRotate, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "direction" ), node::parserDirection, { makeParameter< ParameterType::ePoint3F >() } );
		context.addParser( cuT( "scale" ), node::parserScale, { makeParameter< ParameterType::ePoint3F >() } );
		context.addPopParser( cuT( "}" ), node::parserEnd );
	}

	void SceneNode::attachTo( SceneNode & node )
	{
		doAttachTo( node );
		onParentChanged( *this );
		markDirty();
	}

	void SceneNode::detach( bool cleanup )noexcept
	{
		if ( cleanup || !m_static )
		{
			doDetach();
		}
		else
		{
			CU_Require( cleanup || !m_static );
		}
	}

	bool SceneNode::hasChild( castor::String const & name )const
	{
		bool found = false;

		if ( m_children.find( name ) == m_children.end() )
		{
			found = m_children.end() != std::find_if( m_children.begin()
				, m_children.end()
				, [&name]( castor::Pair< castor::String, SceneNodeRPtr > const & pair )
				{
					return pair.second->hasChild( name );
				} );
		}

		return found;
	}

	void SceneNode::addChild( SceneNode & child )
	{
		doAddChild( child );
	}

	void SceneNode::detachChild( SceneNode const & child )noexcept
	{
		doDetachChild( child.getName() );
	}

	void SceneNode::detachChild( castor::String const & childName )noexcept
	{
		doDetachChild( childName );
	}

	void SceneNode::detachChildren( bool cleanup )noexcept
	{
		doDetachChildren( cleanup );
	}

	void SceneNode::yaw( castor::Angle const & angle )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			rotate( castor::Quaternion::fromAxisAngle( castor::Point3d( 0.0, 1.0, 0.0 ), angle ) );
		}
	}

	void SceneNode::pitch( castor::Angle const & angle )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			rotate( castor::Quaternion::fromAxisAngle( castor::Point3d( 1.0, 0.0, 0.0 ), angle ) );
		}
	}

	void SceneNode::roll( castor::Angle const & angle )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			rotate( castor::Quaternion::fromAxisAngle( castor::Point3d( 0.0, 0.0, 1.0 ), angle ) );
		}
	}

	void SceneNode::markDirty()
	{
		m_scene->markDirty( *this );
	}

	void SceneNode::rotate( castor::Quaternion const & orientation )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_orientation *= orientation;
			doUpdateChildsDerivedTransform();
			m_mtxChanged = true;
			m_mtxSet = false;
			markDirty();
		}
	}

	void SceneNode::translate( castor::Point3f const & position )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_position += position;
			doUpdateChildsDerivedTransform();
			m_mtxChanged = true;
			m_mtxSet = false;
			markDirty();
		}
	}

	void SceneNode::scale( castor::Point3f const & scale )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_scale *= scale;
			doUpdateChildsDerivedTransform();
			m_mtxChanged = true;
			m_mtxSet = false;
			markDirty();
		}
	}

	SceneNodeAnimation & SceneNode::createAnimation( castor::String const & name )
	{
		CU_Require( !m_static );

		if ( !hasAnimation( name ) )
		{
			addAnimation( castor::makeUniqueDerived< Animation, SceneNodeAnimation >( *this, name ) );
		}

		return doGetAnimation< SceneNodeAnimation >( name );
	}

	void SceneNode::removeAnimation( castor::String const & name )
	{
		CU_Require( !m_static );

		if ( hasAnimation( name ) )
		{
			doRemoveAnimation( name );
		}
	}

	void SceneNode::setOrientation( castor::Quaternion const & orientation )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_orientation = orientation;
			doUpdateChildsDerivedTransform();
			m_mtxChanged = true;
			m_mtxSet = false;
			markDirty();
		}
	}

	void SceneNode::setPosition( castor::Point3f const & position )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_position = position;
			doUpdateChildsDerivedTransform();
			m_mtxChanged = true;
			m_mtxSet = false;
			markDirty();
		}
	}

	void SceneNode::setScale( castor::Point3f const & scale )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_scale = scale;
			doUpdateChildsDerivedTransform();
			m_mtxChanged = true;
			m_mtxSet = false;
			markDirty();
		}
	}

	void SceneNode::setTransformationMatrix( castor::Matrix4x4f const & transform )
	{
		CU_Require( !m_static );

		if ( !m_static )
		{
			m_transform = transform;
			doUpdateChildsDerivedTransform();
			castor::matrix::decompose( m_transform
				, m_position
				, m_scale
				, m_orientation );
			m_mtxChanged = true;
			m_mtxSet = true;
			markDirty();
		}
	}

	castor::Point3f SceneNode::getDerivedPosition()const
	{
		castor::Point3f result( m_position );

		if ( auto parent = getParent() )
		{
			result = castor::matrix::getTransformed( parent->getDerivedTransformationMatrix(), m_position );
		}

		return result;
	}

	castor::Quaternion SceneNode::getDerivedOrientation()const
	{
		castor::Quaternion result( m_orientation );

		if ( auto parent = getParent() )
		{
			result  = result * parent->getDerivedOrientation();
		}

		return result;
	}

	castor::Point3f SceneNode::getDerivedScale()const
	{
		castor::Point3f result( m_scale );

		if ( auto parent = getParent() )
		{
			result *= parent->getDerivedScale();
		}

		return result;
	}

	castor::Matrix4x4f const & SceneNode::getTransformationMatrix()const
	{
		return m_transform;
	}

	castor::Matrix4x4f const & SceneNode::getDerivedTransformationMatrix()const
	{
		return m_derivedTransform;
	}

	void SceneNode::setVisible( bool visible )
	{
		if ( m_visible != visible )
		{
			m_visible = visible;
			getScene()->markDirty( *this );
		}
	}

	bool SceneNode::isVisible()const noexcept
	{
		auto parent = m_parent;
		return isDisplayable()
			&& m_visible
			&& ( parent ? parent->isVisible() : true );
	}

	SceneNode::SceneNodeMap const & SceneNode::getChildren()const
	{
		return m_children;
	}

	SceneNodeRPtr SceneNode::getChild( castor::String const & name )const
	{
		return ( m_children.find( name ) != m_children.end() ? m_children.find( name )->second : nullptr );
	}

	SceneNode::MovableArray const & SceneNode::getObjects()const
	{
		return m_objects;
	}

	void SceneNode::doComputeMatrix()
	{
		if ( m_mtxChanged )
		{
			m_derivedMtxChanged = true;

			if ( !m_mtxSet )
			{
				castor::matrix::setTransform( m_transform, m_position, m_scale, m_orientation );
			}

			m_mtxChanged = false;
		}

		if ( m_derivedMtxChanged )
		{
			if ( auto parent = getParent() )
			{
				parent->doComputeMatrix();
				m_derivedTransform = parent->getDerivedTransformationMatrix() * m_transform;
			}
			else
			{
				m_derivedTransform = m_transform;
			}

			m_derivedMtxChanged = false;
		}
	}

	void SceneNode::doUpdateChildsDerivedTransform()
	{
		for ( auto const & [key,current] : m_children )
		{
			if ( current )
			{
				current->doUpdateChildsDerivedTransform();
				current->m_derivedMtxChanged = true;
			}
		}
	}

	void SceneNode::doAttachTo( SceneNode & node )
	{
		auto old = getParent();

		if ( old != &node )
		{
			if ( old )
			{
				m_parent = nullptr;
				old->detachChild( *this );
			}

			m_parent = &node;

			if ( m_parent )
			{
				m_displayable = m_parent->m_displayable;
				m_parent->addChild( *this );
				m_mtxChanged = true;
			}
		}
	}

	void SceneNode::doDetach()noexcept
	{
		auto parent = getParent();

		if ( parent )
		{
			m_displayable = false;
			m_parent = nullptr;
			parent->detachChild( *this );
			m_mtxChanged = true;
			markDirty();
		}
	}

	void SceneNode::doAddChild( SceneNode & child )
	{
		auto name = child.getName();

		if ( m_children.find( name ) == m_children.end() )
		{
			m_children.insert( std::make_pair( name, &child ) );
		}
		else
		{
			log::warn << m_name << cuT( " - Can't add SceneNode " ) << name << cuT( " - Already in childs" ) << std::endl;
		}
	}

	void SceneNode::doDetachChild( castor::String const & childName )noexcept
	{
		auto it = m_children.find( childName );

		if ( it != m_children.end() )
		{
			auto current = it->second;
			m_children.erase( it );

			if ( current )
			{
				current->doDetach();
			}
		}
		else
		{
			log::warn << m_name << cuT( " - Can't remove SceneNode " ) << childName << cuT( " - Not in childs" ) << std::endl;
		}
	}

	void SceneNode::doDetachChildren( bool cleanup )noexcept
	{
		SceneNodeMap flush;
		castor::swap( flush, m_children );

		for ( auto const & [_, current] : flush )
		{
			if ( current )
			{
				current->detach( cleanup );
			}
		}
	}

	castor::String getPrefix( NodeContext const & context )
	{
		return getPrefix( *context.scene );
	}

	Engine * getEngine( NodeContext const & context )
	{
		return getEngine( *context.scene );
	}
}
