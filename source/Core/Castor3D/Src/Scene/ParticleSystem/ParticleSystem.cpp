#include "ParticleSystem.hpp"

#include "Engine.hpp"
#include "Material/Material.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Scene/ParticleSystem/ComputeParticleSystem.hpp"
#include "Scene/ParticleSystem/CpuParticleSystem.hpp"

#include <Core/Device.hpp>
#include <Pipeline/VertexLayout.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		String getFormatName( ParticleFormat format )
		{
			switch ( format )
			{
			case castor3d::ParticleFormat::eInt:
				return cuT( "int" );
			case castor3d::ParticleFormat::eVec2i:
				return cuT( "vec2i" );
			case castor3d::ParticleFormat::eVec3i:
				return cuT( "vec3i" );
			case castor3d::ParticleFormat::eVec4i:
				return cuT( "vec4i" );
			case castor3d::ParticleFormat::eUInt:
				return cuT( "uint" );
			case castor3d::ParticleFormat::eVec2ui:
				return cuT( "vec2ui" );
			case castor3d::ParticleFormat::eVec3ui:
				return cuT( "vec3ui" );
			case castor3d::ParticleFormat::eVec4ui:
				return cuT( "vec4ui" );
			case castor3d::ParticleFormat::eFloat:
				return cuT( "float" );
			case castor3d::ParticleFormat::eVec2f:
				return cuT( "vec2f" );
			case castor3d::ParticleFormat::eVec3f:
				return cuT( "vec3f" );
			case castor3d::ParticleFormat::eVec4f:
				return cuT( "vec4f" );
			case castor3d::ParticleFormat::eMat2f:
				return cuT( "mat2x2f" );
			case castor3d::ParticleFormat::eMat3f:
				return cuT( "mat3x3f" );
			case castor3d::ParticleFormat::eMat4f:
				return cuT( "mat4x4f" );
			default:
				assert( false );
				return cuT( "vec4f" );
				break;
			}
		}

		renderer::Format convert( ParticleFormat format )
		{
			switch ( format )
			{
			case castor3d::ParticleFormat::eInt:
				return renderer::Format::eR32_SINT;
			case castor3d::ParticleFormat::eVec2i:
				return renderer::Format::eR32G32_SINT;
			case castor3d::ParticleFormat::eVec3i:
				return renderer::Format::eR32G32B32_SINT;
			case castor3d::ParticleFormat::eVec4i:
				return renderer::Format::eR32G32B32A32_SINT;
			case castor3d::ParticleFormat::eUInt:
				return renderer::Format::eR32_UINT;
			case castor3d::ParticleFormat::eVec2ui:
				return renderer::Format::eR32G32_UINT;
			case castor3d::ParticleFormat::eVec3ui:
				return renderer::Format::eR32G32B32_UINT;
			case castor3d::ParticleFormat::eVec4ui:
				return renderer::Format::eR32G32B32A32_UINT;
			case castor3d::ParticleFormat::eFloat:
				return renderer::Format::eR32_SFLOAT;
			case castor3d::ParticleFormat::eVec2f:
				return renderer::Format::eR32G32_SFLOAT;
			case castor3d::ParticleFormat::eVec3f:
				return renderer::Format::eR32G32B32_SFLOAT;
			case castor3d::ParticleFormat::eVec4f:
				return renderer::Format::eR32G32B32A32_SFLOAT;
			default:
				assert( false );
				return renderer::Format::eR32G32B32A32_SFLOAT;
				break;
			}
		}
	}

	ParticleSystem::TextWriter::TextWriter( String const & tabs )
		: MovableObject::TextWriter{ tabs }
	{
	}

	bool ParticleSystem::TextWriter::operator()( ParticleSystem const & obj, castor::TextFile & file )
	{
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "particle_system \"" ) + obj.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::checkError( result, "ParticleSystem name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( obj, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tparticles_count " ) + string::toString( obj.getMaxParticlesCount(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particles count" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tdimensions " )
				+ string::toString( obj.getDimensions()[0], std::locale{ "C" } ) + cuT( " " )
				+ string::toString( obj.getDimensions()[1], std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem dimensions" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tmaterial \"" ) + obj.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem material" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tparticle\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );

			if ( result && !obj.getParticleType().empty() )
			{
				result = file.writeText( m_tabs + cuT( "\t\ttype \"" ) + obj.getParticleType() + cuT( "\"\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );
			}

			auto values = obj.getDefaultValues();

			for ( auto & var : obj.getParticleVariables() )
			{
				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "\t\tvariable \"" ) + var.m_name + cuT( "\" " ) + getFormatName( var.m_dataType ) + cuT( " " ) + values[cuT( "out_" ) + var.m_name] + cuT( "\n" ) ) > 0;
					MovableObject::TextWriter::checkError( result, "ParticleSystem particle variable" );
				}
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );
			}
		}

		if ( result && obj.m_csImpl->hasUpdateProgram() )
		{
			result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "cs_shader_program" ) )( obj.m_csImpl->getUpdateProgram(), file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	ParticleSystem::ParticleSystem( String const & name
		, Scene & scene
		, SceneNodeSPtr parent
		, uint32_t count )
		: MovableObject{ name, scene, MovableType::eParticleEmitter, parent }
		, m_particlesCount{ count }
		, m_csImpl{ std::make_unique< ComputeParticleSystem >( *this ) }
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::initialise()
	{
		auto & device = getCurrentDevice( *this );
		auto vertexLayout = std::make_unique< renderer::VertexLayout >( 1u, m_inputs.stride(), renderer::VertexInputRate::eInstance );
		uint32_t index{ 2u };

		for ( auto & attribute : m_inputs )
		{
			if ( attribute.m_dataType == ParticleFormat::eMat2f )
			{
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32_SFLOAT, attribute.m_offset );
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32_SFLOAT, attribute.m_offset + 8u );
			}
			else if ( attribute.m_dataType == ParticleFormat::eMat3f )
			{
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32_SFLOAT, attribute.m_offset );
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32_SFLOAT, attribute.m_offset + 12u );
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32_SFLOAT, attribute.m_offset + 24u );
			}
			else if ( attribute.m_dataType == ParticleFormat::eMat4f )
			{
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32A32_SFLOAT, attribute.m_offset );
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32A32_SFLOAT, attribute.m_offset + 16u );
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32A32_SFLOAT, attribute.m_offset + 32u );
				vertexLayout->createAttribute( index++, renderer::Format::eR32G32B32A32_SFLOAT, attribute.m_offset + 48u );
			}
			else
			{
				vertexLayout->createAttribute( index++, convert( attribute.m_dataType ), attribute.m_offset );
			}
		}

		auto stride = vertexLayout->getStride();
		m_particlesBillboard = std::make_unique< BillboardBase >( *getScene()
			, getScene()->getObjectRootNode()
			, std::move( vertexLayout )
			, std::make_unique< renderer::VertexBufferBase >( device
				, stride * m_particlesCount
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible ) );
		m_particlesBillboard->setBillboardType( BillboardType::eSpherical );
		m_particlesBillboard->setDimensions( m_dimensions );
		m_particlesBillboard->setMaterial( m_material.lock() );
		bool result = m_particlesBillboard->initialise( m_particlesCount );
		getScene()->getBillboardPools().registerElement( *m_particlesBillboard );

		if ( result )
		{
			result = m_csImpl->initialise();
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Using Compute Shader Particle System" ) );
			m_impl = m_csImpl.get();
		}
		else
		{
			result = m_cpuImpl->initialise();

			if ( result )
			{
				Logger::logInfo( cuT( "Using CPU Particle System" ) );
				m_impl = m_cpuImpl.get();
			}
		}

		m_timer.getElapsed();
		return result;
	}

	void ParticleSystem::cleanup()
	{
		getScene()->getBillboardPools().unregisterElement( *m_particlesBillboard );
		m_particlesBillboard->cleanup();
		m_particlesBillboard.reset();
		m_csImpl->cleanup();
		m_cpuImpl->cleanup();
		m_impl = nullptr;
	}

	void ParticleSystem::update( RenderPassTimer & timer
		, uint32_t index )
	{
		REQUIRE( m_impl );
		auto time = std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
		}

		m_totalTime += time;
		m_activeParticlesCount = m_impl->update( timer, time, m_totalTime, index );

		if ( getBillboards()->getCount() != m_activeParticlesCount )
		{
			getBillboards()->setCount( m_activeParticlesCount );
			getBillboards()->update();
		}

		m_firstUpdate = false;
	}

	void ParticleSystem::setMaterial( MaterialSPtr material )
	{
		m_material = material;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setMaterial( material );
		}
	}

	void ParticleSystem::setDimensions( Point2f const & dimensions )
	{
		m_dimensions = dimensions;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setDimensions( dimensions );
		}
	}

	void ParticleSystem::setParticleType( castor::String const & value )
	{
		auto & factory = getScene()->getEngine()->getParticleFactory();
		m_particleType = value;

		if ( factory.isTypeRegistered( value ) )
		{
			m_cpuImpl = factory.create( value, *this );
		}
		else
		{
			CASTOR_EXCEPTION( cuT( "Particle type [" ) + value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
		}
	}

	MaterialSPtr ParticleSystem::getMaterial()const
	{
		return m_material.lock();
	}

	Point2f const & ParticleSystem::getDimensions()const
	{
		return m_dimensions;
	}

	void ParticleSystem::addParticleVariable( String const & name
		, ParticleFormat type
		, String const & defaultValue )
	{
		m_csImpl->addParticleVariable( name, type, defaultValue );
		m_cpuImpl->addParticleVariable( name, type, defaultValue );
		m_inputs.push_back( ParticleElementDeclaration{ name, 0u, type, m_inputs.stride(), 1u } );
		m_defaultValues[cuT ("out_") + name] = defaultValue;
	}

	void ParticleSystem::setCSUpdateProgram( ShaderProgramSPtr program )
	{
		m_csImpl->setUpdateProgram( program );
	}

	void ParticleSystem::setCSGroupSizes( castor::Point3i sizes )
	{
		m_csImpl->setGroupSizes( sizes );
	}
}
