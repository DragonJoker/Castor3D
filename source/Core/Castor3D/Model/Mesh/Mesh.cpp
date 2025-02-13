#include "Castor3D/Model/Mesh/Mesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, MeshCache )
CU_ImplementSmartPtr( castor3d, Mesh )

namespace castor3d
{
	namespace mesh
	{
		static CU_ImplementAttributeParserBlock( parserMeshType, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				Parameters parameters;

				if ( params.size() > 1 )
				{
					parameters.parse( params[1]->get< castor::String >() );
				}

				auto const & factory = getEngine( *blockContext )->getMeshFactory();
				factory.create( params[0]->get< castor::String >() )->generate( *blockContext->mesh, parameters );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserMeshSubmesh, MeshContext, SubmeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				newBlockContext->mesh = blockContext;
				newBlockContext->submesh = blockContext->mesh->createSubmesh();
				newBlockContext->submesh->createComponent< DefaultRenderComponent >();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSubmesh )

		static CU_ImplementAttributeParserBlock( parserMeshImport, MeshContext )
		{
			if ( auto mesh = blockContext->mesh )
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					fillMeshImportParameters( context, params[1]->get< castor::String >(), parameters );
				}

				if ( !MeshImporter::importData( *mesh
					, pathFile
					, parameters
					, true ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
					blockContext->mesh = {};
				}
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshAnimImport, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					fillMeshImportParameters( context, params[1]->get< castor::String >(), parameters );
				}

				auto animation = castor::makeUnique< MeshAnimation >( *blockContext->mesh
					, pathFile.getFileName() );

				if ( !AnimationImporter::importData( *animation
					, pathFile
					, parameters ) )
				{
					CU_ParsingError( cuT( "Mesh animation Import failed" ) );
				}
				else
				{
					blockContext->mesh->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshSingleAnimImport, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				auto animName = params[0]->get< castor::String >();
				auto path = params[1]->get< castor::Path >();
				castor::Path pathFile = context.file.getPath() / path;
				Parameters parameters;

				if ( params.size() > 2 )
				{
					fillMeshImportParameters( context, params[2]->get< castor::String >(), parameters );
				}

				auto animation = castor::makeUnique< MeshAnimation >( *blockContext->mesh
					, animName );

				if ( !AnimationImporter::importData( *animation
					, pathFile
					, parameters ) )
				{
					CU_ParsingError( cuT( "Mesh animation Import failed" ) );
				}
				else
				{
					blockContext->mesh->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshMorphTargetImport, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No mesh initialised." ) );
			}
			else
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					fillMeshImportParameters( context, params[1]->get< castor::String >(), parameters );
				}

				Mesh mesh{ cuT( "MorphImport" ), *blockContext->mesh->getScene() };

