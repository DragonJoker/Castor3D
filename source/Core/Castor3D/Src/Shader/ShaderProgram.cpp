#include "ShaderProgram.hpp"

#include "Render/RenderSystem.hpp"
#include "Shader/AtomicCounterBuffer.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/ShaderStorageBuffer.hpp"

#include <Stream/StreamPrefixManipulators.hpp>

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
		bool l_return = false;
		bool l_hasFile = false;
		uint8_t i = 0;
		uint8_t j = 0;

		while ( i < uint8_t( ShaderType::eCount ) && !l_hasFile )
		{
			if ( p_shaderProgram.HasObject( ShaderType( i ) ) )
			{
				while ( j < uint8_t( ShaderModel::eCount ) && !l_hasFile )
				{
					l_hasFile = !p_shaderProgram.GetFile( ShaderType( i ), ShaderModel( j ) ).empty();
					++j;
				}
			}

			++i;
		}

		if ( l_hasFile )
		{
			auto l_tabs = m_tabs + cuT( "\t" );
			ShaderObjectSPtr l_object;
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + m_name + cuT( "\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			CheckError( l_return, "Shader program" );

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ) && l_return; i++ )
			{
				if ( p_shaderProgram.HasObject( ShaderType( i ) ) )
				{
					l_return = ShaderObject::TextWriter( l_tabs )( *p_shaderProgram.m_shaders[i], p_file );
				}
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}
		else
		{
			l_return = true;
		}

		return l_return;
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

	const String ShaderProgram::CameraPos = cuT( "c3d_v3CameraPosition" );
	const String ShaderProgram::CameraFarPlane = cuT( "c3d_fCameraFarPlane" );
	const String ShaderProgram::AmbientLight = cuT( "c3d_v4AmbientLight" );
	const String ShaderProgram::BackgroundColour = cuT( "c3d_v4BackgroundColour" );
	const String ShaderProgram::FogType = cuT( "c3d_iFogType" );
	const String ShaderProgram::FogDensity = cuT( "c3d_fFogDensity" );
	const String ShaderProgram::LightsCount = cuT( "c3d_iLightsCount" );
	const String ShaderProgram::Lights = cuT( "c3d_sLights" );
	const String ShaderProgram::MatDiffuse = cuT( "c3d_v4MatDiffuse" );
	const String ShaderProgram::MatSpecular = cuT( "c3d_v4MatSpecular" );
	const String ShaderProgram::MatEmissive = cuT( "c3d_v4MatEmissive" );
	const String ShaderProgram::MatShininess = cuT( "c3d_fMatShininess" );
	const String ShaderProgram::MatOpacity = cuT( "c3d_fMatOpacity" );
	const String ShaderProgram::Time = cuT( "c3d_fTime" );
	const String ShaderProgram::Bones = cuT( "c3d_mtxBones" );
	const String ShaderProgram::Dimensions = cuT( "c3d_v2iDimensions" );
	const String ShaderProgram::WindowSize = cuT( "c3d_v2iWindowSize" );
	const String ShaderProgram::OvPosition = cuT( "c3d_v2iPosition" );
	const String ShaderProgram::MapDiffuse = cuT( "c3d_mapDiffuse" );
	const String ShaderProgram::MapSpecular = cuT( "c3d_mapSpecular" );
	const String ShaderProgram::MapEmissive = cuT( "c3d_mapEmissive" );
	const String ShaderProgram::MapNormal = cuT( "c3d_mapNormal" );
	const String ShaderProgram::MapOpacity = cuT( "c3d_mapOpacity" );
	const String ShaderProgram::MapGloss = cuT( "c3d_mapGloss" );
	const String ShaderProgram::MapHeight = cuT( "c3d_mapHeight" );
	const String ShaderProgram::MapReflection = cuT( "c3d_mapReflection" );
	const String ShaderProgram::MapShadow = cuT( "c3d_mapShadow" );
	const String ShaderProgram::MapText = cuT( "c3d_mapText" );
	const String ShaderProgram::ShadowReceiver = cuT( "c3d_iShadowReceiver" );

	const String ShaderProgram::BufferMatrix = cuT( "Matrices" );
	const String ShaderProgram::BufferModelMatrix = cuT( "ModelMatrices" );
	const String ShaderProgram::BufferScene = cuT( "Scene" );
	const String ShaderProgram::BufferPass = cuT( "Pass" );
	const String ShaderProgram::BufferModel = cuT( "Model" );
	const String ShaderProgram::BufferBillboards = cuT( "Billboards" );
	const String ShaderProgram::BufferSkinning = cuT( "Skinning" );
	const String ShaderProgram::BufferMorphing = cuT( "Morphing" );
	const String ShaderProgram::BufferOverlay = cuT( "Overlay" );

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	ShaderObjectSPtr ShaderProgram::CreateObject( ShaderType p_type )
	{
		ShaderObjectSPtr l_return;
		REQUIRE( p_type > ShaderType::eNone && p_type < ShaderType::eCount );

		if ( p_type > ShaderType::eNone && p_type < ShaderType::eCount )
		{
			l_return = DoCreateObject( p_type );
			m_shaders[size_t( p_type )] = l_return;
			size_t i = size_t( ShaderModel::eModel1 );

			for ( auto const & l_file : m_arrayFiles )
			{
				if ( !l_file.empty() )
				{
					m_shaders[size_t( p_type )]->SetFile( ShaderModel( i++ ), l_file );
				}
			}
		}

		return l_return;
	}

	void ShaderProgram::DoCleanup()
	{
		for ( auto l_shader : m_activeShaders )
		{
			if ( l_shader )
			{
				l_shader->Detach();
				l_shader->FlushUniforms();
				l_shader->Destroy();
			}
		}

		m_activeShaders.clear();
		clear_container( m_arrayFiles );
	}

	void ShaderProgram::SetFile( ShaderModel p_eModel, Path const & p_path )
	{
		m_arrayFiles[size_t( p_eModel )] = p_path;
		uint8_t i = uint8_t( ShaderType::eVertex );

		for ( auto l_shader : m_shaders )
		{
			if ( l_shader && GetRenderSystem()->GetGpuInformations().HasShaderType( ShaderType( i++ ) ) )
			{
				l_shader->SetFile( p_eModel, p_path );
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

	void ShaderProgram::SetFile( ShaderType p_target, ShaderModel p_eModel, Path const & p_pathFile )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			if ( p_eModel == ShaderModel::eCount )
			{
				for ( uint8_t i = 0; i < uint8_t( ShaderModel::eCount ); ++i )
				{
					if ( GetRenderSystem()->GetGpuInformations().CheckSupport( ShaderModel( i ) ) )
					{
						m_shaders[size_t( p_target )]->SetFile( ShaderModel( i ), p_pathFile );
					}
				}
			}
			else
			{
				m_shaders[size_t( p_target )]->SetFile( p_eModel, p_pathFile );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Setting the source file of a non available shader object" );
		}

		ResetToCompile();
	}

	Path ShaderProgram::GetFile( ShaderType p_target, ShaderModel p_eModel )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		Path l_pathReturn;

		if ( m_shaders[size_t( p_target )] )
		{
			l_pathReturn = m_shaders[size_t( p_target )]->GetFile( p_eModel );
		}

		return l_pathReturn;
	}

	bool ShaderProgram::HasFile( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		bool l_return = false;

		if ( m_shaders[size_t( p_target )] )
		{
			l_return = m_shaders[size_t( p_target )]->HasFile();
		}

		return l_return;
	}

	void ShaderProgram::SetSource( ShaderType p_target, ShaderModel p_eModel, String const & p_strSource )
	{
		if ( m_shaders[size_t( p_target )] )
		{
			if ( p_eModel == ShaderModel::eCount )
			{
				for ( uint8_t i = 0; i < uint8_t( ShaderModel::eCount ); ++i )
				{
					if ( GetRenderSystem()->GetGpuInformations().CheckSupport( ShaderModel( i ) ) )
					{
						m_shaders[size_t( p_target )]->SetSource( ShaderModel( i ), p_strSource );
					}
				}
			}
			else
			{
				m_shaders[size_t( p_target )]->SetSource( p_eModel, p_strSource );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Setting the source of a non available shader object" );
		}

		ResetToCompile();
	}

	String ShaderProgram::GetSource( ShaderType p_target, ShaderModel p_eModel )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		String l_strReturn;

		if ( m_shaders[size_t( p_target )] )
		{
			l_strReturn = m_shaders[size_t( p_target )]->GetSource( p_eModel );
		}

		return l_strReturn;
	}

	bool ShaderProgram::HasSource( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		bool l_return = false;

		if ( m_shaders[size_t( p_target )] )
		{
			l_return = m_shaders[size_t( p_target )]->HasSource();
		}

		return l_return;
	}

	bool ShaderProgram::HasObject( ShaderType p_target )const
	{
		return m_shaders[size_t( p_target )] && m_shaders[size_t( p_target )]->HasSource();
	}

	ShaderStatus ShaderProgram::GetObjectStatus( ShaderType p_target )const
	{
		REQUIRE( m_shaders[size_t( p_target )] );
		ShaderStatus l_return = ShaderStatus::eDontExist;

		if ( m_shaders[size_t( p_target )] )
		{
			l_return = m_shaders[size_t( p_target )]->GetStatus();
		}

		return l_return;
	}

	PushUniformSPtr ShaderProgram::CreateUniform( UniformType p_type, String const & p_name, ShaderType p_shader, int p_iNbOcc )
	{
		REQUIRE( m_shaders[size_t( p_shader )] );
		PushUniformSPtr l_return = FindUniform( p_type, p_name, p_shader );

		if ( !l_return )
		{
			l_return = DoCreateUniform( p_type, p_iNbOcc );
			l_return->GetBaseUniform().SetName( p_name );

			if ( m_shaders[size_t( p_shader )] )
			{
				m_shaders[size_t( p_shader )]->AddUniform( l_return );
			}
		}

		return l_return;
	}

	PushUniformSPtr ShaderProgram::FindUniform( UniformType p_type, Castor::String const & p_name, ShaderType p_shader )const
	{
		REQUIRE( m_shaders[size_t( p_shader )] );
		PushUniformSPtr l_return;

		if ( m_shaders[size_t( p_shader )] )
		{
			l_return = m_shaders[size_t( p_shader )]->FindUniform( p_name );

			if ( l_return && l_return->GetBaseUniform().GetFullType() != p_type )
			{
				Logger::LogError( cuT( "Frame variable named " ) + p_name + cuT( " exists but with a different type" ) );
				l_return.reset();
			}
		}

		return l_return;
	}

	ShaderStorageBuffer & ShaderProgram::CreateStorageBuffer( Castor::String const & p_name, ShaderTypeFlags const & p_shaderMask )
	{
		auto l_it = m_storageBuffersByName.find( p_name );

		if ( l_it == m_storageBuffersByName.end() )
		{
			auto l_ssbo = std::make_shared< ShaderStorageBuffer >( p_name, *this );
			m_listStorageBuffers.push_back( l_ssbo );
			l_it = m_storageBuffersByName.insert( { p_name, l_ssbo } ).first;

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); ++i )
			{
				if ( CheckFlag( p_shaderMask, uint8_t( 0x01 << i ) ) )
				{
					REQUIRE( m_shaders[i] );
					m_storageBuffers[i].push_back( l_ssbo );
				}
			}
		}

		return *l_it->second.lock();
	}

	ShaderStorageBufferSPtr ShaderProgram::FindStorageBuffer( Castor::String const & p_name )const
	{
		ShaderStorageBufferSPtr l_buffer;
		auto l_it = m_storageBuffersByName.find( p_name );

		if ( l_it != m_storageBuffersByName.end() )
		{
			l_buffer = l_it->second.lock();
		}

		return l_buffer;
	}

	AtomicCounterBuffer & ShaderProgram::CreateAtomicCounterBuffer( String const & p_name, ShaderTypeFlags const & p_shaderMask )
	{
		auto l_it = m_atomicCounterBuffersByName.find( p_name );

		if ( l_it == m_atomicCounterBuffersByName.end() )
		{
			auto l_ssbo = std::make_shared< AtomicCounterBuffer >( p_name, *this );
			m_listAtomicCounterBuffers.push_back( l_ssbo );
			l_it = m_atomicCounterBuffersByName.insert( { p_name, l_ssbo } ).first;

			for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); ++i )
			{
				if ( CheckFlag( p_shaderMask, uint8_t( 0x01 << i ) ) )
				{
					REQUIRE( m_shaders[i] );
					m_atomicCounterBuffers[i].push_back( l_ssbo );
				}
			}
		}

		return *l_it->second.lock();
	}

	AtomicCounterBufferSPtr ShaderProgram::FindAtomicCounterBuffer( Castor::String const & p_name )const
	{
		AtomicCounterBufferSPtr l_buffer;
		auto l_it = m_atomicCounterBuffersByName.find( p_name );

		if ( l_it != m_atomicCounterBuffersByName.end() )
		{
			l_buffer = l_it->second.lock();
		}

		return l_buffer;
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

			for ( auto l_shader : m_shaders )
			{
				if ( l_shader && l_shader->HasSource() )
				{
					l_shader->Destroy();
					l_shader->Create();

					if ( !l_shader->Compile() && l_shader->GetStatus() == ShaderStatus::eError )
					{
						Logger::LogError( cuT( "ShaderProgram::Initialise - COMPILER ERROR" ) );
						l_shader->Destroy();
						m_status = ProgramStatus::eError;
					}
					else
					{
						l_shader->AttachTo( *this );
						m_activeShaders.push_back( l_shader );
					}
				}
			}

			if ( !Link() )
			{
				Logger::LogError( cuT( "ShaderProgram::Initialise - LINKER ERROR" ) );

				for ( auto l_shader : m_activeShaders )
				{
					StringStream l_source;
					l_source << format::line_prefix();
					l_source << l_shader->GetLoadedSource();
					Logger::LogDebug( l_source.str() );
					l_shader->Destroy();
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
			for ( auto l_shader : m_activeShaders )
			{
				l_shader->Bind();
			}
		}
	}

	void ShaderProgram::DoUnbind()const
	{
		if ( m_status == ProgramStatus::eLinked )
		{
			for ( auto l_shader : m_activeShaders )
			{
				l_shader->Unbind();
			}
		}
	}

	bool ShaderProgram::DoLink()
	{
		if ( m_status != ProgramStatus::eError )
		{
			if ( m_status != ProgramStatus::eLinked )
			{
				for ( auto l_shader : m_activeShaders )
				{
					for ( auto l_it : l_shader->GetUniforms() )
					{
						l_it->Initialise();
					}
				}

				m_status = ProgramStatus::eLinked;
			}
		}

		return m_status == ProgramStatus::eLinked;
	}
}
