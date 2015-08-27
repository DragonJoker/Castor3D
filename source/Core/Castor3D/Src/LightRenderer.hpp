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
#ifndef ___C3D_LIGHT_RENDERER_H___
#define ___C3D_LIGHT_RENDERER_H___

#include "Renderer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Renders a light, applies it's colours values...
	\~french
	\brief		Rend une lumière, ses couleurs, positions, ...
	*/
	class C3D_API LightRenderer : public Renderer<Light, LightRenderer>
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		LightRenderer( RenderSystem * p_pRenderSystem );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~LightRenderer();
		/**
		 *\~english
		 *\brief		Initialises the renderer
		 *\~french
		 *\brief		Initialise le renderer
		 */
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Enables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Active la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Enable() = 0;
		/**
		 *\~english
		 *\brief		Disables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Désactive la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Disable() = 0;
		/**
		 *\~english
		 *\brief		Enables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Active la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Bind() {}
		/**
		 *\~english
		 *\brief		Disables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Désactive la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Unbind() {}
		/**
		 *\~english
		 *\brief		Applies ambient colour
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la couleur ambiante
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyAmbient() = 0;
		/**
		 *\~english
		 *\brief		Applies diffuse colour
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la couleur diffuse
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyDiffuse() = 0;
		/**
		 *\~english
		 *\brief		Applies specular colour
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la couleur speculaire
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplySpecular() = 0;
		/**
		 *\~english
		 *\brief		Applies light position
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la position de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyPosition() = 0;
		/**
		 *\~english
		 *\brief		Applies light orientation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'orientation de la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyOrientation() = 0;
		/**
		 *\~english
		 *\brief		Applies light constant attenuation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'attenuation constante de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyConstantAtt( float p_fConstant ) = 0;
		/**
		 *\~english
		 *\brief		Applies light linear attenuation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'attenuation linéaire de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyLinearAtt( float p_fLinear ) = 0;
		/**
		 *\~english
		 *\brief		Applies light quadratic attenuation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'attenuation quadratique de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyQuadraticAtt( float p_fQuadratic ) = 0;
		/**
		 *\~english
		 *\brief		Applies light exponent
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'exposant de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyExponent( float p_fExponent ) = 0;
		/**
		 *\~english
		 *\brief		Applies light cut off
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'angle du cône de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyCutOff( float p_fCutOff ) = 0;
		/**
		 *\~english
		 *\brief		Enables the light
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Active la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void EnableShader( ShaderProgramBase * p_pProgram ) = 0;
		/**
		 *\~english
		 *\brief		Disables the light
		 *\remark		Shader implementation
		 *\~french
		 *\brief		désactive la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void DisableShader( ShaderProgramBase * p_pProgram ) = 0;
		/**
		 *\~english
		 *\brief		Applies ambient colour
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la couleur ambiante
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyAmbientShader() = 0;
		/**
		 *\~english
		 *\brief		Applies diffuse colour
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la couleur diffuse
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyDiffuseShader() = 0;
		/**
		 *\~english
		 *\brief		Applies specular colour
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la couleur speculaire
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplySpecularShader() = 0;
		/**
		 *\~english
		 *\brief		Applies light position
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la position de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyPositionShader() = 0;
		/**
		 *\~english
		 *\brief		Applies light orientation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'orientation de la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyOrientationShader() = 0;
		/**
		 *\~english
		 *\brief		Applies light constant attenuation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'attenuation constante de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyConstantAttShader( float p_fConstant ) = 0;
		/**
		 *\~english
		 *\brief		Applies light linear attenuation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'attenuation linéaire de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyLinearAttShader( float p_fLinear ) = 0;
		/**
		 *\~english
		 *\brief		Applies light quadratic attenuation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'attenuation quadratique de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyQuadraticAttShader( float p_fQuadratic ) = 0;
		/**
		 *\~english
		 *\brief		Applies light exponent
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'exposant de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyExponentShader( float p_fExponent ) = 0;
		/**
		 *\~english
		 *\brief		Applies light cut off
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'angle du cône de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyCutOffShader( float p_fCutOff ) = 0;
	};
}

#endif
