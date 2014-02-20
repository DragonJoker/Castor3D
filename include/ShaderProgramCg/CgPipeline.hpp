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
#ifndef ___Cg_Pipeline___
#define ___Cg_Pipeline___

#include "Module_CgShader.hpp"

#if defined( Cg_Direct3D11 )
namespace Dx11Render
{
	class DxPipeline;
}
#endif

#if defined( Cg_Direct3D10 )
namespace Dx10Render
{
	class DxPipeline;
}
#endif

#if defined( Cg_Direct3D9 )
namespace Dx9Render
{
	class DxPipeline;
}
#endif

#if defined( Cg_OpenGL )
namespace GlRender
{
	class GlPipeline;
}
#endif

namespace CgShader
{
	class CgPipeline : public Castor3D::IPipelineImpl
	{
	protected:
		typedef std::map<	CGprogram,	CGparameter	>	ParameterProgramMap;

	protected:
		//! Cg context, used for Cg programs creation
		CGcontext	m_cgContext;
		CGprofile	m_mapSupportedProfiles[Castor3D::eSHADER_TYPE_COUNT];

	public:
		CgPipeline( Castor3D::Pipeline * p_pPipeline );
		virtual ~CgPipeline();

		void Initialise();
		inline CGcontext GetCgContext()const									{ return m_cgContext;						}
		inline CGprofile GetCgProfile( Castor3D::eSHADER_TYPE p_eType )const	{ return m_mapSupportedProfiles[p_eType];	}

	protected:
		virtual void DoCheckCgAvailableProfiles()=0;
	};

#if defined( Cg_Direct3D11 )
	class CgD3D11Pipeline : public CgPipeline
	{
	private:
		Dx11Render::DxPipeline *	m_pDxPipeline;
		D3D11_VIEWPORT				m_viewport;

	public:
		CgD3D11Pipeline( Dx11Render::DxPipeline * p_pPipeline );
		virtual ~CgD3D11Pipeline();

		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight);

	private:
		virtual void DoCheckCgAvailableProfiles();

		void DoIsCgProfileSupported( Castor3D::eSHADER_TYPE p_eType, char const * p_szName );
	};
#endif

#if defined( Cg_Direct3D10 )
	class CgD3D10Pipeline : public CgPipeline
	{
	private:
		Dx10Render::DxPipeline *	m_pDxPipeline;
		D3D10_VIEWPORT				m_viewport;

	public:
		CgD3D10Pipeline( Dx10Render::DxPipeline * p_pPipeline );
		virtual ~CgD3D10Pipeline();

		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight);

	private:
		virtual void DoCheckCgAvailableProfiles();

		void DoIsCgProfileSupported( Castor3D::eSHADER_TYPE p_eType, char const * p_szName );
	};
#endif

#if defined( Cg_Direct3D9 )
	class CgD3D9Pipeline : public CgPipeline
	{
	private:
		Dx9Render::DxPipeline *	m_pDxPipeline;
		D3DVIEWPORT9				m_viewport;

	public:
		CgD3D9Pipeline( Dx9Render::DxPipeline * p_pPipeline );
		virtual ~CgD3D9Pipeline();

		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight);

	private:
		virtual void DoCheckCgAvailableProfiles();

		void DoIsCgProfileSupported( Castor3D::eSHADER_TYPE p_eType, char const * p_szName );
	};
#endif

#if defined( Cg_OpenGL )
	class CgGlPipeline : public CgPipeline
	{
	protected:
		GlRender::GlPipeline *	m_pGlPipeline;

	public:
		CgGlPipeline( GlRender::GlPipeline * p_pPipeline );
		virtual ~CgGlPipeline();

		virtual void	ApplyViewport				( int p_iWindowWidth, int p_iWindowHeight);

	private:
		virtual void DoCheckCgAvailableProfiles();

		void DoIsCgProfileSupported( Castor3D::eSHADER_TYPE p_eType, char const * p_szName );
	};
#endif
}

#endif