				if ( !MeshImporter::importData( mesh
					, pathFile
					, parameters
					, false ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
				}
				else if ( mesh.getSubmeshCount() == blockContext->mesh->getSubmeshCount() )
				{
					for ( auto & morphSubmesh : mesh )
					{
						auto id = morphSubmesh->getId();
						auto submesh = blockContext->mesh->getSubmesh( id );
						auto component = submesh->hasComponent( MorphComponent::TypeName )
							? submesh->getComponent< MorphComponent >()
							: submesh->createComponent< MorphComponent >();
						castor3d::SubmeshAnimationBuffer buffer;
					
						if ( morphSubmesh->hasComponent( PositionsComponent::TypeName ) )
						{
							buffer.positions = morphSubmesh->getPositions();
							uint32_t index = 0u;

							for ( auto & position : buffer.positions )
							{
								position -= submesh->getPositions()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( NormalsComponent::TypeName ) )
						{
							buffer.normals = morphSubmesh->getNormals();
							uint32_t index = 0u;

							for ( auto & normal : buffer.normals )
							{
								normal -= submesh->getNormals()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( TangentsComponent::TypeName ) )
						{
							buffer.tangents = morphSubmesh->getTangents();
							uint32_t index = 0u;

							for ( auto & tangent : buffer.tangents )
							{
								tangent -= submesh->getTangents()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( BitangentsComponent::TypeName ) )
						{
							buffer.bitangents = morphSubmesh->getBitangents();
							uint32_t index = 0u;

							for ( auto & bitangent : buffer.bitangents )
							{
								bitangent -= submesh->getBitangents()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords0Component::TypeName ) )
						{
							buffer.texcoords0 = morphSubmesh->getTexcoords0();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords0 )
							{
								texcoord -= submesh->getTexcoords0()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords1Component::TypeName ) )
						{
							buffer.texcoords1 = morphSubmesh->getTexcoords1();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords1 )
							{
								texcoord -= submesh->getTexcoords1()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords2Component::TypeName ) )
						{
							buffer.texcoords2 = morphSubmesh->getTexcoords2();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords2 )
							{
								texcoord -= submesh->getTexcoords2()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords3Component::TypeName ) )
						{
							buffer.texcoords3 = morphSubmesh->getTexcoords3();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords3 )
							{
								texcoord -= submesh->getTexcoords3()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( ColoursComponent::TypeName ) )
						{
							buffer.colours = morphSubmesh->getColours();
							uint32_t index = 0u;

							for ( auto & colour : buffer.colours )
							{
								colour -= submesh->getColours()[index++];
							}
						}

						component->getData().addMorphTarget( castor::move( buffer ) );
					}

					mesh.cleanup();
				}
				else
				{
					CU_ParsingError( cuT( "The new mesh doesn't match the original mesh" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterial, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );

				if ( auto material = getEngine( *blockContext )->findMaterial( name ) )
				{
					for ( auto const & submesh : *blockContext->mesh )
					{
						submesh->setDefaultMaterial( material );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterials, MeshContext )
		{
			// Only push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eMeshDefaultMaterials, blockContext )

		static CU_ImplementAttributeParserBlock( parserMeshSkeleton, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );

				if ( auto skeleton = blockContext->mesh->getScene()->findSkeleton( name ) )
				{
					blockContext->mesh->setSkeleton( skeleton );
				}
				else
				{
					CU_ParsingError( cuT( "Skeleton [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshMorphAnimation, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				blockContext->morphAnimation = castor::makeUnique< MeshAnimation >( *blockContext->mesh, params[0]->get< castor::String >() );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eMorphAnimation, blockContext )

		static CU_ImplementAttributeParserBlock( parserMeshEnd, MeshContext )
		{
			if ( auto mesh = blockContext->mesh )
			{
				blockContext->mesh = {};

				if ( blockContext->ownMesh )
				{
					mesh->getScene()->addMesh( mesh->getName()
						, blockContext->ownMesh
						, true );
				}

				if ( blockContext->geometry )
				{
					blockContext->geometry->geometry->setMesh( mesh );
				}

				for ( auto const & submesh : *mesh )
				{
					if ( !submesh->hasRenderComponent() )
					{
						submesh->createComponent< DefaultRenderComponent >();
					}

					mesh->getScene()->getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterialsMaterial, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[1]->get< castor::String >(), *blockContext );
				uint16_t index;

				if ( auto material = getEngine( *blockContext )->findMaterial( name ) )
				{
					if ( blockContext->mesh->getSubmeshCount() > params[0]->get( index ) )
					{
						auto submesh = blockContext->mesh->getSubmesh( index );
						submesh->setDefaultMaterial( material );
					}
					else
					{
						CU_ParsingError( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterialsEnd, MeshContext )
		{
			// Only push the block
		}
		CU_EndAttributePop()
	}

	const castor::String ResourceCacheTraitsT< castor3d::Mesh, castor::String >::Name = cuT( "Mesh" );

	Mesh::Mesh( castor::String const & name, Scene & scene )
		: castor::Named{ name }
		, Animable{ *scene.getEngine() }
		, m_scene{ &scene }
	{
	}

	Mesh::~Mesh()noexcept
	{
		CU_Assert( m_submeshes.empty(), "Did you forget to call Mesh::cleanup ?" );
	}

	void Mesh::initialise()
	{
		auto it = m_submeshes.begin();

		while ( it != m_submeshes.end() )
		{
			auto & submesh = *it;

			if ( submesh->getPointsCount() > 0
				&& ( !submesh->getIndexMapping()
					|| submesh->getIndexMapping()->getCount() > 0 ) )
			{
				submesh->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
				++it;
			}
			else
			{
				it = m_submeshes.erase( it );
			}
		}
	}

	void Mesh::cleanup()
	{
		Animable::cleanupAnimations();

		for ( auto & submesh : m_submeshes )
		{
			submesh->cleanup( getEngine()->getRenderSystem()->getRenderDevice() );
		}

		m_submeshes.clear();
	}

	crg::FramePassArray Mesh::record( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses )
	{
		for ( auto & submesh : *this )
		{
			previousPasses = submesh->record( resources, graph, castor::move( previousPasses ) );
		}

		return previousPasses;
	}

	void Mesh::registerDependencies( crg::FramePass & pass )const
	{
		for ( auto & submesh : *this )
		{
			submesh->registerDependencies( pass );
		}
	}

	void Mesh::update( CpuUpdater & updater )
	{
		bool dirty = false;

		for ( auto const & submesh : m_submeshes )
		{
			dirty = submesh->update( updater ) || dirty;
		}

		if ( dirty )
		{
			onChange( *this );
		}
	}

	void Mesh::updateContainers()
	{
		if ( !m_submeshes.empty() )
		{
			m_box = m_submeshes[0]->getBoundingBox();

			for ( auto i = 1u; i < m_submeshes.size(); ++i )
			{
				m_box = m_box.getUnion( m_submeshes[i]->getBoundingBox() );
			}

			m_sphere.load( m_box );
		}
	}

	void Mesh::computeContainers()
	{
		for ( auto const & submesh : m_submeshes )
		{
			submesh->computeContainers();
		}

		updateContainers();
	}

	uint32_t Mesh::getFaceCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto const & submesh : m_submeshes )
		{
			nbFaces += submesh->getFaceCount();
		}

		return nbFaces;
	}

	uint32_t Mesh::getVertexCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto const & submesh : m_submeshes )
		{
			nbFaces += submesh->getPointsCount();
		}

		return nbFaces;
	}

	SubmeshRPtr Mesh::getSubmesh( uint32_t index )const
	{
		SubmeshRPtr result{};

		if ( index < m_submeshes.size() )
		{
			result = m_submeshes[index].get();
		}

		return result;
	}

	SubmeshRPtr Mesh::createSubmesh()
	{
		return m_submeshes.emplace_back( castor::makeUnique< Submesh >( *this
			, getSubmeshCount() ) ).get();
	}

	SubmeshRPtr Mesh::createDefaultSubmesh()
	{
		auto result = createSubmesh();
		result->createComponent< PositionsComponent >();
		result->createComponent< NormalsComponent >();
		result->createComponent< TangentsComponent >();
		result->createComponent< Texcoords0Component >();
		result->createComponent< DefaultRenderComponent >();
		return result;
	}

	void Mesh::removeSubmesh( Submesh const & submesh )
	{
		auto it = std::find_if( m_submeshes.begin()
			, m_submeshes.end()
			, [&submesh]( SubmeshUPtr const & lookup )
			{
				return &submesh == lookup.get();
			} );

		if ( it != m_submeshes.end() )
		{
			m_submeshes.erase( it );
		}
	}

	void Mesh::computeNormals( bool reverted )
	{
		for ( auto const & submesh : m_submeshes )
		{
			submesh->computeNormals( reverted );
		}
	}

	void Mesh::setSkeleton( SkeletonRPtr skeleton )
	{
		m_skeleton = skeleton;
		m_skeleton->computeContainers( *this );
	}

	MeshAnimation & Mesh::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			addAnimation( castor::makeUniqueDerived< Animation, MeshAnimation >( *this, name ) );
		}

		return doGetAnimation< MeshAnimation >( name );
	}

	void Mesh::removeAnimation( castor::String const & name )
	{
		if ( hasAnimation( name ) )
		{
			doRemoveAnimation( name );
		}
	}

	void Mesh::cloneInto( Mesh & output )const
	{
		output.m_box = m_box;
		output.m_sphere = m_sphere;
		output.m_serialisable = m_serialisable;

		for ( auto & submesh : m_submeshes )
		{
			submesh->cloneInto( *output.createSubmesh() );
		}
	}

	void Mesh::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< MeshContext > meshCtx{ result, CSCNSection::eMesh };
		BlockParserContextT< MeshContext > materialsCtx{ result, CSCNSection::eMeshDefaultMaterials, CSCNSection::eMesh };

		meshCtx.addParser( cuT( "type" ), mesh::parserMeshType, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
		meshCtx.addParser( cuT( "import" ), mesh::parserMeshImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		meshCtx.addParser( cuT( "import_anim" ), mesh::parserMeshAnimImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		meshCtx.addParser( cuT( "import_single_anim" ), mesh::parserMeshSingleAnimImport, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		meshCtx.addParser( cuT( "import_morph_target" ), mesh::parserMeshMorphTargetImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
		meshCtx.addParser( cuT( "default_material" ), mesh::parserMeshDefaultMaterial, { makeParameter< ParameterType::eName >() } );
		meshCtx.addParser( cuT( "skeleton" ), mesh::parserMeshSkeleton, { makeParameter< ParameterType::eName >() } );
		meshCtx.addPushParser( cuT( "morph_animation" ), CSCNSection::eMorphAnimation, mesh::parserMeshMorphAnimation, { makeParameter< ParameterType::eName >() } );
		meshCtx.addPushParser( cuT( "submesh" ), CSCNSection::eSubmesh, mesh::parserMeshSubmesh );
		meshCtx.addPushParser( cuT( "default_materials" ), CSCNSection::eMeshDefaultMaterials, mesh::parserMeshDefaultMaterials );
		meshCtx.addPopParser( cuT( "}" ), mesh::parserMeshEnd );

		materialsCtx.addParser( cuT( "material" ), mesh::parserMeshDefaultMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
		materialsCtx.addPopParser( cuT( "}" ), mesh::parserMeshDefaultMaterialsEnd );

		Submesh::addParsers( result );
		MeshAnimation::addParsers( result );
	}

	castor::String getPrefix( MeshContext const & context )
	{
		return getPrefix( *context.scene );
	}

	Engine * getEngine( MeshContext const & context )
	{
		return getEngine( *context.scene );
	}
}
