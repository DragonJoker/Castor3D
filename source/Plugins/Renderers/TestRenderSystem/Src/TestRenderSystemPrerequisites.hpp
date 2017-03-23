/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___TRS_RENDER_SYSTEM_PREREQUISITES_H___
#define ___TRS_RENDER_SYSTEM_PREREQUISITES_H___

#include <CastorUtils.hpp>
#include <Castor3DPrerequisites.hpp>

#ifdef _WIN32
#	ifdef TestRenderSystem_EXPORTS
#		define C3D_Test_API __declspec( dllexport )
#	else
#		define C3D_Test_API __declspec( dllimport )
#	endif
#else
#	define C3D_Test_API
#endif

namespace TestRender
{
	using Castor::real;

	class TestRenderSystem;
	class TestAttributeBase;
	template< typename T, uint32_t Columns, uint32_t Rows > class TestMatAttribute;
	template< typename T, uint32_t Count > class TestAttribute;
	template< typename T > class TestBufferBase;
	template< typename T > class TestBuffer;
	class TestVertexBufferObject;
	class Test3VertexBufferObject;
	class TestGeometryBuffers;
	class TestIndexArray;
	class TestVertexArray;
	class TestIndexBufferObject;
	class TestUniformBufferObject;
	class TestTextureBufferObject;
	typedef TestMatAttribute< real, 4, 4 > TestAttributeMat4;
	typedef TestMatAttribute< real, 3, 3 > TestAttributeMat3;
	typedef TestMatAttribute< real, 2, 2 > TestAttributeMat2;
	typedef TestAttribute< real, 4 > TestAttribute4r;
	typedef TestAttribute< real, 3 > TestAttribute3r;
	typedef TestAttribute< real, 2 > TestAttribute2r;
	typedef TestAttribute< real, 1 > TestAttribute1r;
	typedef TestAttribute< int, 4 > TestAttribute4i;
	typedef TestAttribute< int, 3 > TestAttribute3i;
	typedef TestAttribute< int, 2 > TestAttribute2i;
	typedef TestAttribute< int, 1 > TestAttribute1i;
	typedef TestAttribute< uint32_t, 1 > TestAttribute1ui;
	DECLARE_SMART_PTR( TestAttributeBase );
	DECLARE_SMART_PTR( TestIndexArray );
	DECLARE_SMART_PTR( TestVertexArray );
	DECLARE_SMART_PTR( TestIndexBufferObject );
	DECLARE_SMART_PTR( TestVertexBufferObject );
	DECLARE_SMART_PTR( TestTextureBufferObject );
	DECLARE_VECTOR(	TestAttributeBaseSPtr, TestAttributePtr );
	DECLARE_VECTOR(	TestTextureBufferObjectSPtr, TestTextureBufferObjectPtr );

	class TestShaderObject;
	class TestShaderProgram;
	class TestProgramInputLayout;
	template< typename Type > struct OneVariableBinder;
	template< typename Type, uint32_t Count > struct UniformVariableBinder;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder;
	class TestUniformBase;
	struct TestVariableApplyerBase;
	class UboVariableInfos;
	class TestUniformBuffer;
	template< typename T > class TestOneUniform;
	template< typename T, uint32_t Count > class TestPointUniform;
	template< typename T, uint32_t Rows, uint32_t Columns > class TestMatrixUniform;
	DECLARE_SMART_PTR( UboVariableInfos );
	DECLARE_SMART_PTR( TestVariableApplyerBase );
	DECLARE_SMART_PTR( TestUniformBase );
	DECLARE_SMART_PTR( TestShaderObject );
	DECLARE_SMART_PTR( TestShaderProgram );
	DECLARE_VECTOR( TestShaderProgramSPtr, TestShaderProgramPtr );
	DECLARE_VECTOR( TestShaderObjectSPtr, TestShaderObjectPtr );
	DECLARE_MAP( Castor::String, TestUniformBaseSPtr, TestUniformPtrStr );
	DECLARE_MAP( Castor3D::UniformSPtr, UboVariableInfosSPtr, UboVariableInfos );
	DECLARE_MAP( Castor3D::UniformSPtr, TestVariableApplyerBaseSPtr, VariableApplyer );

	class TestRenderBuffer;
	class TestColourRenderBuffer;
	class TestDepthRenderBuffer;
	class TestStencilRenderBuffer;
	class TestRenderBufferAttachment;
	class TestTextureAttachment;
	class TestFrameBuffer;
	DECLARE_SMART_PTR( TestFrameBuffer );
	DECLARE_SMART_PTR( TestRenderBuffer );
	DECLARE_SMART_PTR( TestColourRenderBuffer );
	DECLARE_SMART_PTR( TestDepthRenderBuffer );
	DECLARE_SMART_PTR( TestTextureAttachment );
	DECLARE_SMART_PTR( TestRenderBufferAttachment );
	DECLARE_VECTOR(	TestDepthRenderBufferSPtr, TestDepthRenderBufferPtr );
	DECLARE_VECTOR(	TestTextureAttachmentSPtr, TestTextureAttachmentPtr );
	DECLARE_VECTOR(	TestRenderBufferAttachmentSPtr, TestRenderBufferAttachmentPtr );

	class TestBillboardList;
	class TestBlendState;
	class TestDepthStencilState;
	class TestRasteriserState;
	class TestPipelineImpl;
	class TestContext;
	class TestContextImpl;
	class TestRenderSystem;
	DECLARE_SMART_PTR( TestContext );

	class TestTexture;
	class TestTextureStorage;
	class TestTexturePboStorage;
	class TestTextureTboStorage;
	class TestGpuIoBuffer;
	class TestDownloadPixelBuffer;
	class TestUploadPixelBuffer;
	DECLARE_SMART_PTR( TestTextureStorage );
	DECLARE_SMART_PTR( TestGpuIoBuffer );
	DECLARE_SMART_PTR( TestDownloadPixelBuffer );
	DECLARE_SMART_PTR( TestUploadPixelBuffer );

	class TestOverlayRenderer;
	class TestRenderTarget;
	class TestRenderWindow;
}

#endif
