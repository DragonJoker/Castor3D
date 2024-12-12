#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/ParticleSystem/ComputeParticleSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

#include <ashespp/Core/Device.hpp>

CU_ImplementSmartPtr( castor3d, ParticleSystem )

namespace castor3d
{
	namespace ptclsys
	{
		static size_t getVkCount( ParticleFormat format )
		{
			switch ( format )
			{
			case ParticleFormat::eMat2f:
				return 2u;
			case ParticleFormat::eMat3f:
				return 3u;
			case ParticleFormat::eMat4f:
				return 4u;
			default:
				return 1u;
			}
		}

		static ParticleFormat getComponent( ParticleFormat format )
		{
			switch ( format )
			{
			case ParticleFormat::eVec2i:
			case ParticleFormat::eVec3i:
			case ParticleFormat::eVec4i:
				return ParticleFormat::eInt;
			case ParticleFormat::eVec2ui:
			case ParticleFormat::eVec3ui:
			case ParticleFormat::eVec4ui:
				return ParticleFormat::eUInt;
			case ParticleFormat::eVec2f:
			case ParticleFormat::eVec3f:
			case ParticleFormat::eVec4f:
				return ParticleFormat::eFloat;
			case ParticleFormat::eMat2f:
				return ParticleFormat::eVec2f;
			case ParticleFormat::eMat3f:
				return ParticleFormat::eVec3f;
			case ParticleFormat::eMat4f:
				return ParticleFormat::eVec4f;
			default:
				return format;
			}
		}

		static VkFormat getVkFormat( ParticleFormat format )
		{
			switch ( format )
			{
			case ParticleFormat::eInt:
				return VK_FORMAT_R32_SINT;
			case ParticleFormat::eVec2i:
				return VK_FORMAT_R32G32_SINT;
			case ParticleFormat::eVec3i:
				return VK_FORMAT_R32G32B32_SINT;
			case ParticleFormat::eVec4i:
				return VK_FORMAT_R32G32B32A32_SINT;
			case ParticleFormat::eUInt:
				return VK_FORMAT_R32_UINT;
			case ParticleFormat::eVec2ui:
				return VK_FORMAT_R32G32_UINT;
			case ParticleFormat::eVec3ui:
				return VK_FORMAT_R32G32B32_UINT;
			case ParticleFormat::eVec4ui:
				return VK_FORMAT_R32G32B32A32_UINT;
			case ParticleFormat::eFloat:
				return VK_FORMAT_R32_SFLOAT;
			case ParticleFormat::eVec2f:
			case ParticleFormat::eMat2f:
				return VK_FORMAT_R32G32_SFLOAT;
			case ParticleFormat::eVec3f:
			case ParticleFormat::eMat3f:
				return VK_FORMAT_R32G32B32_SFLOAT;
			case ParticleFormat::eVec4f:
			case ParticleFormat::eMat4f:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			default:
				assert( false );
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
		}

		static VkDeviceSize getComponentSize( ParticleFormat format )
		{
			return getSize( getComponent( format ) );
		}

		static CU_ImplementAttributeParserBlock( parserSystemParent, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );

				if ( auto node = blockContext->scene->scene->tryFindSceneNode( name ) )
				{
					blockContext->parentNode = node;
				}
				else
				{
					CU_ParsingError( cuT( "No scene node named " ) + name );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSystemCount, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->particleCount );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSystemMaterial, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );

				if ( auto material = getEngine( *blockContext )->tryFindMaterial( name ) )
				{
					blockContext->material = material;
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSystemDimensions, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->dimensions );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSystemParticle, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( blockContext->particleCount == 0 )
			{
				CU_ParsingError( cuT( "particles_count has not been specified." ) );
			}
			else if ( blockContext->dimensions[0] == 0 || blockContext->dimensions[1] == 0 )
			{
				CU_ParsingError( cuT( "one component of the particles dimensions is 0." ) );
			}
			else
			{
				if ( !blockContext->material )
				{
					blockContext->material = getEngine( *blockContext )->getMaterialCache().getDefaultMaterial();
				}

				blockContext->particleSystem = blockContext->scene->scene->tryFindParticleSystem( blockContext->name );

				if ( !blockContext->particleSystem )
				{
					auto node = blockContext->parentNode;

					if ( !node )
					{
						node = blockContext->scene->scene->getObjectRootNode();
					}

					blockContext->parentNode = nullptr;
					blockContext->ownParticleSystem = blockContext->scene->scene->createParticleSystem( blockContext->name
						, *blockContext->scene->scene
						, *node
						, blockContext->particleCount );
					blockContext->particleSystem = blockContext->ownParticleSystem.get();
				}

				blockContext->particleSystem->setMaterial( blockContext->material );
				blockContext->particleSystem->setDimensions( blockContext->dimensions );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eParticle, blockContext )

