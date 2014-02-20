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
#ifndef ___Dx9_ShaderProgram___
#define ___Dx9_ShaderProgram___

#include "Module_Dx9Render.hpp"

namespace Dx9Render
{
	class DxShaderProgram : public Castor3D::ShaderProgramBase
	{
	protected:
		Castor::String		m_linkerLog;
		ID3DXEffect *		m_pEffect;
		uint8_t				m_byIndex;

	public:
		DxShaderProgram( DxRenderSystem * p_pRenderSystem );
		virtual ~DxShaderProgram();

		virtual void Initialise();
		/**
		 * Get Linker Messages
		 */
		virtual void RetrieveLinkerLog( Castor::String & strLog);
		
		virtual void Cleanup();
		virtual void Begin( uint8_t p_byIndex, uint8_t p_byCount );
		virtual void End();

		virtual void SetCameraPos( Castor::Point3f const & p_ptPosition );
		virtual void SetAmbientLight( Castor::Colour const & p_crColour);
		virtual void SetMaterialAmbient( Castor::Colour const & p_crColour);
		virtual void SetMaterialDiffuse( Castor::Colour const & p_crColour);
		virtual void SetMaterialSpecular( Castor::Colour const & p_crColour);
		virtual void SetMaterialEmissive( Castor::Colour const & p_crColour);
		virtual void SetMaterialShininess( float p_fShine);
		virtual void SetMaterialOpacity( float p_fOpacity );

		int GetAttributeLocation( Castor::String const & CU_PARAM_UNUSED( p_strName ) )const{ return 0; }
		inline ID3DXEffect *& GetShaderEffect() { return m_pEffect; }
		inline ID3DXEffect * const & GetShaderEffect()const { return m_pEffect; }

	private:
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags );
		virtual Castor::String DoGetPixelShaderSource( uint32_t p_uiFlags );
		virtual Castor::String DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass );
		virtual Castor::String DoGetDeferredPixelShaderSource( uint32_t p_uiFlags );
		Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_eType );
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable > DoCreateTextureVariable( int p_iNbOcc );
	};
}

#endif
