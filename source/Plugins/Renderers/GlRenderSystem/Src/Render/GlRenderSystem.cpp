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
			uint32_t result{ 0u };
			HDC hdc = ::CreateDC( "DISPLAY", 0, 0, 0 );

			if ( hdc )
			{
				std::array< uint32_t, 5 > input = { 0, 0, 0x27, 0, 0 };
				std::array< uint32_t, 5 > output = { 0, 0, 0, 0, 0 };
				int const size = int( input.size() * sizeof( uint32_t ) );

				if ( ::ExtEscape( hdc, 0x7032, size, LPCSTR( input.data() ), size, LPSTR( output.data() ) ) > 0 )
				{
					result = output[3] * 1048576;
				}

				::DeleteDC( hdc );
			}

			return result;
		}

#else

		uint32_t GetVideoMemorySizeBytes()
		{
			return 0u;
		}

#endif
	}

	String GlRenderSystem::Name = cuT( "OpenGL Renderer" );
	String GlRenderSystem::Type = cuT( "opengl" );

	GlRenderSystem::GlRenderSystem( Engine & engine )
		: RenderSystem( engine, Name )
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

	RenderSystemUPtr GlRenderSystem::Create( Engine & engine )
	{
		return std::make_unique< GlRenderSystem >( engine );
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

				std::array< int, 3 > value{};
				GetOpenGl().GetIntegerv( GlMin::eMapBufferAlignment, value.data() );
				m_gpuInformations.SetMinValue( GpuMin::eMapBufferAlignment, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMin::eProgramTexelOffset, value.data() );
				m_gpuInformations.SetMinValue( GpuMin::eProgramTexelOffset, value[0] );
				value = { 0, 0, 0 };

				if ( m_gpuInformations.HasShaderType( ShaderType::eCompute ) )
				{
					GetOpenGl().GetIntegerv( GlMax::eComputeUniformBlocks, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeUniformBlocks, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeTextureImageUnits, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeTextureImageUnits, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeUniformComponents, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeUniformComponents, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeAtomicCounters, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeAtomicCounters, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeAtomicCounterBuffers, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeAtomicCounterBuffers, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eCombinedComputeUniformComponents, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeCombinedUniformComponents, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eComputeWorkGroupInvocations, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupInvocations, value[0] );
					value = { 0, 0, 0 };
					//GetOpenGl().GetIntegerv( GlMax::eComputeWorkGroupCount, value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupCount, value[0] );l_value = { 0, 0, 0 };
					//GetOpenGl().GetIntegerv( GlMax::eComputeWorkGroupSize, value.data() );
					//m_gpuInformations.SetMaxValue( GpuMax::eComputeWorkGroupSize, value[0] );l_value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ARB_shader_storage_buffer_object ) )
				{
					if ( m_gpuInformations.HasShaderType( ShaderType::eCompute ) )
					{
						GetOpenGl().GetIntegerv( GlMax::eComputeShaderStorageBlocks, value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::eComputeShaderStorageBlocks, value[0] );
						value = { 0, 0, 0 };
						GetOpenGl().GetIntegerv( GlMax::eCombinedShaderStorageBlocks, value.data() );
						m_gpuInformations.SetMaxValue( GpuMax::eCombinedShaderStorageBlocks, value[0] );
						value = { 0, 0, 0 };
					}

					GetOpenGl().GetIntegerv( GlMax::eFragmentShaderStorageBlocks, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eFragmentShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eGeometryShaderStorageBlocks, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eGeometryShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eTessControlShaderStorageBlocks, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eTessControlShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eTessEvaluationShaderStorageBlocks, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eTessEvaluationShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eVertexShaderStorageBlocks, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eVertexShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					GetOpenGl().GetIntegerv( GlMax::eShaderStorageBufferBindings, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eShaderStorageBufferBindings, value[0] );
					value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ARB_explicit_uniform_location ) )
				{
					GetOpenGl().GetIntegerv( GlMax::eUniformLocations, value.data() );
					m_gpuInformations.SetMaxValue( GpuMax::eUniformLocations, value[0] );
					value = { 0, 0, 0 };
				}

				if ( GetOpenGl().HasExtension( ATI_meminfo ) )
				{
					GetOpenGl().GetIntegerv( GlGpuInfo::eVBOFreeMemoryATI, &value[0] );
					GetOpenGl().GetIntegerv( GlGpuInfo::eTextureFreeMemoryATI, &value[1] );
					GetOpenGl().GetIntegerv( GlGpuInfo::eRenderbufferFreeMemoryATI, &value[2] );
					m_gpuInformations.SetTotalMemorySize( value[0] + value[1] + value[2] );
					value = { 0, 0, 0 };
				}
				else if ( GetOpenGl().HasExtension( NVX_gpu_memory_info ) )
				{
					GetOpenGl().GetIntegerv( GlGpuInfo::eTotalAvailableMemNVX, value.data() );
					m_gpuInformations.SetTotalMemorySize( value[0] );
					value = { 0, 0, 0 };
				}
				else
				{
					m_gpuInformations.SetTotalMemorySize( GetVideoMemorySizeBytes() );
				}

				GetOpenGl().GetIntegerv( GlMax::eDebugGroupStackDepth, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDebugGroupStackDepth, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTexture3DSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTexture3DSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureArrayLayers, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eArrayTextureLayers, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eClipDistances, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eClipDistances, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eColorTextureSamples, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eColourTextureSamples, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedAtomicCounters, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedFragmentUniformComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentCombinedUniformComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedGeometryUniformComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryCombinedUniformComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedTextureImageUnits, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedUniformBlocks, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eCombinedUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eCombinedVertexUniformComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexCombinedUniformComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureCubeSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureCubeMapSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eDepthTextureSamples, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDepthTextureSamples, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eDrawBuffers, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDrawBuffers, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eDualSourceDrawBuffers, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eDualSourceDrawBuffers, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eElementsIndices, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eElementIndices, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eElementsVertices, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eElementVertices, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentAtomicCounters, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentInputComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentInputComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentUniformComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentUniformVectors, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformVectors, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFragmentUniformBlocks, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFragmentUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferWidth, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferWidth, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferHeight, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferHeight, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferLayers, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferLayers, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eFramebufferSamples, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eFramebufferSamples, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryAtomicCounters, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryInputComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryInputComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryOutputComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryOutputComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryTextureImageUnits, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryUniformBlocks, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eGeometryUniformComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eGeometryUniformComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eIntegerSamples, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eIntegerSamples, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eLabelLength, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eLabelLength, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eProgramTexelOffset, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eProgramTexelOffset, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eRectangleTextureSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureRectangleSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eRenderbufferSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eRenderbufferSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eSampleMaskWords, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eSampleMaskWords, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eServerWaitTimeout, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eServerWaitTimeout, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTessControlAtomicCounters, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTessControlAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTessEvaluationAtomicCounters, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTessEvaluationAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureBufferSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureBufferSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureImageUnits, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureLODBias, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureLodBias, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eTextureSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eTextureSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eUniformBufferBindings, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eUniformBufferBindings, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eUniformBlockSize, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eUniformBlockSize, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVaryingComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVaryingVectors, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingVectors, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVaryingFloats, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVaryingFloats, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexAtomicCounters, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexAttribs, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexAttribs, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexTextureImageUnits, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexUniformComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexUniformVectors, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformVectors, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexOutputComponents, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexOutputComponents, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eVertexUniformBlocks, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eVertexUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eViewportDims, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eViewportWidth, value[0] );
				m_gpuInformations.SetMaxValue( GpuMax::eViewportHeight, value[1] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eViewports, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eViewports, value[0] );
				value = { 0, 0, 0 };
				GetOpenGl().GetIntegerv( GlMax::eSamples, value.data() );
				m_gpuInformations.SetMaxValue( GpuMax::eSamples, value[0] );
				value = { 0, 0, 0 };
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

	GpuBufferUPtr GlRenderSystem::CreateBuffer( BufferType p_type )
	{
		GpuBufferUPtr result;

		switch ( p_type )
		{
		case BufferType::eArray:
			result = std::make_unique< GlBuffer >( *this
				, GetOpenGl()
				, GlBufferTarget::eArray );
			break;

		case BufferType::eElementArray:
			result = std::make_unique< GlBuffer >( *this
				, GetOpenGl()
				, GlBufferTarget::eElementArray );
			break;

		case BufferType::eUniform:
			result = std::make_unique< GlBuffer >( *this
				, GetOpenGl()
				, GlBufferTarget::eUniform );
			break;

		case BufferType::eAtomicCounter:
			if ( GetOpenGl().HasSsbo() )
			{
				result = std::make_unique< GlBuffer >( *this
					, GetOpenGl()
					, GlBufferTarget::eAtomicCounter );
			}
			break;

		case BufferType::eShaderStorage:
			if ( GetOpenGl().HasSsbo() )
			{
				result = std::make_unique< GlBuffer >( *this
					, GetOpenGl()
					, GlBufferTarget::eShaderStorage );
			}
			break;
		}

		return result;
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
		TextureStorageUPtr result;

		if ( p_type == TextureStorageType::eBuffer )
		{
			result = std::make_unique< GlTextureStorage< GlTboTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
		}

		if ( !result )
		{
			if ( !CheckFlag( p_gpuAccess, AccessType::eWrite ) )
			{
				if ( GetOpenGl().HasExtension( ARB_texture_storage, false )
					 && p_cpuAccess == AccessType::eNone )
				{
					result = std::make_unique< GlTextureStorage< GlImmutableTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
				else
				{
					result = std::make_unique< GlTextureStorage< GlDirectTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
			}
			else if ( p_cpuAccess != AccessType::eNone )
			{
				result = std::make_unique< GlTextureStorage< GlPboTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
			}
			else
			{
				result = std::make_unique< GlTextureStorage< GlGpuOnlyTextureStorageTraits > >( GetOpenGl(), *this, p_type, p_image, AccessType::eNone, p_gpuAccess );
			}
		}

		return result;
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
		std::string name;
		return m_tracker.Track( p_object, p_name, p_file, p_line, name );
	}

	bool GlRenderSystem::Untrack( void * p_object )
	{
		GpuObjectTracker::ObjectDeclaration decl;
		return m_tracker.Untrack( p_object, decl );
	}

#endif

}
