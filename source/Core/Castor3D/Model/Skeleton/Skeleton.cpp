#include "Castor3D/Model/Skeleton/Skeleton.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"
#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, SkeletonCache )
CU_ImplementSmartPtr( c3d, Skeleton )

namespace c3d
{
	namespace skel
	{
		static CU_ImplementAttributeParserBlock( parserSkeletonImport, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				Path path;
				Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					auto importParams = params[1]->get< String >();
					fillMeshImportParameters( context, importParams, parameters );
				}

				if ( !SkeletonImporter::importData( *blockContext->skeleton
					, pathFile
					, parameters ) )
				{
					CU_ParsingError( cuT( "Skeleton Import failed" ) );
					blockContext->skeleton = nullptr;
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserArmature, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				blockContext->inverseTransform = Matrix4x4f::getIdentity();
				blockContext->nodeParentName = {};
				blockContext->nodeName = {};
				blockContext->nodeType = {};
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eSkeletonArmature, blockContext )

		static CU_ImplementAttributeParserBlock( parserArmatureNode, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				params[0]->get( blockContext->nodeName );
				blockContext->nodeType = SkeletonNodeType::eNode;
				blockContext->nodeParentName = {};
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eSkeletonNode, blockContext )

		static CU_ImplementAttributeParserBlock( parserArmatureBone, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				params[0]->get( blockContext->nodeName );
				blockContext->nodeType = SkeletonNodeType::eBone;
				blockContext->inverseTransform = Matrix4x4f::getIdentity();
				blockContext->nodeParentName = {};
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eSkeletonBone, blockContext )