		static CU_ImplementAttributeParserNewBlock( parserSystemCSShader, ParticleSystemContext, ProgramContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				newBlockContext->shaderProgram = getEngine( *blockContext )->getShaderProgramCache().getNewProgram( blockContext->name, true );
				newBlockContext->particleSystem = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eShaderProgram )

		static CU_ImplementAttributeParserBlock( parserSystemEnd, ParticleSystemContext )
		{
			if ( !blockContext->particleSystem )
			{
				CU_ParsingError( cuT( "No particle system initialised." ) );
			}
			else
			{
				blockContext->parentNode = nullptr;
				log::info << "Loaded sampler [" << blockContext->particleSystem->getName() << "]" << std::endl;

				if ( blockContext->ownParticleSystem )
				{
					blockContext->scene->scene->addParticleSystem( blockContext->particleSystem->getName()
						, blockContext->ownParticleSystem
						, true );
				}

				blockContext->particleSystem = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserType, ParticleSystemContext )
		{
			if ( !blockContext->particleSystem )
			{
				CU_ParsingError( cuT( "No particle system initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String type;

				if ( Engine const * engine = getEngine( *blockContext );
					engine && !engine->getParticleFactory().isTypeRegistered( castor::string::lowerCase( params[0]->get( type ) ) ) )
				{
					CU_ParsingError( cuT( "Particle type [" ) + type + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					blockContext->particleSystem->setParticleType( type );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVariable, ParticleSystemContext )
		{
			if ( !blockContext->particleSystem )
			{
				CU_ParsingError( cuT( "No particle system initialised." ) );
			}
			else if ( params.size() < 2 )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String value;

				if ( params.size() > 2 )
				{
					params[2]->get( value );
				}

				blockContext->particleSystem->addParticleVariable( params[0]->get< castor::String >(), ParticleFormat( params[1]->get< uint32_t >() ), value );
			}
		}
		CU_EndAttribute()
	}

	//*************************************************************************************************

	ParticleSystem::ParticleSystem( castor::String const & name
		, Scene & scene
		, SceneNode & node
		, uint32_t count )
		: MovableObject{ name, scene, MovableType::eParticleEmitter, node }
		, m_particlesCount{ count }
		, m_csImpl{ castor::makeUnique< ComputeParticleSystem >( *this ) }
	{
	}

	bool ParticleSystem::initialise( RenderDevice const & device )
	{
		ashes::VkVertexInputBindingDescriptionArray bindings
		{
			{ 1u, m_inputs.stride(), VK_VERTEX_INPUT_RATE_INSTANCE }
		};
		uint32_t index{ 2u };
		uint32_t stride{ 0u };
		ashes::VkVertexInputAttributeDescriptionArray attributes;

		for ( auto const & attribute : m_inputs )
		{
			auto fmt = ptclsys::getVkFormat( attribute.m_dataType );
			auto compSize = uint32_t( ptclsys::getComponentSize( attribute.m_dataType ) );
			uint32_t offset = 0u;

			for ( size_t i = 0u; i < ptclsys::getVkCount( attribute.m_dataType ); ++i )
			{
				attributes.push_back( { index++, 1u, fmt, attribute.m_offset + offset } );
				offset += compSize;
			}

			stride += uint32_t( getSize( attribute.m_dataType ) );
		}

		auto align = device.renderSystem.getValue( GpuMin::eBufferMapSize );
		auto size = ashes::getAlignedSize( getMaxParticlesCount() * m_inputs.stride(), align );
		m_particlesBillboard = castor::makeUnique< BillboardBase >( *getScene()
			, getScene()->getObjectRootNode()
			, castor::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u, bindings, attributes )
			, stride
			, device.bufferPool->getBuffer< uint8_t >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, size
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) );
		m_particlesBillboard->setBillboardType( BillboardType::eSpherical );
		m_particlesBillboard->setDimensions( m_dimensions );
		m_particlesBillboard->setMaterial( m_material );
		bool result = m_particlesBillboard->initialise( device, m_particlesCount );

		if ( result )
		{
			result = m_csImpl->initialise( device );
		}

		if ( result )
		{
			log::info << cuT( "Using Compute Shader Particle System" ) << std::endl;
			m_impl = m_csImpl.get();
			m_particlesBillboard->setGpuFilled( true );
		}
		else
		{
			result = m_cpuImpl->initialise( device );

			if ( result )
			{
				log::info << cuT( "Using CPU Particle System" ) << std::endl;
				m_impl = m_cpuImpl.get();
			}
		}

		m_timer.getElapsed();
		return result;
	}

	void ParticleSystem::cleanup( RenderDevice const & device )
	{
		m_particlesBillboard->cleanup( device );
		m_particlesBillboard.reset();
		m_csImpl->cleanup( device );
		m_cpuImpl->cleanup( device );
		m_impl = nullptr;
	}

	void ParticleSystem::update( CpuUpdater & updater )
	{
		if ( !m_impl )
		{
			return;
		}

		auto time = std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
		}

		time = updater.tslf > 0_ms
			? updater.tslf
			: time;
		m_time = time;
		m_totalTime += time;
		updater.time = m_time;
		updater.total = m_totalTime;
		m_impl->update( updater );
	}

	void ParticleSystem::update( GpuUpdater & updater )
	{
		if ( !m_impl )
		{
			return;
		}

		updater.time = m_time;
		updater.total = m_totalTime;
		m_activeParticlesCount = m_impl->update( updater );
		updater.info.particlesCount += m_activeParticlesCount;

		if ( getBillboards()->getCount() != m_activeParticlesCount )
		{
			getBillboards()->setCount( m_activeParticlesCount );
			getBillboards()->update( updater );
		}

		m_firstUpdate = false;
	}

	void ParticleSystem::setMaterial( MaterialObs material )
	{
		m_material = material;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setMaterial( material );
		}
	}

