/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
	class TestMatrixBufferObject;
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
	template< typename Type, uint32_t Count > struct PointVariableBinder;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder;
	class TestFrameVariableBase;
	struct TestVariableApplyerBase;
	class UboVariableInfos;
	class TestFrameVariableBuffer;
	template< typename T > class TestOneFrameVariable;
	template< typename T, uint32_t Count > class TestPointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns > class TestMatrixFrameVariable;
	DECLARE_SMART_PTR( UboVariableInfos );
	DECLARE_SMART_PTR( TestVariableApplyerBase );
	DECLARE_SMART_PTR( TestFrameVariableBase );
	DECLARE_SMART_PTR( TestShaderObject );
	DECLARE_SMART_PTR( TestShaderProgram );
	DECLARE_VECTOR( TestShaderProgramSPtr, TestShaderProgramPtr );
	DECLARE_VECTOR( TestShaderObjectSPtr, TestShaderObjectPtr );
	DECLARE_MAP( Castor::String, TestFrameVariableBaseSPtr, TestFrameVariablePtrStr );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, UboVariableInfosSPtr, UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, TestVariableApplyerBaseSPtr, VariableApplyer );

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
