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
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream, format::BasePrefixer< CharType, PrefixType > const & Prefix )
		{
			format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * sbuf = dynamic_cast< format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				sbuf = format::installPrefixBuffer< PrefixType >( stream );
				stream.register_callback( format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}
	}

	//*************************************************************************************************

	ShaderProgram::TextWriter::TextWriter( String const & p_tabs, String const & p_name )
		: castor::TextWriter< ShaderProgram >{ p_tabs }
		, m_name{ p_name }
	{
	}

	bool ShaderProgram::TextWriter::operator()( ShaderProgram const & p_shaderProgram, TextFile & p_file )
	{
		bool result = false;
		bool hasFile = false;
		uint8_t i = 0;
		uint8_t j = 0;

		while ( i < uint8_t( ShaderType::eCount ) && !hasFile )
		{
			if ( p_shaderProgram.hasObject( ShaderType( i ) ) )
			{
				hasFile = !p_shaderProgram.getFile( ShaderType( i ) ).empty();
			}

			++i;
		}

		if ( hasFile )
		{
			auto tabs = m_tabs + cuT( "\t" );
			ShaderObjectSPtr object;
			result = p_file.writeText( cuT( "\n" ) + m_tabs + m_name + cuT( "\n" ) ) > 0
					   && p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			checkError( result, "Shader program" );

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ) && result; i++ )
			{
				if ( p_shaderProgram.hasObject( ShaderType( i ) ) )
				{
					result = ShaderObject::TextWriter( tabs )( *p_shaderProgram.m_shaders[i], p_file );
				}
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
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

	ShaderObjectSPtr ShaderProgram::createObject( ShaderType p_type )
	{
		ShaderObjectSPtr result;
		REQUIRE( p_type > ShaderType::eNone && p_type < ShaderType::eCount );

		if ( p_type > ShaderType::eNone && p_type < ShaderType::eCount )
		{
			result = doCreateObject( p_type );
			m_shaders[size_t( p_type )] = result;
			size_t i = size_t( ShaderModel::eModel1 );

			if ( !m_file.empty() )
			{
				m_shaders[size_t( p_type )]->setFile( m_file );
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

	void ShaderProgram::setFile( Path const & p_path )
	{
		m_file = p_path;
		uint8_t i = uint8_t( ShaderType::eVertex );

		for ( auto shader : m_shaders )
		{
			if ( shader && getRenderSystem()->getGpuInformations().hasShaderType( ShaderType( i++ ) ) )
			{
				shader->setFile( p_path );
			}
		}

		resetToCompile();
	}

	void ShaderProgram::resetToCompile()
	{
		m_status = ProgramStatus::eNotLinked;
	}

	void ShaderProgram::setInputType( ShaderType p_target, Topology p_topology )
	{
		REQUIRE( m_shaders[size_t( p_target )] && p_target == ShaderType::eGeometry );
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->setInputType( p_topology );
		}
	}

	void ShaderProgram::setOutputType( ShaderType p_target, Topology p_topology )
	{
		REQUIRE( m_shaders[size_t( p_target )] && p_target == ShaderType::eGeometry );
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->setOutputType( p_topology );
		}
	}

	void ShaderProgram::setOutputVtxCount( ShaderType p_target, uint8_t p_count )
	{
		REQUIRE( m_shaders[size_t( p_target )] && p_target == ShaderType::eGeometry );
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->setOutputVtxCount( p_count );
		}
	}

	void ShaderProgram::setFile( ShaderType p_target, Path const & p_pathFile )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->setFile( p_pathFile );
		}
		else
		{
			CASTOR_EXCEPTION( "setting the source file of a non available shader object" );
		}

		resetToCompile();
	}

	Path ShaderProgram::getFile( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		Path pathReturn;

		if ( m_shaders[size_t( p_target )] )
		{
			pathReturn = m_shaders[size_t( p_target )]->getFile();
		}

		return pathReturn;
	}

	bool ShaderProgram::hasFile( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		bool result = false;

		if ( m_shaders[size_t( p_target )] )
		{
			result = m_shaders[size_t( p_target )]->hasFile();
		}

		return result;
	}

	void ShaderProgram::setSource( ShaderType p_target, String const & p_source )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->setSource( p_source );
		}
		else
		{
			CASTOR_EXCEPTION( "setting the source of a non available shader object" );
		}

		resetToCompile();
	}

	void ShaderProgram::setSource( ShaderType p_target, glsl::Shader const & p_source )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->setSource( p_source );
		}
		else
		{
			CASTOR_EXCEPTION( "setting the source of a non available shader object" );
		}

		resetToCompile();
	}

	String ShaderProgram::getSource( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		String strReturn;

		if ( m_shaders[size_t( p_target )] )
		{
			strReturn = m_shaders[size_t( p_target )]->getSource();
		}

		return strReturn;
	}

	bool ShaderProgram::hasSource( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		bool result = false;

		if ( m_shaders[size_t( p_target )] )
		{
			result = m_shaders[size_t( p_target )]->hasSource();
		}

		return result;
	}

	bool ShaderProgram::hasObject( ShaderType p_target )const
	{
		return m_shaders[size_t( p_target )] && m_shaders[size_t( p_target )]->hasSource();
	}

	ShaderStatus ShaderProgram::getObjectStatus( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		ShaderStatus result = ShaderStatus::edontExist;

		if ( m_shaders[size_t( p_target )] )
		{
			result = m_shaders[size_t( p_target )]->getStatus();
		}

		return result;
	}

	PushUniformSPtr ShaderProgram::createUniform( UniformType p_type, String const & p_name, ShaderType p_shader, int p_iNbOcc )
	{
		REQUIRE( m_shaders[size_t( p_shader )] );
		PushUniformSPtr result = findUniform( p_type, p_name, p_shader );

		if ( !result )
		{
			result = doCreateUniform( p_type, p_iNbOcc );
			result->getBaseUniform().setName( p_name );

			if ( m_shaders[size_t( p_shader )] )
			{
				m_shaders[size_t( p_shader )]->addUniform( result );
			}
		}

		return result;
	}

	PushUniformSPtr ShaderProgram::findUniform( UniformType p_type, castor::String const & p_name, ShaderType p_shader )const
	{
		REQUIRE( m_shaders[size_t( p_shader )] );
		PushUniformSPtr result;

		if ( m_shaders[size_t( p_shader )] )
		{
			result = m_shaders[size_t( p_shader )]->findUniform( p_name );

			if ( result && result->getBaseUniform().getFullType() != p_type )
			{
				Logger::logError( cuT( "Frame variable named " ) + p_name + cuT( " exists but with a different type" ) );
				result.reset();
			}
		}

		return result;
	}

	AtomicCounterBuffer & ShaderProgram::createAtomicCounterBuffer( String const & p_name, ShaderTypeFlags const & p_shaderMask )
	{
		auto it = m_atomicCounterBuffersByName.find( p_name );

		if ( it == m_atomicCounterBuffersByName.end() )
		{
			auto ssbo = std::make_shared< AtomicCounterBuffer >( p_name, *this );
			m_listAtomicCounterBuffers.push_back( ssbo );
			it = m_atomicCounterBuffersByName.insert( { p_name, ssbo } ).first;

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); ++i )
			{
				if ( checkFlag( p_shaderMask, uint8_t( 0x01 << i ) ) )
				{
					REQUIRE( m_shaders[i] );
					m_atomicCounterBuffers[i].push_back( ssbo );
				}
			}
		}

		return *it->second.lock();
	}

	AtomicCounterBufferSPtr ShaderProgram::findAtomicCounterBuffer( castor::String const & p_name )const
	{
		AtomicCounterBufferSPtr buffer;
		auto it = m_atomicCounterBuffersByName.find( p_name );

		if ( it != m_atomicCounterBuffersByName.end() )
		{
			buffer = it->second.lock();
		}

		return buffer;
	}

	PushUniformList & ShaderProgram::getUniforms( ShaderType p_type )
	{
		REQUIRE( m_shaders[size_t( p_type )] );
		return m_shaders[size_t( p_type )]->getUniforms();
	}

	PushUniformList const & ShaderProgram::getUniforms( ShaderType p_type )const
	{
		REQUIRE( m_shaders[size_t( p_type )] );
		return m_shaders[size_t( p_type )]->getUniforms();
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
