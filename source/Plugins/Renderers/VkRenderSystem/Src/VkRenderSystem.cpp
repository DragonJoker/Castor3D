#include "VkRenderSystem.hpp"

#include <Engine.hpp>
#include <Shader/GlslToSpv.hpp>

#include <CompilerSpirV/compileSpirV.hpp>

#define C3DVkRenderer_HasGLSL 0
#if C3DVkRenderer_HasGLSL
#	include <CompilerGlsl/compileGlsl.hpp>
#endif

#include <Data/BinaryFile.hpp>
#include <Log/Logger.hpp>

#include <VkRenderer/Core/VkCreateRenderer.hpp>

#if C3DVkRenderer_HasSPIRVCross
#	include "spirv_cpp.hpp"
#	include "spirv_cross_util.hpp"
#	include "spirv_glsl.hpp"
#endif

#define C3DVkRenderer_DebugSpirV 1

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
		: castor3d::RenderSystem( engine, Name, true, true, false )
	{
		ashes::Logger::setTraceCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					Logger::logTrace( msg );
				}
				else
				{
					Logger::logTraceNoNL( msg );
				}
			} );
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
#if !defined( NDEBUG )

#if C3DVkRenderer_HasGLSL
			auto glsl = glsl::compileGlsl( *module.shader
				, ast::SpecialisationInfo{}
				, glsl::GlslConfig
				{
					module.shader->getType(),
					460,
					true,
					false,
					true,
					true,
					true,
					true,
				} );
#else
			std::string glsl;
#endif

			// Don't do this at home !
			const_cast< castor3d::ShaderModule & >( module ).source = glsl + "\n" + spirv::writeSpirv( *module.shader );

			result = spirv::serialiseSpirv( *module.shader );

#	if C3DVkRenderer_HasSPIRVCross && C3DVkRenderer_DebugSpirV

			std::string name = module.name + "_" + getName( module.stage );

			try
			{
				auto glsl2 = compileSpvToGlsl( *getMainDevice()
					, result
					, module.stage );
				const_cast< castor3d::ShaderModule & >( module ).source += "\n" + glsl2;
			}
			catch ( std::exception & exc )
			{
				std::cerr << module.source << std::endl;
				std::cerr << exc.what() << std::endl;
				{
					BinaryFile file{ File::getExecutableDirectory() / ( name + "_sdw.spv" )
						, File::OpenMode::eWrite };
					file.writeArray( result.data()
						, result.size() );
				}

				auto ref = castor3d::compileGlslToSpv( *getMainDevice()
					, module.stage
					, glsl );
				{
					BinaryFile file{ File::getExecutableDirectory() / ( name + "_glslang.spv" )
						, File::OpenMode::eWrite };
					file.writeArray( ref.data()
						, ref.size() );
				}
			}

#	endif
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
