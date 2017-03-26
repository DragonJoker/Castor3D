#include "Render/GlRenderSystem.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlBackBuffers.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Mesh/GlGeometryBuffers.hpp"
#include "Miscellaneous/GlComputePipeline.hpp"
#include "Miscellaneous/GlQuery.hpp"
#include "Miscellaneous/GlTransformFeedback.hpp"
#include "Render/GlContext.hpp"
#include "Render/GlRenderPipeline.hpp"
#include "Render/GlViewport.hpp"
#include "Shader/GlUniformBufferBinding.hpp"
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

#include <GlslWriter.hpp>

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Technique/RenderTechnique.hpp>

#include <GL/gl.h>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
#if defined( CASTOR_PLATFORM_WINDOWS )

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

		template< typename T >
		std::unique_ptr< GpuBuffer< T > > DoCreateBuffer( GlRenderSystem & p_renderSystem, BufferType p_type )
		{
			std::unique_ptr< GpuBuffer< T > > l_return;

			switch ( p_type )
			{
			case BufferType::eArray:
				l_return = std::make_unique< GlBuffer< T > >( p_renderSystem, p_renderSystem.GetOpenGl(), GlBufferTarget::eArray );
				break;

			case BufferType::eElementArray:
				l_return = std::make_unique< GlBuffer< T > >( p_renderSystem, p_renderSystem.GetOpenGl(), GlBufferTarget::eElementArray );
				break;

			case BufferType::eUniform:
				l_return = std::make_unique< GlBuffer< T > >( p_renderSystem, p_renderSystem.GetOpenGl(), GlBufferTarget::eUniform );
				break;

			case BufferType::eAtomicCounter:
				if ( p_renderSystem.GetOpenGl().HasSsbo() )
				{
					l_return = std::make_unique< GlBuffer< T > >( p_renderSystem, p_renderSystem.GetOpenGl(), GlBufferTarget::eAtomicCounter );
				}
				break;

			case BufferType::eShaderStorage:
				if ( p_renderSystem.GetOpenGl().HasSsbo() )
				{
					l_return = std::make_unique< GlBuffer< T > >( p_renderSystem, p_renderSystem.GetOpenGl(), GlBufferTarget::eShaderStorage );
				}
				break;
			}

			return l_return;
		}
	}

	String GlRenderSystem::Name = cuT( "OpenGL Renderer" );
	String GlRenderSystem::Type = cuT( "opengl" );

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
				m_gpuInformations.UpdateFeature( GpuFeature::eConstantsBuffers, GetOpenGl().HasUbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::eTextureBuffers, GetOpenGl().HasTbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::eInstancing, GetOpenGl().HasInstancing() );
				m_gpuInformations.UpdateFeature( GpuFeature::eAccumulationBuffer, true );
				m_gpuInformations.UpdateFeature( GpuFeature::eNonPowerOfTwoTextures, GetOpenGl().HasNonPowerOfTwoTextures() );
				m_gpuInformations.UpdateFeature( GpuFeature::eAtomicCounterBuffers, GetOpenGl().HasExtension( ARB_shader_atomic_counters, false ) );
				m_gpuInformations.UpdateFeature( GpuFeature::eImmutableTextureStorage, GetOpenGl().HasExtension( ARB_texture_storage, false ) );
				m_gpuInformations.UpdateFeature( GpuFeature::eShaderStorageBuffers, GetOpenGl().HasSsbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::eTransformFeedback, GetOpenGl().HasExtension( ARB_transform_feedback2 ) );

				m_openGlMajor = GetOpenGl().GetVersion() / 10;
				m_openGlMinor = GetOpenGl().GetVersion() % 10;

				Logger::LogInfo( StringStream() << cuT( "Using version " ) << m_openGlMajor << cuT( "." ) << m_openGlMinor << cuT( " core functions" ) );
				m_gpuInformations.UseShaderType( ShaderType::eCompute, GetOpenGl().HasCSh() );
				m_gpuInformations.UseShaderType( ShaderType::eHull, GetOpenGl().HasTSh() );
				m_gpuInformations.UseShaderType( ShaderType::eDomain, GetOpenGl().HasTSh() );
				m_gpuInformations.UseShaderType( ShaderType::eGeometry, GetOpenGl().HasGSh() );
				m_gpuInformations.UseShaderType( ShaderType::ePixel, GetOpenGl().HasPSh() );
				m_gpuInformations.UseShaderType( ShaderType::eVertex, GetOpenGl().HasVSh() );

				std::array< int, 3 > l_value{};
				GetOpenGl().GetIntegerv( GlMin::eMapBufferAlignment, l_value.data() );
				m_gpuInformations.SetMinValue( GpuMin::eMapBufferAlignment, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMin::eProgramTexelOffset, l_value.data() );
				m_gpuInformations.SetMinValue( GpuMin::eProgramTexelOffset, l_value[0] );
				l_value = { 0, 0, 0 };

				if ( m_gpuInformations.HasShaderType( ShaderType::eCompute ) )
				{
					GetOpenGl().GetIntegerv( GlMax::eComputeUniformBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeUniformBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeTextureImageUnits, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeTextureImageUnits, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeUniformComponents, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeUniformComponents, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeAtomicCounters, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeAtomicCounters, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeAtomicCounterBuffers, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeAtomicCounterBuffers, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eCombinedComputeUniformComponents, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeCombinedUniformComponents, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeWorkGroupInvocations, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupInvocations, l_value[0] );
					l_value = { 0, 0, 0 };
					//GetOpenGl().GetIntegerv( GlMax::eComputeWorkGroupCount, l_value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupCount, l_value[0] );l_value = { 0, 0, 0 };
					//GetOpenGl().GetIntegerv( GlMax::eComputeWorkGroupSize, l_value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupSize, l_value[0] );l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ARB_shader_storage_buffer_object ) )
				{
					if ( m_gpuInformations.HasShaderType( ShaderType::eCompute ) )
					{
						GetOpenGl().GetIntegerv( GlMax::eComputeShaderStorageBlocks, l_value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::eComputeShaderStorageBlocks, l_value[0] );
						l_value = { 0, 0, 0 };
						GetOpenGl().GetIntegerv( GlMax::eCombinedShaderStorageBlocks, l_value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::eCombinedShaderStorageBlocks, l_value[0] );
						l_value = { 0, 0, 0 };
					}

					GetOpenGl().GetIntegerv( GlMax::eFragmentShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eFragmentShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eGeometryShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eGeometryShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eTessControlShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eTessControlShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eTessEvaluationShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eTessEvaluationShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eVertexShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eVertexShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eShaderStorageBufferBindings, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eShaderStorageBufferBindings, l_value[0] );
					l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ARB_explicit_uniform_location ) )
				{
					GetOpenGl().GetIntegerv( GlMax::eUniformLocations, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eUniformLocations, l_value[0] );
					l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ATI_meminfo ) )
				{
					GetOpenGl().GetIntegerv( GlGpuInfo::eVBOFreeMemoryATI, &l_value[0] );
					GetOpenGl().GetIntegerv( GlGpuInfo::eTextureFreeMemoryATI, &l_value[1] );
					GetOpenGl().GetIntegerv( GlGpuInfo::eRenderbufferFreeMemoryATI, &l_value[2] );
					m_gpuInformations.SetTotalMemorySize( l_value[0] + l_value[1] + l_value[2] );
					l_value = { 0, 0, 0 };
				}
				else if ( GetOpenGl().HasExtension( NVX_gpu_memory_info ) )
				{
					GetOpenGl().GetIntegerv( GlGpuInfo::eTotalAvailableMemNVX, l_value.data() );
					m_gpuInformations.SetTotalMemorySize( l_value[0] );
					l_value = { 0, 0, 0 };
				}
				else
				{
					m_gpuInformations.SetTotalMemorySize( GetVideoMemorySizeBytes() );
				}

				GetOpenGl().GetIntegerv( GlMax::eDebugGroupStackDepth, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDebugGroupStackDepth, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTexture3DSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTexture3DSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureArrayLayers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eArrayTextureLayers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eClipDistances, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eClipDistances, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eColorTextureSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eColourTextureSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedFragmentUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedGeometryUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedVertexUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureCubeSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureCubeMapSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eDepthTextureSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDepthTextureSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eDrawBuffers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDrawBuffers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eDualSourceDrawBuffers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDualSourceDrawBuffers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eElementsIndices, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eElementIndices, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eElementsVertices, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eElementVertices, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentInputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentInputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentUniformVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferWidth, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferWidth, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferHeight, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferHeight, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferLayers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferLayers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryInputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryInputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryOutputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryOutputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eIntegerSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eIntegerSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eLabelLength, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eLabelLength, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eProgramTexelOffset, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eProgramTexelOffset, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eRectangleTextureSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureRectangleSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eRenderbufferSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eRenderbufferSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eSampleMaskWords, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eSampleMaskWords, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eServerWaitTimeout, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eServerWaitTimeout, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTessControlAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTessControlAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTessEvaluationAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTessEvaluationAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureBufferSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureBufferSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureLODBias, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureLodBias, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eUniformBufferBindings, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eUniformBufferBindings, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eUniformBlockSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eUniformBlockSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVaryingComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVaryingVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVaryingFloats, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingFloats, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexAttribs, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexAttribs, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexUniformVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexOutputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexOutputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eViewportDims, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eViewportWidth, l_value[0] );
				m_gpuInformations.SetMaxValue( GpuMax::eViewportHeight, l_value[1] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eViewports, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eViewports, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eSamples, l_value[0] );
				l_value = { 0, 0, 0 };
			}
		}

		return m_extensionsInit;
	}

	ContextSPtr GlRenderSystem::CreateContext()
	{
		return std::make_shared< GlContext >( *this, GetOpenGl() );
	}

	GeometryBuffersSPtr GlRenderSystem::CreateGeometryBuffers( Topology p_topology
		, ShaderProgram const & p_program )
	{
		return std::make_shared< GlGeometryBuffers >( GetOpenGl(), p_topology, p_program );
	}

	RenderPipelineUPtr GlRenderSystem::CreateRenderPipeline( DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_bdState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		return std::make_unique< GlRenderPipeline >( GetOpenGl(), *this, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags );
	}

	ComputePipelineUPtr GlRenderSystem::CreateComputePipeline( ShaderProgram & p_program )
	{
		return std::make_unique< GlComputePipeline >( GetOpenGl(), *this, p_program );
	}

	SamplerSPtr GlRenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< GlSampler >( GetOpenGl(), this, p_name );
	}

	ShaderProgramSPtr GlRenderSystem::CreateShaderProgram()
	{
		return std::make_shared< GlShaderProgram >( GetOpenGl(), *this );
	}

	UniformBufferBindingUPtr GlRenderSystem::CreateUniformBufferBinding( UniformBuffer & p_ubo
		, ShaderProgram const & p_program )
	{
		return std::make_unique< GlUniformBufferBinding >( GetOpenGl()
			, p_ubo
			, static_cast< GlShaderProgram const & >( p_program ) );
	}

	std::unique_ptr< GpuBuffer< uint8_t > > GlRenderSystem::CreateUInt8Buffer( BufferType p_type )
	{
		return DoCreateBuffer< uint8_t >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< uint16_t > > GlRenderSystem::CreateUInt16Buffer( BufferType p_type )
	{
		return DoCreateBuffer< uint16_t >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< uint32_t > > GlRenderSystem::CreateUInt32Buffer( BufferType p_type )
	{
		return DoCreateBuffer< uint32_t >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< float > > GlRenderSystem::CreateFloatBuffer( BufferType p_type )
	{
		return DoCreateBuffer< float >( *this, p_type );
	}

	TransformFeedbackUPtr GlRenderSystem::CreateTransformFeedback( BufferDeclaration const & p_computed
		, Topology p_topology
		, ShaderProgram & p_program )
	{
		return std::make_unique< GlTransformFeedback >( GetOpenGl(), *this, p_computed, p_topology, p_program );
	}

	TextureLayoutSPtr GlRenderSystem::CreateTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess )
	{
		return std::make_shared< GlTexture >( GetOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureLayoutSPtr GlRenderSystem::CreateTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess
		, PixelFormat p_format
		, Size const & p_size )
	{
		return std::make_shared< GlTexture >( GetOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr GlRenderSystem::CreateTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess
		, PixelFormat p_format
		, Point3ui const & p_size )
	{
		return std::make_shared< GlTexture >( GetOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr GlRenderSystem::CreateTextureStorage( TextureStorageType p_type
		, TextureLayout & p_image
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess )
	{
		TextureStorageUPtr l_return;

		if ( p_type == TextureStorageType::eBuffer )
		{
			l_return = std::make_unique< GlTextureStorage< GlTboTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
		}

		if ( !l_return )
		{
			if ( !CheckFlag( p_gpuAccess, AccessType::eWrite ) )
			{
				if ( GetOpenGl().HasExtension( ARB_texture_storage, false )
					 && p_cpuAccess == AccessType::eNone )
				{
					l_return = std::make_unique< GlTextureStorage< GlImmutableTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
				else
				{
					l_return = std::make_unique< GlTextureStorage< GlDirectTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
			}
			else if ( p_cpuAccess != AccessType::eNone )
			{
				l_return = std::make_unique< GlTextureStorage< GlPboTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
			}
			else
			{
				l_return = std::make_unique< GlTextureStorage< GlGpuOnlyTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, AccessType::eNone, p_gpuAccess );
			}
		}

		return l_return;
	}

	FrameBufferSPtr GlRenderSystem::CreateFrameBuffer()
	{
		return std::make_shared< GlFrameBuffer >( GetOpenGl(), *GetEngine() );
	}

	BackBuffersSPtr GlRenderSystem::CreateBackBuffers()
	{
		return std::make_shared< GlBackBuffers >( GetOpenGl(), *GetEngine() );
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

	bool GlRenderSystem::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
	{
		std::string l_name;
		return m_tracker.Track( p_object, p_name, p_file, p_line, l_name );
	}

	bool GlRenderSystem::Untrack( void * p_object )
	{
		GpuObjectTracker::ObjectDeclaration l_decl;
		return m_tracker.Untrack( p_object, l_decl );
	}

#endif

}
