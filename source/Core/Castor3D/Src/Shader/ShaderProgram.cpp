#include "ShaderProgram.hpp"

#include "Render/RenderSystem.hpp"
#include "Shader/AtomicCounterBuffer.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/ShaderStorageBuffer.hpp"

#include <Stream/StreamPrefixManipulators.hpp>
#include <GlslShader.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream
			, format::BasePrefixer< CharType, PrefixType > const & prefix )
		{
			auto * sbuf = dynamic_cast< format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				format::installPrefixBuffer< PrefixType >( stream );
				stream.register_callback( format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}
	}

	//*************************************************************************************************

	ShaderProgram::TextWriter::TextWriter( String const & tabs
		, String const & name )
		: castor::TextWriter< ShaderProgram >{ tabs }
		, m_name{ name }
	{
	}

	bool ShaderProgram::TextWriter::operator()( ShaderProgram const & shaderProgram
		, TextFile & file )
	{
		bool result = false;
		bool hasFile = false;
		uint8_t i = 0;

		while ( i < uint8_t( ShaderType::eCount ) && !hasFile )
		{
			if ( shaderProgram.hasObject( ShaderType( i ) ) )
			{
				hasFile = !shaderProgram.getFile( ShaderType( i ) ).empty();
			}

			++i;
		}

		if ( hasFile )
		{
			auto tabs = m_tabs + cuT( "\t" );
			ShaderObjectSPtr object;
			result = file.writeText( cuT( "\n" ) + m_tabs + m_name + cuT( "\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			checkError( result, "Shader program" );

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ) && result; i++ )
			{
				if ( shaderProgram.hasObject( ShaderType( i ) ) )
				{
					result = ShaderObject::TextWriter( tabs )( *shaderProgram.m_shaders[i], file );
				}
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}
		else
		{
			result = true;
		}

		return result;
	}

	//*************************************************************************************************

	struct VariableApply
	{
		template <class T> void operator()( T & p ) const
		{
			p->apply();
		}
	};

	//*************************************************************************************************

	const String ShaderProgram::Position = cuT( "position" );
	const String ShaderProgram::Normal = cuT( "normal" );
	const String ShaderProgram::Tangent = cuT( "tangent" );
	const String ShaderProgram::Bitangent = cuT( "bitangent" );
	const String ShaderProgram::Texture = cuT( "texture" );
	const String ShaderProgram::Colour = cuT( "colour" );
	const String ShaderProgram::Position2 = cuT( "position2" );
	const String ShaderProgram::Normal2 = cuT( "normal2" );
	const String ShaderProgram::Tangent2 = cuT( "tangent2" );
	const String ShaderProgram::Bitangent2 = cuT( "bitangent2" );
	const String ShaderProgram::Texture2 = cuT( "texture2" );
	const String ShaderProgram::Colour2 = cuT( "colour2" );
	const String ShaderProgram::Text = cuT( "text" );
	const String ShaderProgram::BoneIds0 = cuT( "bone_ids0" );
	const String ShaderProgram::BoneIds1 = cuT( "bone_ids1" );
	const String ShaderProgram::Weights0 = cuT( "weights0" );
	const String ShaderProgram::Weights1 = cuT( "weights1" );
	const String ShaderProgram::Transform = cuT( "transform" );
	const String ShaderProgram::Material = cuT( "material" );

	const String ShaderProgram::Lights = cuT( "c3d_sLights" );
	const String ShaderProgram::MapDiffuse = cuT ("c3d_mapDiffuse");
	const String ShaderProgram::MapAlbedo = cuT( "c3d_mapAlbedo" );
	const String ShaderProgram::MapSpecular = cuT ("c3d_mapSpecular");
	const String ShaderProgram::MapRoughness = cuT( "c3d_mapRoughness" );
	const String ShaderProgram::MapEmissive = cuT( "c3d_mapEmissive" );
	const String ShaderProgram::MapNormal = cuT( "c3d_mapNormal" );
	const String ShaderProgram::MapOpacity = cuT( "c3d_mapOpacity" );
	const String ShaderProgram::MapGloss = cuT( "c3d_mapGloss" );
	const String ShaderProgram::MapMetallic = cuT ("c3d_mapMetallic");
	const String ShaderProgram::MapHeight = cuT( "c3d_mapHeight" );
	const String ShaderProgram::MapEnvironment = cuT( "c3d_mapEnvironment" );
	const String ShaderProgram::MapAmbientOcclusion = cuT( "c3d_mapAmbientOcclusion" );
	const String ShaderProgram::MapTransmittance = cuT( "c3d_mapTransmittance" );
	const String ShaderProgram::MapIrradiance = cuT( "c3d_mapIrradiance" );
	const String ShaderProgram::MapPrefiltered = cuT( "c3d_mapPrefiltered" );
	const String ShaderProgram::MapBrdf = cuT( "c3d_mapBrdf" );
	const String ShaderProgram::MapText = cuT( "c3d_mapText" );

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	ShaderObjectSPtr ShaderProgram::createObject( ShaderType type )
	{
		ShaderObjectSPtr result;
		REQUIRE( type > ShaderType::eNone && type < ShaderType::eCount );

		if ( type > ShaderType::eNone && type < ShaderType::eCount )
		{
			result = doCreateObject( type );
			m_shaders[size_t( type )] = result;

			if ( !m_file.empty() )
			{
				m_shaders[size_t( type )]->setFile( m_file );
			}
		}

		return result;
	}

	void ShaderProgram::doCleanup()
	{
		for ( auto shader : m_activeShaders )
		{
			if ( shader )
			{
				shader->detach();
				shader->flushUniforms();
				shader->destroy();
			}
		}

		m_activeShaders.clear();
		m_file.clear();
	}

	void ShaderProgram::setFile( Path const & path )
	{
		m_file = path;
		uint8_t i = uint8_t( ShaderType::eVertex );

		for ( auto shader : m_shaders )
		{
			if ( shader && getRenderSystem()->getGpuInformations().hasShaderType( ShaderType( i++ ) ) )
			{
				shader->setFile( path );
			}
		}

		resetToCompile();
	}

	void ShaderProgram::resetToCompile()
	{
		m_status = ProgramStatus::eNotLinked;
	}

	void ShaderProgram::setInputType( ShaderType target, Topology topology )
	{
		REQUIRE( m_shaders[size_t( target )] && target == ShaderType::eGeometry );
		if ( m_shaders[size_t( target )] )
		{
			m_shaders[size_t( target )]->setInputType( topology );
		}
	}

	void ShaderProgram::setOutputType( ShaderType target, Topology topology )
	{
		REQUIRE( m_shaders[size_t( target )] && target == ShaderType::eGeometry );
		if ( m_shaders[size_t( target )] )
		{
			m_shaders[size_t( target )]->setOutputType( topology );
		}
	}

	void ShaderProgram::setOutputVtxCount( ShaderType target, uint8_t count )
	{
		REQUIRE( m_shaders[size_t( target )] && target == ShaderType::eGeometry );
		if ( m_shaders[size_t( target )] )
		{
			m_shaders[size_t( target )]->setOutputVtxCount( count );
		}
	}

	void ShaderProgram::setFile( ShaderType target, Path const & pathFile )
	{
		if ( m_shaders[size_t( target )] )
		{
			m_shaders[size_t( target )]->setFile( pathFile );
		}
		else
		{
			CASTOR_EXCEPTION( "setting the source file of a non available shader object" );
		}

		resetToCompile();
	}

	Path ShaderProgram::getFile( ShaderType target )const
	{
		REQUIRE( m_shaders[size_t( target )] );
		Path pathReturn;

		if ( m_shaders[size_t( target )] )
		{
			pathReturn = m_shaders[size_t( target )]->getFile();
		}

		return pathReturn;
	}

	bool ShaderProgram::hasFile( ShaderType target )const
	{
		REQUIRE( m_shaders[size_t( target )] );
		bool result = false;

		if ( m_shaders[size_t( target )] )
		{
			result = m_shaders[size_t( target )]->hasFile();
		}

		return result;
	}

	void ShaderProgram::setSource( ShaderType target, String const & source )
	{
		if ( m_shaders[size_t( target )] )
		{
			m_shaders[size_t( target )]->setSource( source );
		}
		else
		{
			CASTOR_EXCEPTION( "setting the source of a non available shader object" );
		}

		resetToCompile();
	}

	void ShaderProgram::setSource( ShaderType target, glsl::Shader const & source )
	{
		if ( m_shaders[size_t( target )] )
		{
			m_shaders[size_t( target )]->setSource( source );
		}
		else
		{
			CASTOR_EXCEPTION( "setting the source of a non available shader object" );
		}

		resetToCompile();
	}

	String ShaderProgram::getSource( ShaderType target )const
	{
		REQUIRE( m_shaders[size_t( target )] );
		String strReturn;

		if ( m_shaders[size_t( target )] )
		{
			strReturn = m_shaders[size_t( target )]->getSource();
		}

		return strReturn;
	}

	bool ShaderProgram::hasSource( ShaderType target )const
	{
		REQUIRE( m_shaders[size_t( target )] );
		bool result = false;

		if ( m_shaders[size_t( target )] )
		{
			result = m_shaders[size_t( target )]->hasSource();
		}

		return result;
	}

	bool ShaderProgram::hasObject( ShaderType target )const
	{
		return m_shaders[size_t( target )] && m_shaders[size_t( target )]->hasSource();
	}

	ShaderStatus ShaderProgram::getObjectStatus( ShaderType target )const
	{
		REQUIRE( m_shaders[size_t( target )] );
		ShaderStatus result = ShaderStatus::edontExist;

		if ( m_shaders[size_t( target )] )
		{
			result = m_shaders[size_t( target )]->getStatus();
		}

		return result;
	}

	PushUniformSPtr ShaderProgram::createUniform( UniformType type
		, String const & name
		, ShaderType shader
		, int nbOccurences )
	{
		REQUIRE( m_shaders[size_t( shader )] );
		PushUniformSPtr result;

		if ( m_shaders[size_t( shader )] )
		{
			result = m_shaders[size_t( shader )]->createUniform( type, name, nbOccurences );
		}

		return result;
	}

	PushUniformSPtr ShaderProgram::findUniform( UniformType type
		, castor::String const & name
		, ShaderType shader )const
	{
		REQUIRE( m_shaders[size_t( shader )] );
		PushUniformSPtr result;

		if ( m_shaders[size_t( shader )] )
		{
			result = m_shaders[size_t( shader )]->findUniform( type, name );
		}

		return result;
	}

	AtomicCounterBuffer & ShaderProgram::createAtomicCounterBuffer( String const & name, ShaderTypeFlags const & shaderMask )
	{
		auto it = m_atomicCounterBuffersByName.find( name );

		if ( it == m_atomicCounterBuffersByName.end() )
		{
			auto ssbo = std::make_shared< AtomicCounterBuffer >( name, *this );
			m_listAtomicCounterBuffers.push_back( ssbo );
			it = m_atomicCounterBuffersByName.insert( { name, ssbo } ).first;

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); ++i )
			{
				if ( checkFlag( shaderMask, uint8_t( 0x01 << i ) ) )
				{
					REQUIRE( m_shaders[i] );
					m_atomicCounterBuffers[i].push_back( ssbo );
				}
			}
		}

		return *it->second.lock();
	}

	AtomicCounterBufferSPtr ShaderProgram::findAtomicCounterBuffer( castor::String const & name )const
	{
		AtomicCounterBufferSPtr buffer;
		auto it = m_atomicCounterBuffersByName.find( name );

		if ( it != m_atomicCounterBuffersByName.end() )
		{
			buffer = it->second.lock();
		}

		return buffer;
	}

	PushUniformList & ShaderProgram::getUniforms( ShaderType type )
	{
		REQUIRE( m_shaders[size_t( type )] );
		return m_shaders[size_t( type )]->getUniforms();
	}

	PushUniformList const & ShaderProgram::getUniforms( ShaderType type )const
	{
		REQUIRE( m_shaders[size_t( type )] );
		return m_shaders[size_t( type )]->getUniforms();
	}

	bool ShaderProgram::doInitialise()
	{
		if ( m_status == ProgramStatus::eNotLinked )
		{
			m_activeShaders.clear();

			for ( auto shader : m_shaders )
			{
				if ( shader && shader->hasSource() )
				{
					shader->destroy();
					shader->create();

					if ( !shader->compile() && shader->getStatus() == ShaderStatus::eError )
					{
						Logger::logError( cuT( "ShaderProgram::Initialise - COMPILER ERROR" ) );
						StringStream source;
						source << format::LinePrefix();
						source << shader->getSource();
						Logger::logWarning( source.str() );
						shader->destroy();
						m_status = ProgramStatus::eError;
					}
					else
					{
						shader->attachTo( *this );
						m_activeShaders.push_back( shader );
					}
				}
			}

			if ( m_status != ProgramStatus::eError )
			{
				if ( !link() )
				{
					Logger::logError( cuT( "ShaderProgram::Initialise - LINKER ERROR" ) );

					for ( auto shader : m_activeShaders )
					{
						StringStream source;
						source << format::LinePrefix();
						source << shader->getSource();
						Logger::logWarning( source.str() );
						shader->destroy();
					}

					m_status = ProgramStatus::eError;
				}
				else
				{
					Logger::logDebug( cuT( "ShaderProgram::Initialise - Program Linked successfully" ) );
				}
			}
		}

		return m_status == ProgramStatus::eLinked;
	}

	void ShaderProgram::doBind()const
	{
		if ( m_status == ProgramStatus::eLinked )
		{
			for ( auto shader : m_activeShaders )
			{
				shader->bind();
			}
		}
	}

	void ShaderProgram::doUnbind()const
	{
		if ( m_status == ProgramStatus::eLinked )
		{
			for ( auto shader : m_activeShaders )
			{
				shader->unbind();
			}
		}
	}

	bool ShaderProgram::doLink()
	{
		if ( m_status != ProgramStatus::eError )
		{
			if ( m_status != ProgramStatus::eLinked )
			{
				for ( auto shader : m_activeShaders )
				{
					for ( auto it : shader->getUniforms() )
					{
						it->initialise();
					}
				}

				m_status = ProgramStatus::eLinked;
			}
		}

		return m_status == ProgramStatus::eLinked;
	}
}
