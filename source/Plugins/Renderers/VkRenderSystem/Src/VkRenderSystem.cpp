#include "VkRenderSystem.hpp"

#include <Engine.hpp>
#include <Shader/GlslToSpv.hpp>

#include <CompilerSpirV/compileSpirV.hpp>
#include <CompilerGlsl/compileGlsl.hpp>

#include <Data/BinaryFile.hpp>
#include <Log/Logger.hpp>

#include <Core/VkCreateRenderer.hpp>

#if C3DVkRenderer_HasSPIRVCross
#	include "spirv_cpp.hpp"
#	include "spirv_cross_util.hpp"
#	include "spirv_glsl.hpp"
#endif

#define C3DVkRenderer_DebugSpirV 0

using namespace castor;

namespace VkRender
{
#if C3DVkRenderer_HasSPIRVCross

	namespace
	{
		struct BlockLocale
		{
			BlockLocale()
			{
				m_prvLoc = std::locale( "" );

				if ( m_prvLoc.name() != "C" )
				{
					std::locale::global( std::locale{ "C" } );
				}
			}

			~BlockLocale()
			{
				if ( m_prvLoc.name() != "C" )
				{
					std::locale::global( m_prvLoc );
				}
			}

		private:
			std::locale m_prvLoc;
		};

		spv::ExecutionModel getExecutionModel( ashes::ShaderStageFlag stage )
		{
			spv::ExecutionModel result{};

			switch ( stage )
			{
			case ashes::ShaderStageFlag::eVertex:
				result = spv::ExecutionModelVertex;
				break;
			case ashes::ShaderStageFlag::eGeometry:
				result = spv::ExecutionModelGeometry;
				break;
			case ashes::ShaderStageFlag::eTessellationControl:
				result = spv::ExecutionModelTessellationControl;
				break;
			case ashes::ShaderStageFlag::eTessellationEvaluation:
				result = spv::ExecutionModelTessellationEvaluation;
				break;
			case ashes::ShaderStageFlag::eFragment:
				result = spv::ExecutionModelFragment;
				break;
			case ashes::ShaderStageFlag::eCompute:
				result = spv::ExecutionModelGLCompute;
				break;
			default:
				assert( false && "Unsupported shader stage flag" );
				break;
			}

			return result;
		}

		void doFillConstant( ashes::SpecialisationInfoBase const & specialisationInfo
			, ashes::SpecialisationMapEntry const & entry
			, spirv_cross::SPIRType const & type
			, spirv_cross::SPIRConstant & constant )
		{
			auto offset = entry.offset;
			auto size = type.width * type.vecsize;

			for ( auto col = 0u; col < type.vecsize; ++col )
			{
				for ( auto vec = 0u; vec < type.vecsize; ++vec )
				{
					std::memcpy( &constant.m.c[col].r[vec]
						, specialisationInfo.getData() + offset
						, size );
					offset += size;
				}
			}
		}

		void doSetEntryPoint( ashes::ShaderStageFlag stage
			, spirv_cross::CompilerGLSL & compiler )
		{
			auto model = getExecutionModel( stage );
			std::string entryPoint;

			for ( auto & e : compiler.get_entry_points_and_stages() )
			{
				if ( entryPoint.empty() && e.execution_model == model )
				{
					entryPoint = e.name;
				}
			}

			if ( entryPoint.empty() )
			{
				throw std::runtime_error{ "Could not find an entry point with stage: " + getName( stage ) };
			}

			compiler.set_entry_point( entryPoint, model );
		}

		void doSetupOptions( ashes::Device const & device
			, spirv_cross::CompilerGLSL & compiler )
		{
			auto options = compiler.get_common_options();
			options.version = device.getShaderVersion();
			options.es = false;
			options.separate_shader_objects = true;
			options.enable_420pack_extension = true;
			options.vertex.fixup_clipspace = false;
			options.vertex.flip_vert_y = true;
			options.vertex.support_nonzero_base_instance = true;
			compiler.set_common_options( options );
		}

		std::string compileSpvToGlsl( ashes::Device const & device
			, ashes::UInt32Array const & spv
			, ashes::ShaderStageFlag stage )
		{
			BlockLocale guard;
			auto compiler = std::make_unique< spirv_cross::CompilerGLSL >( spv );
			doSetEntryPoint( stage, *compiler );
			doSetupOptions( device, *compiler );
			return compiler->compile();
		}
	}

#endif

