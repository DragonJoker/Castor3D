#include "Render/GlES3RenderSystem.hpp"

#include "Buffer/GlES3Buffer.hpp"
#include "Common/OpenGlES3.hpp"
#include "FrameBuffer/GlES3BackBuffers.hpp"
#include "FrameBuffer/GlES3FrameBuffer.hpp"
#include "Mesh/GlES3GeometryBuffers.hpp"
#include "Miscellaneous/GlES3ComputePipeline.hpp"
#include "Miscellaneous/GlES3Query.hpp"
#include "Miscellaneous/GlES3TransformFeedback.hpp"
#include "Render/GlES3Context.hpp"
#include "Render/GlES3RenderPipeline.hpp"
#include "Render/GlES3Viewport.hpp"
#include "Shader/GlES3UniformBufferBinding.hpp"
#include "Shader/GlES3ShaderObject.hpp"
#include "Shader/GlES3ShaderProgram.hpp"
#include "Texture/GlES3DirectTextureStorage.hpp"
#include "Texture/GlES3PboTextureStorage.hpp"
#include "Texture/GlES3Sampler.hpp"
#include "Texture/GlES3ImmutableTextureStorage.hpp"
#include "Texture/GlES3TboTextureStorage.hpp"
#include "Texture/GlES3GpuOnlyTextureStorage.hpp"
#include "Texture/GlES3Texture.hpp"

#include <Log/Logger.hpp>

#include <GlslWriter.hpp>

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Technique/RenderTechnique.hpp>

