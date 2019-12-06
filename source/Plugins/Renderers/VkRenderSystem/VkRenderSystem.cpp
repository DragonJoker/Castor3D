#include "VkRenderSystem/VkRenderSystem.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>

#include <CompilerSpirV/compileSpirV.hpp>

#define C3DVkRenderer_HasGLSL 1
#if C3DVkRenderer_HasGLSL
#	include <CompilerGlsl/compileGlsl.hpp>
#endif

#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Log/Logger.hpp>

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

		spv::ExecutionModel getExecutionModel( VkShaderStageFlagBits stage )
		{
			spv::ExecutionModel result{};

			switch ( stage )
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
				result = spv::ExecutionModelVertex;
				break;
			case VK_SHADER_STAGE_GEOMETRY_BIT:
				result = spv::ExecutionModelGeometry;
				break;
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				result = spv::ExecutionModelTessellationControl;
				break;
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				result = spv::ExecutionModelTessellationEvaluation;
				break;
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				result = spv::ExecutionModelFragment;
				break;
			case VK_SHADER_STAGE_COMPUTE_BIT:
				result = spv::ExecutionModelGLCompute;
				break;
			default:
				assert( false && "Unsupported shader stage flag" );
				break;
			}

			return result;
		}

		void doSetEntryPoint( VkShaderStageFlagBits stage
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
				throw std::runtime_error{ "Could not find an entry point with stage: " + ashes::getName( stage ) };
			}

			compiler.set_entry_point( entryPoint, model );
		}

		void doSetupOptions( castor3d::RenderDevice const & device
			, spirv_cross::CompilerGLSL & compiler )
		{
			auto options = compiler.get_common_options();
			options.version = device->getShaderVersion();
			options.es = false;
			options.separate_shader_objects = true;
			options.enable_420pack_extension = true;
			options.vertex.fixup_clipspace = false;
			options.vertex.flip_vert_y = true;
			options.vertex.support_nonzero_base_instance = true;
			compiler.set_common_options( options );
		}

		std::string compileSpvToGlsl( castor3d::RenderDevice const & device
			, ashes::UInt32Array const & spv
			, VkShaderStageFlagBits stage )
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
	String RenderSystem::Type = cuT( "vk" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, AshPluginDescription desc )
		: castor3d::RenderSystem{ engine, std::move( desc ), true, true, false }
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
		getEngine()->getRenderersList().selectPlugin( m_desc.name );
		Logger::logInfo( cuT( "Using " ) + Name );
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine
		, AshPluginDescription desc )
	{
		return std::make_unique< RenderSystem >( engine
			, std::move( desc ) );
	}

	castor3d::UInt32Array RenderSystem::compileShader( castor3d::ShaderModule const & module )const
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

			std::string name = module.name + "_" + ashes::getName( module.stage );

			try
			{
				auto glsl2 = compileSpvToGlsl( *getMainRenderDevice()
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

				auto ref = castor3d::compileGlslToSpv( *getMainRenderDevice()
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
			result = castor3d::compileGlslToSpv( *getMainRenderDevice()
				, module.stage
				, module.source );
		}

		return result;
	}
}