	String RenderSystem::Name = cuT( "Vulkan Renderer" );
	String RenderSystem::Type = cuT( "vulkan" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
		: castor3d::RenderSystem( engine, Name, true, true )
	{
		ashes::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logDebug( msg );
			}
			else
			{
				Logger::logDebugNoNL( msg );
			}
		} );
		ashes::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logInfo( msg );
			}
			else
			{
				Logger::logInfoNoNL( msg );
			}
		} );
		ashes::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logWarning( msg );
			}
			else
			{
				Logger::logWarningNoNL( msg );
			}
		} );
		ashes::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logError( msg );
			}
			else
			{
				Logger::logErrorNoNL( msg );
			}
		} );
		m_renderer.reset( createRenderer( ashes::Renderer::Configuration
		{
			string::stringCast< char >( appName ),
			"Castor3D",
			enableValidation,
		} ) );
		Logger::logInfo( cuT( "Using " ) + Name );
		auto & gpu = m_renderer->getPhysicalDevice( 0u );
		m_memoryProperties = gpu.getMemoryProperties();
		m_properties = gpu.getProperties();
		m_features = gpu.getFeatures();
		castor3d::initialiseGlslang();
	}

	RenderSystem::~RenderSystem()
	{
		castor3d::cleanupGlslang();
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
	{
		return std::make_unique< RenderSystem >( engine
			, appName
			, enableValidation );
	}

	castor3d::UInt32Array RenderSystem::compileShader( castor3d::ShaderModule const & module )
	{
		castor3d::UInt32Array result;

		if ( module.shader )
		{
#if !defined( NDEBUG ) && C3DVkRenderer_HasSPIRVCross && C3DVkRenderer_DebugSpirV

			result = spirv::serialiseSpirv( *module.shader );
			auto stream = castor::makeStringStream();
			stream << spirv::writeSpirv( *module.shader );
			std::string name = module.name + "_" + getName( module.stage );
			std::clog << std::endl << module.name + " " + getName( module.stage ) << " shader:" << std::endl;
			std::clog << stream.str() << std::endl;
			std::string glsl;

			try
			{
				glsl = compileSpvToGlsl( *getMainDevice()
					, result
					, module.stage );
			}
			catch ( std::exception & exc )
			{
				auto stream = castor::makeStringStream();
				stream << spirv::writeSpirv( *module.shader );
				std::cerr << stream.str() << std::endl;
				std::cerr << exc.what() << std::endl;
				{
					BinaryFile file{ File::getExecutableDirectory() / ( name + "_sdw.spv" )
						, File::OpenMode::eWrite };
					file.writeArray( result.data()
						, result.size() );
				}
				glsl = glsl::compileGlsl( *module.shader
					, ast::SpecialisationInfo{}
					, glsl::GlslConfig
					{
						460,
						true,
						true,
						true,
						true,
						true,
					} );
				result = castor3d::compileGlslToSpv( *getMainDevice()
					, module.stage
					, glsl );
				{
					BinaryFile file{ File::getExecutableDirectory() / ( name + "_glslang.spv" )
						, File::OpenMode::eWrite };
					file.writeArray( result.data()
						, result.size() );
				}
			}

#elif !defined( NDEBUG ) && C3DVkRenderer_DebugSpirV

			auto glsl = glsl::compileGlsl( *module.shader
				, ast::SpecialisationInfo{}
				, glsl::GlslConfig
				{
					460,
					true,
					true,
					true,
					true,
					true,
				} );
			result = castor3d::compileGlslToSpv( *getMainDevice()
				, module.stage
				, glsl );

#elif !defined( NDEBUG )

			auto glsl = glsl::compileGlsl( *module.shader
				, ast::SpecialisationInfo{}
				, glsl::GlslConfig
				{
					460,
					true,
					true,
					true,
					true,
					true,
				} );
			// Don't do this at home !
			auto stream = castor::makeStringStream();
			stream << std::endl << spirv::writeSpirv( *module.shader );
			const_cast< castor3d::ShaderModule & >( module ).source = glsl;
			const_cast< castor3d::ShaderModule & >( module ).source += stream.str();

			result = spirv::serialiseSpirv( *module.shader );
			{
				std::string name = module.name + "_" + getName( module.stage );
				BinaryFile file{ File::getExecutableDirectory() / ( name + "_sdw.spv" )
					, File::OpenMode::eWrite };
				file.writeArray( result.data()
					, result.size() );
			}

			//result = castor3d::compileGlslToSpv( *getMainDevice()
			//	, module.stage
			//	, glsl );

#else

			result = spirv::serialiseSpirv( *module.shader );

#endif
		}
		else
		{
			assert( !module.source.empty() );
			result = castor3d::compileGlslToSpv( *getMainDevice()
				, module.stage
				, module.source );
		}

		return result;
	}
}