	void ParticleSystem::setDimensions( castor::Point2f const & dimensions )
	{
		m_dimensions = dimensions;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setDimensions( dimensions );
		}
	}

	void ParticleSystem::setParticleType( castor::String const & value )
	{
		m_particleType = value;

		if ( auto const & factory = getScene()->getEngine()->getParticleFactory();
			factory.isTypeRegistered( value ) )
		{
			m_cpuImpl = factory.create( value, *this );
		}
		else
		{
			CU_Exception( "Particle type [" + castor::toUtf8( value ) + "] is not registered, make sure you've got the matching plug-in installed." );
		}
	}

	MaterialObs ParticleSystem::getMaterial()const
	{
		return m_material;
	}

	castor::Point2f const & ParticleSystem::getDimensions()const
	{
		return m_dimensions;
	}

	void ParticleSystem::addParticleVariable( castor::String const & name
		, ParticleFormat type
		, castor::String const & defaultValue )
	{
		m_csImpl->addParticleVariable( name, type, defaultValue );
		m_cpuImpl->addParticleVariable( name, type, defaultValue );
		m_inputs.push_back( ParticleElementDeclaration{ name, ElementUsage::eUnknown, type, m_inputs.stride(), 1u } );
		m_defaultValues[cuT ("out_") + name] = defaultValue;
	}

	void ParticleSystem::setCSUpdateProgram( ShaderProgramRPtr program )
	{
		m_csImpl->setUpdateProgram( program );
	}

	void ParticleSystem::setCSGroupSizes( castor::Point3i sizes )
	{
		m_csImpl->setGroupSizes( sizes );
	}

	void ParticleSystem::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< ParticleSystemContext > systemCtx{ result, CSCNSection::eParticleSystem, CSCNSection::eScene };
		BlockParserContextT< ParticleSystemContext > particleCtx{ result, CSCNSection::eParticle, CSCNSection::eParticleSystem };

		systemCtx.addParser( cuT( "parent" ), ptclsys::parserSystemParent, { makeParameter< ParameterType::eName >() } );
		systemCtx.addParser( cuT( "particles_count" ), ptclsys::parserSystemCount, { makeParameter< ParameterType::eUInt32 >() } );
		systemCtx.addParser( cuT( "material" ), ptclsys::parserSystemMaterial, { makeParameter< ParameterType::eName >() } );
		systemCtx.addParser( cuT( "dimensions" ), ptclsys::parserSystemDimensions, { makeParameter< ParameterType::ePoint2F >() } );
		systemCtx.addPushParser( cuT( "particle" ), CSCNSection::eParticle, ptclsys::parserSystemParticle );
		systemCtx.addPushParser( cuT( "cs_shader_program" ), CSCNSection::eShaderProgram, ptclsys::parserSystemCSShader );
		systemCtx.addPopParser( cuT( "}" ), ptclsys::parserSystemEnd );

		particleCtx.addParser( cuT( "variable" ), ptclsys::parserVariable, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eCheckedText, ParticleFormat >(), makeParameter< ParameterType::eText >() } );
		particleCtx.addParser( cuT( "type" ), ptclsys::parserType, { makeParameter< ParameterType::eName >() } );
		particleCtx.addDefaultPopParser();
	}
}