#include <GL/gl.h>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
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

		template< typename T >
		std::unique_ptr< GpuBuffer< T > > DoCreateBuffer( GlES3RenderSystem & p_renderSystem, BufferType p_type )
		{
			std::unique_ptr< GpuBuffer< T > > l_return;

			switch ( p_type )
			{
			case BufferType::eArray:
				l_return = std::make_unique< GlES3Buffer< T > >( p_renderSystem, p_renderSystem.GetOpenGlES3(), GlES3BufferTarget::eArray );
				break;

			case BufferType::eElementArray:
				l_return = std::make_unique< GlES3Buffer< T > >( p_renderSystem, p_renderSystem.GetOpenGlES3(), GlES3BufferTarget::eElementArray );
				break;

			case BufferType::eUniform:
				l_return = std::make_unique< GlES3Buffer< T > >( p_renderSystem, p_renderSystem.GetOpenGlES3(), GlES3BufferTarget::eUniform );
				break;

			case BufferType::eAtomicCounter:
				if ( p_renderSystem.GetOpenGlES3().HasSsbo() )
				{
					l_return = std::make_unique< GlES3Buffer< T > >( p_renderSystem, p_renderSystem.GetOpenGlES3(), GlES3BufferTarget::eAtomicCounter );
				}
				break;

			case BufferType::eShaderStorage:
				if ( p_renderSystem.GetOpenGlES3().HasSsbo() )
				{
					l_return = std::make_unique< GlES3Buffer< T > >( p_renderSystem, p_renderSystem.GetOpenGlES3(), GlES3BufferTarget::eShaderStorage );
				}
				break;
			}

			return l_return;
		}
	}

	String GlES3RenderSystem::Name = cuT( "opengles3" );

	GlES3RenderSystem::GlES3RenderSystem( Engine & p_engine )
		: RenderSystem( p_engine, Name )
		, m_openGlES3Major( 0 )
		, m_openGlES3Minor( 0 )
		, m_useVertexBufferObjects( false )
		, m_extensionsInit( false )
		, m_openGlES3( *this )
	{
	}

	GlES3RenderSystem::~GlES3RenderSystem()
	{
	}

	RenderSystemUPtr GlES3RenderSystem::Create( Engine & p_engine )
	{
		return std::make_unique< GlES3RenderSystem >( p_engine );
	}

	bool GlES3RenderSystem::InitOpenGlES3Extensions()
	{
		if ( !m_extensionsInit )
		{
			if ( !GetOpenGlES3().Initialise() )
			{
				m_extensionsInit = false;
			}
			else
			{
				m_extensionsInit = true;
				m_gpuInformations.SetVendor( GetOpenGlES3().GetVendor() );
				m_gpuInformations.SetRenderer( GetOpenGlES3().GetRenderer() );
				m_gpuInformations.SetVersion( GetOpenGlES3().GetStrVersion() );
				m_gpuInformations.SetShaderLanguageVersion( GetOpenGlES3().GetGlslVersion() );
				m_gpuInformations.UpdateFeature( GpuFeature::eConstantsBuffers, GetOpenGlES3().HasUbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::eTextureBuffers, GetOpenGlES3().HasTbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::eInstancing, GetOpenGlES3().HasInstancing() );
				m_gpuInformations.UpdateFeature( GpuFeature::eAccumulationBuffer, true );
				m_gpuInformations.UpdateFeature( GpuFeature::eNonPowerOfTwoTextures, GetOpenGlES3().HasNonPowerOfTwoTextures() );
				m_gpuInformations.UpdateFeature( GpuFeature::eAtomicCounterBuffers, GetOpenGlES3().HasExtension( ARB_shader_atomic_counters, false ) );
				m_gpuInformations.UpdateFeature( GpuFeature::eImmutableTextureStorage, GetOpenGlES3().HasExtension( ARB_texture_storage, false ) );
				m_gpuInformations.UpdateFeature( GpuFeature::eShaderStorageBuffers, GetOpenGlES3().HasSsbo() );
				m_gpuInformations.UpdateFeature( GpuFeature::eTransformFeedback, GetOpenGlES3().HasExtension( ARB_transform_feedback2 ) );

				m_openGlES3Major = GetOpenGlES3().GetVersion() / 10;
				m_openGlES3Minor = GetOpenGlES3().GetVersion() % 10;

				Logger::LogInfo( StringStream() << cuT( "Using version " ) << m_openGlES3Major << cuT( "." ) << m_openGlES3Minor << cuT( " core functions" ) );
				m_gpuInformations.UseShaderType( ShaderType::eCompute, GetOpenGlES3().HasCSh() );
				m_gpuInformations.UseShaderType( ShaderType::eHull, GetOpenGlES3().HasTSh() );
				m_gpuInformations.UseShaderType( ShaderType::eDomain, GetOpenGlES3().HasTSh() );
				m_gpuInformations.UseShaderType( ShaderType::eGeometry, GetOpenGlES3().HasGSh() );
				m_gpuInformations.UseShaderType( ShaderType::ePixel, GetOpenGlES3().HasPSh() );
				m_gpuInformations.UseShaderType( ShaderType::eVertex, GetOpenGlES3().HasVSh() );

				std::array< int, 3 > l_value{};
				GetOpenGlES3().GetIntegerv( GlES3Min::eMapBufferAlignment, l_value.data() );
				m_gpuInformations.SetMinValue( GpuMin::eMapBufferAlignment, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Min::eProgramTexelOffset, l_value.data() );
				m_gpuInformations.SetMinValue( GpuMin::eProgramTexelOffset, l_value[0] );
				l_value = { 0, 0, 0 };

				if ( m_gpuInformations.HasShaderType( ShaderType::eCompute ) )
				{
					GetOpenGlES3().GetIntegerv( GlES3Max::eComputeUniformBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeUniformBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eComputeTextureImageUnits, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeTextureImageUnits, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eComputeUniformComponents, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeUniformComponents, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eComputeAtomicCounters, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeAtomicCounters, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eComputeAtomicCounterBuffers, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeAtomicCounterBuffers, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedComputeUniformComponents, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeCombinedUniformComponents, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eComputeWorkGroupInvocations, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupInvocations, l_value[0] );
					l_value = { 0, 0, 0 };
					//GetOpenGlES3().GetIntegerv( GlES3Max::eComputeWorkGroupCount, l_value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupCount, l_value[0] );l_value = { 0, 0, 0 };
					//GetOpenGlES3().GetIntegerv( GlES3Max::eComputeWorkGroupSize, l_value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupSize, l_value[0] );l_value = { 0, 0, 0 };
				}

				if ( GetOpenGlES3().HasExtension( ARB_shader_storage_buffer_object ) )
				{
					if ( m_gpuInformations.HasShaderType( ShaderType::eCompute ) )
					{
						GetOpenGlES3().GetIntegerv( GlES3Max::eComputeShaderStorageBlocks, l_value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::eComputeShaderStorageBlocks, l_value[0] );
						l_value = { 0, 0, 0 };
						GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedShaderStorageBlocks, l_value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::eCombinedShaderStorageBlocks, l_value[0] );
						l_value = { 0, 0, 0 };
					}

					GetOpenGlES3().GetIntegerv( GlES3Max::eFragmentShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eFragmentShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eGeometryShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eTessControlShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eTessControlShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eTessEvaluationShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eTessEvaluationShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eVertexShaderStorageBlocks, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eVertexShaderStorageBlocks, l_value[0] );
					l_value = { 0, 0, 0 };
					GetOpenGlES3().GetIntegerv( GlES3Max::eShaderStorageBufferBindings, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eShaderStorageBufferBindings, l_value[0] );
					l_value = { 0, 0, 0 };
				}

				if ( GetOpenGlES3().HasExtension( ARB_explicit_uniform_location ) )
				{
					GetOpenGlES3().GetIntegerv( GlES3Max::eUniformLocations, l_value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eUniformLocations, l_value[0] );
					l_value = { 0, 0, 0 };
				}

				if ( GetOpenGlES3().HasExtension( ATI_meminfo ) )
				{
					GetOpenGlES3().GetIntegerv( GlES3GpuInfo::eVBOFreeMemoryATI, &l_value[0] );
					GetOpenGlES3().GetIntegerv( GlES3GpuInfo::eTextureFreeMemoryATI, &l_value[1] );
					GetOpenGlES3().GetIntegerv( GlES3GpuInfo::eRenderbufferFreeMemoryATI, &l_value[2] );
					m_gpuInformations.SetTotalMemorySize( l_value[0] + l_value[1] + l_value[2] );
					l_value = { 0, 0, 0 };
				}
				else if ( GetOpenGlES3().HasExtension( NVX_gpu_memory_info ) )
				{
					GetOpenGlES3().GetIntegerv( GlES3GpuInfo::eTotalAvailableMemNVX, l_value.data() );
					m_gpuInformations.SetTotalMemorySize( l_value[0] );
					l_value = { 0, 0, 0 };
				}
				else
				{
					m_gpuInformations.SetTotalMemorySize( GetVideoMemorySizeBytes() );
				}

				GetOpenGlES3().GetIntegerv( GlES3Max::eDebugGroupStackDepth, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDebugGroupStackDepth, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTexture3DSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTexture3DSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTextureArrayLayers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eArrayTextureLayers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eClipDistances, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eClipDistances, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eColorTextureSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eColourTextureSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedFragmentUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedGeometryUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eCombinedVertexUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexCombinedUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTextureCubeSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureCubeMapSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eDepthTextureSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDepthTextureSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eDrawBuffers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDrawBuffers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eDualSourceDrawBuffers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDualSourceDrawBuffers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eElementsIndices, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eElementIndices, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eElementsVertices, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eElementVertices, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFragmentAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFragmentInputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentInputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFragmentUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFragmentUniformVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFragmentUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFramebufferWidth, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferWidth, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFramebufferHeight, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferHeight, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFramebufferLayers, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferLayers, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eFramebufferSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryInputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryInputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryOutputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryOutputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eGeometryUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eIntegerSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eIntegerSamples, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eLabelLength, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eLabelLength, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eProgramTexelOffset, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eProgramTexelOffset, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eRectangleTextureSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureRectangleSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eRenderbufferSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eRenderbufferSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eSampleMaskWords, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eSampleMaskWords, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eServerWaitTimeout, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eServerWaitTimeout, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTessControlAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTessControlAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTessEvaluationAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTessEvaluationAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTextureBufferSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureBufferSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTextureLODBias, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureLodBias, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eTextureSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eUniformBufferBindings, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eUniformBufferBindings, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eUniformBlockSize, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eUniformBlockSize, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVaryingComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVaryingVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVaryingFloats, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingFloats, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexAtomicCounters, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexAtomicCounters, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexAttribs, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexAttribs, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexTextureImageUnits, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexTextureImageUnits, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexUniformComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexUniformVectors, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformVectors, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexOutputComponents, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexOutputComponents, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eVertexUniformBlocks, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformBlocks, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eViewportDims, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eViewportWidth, l_value[0] );
				m_gpuInformations.SetMaxValue( GpuMax::eViewportHeight, l_value[1] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eViewports, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eViewports, l_value[0] );
				l_value = { 0, 0, 0 };
				GetOpenGlES3().GetIntegerv( GlES3Max::eSamples, l_value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eSamples, l_value[0] );
				l_value = { 0, 0, 0 };
			}
		}

		return m_extensionsInit;
	}

	ContextSPtr GlES3RenderSystem::CreateContext()
	{
		return std::make_shared< GlES3Context >( *this, GetOpenGlES3() );
	}

	GeometryBuffersSPtr GlES3RenderSystem::CreateGeometryBuffers( Topology p_topology
		, ShaderProgram const & p_program )
	{
		return std::make_shared< GlES3GeometryBuffers >( GetOpenGlES3(), p_topology, p_program );
	}

	RenderPipelineUPtr GlES3RenderSystem::CreateRenderPipeline( DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_bdState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		return std::make_unique< GlES3RenderPipeline >( GetOpenGlES3(), *this, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags );
	}

	ComputePipelineUPtr GlES3RenderSystem::CreateComputePipeline( ShaderProgram & p_program )
	{
		return std::make_unique< GlES3ComputePipeline >( GetOpenGlES3(), *this, p_program );
	}

	SamplerSPtr GlES3RenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< GlES3Sampler >( GetOpenGlES3(), this, p_name );
	}

	ShaderProgramSPtr GlES3RenderSystem::CreateShaderProgram()
	{
		return std::make_shared< GlES3ShaderProgram >( GetOpenGlES3(), *this );
	}

	UniformBufferBindingUPtr GlES3RenderSystem::CreateUniformBufferBinding( UniformBuffer & p_ubo
		, ShaderProgram const & p_program )
	{
		return std::make_unique< GlES3UniformBufferBinding >( GetOpenGlES3()
			, p_ubo
			, static_cast< GlES3ShaderProgram const & >( p_program ) );
	}

	std::unique_ptr< GpuBuffer< uint8_t > > GlES3RenderSystem::CreateUInt8Buffer( BufferType p_type )
	{
		return DoCreateBuffer< uint8_t >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< uint16_t > > GlES3RenderSystem::CreateUInt16Buffer( BufferType p_type )
	{
		return DoCreateBuffer< uint16_t >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< uint32_t > > GlES3RenderSystem::CreateUInt32Buffer( BufferType p_type )
	{
		return DoCreateBuffer< uint32_t >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< float > > GlES3RenderSystem::CreateFloatBuffer( BufferType p_type )
	{
		return DoCreateBuffer< float >( *this, p_type );
	}

	TransformFeedbackUPtr GlES3RenderSystem::CreateTransformFeedback( BufferDeclaration const & p_computed
		, Topology p_topology
		, ShaderProgram & p_program )
	{
		return std::make_unique< GlES3TransformFeedback >( GetOpenGlES3(), *this, p_computed, p_topology, p_program );
	}

	TextureLayoutSPtr GlES3RenderSystem::CreateTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess )
	{
		return std::make_shared< GlES3Texture >( GetOpenGlES3(), *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureLayoutSPtr GlES3RenderSystem::CreateTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess
		, PixelFormat p_format
		, Size const & p_size )
	{
		return std::make_shared< GlES3Texture >( GetOpenGlES3(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr GlES3RenderSystem::CreateTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess
		, PixelFormat p_format
		, Point3ui const & p_size )
	{
		return std::make_shared< GlES3Texture >( GetOpenGlES3(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr GlES3RenderSystem::CreateTextureStorage( TextureStorageType p_type
		, TextureLayout & p_image
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess )
	{
		TextureStorageUPtr l_return;

		if ( p_type == TextureStorageType::eBuffer )
		{
			l_return = std::make_unique< GlES3TextureStorage< GlES3TboTextureStorageTraits > >( GetOpenGlES3(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
		}

		if ( !l_return )
		{
			if ( !CheckFlag( p_gpuAccess, AccessType::eWrite ) )
			{
				if ( GetOpenGlES3().HasExtension( ARB_texture_storage, false )
					 && p_cpuAccess == AccessType::eNone )
				{
					l_return = std::make_unique< GlES3TextureStorage< GlES3ImmutableTextureStorageTraits > >( GetOpenGlES3(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
				else
				{
					l_return = std::make_unique< GlES3TextureStorage< GlES3DirectTextureStorageTraits > >( GetOpenGlES3(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
			}
			else if ( p_cpuAccess != AccessType::eNone )
			{
				l_return = std::make_unique< GlES3TextureStorage< GlES3PboTextureStorageTraits > >( GetOpenGlES3(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
			}
			else
			{
				l_return = std::make_unique< GlES3TextureStorage< GlES3GpuOnlyTextureStorageTraits > >( GetOpenGlES3(), *this, p_type, p_image, AccessType::eNone, p_gpuAccess );
			}
		}

		return l_return;
	}

	FrameBufferSPtr GlES3RenderSystem::CreateFrameBuffer()
	{
		return std::make_shared< GlES3FrameBuffer >( GetOpenGlES3(), *GetEngine() );
	}

	BackBuffersSPtr GlES3RenderSystem::CreateBackBuffers()
	{
		return std::make_shared< GlES3BackBuffers >( GetOpenGlES3(), *GetEngine() );
	}

	GpuQueryUPtr GlES3RenderSystem::CreateQuery( QueryType p_type )
	{
		return std::make_unique< GlES3Query >( *this, p_type );
	}

	IViewportImplUPtr GlES3RenderSystem::CreateViewport( Viewport & p_viewport )
	{
		return std::make_unique< GlES3Viewport >( *this, p_viewport );
	}

	void GlES3RenderSystem::DoInitialise()
	{
		if ( !m_initialised )
		{
			InitOpenGlES3Extensions();

			m_useVertexBufferObjects = GetOpenGlES3().HasVbo();
			m_initialised = true;
		}
	}

	void GlES3RenderSystem::DoCleanup()
	{
		m_extensionsInit = false;
		m_initialised = false;
		GetOpenGlES3().Cleanup();
	}

#if C3D_TRACE_OBJECTS

	bool GlES3RenderSystem::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
	{
		std::string l_name;
		return m_tracker.Track( p_object, p_name, p_file, p_line, l_name );
	}

	bool GlES3RenderSystem::Untrack( void * p_object )
	{
		GpuObjectTracker::ObjectDeclaration l_decl;
		return m_tracker.Untrack( p_object, l_decl );
	}

#endif

}
