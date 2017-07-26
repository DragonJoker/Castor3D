#include "ShaderProgram.hpp"

#include "Render/RenderSystem.hpp"
#include "Shader/AtomicCounterBuffer.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/ShaderStorageBuffer.hpp"

#include <Stream/StreamPrefixManipulators.hpp>
#include <GlslShader.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream, format::base_prefixer< CharType, PrefixType > const & prefix )
		{
			format::basic_prefix_buffer< format::base_prefixer< CharType, PrefixType >, CharType > * sbuf = dynamic_cast< format::basic_prefix_buffer< format::base_prefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				sbuf = format::install_prefix_buffer< PrefixType >( stream );
				stream.register_callback( format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}
	}

	//*************************************************************************************************

	ShaderProgram::TextWriter::TextWriter( String const & p_tabs, String const & p_name )
		: Castor::TextWriter< ShaderProgram >{ p_tabs }
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
			if ( p_shaderProgram.HasObject( ShaderType( i ) ) )
			{
				hasFile = !p_shaderProgram.GetFile( ShaderType( i ) ).empty();
			}

			++i;
		}

		if ( hasFile )
		{
			auto tabs = m_tabs + cuT( "\t" );
			ShaderObjectSPtr object;
			result = p_file.WriteText( cuT( "\n" ) + m_tabs + m_name + cuT( "\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			CheckError( result, "Shader program" );

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ) && result; i++ )
			{
				if ( p_shaderProgram.HasObject( ShaderType( i ) ) )
				{
					result = ShaderObject::TextWriter( tabs )( *p_shaderProgram.m_shaders[i], p_file );
				}
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
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
			p->Apply();
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

	const String ShaderProgram::CameraPos = cuT( "c3d_v3CameraPosition" );
	const String ShaderProgram::CameraFarPlane = cuT( "c3d_fCameraFarPlane" );
	const String ShaderProgram::CameraNearPlane = cuT( "c3d_fCameraNearPlane" );
	const String ShaderProgram::AmbientLight = cuT( "c3d_v4AmbientLight" );
	const String ShaderProgram::BackgroundColour = cuT( "c3d_v4BackgroundColour" );
	const String ShaderProgram::FogType = cuT( "c3d_iFogType" );
	const String ShaderProgram::FogDensity = cuT( "c3d_fFogDensity" );
	const String ShaderProgram::LightsCount = cuT( "c3d_lightsCount" );
	const String ShaderProgram::Lights = cuT( "c3d_sLights" );
	const String ShaderProgram::Exposure = cuT( "c3d_fExposure" );
	const String ShaderProgram::Gamma = cuT( "c3d_fGamma" );
	const String ShaderProgram::MatDiffuse = cuT( "c3d_v4MatDiffuse" );
	const String ShaderProgram::MatSpecular = cuT( "c3d_v4MatSpecular" );
	const String ShaderProgram::MatEmissive = cuT( "c3d_v4MatEmissive" );
	const String ShaderProgram::MatShininess = cuT( "c3d_fMatShininess" );
	const String ShaderProgram::MatAlbedo = cuT( "c3d_v4MatAlbedo" );
	const String ShaderProgram::MatRoughness = cuT( "c3d_fRoughness" );
	const String ShaderProgram::MatMetallic = cuT( "c3d_fMetallic" );
	const String ShaderProgram::MatOpacity = cuT( "c3d_fMatOpacity" );
	const String ShaderProgram::MatRefractionRatio = cuT( "c3d_refractionRatio" );
	const String ShaderProgram::Time = cuT( "c3d_fTime" );
	const String ShaderProgram::Bones = cuT( "c3d_mtxBones" );
	const String ShaderProgram::Dimensions = cuT( "c3d_v2iDimensions" );
	const String ShaderProgram::WindowSize = cuT( "c3d_v2iWindowSize" );
	const String ShaderProgram::OvPosition = cuT( "c3d_v2iPosition" );
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
	const String ShaderProgram::MapShadow = cuT( "c3d_mapShadow" );
	const String ShaderProgram::MapText = cuT( "c3d_mapText" );
	const String ShaderProgram::ShadowReceiver = cuT( "c3d_shadowReceiver" );
	const String ShaderProgram::MaterialIndex = cuT( "c3d_materialIndex" );
	const String ShaderProgram::EnvironmentIndex = cuT( "c3d_envMapIndex" );

	const String ShaderProgram::BufferMatrix = cuT( "Matrices" );
	const String ShaderProgram::BufferModelMatrix = cuT( "ModelMatrices" );
	const String ShaderProgram::BufferHdrConfig = cuT( "HdrConfig" );
	const String ShaderProgram::BufferScene = cuT( "Scene" );
	const String ShaderProgram::BufferPass = cuT( "Pass" );
	const String ShaderProgram::BufferModel = cuT( "Model" );
	const String ShaderProgram::BufferBillboards = cuT( "Billboards" );
	const String ShaderProgram::BufferSkinning = cuT( "Skinning" );
	const String ShaderProgram::BufferMorphing = cuT( "Morphing" );
	const String ShaderProgram::BufferOverlay = cuT( "Overlay" );

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	ShaderObjectSPtr ShaderProgram::CreateObject( ShaderType p_type )
	{
		ShaderObjectSPtr result;
		REQUIRE( p_type > ShaderType::eNone && p_type < ShaderType::eCount );

		if ( p_type > ShaderType::eNone && p_type < ShaderType::eCount )
		{
			result = DoCreateObject( p_type );
			m_shaders[size_t( p_type )] = result;
			size_t i = size_t( ShaderModel::eModel1 );

			if ( !m_file.empty() )
			{
				m_shaders[size_t( p_type )]->SetFile( m_file );
			}
		}

		return result;
	}

	void ShaderProgram::DoCleanup()
	{
		for ( auto shader : m_activeShaders )
		{
			if ( shader )
			{
				shader->Detach();
				shader->FlushUniforms();
				shader->Destroy();
			}
		}

		m_activeShaders.clear();
		m_file.clear();
	}

	void ShaderProgram::SetFile( Path const & p_path )
	{
		m_file = p_path;
		uint8_t i = uint8_t( ShaderType::eVertex );

		for ( auto shader : m_shaders )
		{
			if ( shader && GetRenderSystem()->GetGpuInformations().HasShaderType( ShaderType( i++ ) ) )
			{
				shader->SetFile( p_path );
			}
		}

		ResetToCompile();
	}

	void ShaderProgram::ResetToCompile()
	{
		m_status = ProgramStatus::eNotLinked;
	}

	void ShaderProgram::SetInputType( ShaderType p_target, Topology p_topology )
	{
		REQUIRE( m_shaders[size_t( p_target )] && p_target == ShaderType::eGeometry );
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->SetInputType( p_topology );
		}
	}

	void ShaderProgram::SetOutputType( ShaderType p_target, Topology p_topology )
	{
		REQUIRE( m_shaders[size_t( p_target )] && p_target == ShaderType::eGeometry );
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->SetOutputType( p_topology );
		}
	}

	void ShaderProgram::SetOutputVtxCount( ShaderType p_target, uint8_t p_count )
	{
		REQUIRE( m_shaders[size_t( p_target )] && p_target == ShaderType::eGeometry );
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->SetOutputVtxCount( p_count );
		}
	}

	void ShaderProgram::SetFile( ShaderType p_target, Path const & p_pathFile )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->SetFile( p_pathFile );
		}
		else
		{
			CASTOR_EXCEPTION( "Setting the source file of a non available shader object" );
		}

		ResetToCompile();
	}

	Path ShaderProgram::GetFile( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		Path pathReturn;

		if ( m_shaders[size_t( p_target )] )
		{
			pathReturn = m_shaders[size_t( p_target )]->GetFile();
		}

		return pathReturn;
	}

	bool ShaderProgram::HasFile( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		bool result = false;

		if ( m_shaders[size_t( p_target )] )
		{
			result = m_shaders[size_t( p_target )]->HasFile();
		}

		return result;
	}

	void ShaderProgram::SetSource( ShaderType p_target, String const & p_source )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->SetSource( p_source );
		}
		else
		{
			CASTOR_EXCEPTION( "Setting the source of a non available shader object" );
		}

		ResetToCompile();
	}

	void ShaderProgram::SetSource( ShaderType p_target, GLSL::Shader const & p_source )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			m_shaders[size_t( p_target )]->SetSource( p_source );
		}
		else
		{
			CASTOR_EXCEPTION( "Setting the source of a non available shader object" );
		}

		ResetToCompile();
	}

	String ShaderProgram::GetSource( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		String strReturn;

		if ( m_shaders[size_t( p_target )] )
		{
			strReturn = m_shaders[size_t( p_target )]->GetSource();
		}

		return strReturn;
	}

	bool ShaderProgram::HasSource( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		bool result = false;

		if ( m_shaders[size_t( p_target )] )
		{
			result = m_shaders[size_t( p_target )]->HasSource();
		}

		return result;
	}

	bool ShaderProgram::HasObject( ShaderType p_target )const
	{
		return m_shaders[size_t( p_target )] && m_shaders[size_t( p_target )]->HasSource();
	}

	ShaderStatus ShaderProgram::GetObjectStatus( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		ShaderStatus result = ShaderStatus::eDontExist;

		if ( m_shaders[size_t( p_target )] )
		{
			result = m_shaders[size_t( p_target )]->GetStatus();
		}

		return result;
	}

	PushUniformSPtr ShaderProgram::CreateUniform( UniformType p_type, String const & p_name, ShaderType p_shader, int p_iNbOcc )
	{
		REQUIRE( m_shaders[size_t( p_shader )] );
		PushUniformSPtr result = FindUniform( p_type, p_name, p_shader );

		if ( !result )
		{
			result = DoCreateUniform( p_type, p_iNbOcc );
			result->GetBaseUniform().SetName( p_name );

			if ( m_shaders[size_t( p_shader )] )
			{
				m_shaders[size_t( p_shader )]->AddUniform( result );
			}
		}

		return result;
	}

	PushUniformSPtr ShaderProgram::FindUniform( UniformType p_type, Castor::String const & p_name, ShaderType p_shader )const
	{
		REQUIRE( m_shaders[size_t( p_shader )] );
		PushUniformSPtr result;

		if ( m_shaders[size_t( p_shader )] )
		{
			result = m_shaders[size_t( p_shader )]->FindUniform( p_name );

			if ( result && result->GetBaseUniform().GetFullType() != p_type )
			{
				Logger::LogError( cuT( "Frame variable named " ) + p_name + cuT( " exists but with a different type" ) );
				result.reset();
			}
		}

		return result;
	}

	AtomicCounterBuffer & ShaderProgram::CreateAtomicCounterBuffer( String const & p_name, ShaderTypeFlags const & p_shaderMask )
	{
		auto it = m_atomicCounterBuffersByName.find( p_name );

		if ( it == m_atomicCounterBuffersByName.end() )
		{
			auto ssbo = std::make_shared< AtomicCounterBuffer >( p_name, *this );
			m_listAtomicCounterBuffers.push_back( ssbo );
			it = m_atomicCounterBuffersByName.insert( { p_name, ssbo } ).first;

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); ++i )
			{
				if ( CheckFlag( p_shaderMask, uint8_t( 0x01 << i ) ) )
				{
					REQUIRE( m_shaders[i] );
					m_atomicCounterBuffers[i].push_back( ssbo );
				}
			}
		}

		return *it->second.lock();
	}

	AtomicCounterBufferSPtr ShaderProgram::FindAtomicCounterBuffer( Castor::String const & p_name )const
	{
		AtomicCounterBufferSPtr buffer;
		auto it = m_atomicCounterBuffersByName.find( p_name );

		if ( it != m_atomicCounterBuffersByName.end() )
		{
			buffer = it->second.lock();
		}

		return buffer;
	}

	PushUniformList & ShaderProgram::GetUniforms( ShaderType p_type )
	{
		REQUIRE( m_shaders[size_t( p_type )] );
		return m_shaders[size_t( p_type )]->GetUniforms();
	}

	PushUniformList const & ShaderProgram::GetUniforms( ShaderType p_type )const
	{
		REQUIRE( m_shaders[size_t( p_type )] );
		return m_shaders[size_t( p_type )]->GetUniforms();
	}

	bool ShaderProgram::DoInitialise()
	{
		if ( m_status == ProgramStatus::eNotLinked )
		{
			m_activeShaders.clear();

			for ( auto shader : m_shaders )
			{
				if ( shader && shader->HasSource() )
				{
					shader->Destroy();
					shader->Create();

					if ( !shader->Compile() && shader->GetStatus() == ShaderStatus::eError )
					{
						Logger::LogError( cuT( "ShaderProgram::Initialise - COMPILER ERROR" ) );
						shader->Destroy();
						m_status = ProgramStatus::eError;
					}
					else
					{
						shader->AttachTo( *this );
						m_activeShaders.push_back( shader );
					}
				}
			}

			if ( !Link() )
			{
				Logger::LogError( cuT( "ShaderProgram::Initialise - LINKER ERROR" ) );

				for ( auto shader : m_activeShaders )
				{
					StringStream source;
					source << format::line_prefix();
					source << shader->GetSource();
					Logger::LogDebug( source.str() );
					shader->Destroy();
				}

				m_status = ProgramStatus::eError;
			}
			else
			{
				Logger::LogDebug( cuT( "ShaderProgram::Initialise - Program Linked successfully" ) );
			}
		}

		return m_status == ProgramStatus::eLinked;
	}

	void ShaderProgram::DoBind()const
	{
		if ( m_status == ProgramStatus::eLinked )
		{
			for ( auto shader : m_activeShaders )
			{
				shader->Bind();
			}
		}
	}

	void ShaderProgram::DoUnbind()const
	{
		if ( m_status == ProgramStatus::eLinked )
		{
			for ( auto shader : m_activeShaders )
			{
				shader->Unbind();
			}
		}
	}

	bool ShaderProgram::DoLink()
	{
		if ( m_status != ProgramStatus::eError )
		{
			if ( m_status != ProgramStatus::eLinked )
			{
				for ( auto shader : m_activeShaders )
				{
					for ( auto it : shader->GetUniforms() )
					{
						it->Initialise();
					}
				}

				m_status = ProgramStatus::eLinked;
			}
		}

		return m_status == ProgramStatus::eLinked;
	}
}
