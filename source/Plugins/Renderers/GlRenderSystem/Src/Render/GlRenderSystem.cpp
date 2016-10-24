#include "Render/GlRenderSystem.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlBackBuffers.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Mesh/GlGeometryBuffers.hpp"
#include "Miscellaneous/GlQuery.hpp"
#include "Miscellaneous/GlTransformFeedback.hpp"
#include "Render/GlContext.hpp"
#include "Render/GlPipeline.hpp"
#include "Render/GlViewport.hpp"
#include "Shader/GlFrameVariableBuffer.hpp"
#include "Shader/GlShaderObject.hpp"
#include "Shader/GlShaderProgram.hpp"
#include "Texture/GlDirectTextureStorage.hpp"
#include "Texture/GlPboTextureStorage.hpp"
#include "Texture/GlSampler.hpp"
#include "Texture/GlImmutableTextureStorage.hpp"
#include "Texture/GlTboTextureStorage.hpp"
#include "Texture/GlGpuOnlyTextureStorage.hpp"
#include "Texture/GlTexture.hpp"

#include <Log/Logger.hpp>

#include <GlslSource.hpp>

#include <Engine.hpp>
#include <ShaderCache.hpp>
#include <Technique/RenderTechnique.hpp>

#include <GL/gl.h>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
#if defined( _WIN32 )

		uint32_t GetVideoMemorySizeBytes()
		{
			uint32_t l_return{ 0u };
			HDC l_hdc = ::CreateDC( "DISPLAY", 0, 0, 0 );

			if ( l_hdc )
			{
				std::array< uint32_t, 5 > l_input = { 0, 0, 0x27, 0, 0 };
				std::array< uint32_t, 5 > l_output = { 0, 0, 0, 0, 0 };
				int const l_size = int( l_input.size() * sizeof( uint32_t ) );

				if ( ::ExtEscape( l_hdc, 0x7032, l_size, LPCSTR( l_input.data() ), l_size, LPSTR( l_output.data() ) ) > 0 )
				{
					l_return = l_output[3] * 1048576;
				}

				::DeleteDC( l_hdc );
			}

			return l_return;
		}

#else

		uint32_t GetVideoMemorySizeBytes()
		{
			return 0u;
		}

