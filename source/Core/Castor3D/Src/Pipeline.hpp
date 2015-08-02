/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_PIPELINE_H___
#define ___C3D_PIPELINE_H___

#include "Castor3DPrerequisites.hpp"

#include <stack>
#include <SquareMatrix.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	//! Implementation of the rendering pipeline
	/*!
	Defines the various matrices, applies the numerous transformation it can support and applies them
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class C3D_API Pipeline
	{
	public:
		typedef Castor::Matrix4x4r matrix4x4;
		typedef Castor::Matrix3x3r matrix3x3;

	protected:
		friend class IPipelineImpl;

	private:
		typedef std::stack<matrix4x4> MatrixStack;
		typedef std::set< ShaderObjectBaseSPtr >	ShaderObjectSet;

	protected:
		Pipeline( RenderSystem * p_pRenderSystem );

	public:
		virtual ~Pipeline();

		virtual void Initialise() = 0;
		virtual void UpdateFunctions( Castor3D::ShaderProgramBase * p_pProgram ) = 0;

		inline void SetImpl( IPipelineImpl * p_pImpl )
		{
			m_pPipelineImpl = p_pImpl;
		}
		inline RenderSystem * GetRenderSystem()const
		{
			return m_pRenderSystem;
		}
		inline void SetRenderSystem( RenderSystem * val )
		{
			m_pRenderSystem = val;
		}
		inline IPipelineImpl * GetImpl()const
		{
			return m_pPipelineImpl;
		}
		matrix4x4 const & GetMatrix( eMTXMODE p_eMode )const;
		matrix4x4 & GetMatrix( eMTXMODE p_eMode );
		bool HasMatrix( eMTXMODE p_eMode )const;
		eMTXMODE MatrixMode( eMTXMODE p_eMode );
		bool LoadIdentity();
		bool PushMatrix();
		bool PopMatrix();
		bool Translate( Castor::Point3r const & p_translate );
		bool Rotate( Castor::Quaternion const & p_rotate );
		bool Scale( Castor::Point3r const & p_scale );
		bool MultMatrix( Castor::Matrix4x4r const & p_matrix );
		bool MultMatrix( real const * p_matrix );
		bool Perspective( Castor::Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar );
		bool Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		bool Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		bool Project( Castor::Point3r const & p_ptObj, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		bool UnProject( Castor::Point3i const & p_ptWin, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		bool PickMatrix( real x, real y, real width, real height, int viewport[4] );
		void ApplyProjection( ShaderProgramBase & p_program );
		void ApplyModel( ShaderProgramBase & p_program );
		void ApplyView( ShaderProgramBase & p_program );
		void ApplyNormal( ShaderProgramBase & p_program );
		void ApplyModelView( ShaderProgramBase & p_program );
		void ApplyProjectionView( ShaderProgramBase & p_program );
		void ApplyProjectionModelView( ShaderProgramBase & p_program );
		void ApplyTexture0( ShaderProgramBase & p_program );
		void ApplyTexture1( ShaderProgramBase & p_program );
		void ApplyTexture2( ShaderProgramBase & p_program );
		void ApplyTexture3( ShaderProgramBase & p_program );
		void ApplyMatrices( ShaderProgramBase & p_program );
		void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight );

	private:
		void DoApplyMatrix( eMTXMODE p_eMatrix, Castor::String const & p_strName, ShaderProgramBase & p_program );
		void DoApplyMatrix( matrix4x4 const & p_matrix, Castor::String const & p_strName, ShaderProgramBase & p_program );

	public:
		static const Castor::String MtxProjection;
		static const Castor::String MtxModel;
		static const Castor::String MtxView;
		static const Castor::String MtxModelView;
		static const Castor::String MtxProjectionView;
		static const Castor::String MtxProjectionModelView;
		static const Castor::String MtxNormal;
		static const Castor::String MtxTexture0;
		static const Castor::String MtxTexture1;
		static const Castor::String MtxTexture2;
		static const Castor::String MtxTexture3;
		static const Castor::String MtxBones;

	public:
		matrix4x4 m_mtxIdentity;

	protected:
		matrix4x4 m_matTmp;
		MatrixStack m_matrix[eMTXMODE_COUNT];
		matrix4x4 m_mtxModelView;
		matrix4x4 m_mtxProjectionView;
		matrix4x4 m_mtxProjectionModelView;
		matrix4x4 m_mtxNormal;
		eMTXMODE m_eCurrentMode;
		RenderSystem * m_pRenderSystem;
		IPipelineImpl * m_pPipelineImpl;
		ShaderObjectSet m_setShaders[eMTXMODE_COUNT];
		ShaderObjectSet m_setNmlShaders;
		ShaderObjectSet m_setMVShaders;
		ShaderObjectSet m_setPMVShaders;
	};
	/*!
	\author Sylvain DOREMUS
	\date 21/02/2012
	\~english
	\brief
	\remark
	\~french
	\brief
	\remark
	*/
	class C3D_API IPipelineImpl
	{
	protected:
		typedef Pipeline::matrix4x4 matrix4x4;
		typedef Pipeline::matrix3x3 matrix3x3;

	public:
		IPipelineImpl( Pipeline * p_pPipeline );
		virtual ~IPipelineImpl();

		virtual void Initialise();
		virtual eMTXMODE MatrixMode( eMTXMODE p_eMode );
		virtual bool LoadIdentity();
		virtual bool PushMatrix();
		virtual bool PopMatrix();
		virtual bool Translate( Castor::Point3r const & p_translate );
		virtual bool Rotate( Castor::Quaternion const & p_rotate );
		virtual bool Scale( Castor::Point3r const & p_scale );
		virtual bool MultMatrix( Castor::Matrix4x4r const & p_matrix );
		virtual bool MultMatrix( real const * p_matrix );
		virtual bool Perspective( Castor::Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar );
		virtual bool Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		virtual bool Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		virtual bool Project( Castor::Point3r const & p_ptObj, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		virtual bool UnProject( Castor::Point3i const & p_ptWin, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		virtual bool PickMatrix( real x, real y, real width, real height, int viewport[4] );
		virtual void ApplyViewport( int CU_PARAM_UNUSED( p_iWindowWidth ), int CU_PARAM_UNUSED( p_iWindowHeight ) ) {}
		virtual void ApplyMatrices( ShaderProgramBase & CU_PARAM_UNUSED( p_program ) ) {}

		inline eMTXMODE GetCurrentMode()
		{
			return m_pPipeline->m_eCurrentMode;
		}
		inline matrix4x4 & GetCurrentMatrix()
		{
			return m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top();
		}
		inline matrix4x4 const & GetCurrentMatrix()const
		{
			return m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top();
		}

	protected:
		Pipeline * m_pPipeline;
	};
}

#pragma warning( pop )

#endif