		static CU_ImplementAttributeParserBlock( parserArmatureNodeParent, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				params[0]->get( blockContext->nodeParentName );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserArmatureBoneTransform, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else if ( params.size() < 4u )
				CU_ParsingError( cuT( "Not enough parameters for a Matrix4x4." ) );
			else
			{
				params[0]->get( blockContext->inverseTransform[0] );
				params[1]->get( blockContext->inverseTransform[1] );
				params[2]->get( blockContext->inverseTransform[2] );
				params[3]->get( blockContext->inverseTransform[3] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserArmatureNodeEnd, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				auto node = ( blockContext->nodeType == SkeletonNodeType::eBone )
					? blockContext->skeleton->createBone( blockContext->nodeName, blockContext->inverseTransform )
					: blockContext->skeleton->createNode( blockContext->nodeName );

				if ( auto parent = blockContext->nodeParentName.empty()
					? nullptr : blockContext->skeleton->findNode( blockContext->nodeParentName ) )
					blockContext->skeleton->setNodeParent( *node, *parent );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserArmatureEnd, SkeletonContext )
		{
			// Nothing to do
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSkeletonAnimImport, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				Path path;
				Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( auto prefix = getPrefix( *blockContext );
					!prefix.empty() )
				{
					if ( prefix[prefix.size() - 1u] == cuT( '-' ) )
					{
						prefix = prefix.substr( 0u, prefix.size() - 1u );
					}
					parameters.add( cuT( "prefix" ), prefix );
				}

				if ( params.size() > 1 )
				{
					auto importParams = params[1]->get< String >();
					fillMeshImportParameters( context, importParams, parameters );
				}

				auto const & engine = *getEngine( *blockContext );
				auto extension = string::lowerCase( path.getExtension() );

				if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
				{
					CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					String preferredImporter = cuT( "any" );
					parameters.get( cuT( "preferred_importer" ), preferredImporter );
					auto file = engine.getImporterFileFactory().create( extension
						, preferredImporter
						, *blockContext->scene->scene
						, pathFile
						, parameters );

					if ( auto importer = file->createAnimationImporter() )
					{
						for ( auto animName : file->listSkeletonAnimations( *blockContext->skeleton ) )
						{
							auto animation = makeUnique< SkeletonAnimation >( *blockContext->skeleton
								, animName );

							if ( !importer->importData( *animation
								, file.get()
								, parameters ) )
							{
								CU_ParsingError( cuT( "Skeleton animation Import failed" ) );
							}
							else
							{
								blockContext->skeleton->addAnimation( ptrRefCast< Animation >( animation ) );
							}
						}
					}
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkeletonSingleAnimImport, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				auto animRename = params[0]->get< String >();
				auto path = params[1]->get< Path >();
				Path pathFile = context.file.getPath() / path;
				Parameters parameters;

				if ( auto prefix = getPrefix( *blockContext );
					!prefix.empty() )
				{
					if ( prefix[prefix.size() - 1u] == cuT( '-' ) )
					{
						prefix = prefix.substr( 0u, prefix.size() - 1u );
					}
					parameters.add( cuT( "prefix" ), prefix );
				}

				if ( params.size() > 2 )
				{
					auto importParams = params[2]->get< String >();
					fillMeshImportParameters( context, importParams, parameters );
				}

				auto const & engine = *getEngine( *blockContext );
				auto extension = string::lowerCase( path.getExtension() );

				if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
				{
					CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					String preferredImporter = cuT( "any" );
					parameters.get( cuT( "preferred_importer" ), preferredImporter );
					auto file = engine.getImporterFileFactory().create( extension
						, preferredImporter
						, *blockContext->scene->scene
						, pathFile
						, parameters );

					if ( auto importer = file->createAnimationImporter() )
					{
						auto animations = file->listSkeletonAnimations( *blockContext->skeleton );

						if ( animations.empty() )
						{
							CU_ParsingError( cuT( "File [" ) + path + cuT( "] contains no skeleton animation for [" ) + blockContext->skeleton->getName() + cuT( "]." ) );
						}
						else if ( animations.size() > 1 )
						{
							CU_ParsingError( cuT( "File [" ) + path + cuT( "] contains too many skeleton animations, use `import_anim`." ) );
						}
						else
						{
							auto animation = makeUnique< SkeletonAnimation >( *blockContext->skeleton
								, *animations.begin() );

							if ( !importer->importData( *animation
								, file.get()
								, parameters ) )
							{
								CU_ParsingError( cuT( "Skeleton animation Import failed" ) );
							}
							else
							{
								animation->rename( animRename );
								blockContext->skeleton->addAnimation( ptrRefCast< Animation >( animation ) );
							}
						}
					}
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkeletonEnd, SkeletonContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No Scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				log::info << "Loaded skeleton [" << blockContext->skeleton->getName() << "]" << std::endl;
				blockContext->skeleton = nullptr;
			}
		}
		CU_EndAttributePop()
	}

	const String PtrCacheTraitsT< Skeleton, String >::Name = cuT( "Skeleton" );

	Skeleton::Skeleton( String name
		, Scene & scene )
		: Named{ c3d::move( name ) }
		, Animable{ *scene.getEngine() }
		, m_scene{ &scene }
	{
	}

	Skeleton::~Skeleton()noexcept
	{
		Animable::cleanupAnimations();
	}

	SkeletonNode * Skeleton::createNode( String name )
	{
		auto node = makeUnique< SkeletonNode >( c3d::move( name ), *this );
		auto result = node.get();
		m_nodes.emplace_back( c3d::move( node ) );
		return result;
	}

	BoneNode * Skeleton::createBone( String name
		, Matrix4x4f const & offset )
	{
		auto node = makeUniqueDerived< SkeletonNode, BoneNode >( c3d::move( name ), *this, offset, uint32_t( m_bones.size() ) );
		auto result = &static_cast< BoneNode & >( *node );
		m_nodes.emplace_back( c3d::move( node ) );
		m_bones.emplace_back( result );
		return result;
	}

	SkeletonNode * Skeleton::findNode( String const & name )const
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
		auto nodeIt = std::find_if( m_nodes.begin()
			, m_nodes.end()
			, [&node]( SkeletonNodeUPtr const & lookup )
			{
				return lookup.get() == &node;
			} );

		if ( m_nodes.end() == nodeIt )
		{
			CU_Exception( "Skeleton::setBoneParent - Child bone is not in the Skeleton's nodes" );
		}

		auto parentIt = std::find_if( m_nodes.begin()
			, m_nodes.end()
			, [&parent]( SkeletonNodeUPtr const & lookup )
			{
				return lookup.get() == &parent;
			} );

		if ( m_nodes.end() == parentIt )
		{
			CU_Exception( "Skeleton::setBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		parent.addChild( node );
		node.setParent( parent );
		auto parentDist = std::distance( m_nodes.begin(), parentIt );
		auto nodeDist = std::distance( m_nodes.begin(), nodeIt );

		if ( parentDist > nodeDist )
		{
			auto parentNode = c3d::move( *parentIt );
			m_nodes.erase( parentIt );
			m_nodes.emplace( std::next( m_nodes.begin(), nodeDist ), c3d::move( parentNode ) );
		}
	}

	SkeletonAnimation & Skeleton::createAnimation( String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			addAnimation( makeUniqueDerived< Animation, SkeletonAnimation >( *this, name ) );
		}

		return doGetAnimation< SkeletonAnimation >( name );
	}

	void Skeleton::removeAnimation( String const & name )
	{
		if ( hasAnimation( name ) )
		{
			doRemoveAnimation( name );
		}
	}

	void Skeleton::computeContainers( Mesh & mesh )
	{
		auto [it, res] = m_boxes.try_emplace( &mesh );

		if ( res )
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

	void Skeleton::cloneInto( Skeleton & output )const
	{
		output.m_globalInverse = m_globalInverse;

		// First clone the nodes
		for ( auto & node : m_nodes )
		{
			auto & clone = *output.m_nodes.emplace_back( node->clone( output ) ).get();

			if ( clone.getType() == SkeletonNodeType::eBone )
			{
				output.m_bones.push_back( &static_cast< BoneNode & >( clone ) );
			}
		}

		// Then clone the hierarchy
		for ( auto & child : m_nodes )
		{
			if ( auto parent = child->getParent() )
			{
				auto cloneChild = output.findNode( child->getName() );
				auto cloneParent = output.findNode( parent->getName() );

				if ( cloneChild && cloneParent )
				{
					output.setNodeParent( *cloneChild, *cloneParent );
				}
			}
		}
	}

	void Skeleton::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SkeletonContext > skeletonContext{ result, CSCNSection::eSkeleton, CSCNSection::eScene };
		BlockParserContextT< SkeletonContext > armatureContext{ result, CSCNSection::eSkeletonArmature, CSCNSection::eSkeleton };
		BlockParserContextT< SkeletonContext > nodeContext{ result, CSCNSection::eSkeletonNode, CSCNSection::eSkeleton };
		BlockParserContextT< SkeletonContext > boneContext{ result, CSCNSection::eSkeletonBone, CSCNSection::eSkeleton };

		skeletonContext.addParser( cuT( "import" ), skel::parserSkeletonImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		skeletonContext.addParser( cuT( "import_anim" ), skel::parserSkeletonAnimImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		skeletonContext.addParser( cuT( "import_single_anim" ), skel::parserSkeletonSingleAnimImport, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		skeletonContext.addPushParser( cuT( "armature" ), CSCNSection::eSkeletonArmature, skel::parserArmature );
		skeletonContext.addPopParser( cuT( "}" ), skel::parserSkeletonEnd );

		armatureContext.addPushParser( cuT( "node" ), CSCNSection::eSkeletonNode, skel::parserArmatureNode, { makeParameter< ParameterType::eName >() } );
		armatureContext.addPushParser( cuT( "bone" ), CSCNSection::eSkeletonBone, skel::parserArmatureBone, { makeParameter< ParameterType::eName >() } );
		armatureContext.addPopParser( cuT( "}" ), skel::parserArmatureEnd );

		nodeContext.addParser( cuT( "parent" ), skel::parserArmatureNodeParent, { makeParameter< ParameterType::eName >() } );
		nodeContext.addPopParser( cuT( "}" ), skel::parserArmatureNodeEnd );

		boneContext.addParser( cuT( "parent" ), skel::parserArmatureNodeParent, { makeParameter< ParameterType::eName >() } );
		boneContext.addParser( cuT( "inverse_transform" ), skel::parserArmatureBoneTransform, { makeParameter< ParameterType::ePoint4F >(), makeParameter< ParameterType::ePoint4F >(), makeParameter< ParameterType::ePoint4F >(), makeParameter< ParameterType::ePoint4F >() } );
		boneContext.addPopParser( cuT( "}" ), skel::parserArmatureNodeEnd );

		SkeletonAnimation::addParsers( result );
	}

	String getPrefix( SkeletonContext const & context )
	{
		return getPrefix( *context.scene );
	}

	Engine * getEngine( SkeletonContext const & context )
	{
		return getEngine( *context.scene );
	}
}