#endif
	}

	String GlRenderSystem::Name = cuT( "opengl" );

	GlRenderSystem::GlRenderSystem( Engine & p_engine )
		: RenderSystem( p_engine, Name )
		, m_openGlMajor( 0 )
		, m_openGlMinor( 0 )
		, m_useVertexBufferObjects( false )
		, m_extensionsInit( false )
		, m_openGl( *this )
	{
	}

	GlRenderSystem::~GlRenderSystem()
	{
	}

	RenderSystemUPtr GlRenderSystem::Create( Engine & p_engine )
	{
		return std::make_unique< GlRenderSystem >( p_engine );
	}

	bool GlRenderSystem::InitOpenGlExtensions()
	{
		if ( !m_extensionsInit )
		{
			if ( !GetOpenGl().Initialise() )
			{
				m_extensionsInit = false;
			}
			else
			{
				m_extensionsInit = true;
				m_gpuInformations.SetVendor( GetOpenGl().GetVendor() );
				m_gpuInformations.SetRenderer( GetOpenGl().GetRenderer() );
				m_gpuInformations.SetVersion( GetOpenGl().GetStrVersion() );
				m_gpuInformations.SetShaderLanguageVersion( GetOpenGl().GetGlslVersion() );
				m_gpuInformations.UpdateFeature( GpuFeature::ConstantsBuffers, GetOpenGl().HasUbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::TextureBuffers, GetOpenGl().HasTbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::Instancing, GetOpenGl().HasInstancing() );
				m_gpuInformations.UpdateFeature( GpuFeature::AccumulationBuffer, true );
				m_gpuInformations.UpdateFeature( GpuFeature::NonPowerOfTwoTextures, GetOpenGl().HasNonPowerOfTwoTextures() );

				m_openGlMajor = GetOpenGl().GetVersion() / 10;
				m_openGlMinor = GetOpenGl().GetVersion() % 10;

				Logger::LogInfo( StringStream() << cuT( "Using version " ) << m_openGlMajor << cuT( "." ) << m_openGlMinor << cuT( " core functions" ) );
				m_gpuInformations.UseShaderType( ShaderType::Compute, GetOpenGl().HasCSh() );
				m_gpuInformations.UseShaderType( ShaderType::Hull, GetOpenGl().HasTSh() );
				m_gpuInformations.UseShaderType( ShaderType::Domain, GetOpenGl().HasTSh() );
				m_gpuInformations.UseShaderType( ShaderType::Geometry, GetOpenGl().HasGSh() );
				m_gpuInformations.UseShaderType( ShaderType::Pixel, GetOpenGl().HasPSh() );
				m_gpuInformations.UseShaderType( ShaderType::Vertex, GetOpenGl().HasVSh() );

				std::array< int, 3 > l_value{};
				GetOpenGl().GetIntegerv( GlMin::MapBufferAlignment, l_value.data() );
				m_gpuInformations.SetMinValue( GpuMin::MapBufferAlignment, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMin::ProgramTexelOffset, l_value.data() );
				m_gpuInformations.SetMinValue( GpuMin::ProgramTexelOffset, l_value[0] );
				l_value = { 0, 0, 0 };

				if ( m_gpuInformations.HasShaderType( ShaderType::Compute ) )
				{
					GetOpenGl().GetIntegerv( GlMax::ComputeUniformBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeUniformBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::ComputeTextureImageUnits, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeTextureImageUnits, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::ComputeUniformComponents, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeUniformComponents, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::ComputeAtomicCounters, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeAtomicCounters, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::ComputeAtomicCounterBuffers, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeAtomicCounterBuffers, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::CombinedComputeUniformComponents, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeCombinedUniformComponents, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::ComputeWorkGroupInvocations, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ComputeWorkGroupInvocations, l_value[0] );
					l_value = { 0, 0, 0 };
					//GetOpenGl().GetIntegerv( GlMax::ComputeWorkGroupCount, l_value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::ComputeWorkGroupCount, l_value[0] );l_value = { 0, 0, 0 };
					//GetOpenGl().GetIntegerv( GlMax::ComputeWorkGroupSize, l_value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::ComputeWorkGroupSize, l_value[0] );l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ARB_shader_storage_buffer_object ) )
				{
					if ( m_gpuInformations.HasShaderType( ShaderType::Compute ) )
					{
						GetOpenGl().GetIntegerv( GlMax::ComputeShaderStorageBlocks, l_value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::ComputeShaderStorageBlocks, l_value[0] );
						l_value = { 0, 0, 0 };
						GetOpenGl().GetIntegerv( GlMax::CombinedShaderStorageBlocks, l_value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::CombinedShaderStorageBlocks, l_value[0] );
						l_value = { 0, 0, 0 };
					}

					GetOpenGl().GetIntegerv( GlMax::FragmentShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::FragmentShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::GeometryShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::GeometryShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::TessControlShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::TessControlShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::TessEvaluationShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::TessEvaluationShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::VertexShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::VertexShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::ShaderStorageBufferBindings, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::ShaderStorageBufferBindings, l_value[0] );
					l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ARB_explicit_uniform_location ) )
				{
					GetOpenGl().GetIntegerv( GlMax::UniformLocations, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::UniformLocations, l_value[0] );
					l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ATI_meminfo ) )
				{
					GetOpenGl().GetIntegerv( GlGpuInfo::VBOFreeMemoryATI, &l_value[0] );
					GetOpenGl().GetIntegerv( GlGpuInfo::TextureFreeMemoryATI, &l_value[1] );
					GetOpenGl().GetIntegerv( GlGpuInfo::RenderbufferFreeMemoryATI, &l_value[2] );
					m_gpuInformations.SetTotalMemorySize( l_value[0] + l_value[1] + l_value[2] );
					l_value = { 0, 0, 0 };
				}
				else if ( GetOpenGl().HasExtension( NVX_gpu_memory_info ) )
				{
					GetOpenGl().GetIntegerv( GlGpuInfo::TotalAvailableMemNVX, l_value.data() );
					m_gpuInformations.SetTotalMemorySize( l_value[0] );
					l_value = { 0, 0, 0 };
				}
				else
				{
					m_gpuInformations.SetTotalMemorySize( GetVideoMemorySizeBytes() );
				}

				GetOpenGl().GetIntegerv( GlMax::DebugGroupStackDepth, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::DebugGroupStackDepth, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::Texture3DSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::Texture3DSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TextureArrayLayers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ArrayTextureLayers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ClipDistances, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ClipDistances, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ColorTextureSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ColourTextureSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::CombinedAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::CombinedAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::CombinedFragmentUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FragmentCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::CombinedGeometryUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::CombinedTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::CombinedTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::CombinedUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::CombinedUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::CombinedVertexUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TextureCubeSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TextureCubeMapSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::DepthTextureSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::DepthTextureSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::DrawBuffers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::DrawBuffers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::DualSourceDrawBuffers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::DualSourceDrawBuffers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ElementsIndices, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ElementIndices, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ElementsVertices, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ElementVertices, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FragmentAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FragmentAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FragmentInputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FragmentInputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FragmentUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FragmentUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FragmentUniformVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FragmentUniformVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FragmentUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FragmentUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FramebufferWidth, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FramebufferWidth, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FramebufferHeight, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FramebufferHeight, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FramebufferLayers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FramebufferLayers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::FramebufferSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::FramebufferSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::GeometryAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::GeometryInputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryInputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::GeometryOutputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryOutputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::GeometryTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::GeometryUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::GeometryUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::GeometryUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::IntegerSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::IntegerSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::LabelLength, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::LabelLength, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ProgramTexelOffset, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ProgramTexelOffset, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::RectangleTextureSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TextureRectangleSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::RenderbufferSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::RenderbufferSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::SampleMaskWords, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::SampleMaskWords, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ServerWaitTimeout, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ServerWaitTimeout, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TessControlAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TessControlAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TessEvaluationAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TessEvaluationAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TextureBufferSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TextureBufferSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TextureLODBias, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TextureLodBias, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::TextureSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::TextureSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::UniformBufferBindings, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::UniformBufferBindings, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::UniformBlockSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::UniformBlockSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VaryingComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VaryingComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VaryingVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VaryingVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VaryingFloats, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VaryingFloats, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexAttribs, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexAttribs, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexUniformVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexUniformVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexOutputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexOutputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::VertexUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::VertexUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::ViewportDims, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::ViewportWidth, l_value[0] );
				m_gpuInformations.SetMaxValue( GpuMax::ViewportHeight, l_value[1] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::Viewports, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::Viewports, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::Samples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::Samples, l_value[0] );
				l_value = { 0, 0, 0 };
			}
		}

		return m_extensionsInit;
	}

	ContextSPtr GlRenderSystem::CreateContext()
	{
		return std::make_shared< GlContext >( *this, GetOpenGl() );
	}

	GeometryBuffersUPtr GlRenderSystem::CreateGeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
	{
		return std::make_unique< GlGeometryBuffers >( GetOpenGl(), p_topology, p_program );
	}

	PipelineUPtr GlRenderSystem::CreatePipeline( DepthStencilState && p_dsState
												 , RasteriserState && p_rsState
												 , BlendState && p_bdState
												 , MultisampleState && p_msState
												 , ShaderProgram & p_program
												 , PipelineFlags const & p_flags )
	{
		return std::make_unique< GlPipeline >( GetOpenGl(), *this, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags );
	}

	SamplerSPtr GlRenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< GlSampler >( GetOpenGl(), this, p_name );
	}

	ShaderProgramSPtr GlRenderSystem::CreateShaderProgram()
	{
		return std::make_shared< GlShaderProgram >( GetOpenGl(), *this );
	}

	std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > GlRenderSystem::CreateIndexBuffer( CpuBuffer< uint32_t > * p_buffer )
	{
		return std::make_shared< GlBuffer< uint32_t > >( *this, GetOpenGl(), GlBufferTarget::ElementArray, p_buffer );
	}

	std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::CreateVertexBuffer( CpuBuffer< uint8_t > * p_buffer )
	{
		return std::make_shared< GlBuffer< uint8_t > >( *this, GetOpenGl(), GlBufferTarget::Array, p_buffer );
	}

	TransformFeedbackUPtr GlRenderSystem::CreateTransformFeedback( BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
	{
		return std::make_unique< GlTransformFeedback >( GetOpenGl(), *this, p_computed, p_topology, p_program );
	}

	TextureLayoutSPtr GlRenderSystem::CreateTexture( Castor3D::TextureType p_type, AccessType p_cpuAccess, AccessType p_gpuAccess )
	{
		return std::make_shared< GlTexture >( GetOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureLayoutSPtr GlRenderSystem::CreateTexture( Castor3D::TextureType p_type, AccessType p_cpuAccess, AccessType p_gpuAccess, PixelFormat p_format, Size const & p_size )
	{
		return std::make_shared< GlTexture >( GetOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr GlRenderSystem::CreateTexture( Castor3D::TextureType p_type, AccessType p_cpuAccess, AccessType p_gpuAccess, PixelFormat p_format, Point3ui const & p_size )
	{
		return std::make_shared< GlTexture >( GetOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr GlRenderSystem::CreateTextureStorage( TextureStorageType p_type, TextureLayout & p_image, AccessType p_cpuAccess, AccessType p_gpuAccess )
	{
		TextureStorageUPtr l_return;

		if ( p_type == TextureStorageType::Buffer )
		{
			l_return = std::make_unique< GlTextureStorage< GlTboTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
		}

		if ( !l_return )
		{
			if ( !CheckFlag( p_gpuAccess, AccessType::Write ) )
			{
				if ( GetOpenGl().HasExtension( ARB_texture_storage )
					 && p_cpuAccess == AccessType::None )
				{
					l_return = std::make_unique< GlTextureStorage< GlImmutableTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
				else
				{
					l_return = std::make_unique< GlTextureStorage< GlDirectTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
			}
			else if ( p_cpuAccess != AccessType::None )
			{
				l_return = std::make_unique< GlTextureStorage< GlPboTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
			}
			else
			{
				l_return = std::make_unique< GlTextureStorage< GlGpuOnlyTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, AccessType::None, p_gpuAccess );
			}
		}

		return l_return;
	}

	FrameBufferSPtr GlRenderSystem::CreateFrameBuffer()
	{
		return std::make_unique< GlFrameBuffer >( GetOpenGl(), *GetEngine() );
	}

	BackBuffersSPtr GlRenderSystem::CreateBackBuffers()
	{
		return std::make_unique< GlBackBuffers >( GetOpenGl(), *GetEngine() );
	}

	GpuQueryUPtr GlRenderSystem::CreateQuery( QueryType p_type )
	{
		return std::make_unique< GlQuery >( *this, p_type );
	}

	IViewportImplUPtr GlRenderSystem::CreateViewport( Viewport & p_viewport )
	{
		return std::make_unique< GlViewport >( *this, p_viewport );
	}

	void GlRenderSystem::DoInitialise()
	{
		if ( !m_initialised )
		{
			InitOpenGlExtensions();

			m_useVertexBufferObjects = GetOpenGl().HasVbo();
			m_initialised = true;
		}
	}

	void GlRenderSystem::DoCleanup()
	{
		m_extensionsInit = false;
		m_initialised = false;
		GetOpenGl().Cleanup();
	}

#if C3D_TRACE_OBJECTS

	void GlRenderSystem::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
	{
		std::string l_name;

		if ( DoTrack( p_object, p_name, p_file, p_line, l_name ) )
		{
		}
	}

	void GlRenderSystem::Untrack( void * p_object )
	{
		ObjectDeclaration l_decl;

		if ( DoUntrack( p_object, l_decl ) )
		{
		}
	}

#endif

}
