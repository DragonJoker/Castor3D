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
#ifndef ___C3D_RenderPipeline_H___
#define ___C3D_RenderPipeline_H___

#include "Castor3DPrerequisites.hpp"

#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"

#include <Math/SquareMatrix.hpp>
#include <Design/OwnedBy.hpp>

#include <stack>

namespace Castor3D
{
	static const uint32_t C3D_MAX_TEXTURE_MATRICES = 4;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		The rendering pipeline.
	\remark		Defines the various matrices, applies the transformations it can support.
	\~french
	\brief		Le pipeline de rendu.
	\remark		Définit les diverses matrices, applique les transformations supportées.
	*/
	class RenderPipeline
		: public Castor::OwnedBy< RenderSystem >
	{
	private:
		using MatrixStack = std::stack< Castor::Matrix4x4r >;
		using ShaderObjectSet = std::set< ShaderObjectSPtr >;
		using BindingArray = std::vector< std::reference_wrapper< UniformBufferBinding > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The parent RenderSystem.
		 *\param[in]	p_dsState		The depth stencil state.
		 *\param[in]	p_rsState		The rateriser state.
		 *\param[in]	p_blState		The blend state.
		 *\param[in]	p_msState		The multisample state.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_flags			The creation flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem parent.
		 *\param[in]	p_dsState		L'état de stencil et profondeur.
		 *\param[in]	p_rsState		L'état de rastériseur.
		 *\param[in]	p_blState		L'état de mélange.
		 *\param[in]	p_msState		L'état de multi-échantillonnage.
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_flags			Les indicateurs de création.
		 */
		C3D_API explicit RenderPipeline( RenderSystem & p_renderSystem
			, DepthStencilState && p_dsState
			, RasteriserState && p_rsState
			, BlendState && p_blState
			, MultisampleState && p_msState
			, ShaderProgram & p_program
			, PipelineFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Denstructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~RenderPipeline();
		/**
		 *\~english
		 *\brief		Cleans up the pipeline.
		 *\~french
		 *\brief		Nettoie le pipeline.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Applies the pipeline.
		 *\~french
		 *\brief		Applique le pipeline.
		 */
		C3D_API virtual void Apply()const = 0;
		/**
		 *\~english
		 *\brief		Adds a uniform buffer to the pipeline.
		 *\remarks		Creates the binding for this uniform buffer, using the pipeline's program.
		 *\~french
		 *\brief		Ajoute un tampon d'uniformes à ce pipeline.
		 *\remarks		Crée le binding pour ce tampon, en utilisant le programme de ce pipeline.
		 */
		C3D_API void AddUniformBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\return		The shader program.
		 *\~french
		 *\return		Le programme shader.
		 */
		inline ShaderProgram const & GetProgram()const
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The shader program.
		 *\~french
		 *\return		Le programme shader.
		 */
		inline ShaderProgram & GetProgram()
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The uniform buffer bindings.
		 *\~french
		 *\return		Les bindings des tampons de variables uniformes.
		 */
		inline BindingArray const & GetBindings()const
		{
			return m_bindings;
		}
		/**
		 *\~english
		 *\return		The creation flags.
		 *\~french
		 *\return		Les indicateurs de création.
		 */
		inline PipelineFlags const & GetFlags()const
		{
			return m_flags;
		}
		/**
		 *\~english
		 *\return		The count of textures used by the program.
		 *\~french
		 *\return		Le nombre de textures utilisées par le programme.
		 */
		inline uint32_t GetTexturesCount()
		{
			return m_textureCount;
		}
		/**
		 *\~english
		 *\return		The directional lights shadow maps frame variable.
		 *\~french
		 *\return		La variable shader des maps d'ombres pour les lumières directionnelles.
		 */
		inline PushUniform1s & GetDirectionalShadowMapsVariable()
		{
			REQUIRE( m_directionalShadowMaps );
			return *m_directionalShadowMaps;
		}
		/**
		 *\~english
		 *\return		The spot lights shadow maps frame variable.
		 *\~french
		 *\return		La variable shader des maps d'ombres pour les lumières spots.
		 */
		inline PushUniform1s & GetSpotShadowMapsVariable()
		{
			REQUIRE( m_spotShadowMaps );
			return *m_spotShadowMaps;
		}
		/**
		 *\~english
		 *\return		The point lights shadow maps frame variable.
		 *\~french
		 *\return		La variable shader des maps d'ombres pour les lumières ponctuelles.
		 */
		inline PushUniform1s & GetPointShadowMapsVariable()
		{
			REQUIRE( m_pointShadowMaps );
			return *m_pointShadowMaps;
		}
		/**
		 *\~english
		 *\return		The reflection map frame variable.
		 *\~french
		 *\return		La variable shader de la reflection map.
		 */
		inline PushUniform1s & GetEnvironmentMapVariable()
		{
			REQUIRE( m_environmentMap );
			return *m_environmentMap;
		}

	public:
		C3D_API static const Castor::String MtxProjection;
		C3D_API static const Castor::String MtxModel;
		C3D_API static const Castor::String MtxView;
		C3D_API static const Castor::String MtxInvProjection;
		C3D_API static const Castor::String MtxNormal;
		C3D_API static const Castor::String MtxTexture[C3D_MAX_TEXTURE_MATRICES];

	protected:
		//!\~english	The depth stencil state.
		//!\~french		L'état de stencil et profondeur.
		DepthStencilState m_dsState;
		//!\~english	The rateriser state.
		//!\~french		L'état de rastériseur.
		RasteriserState m_rsState;
		//!\~english	The blend state.
		//!\~french		L'état de mélange.
		BlendState m_blState;
		//!\~english	The muultisampling state.
		//!\~french		L'état de multi-échantillonnage.
		MultisampleState m_msState;
		//!\~english	The shader program.
		//!\~french		Le programme shader.
		ShaderProgram & m_program;
		//!\~english	The directional lights shadow maps frame variable.
		//!\~french		La variable shader pour les maps d'ombres des lumières directionnelles.
		PushUniform1sSPtr m_directionalShadowMaps;
		//!\~english	The spot lights shadow maps frame variable.
		//!\~french		La variable shader pour les maps d'ombres des lumières spot.
		PushUniform1sSPtr m_spotShadowMaps;
		//!\~english	The point lights shadow maps frame variable.
		//!\~french		La variable shader pour les maps d'ombres des lumières ponctuelles.
		PushUniform1sSPtr m_pointShadowMaps;
		//!\~english	The environment map frame variable.
		//!\~french		La variable shader pour la texture d'environnement.
		PushUniform1sSPtr m_environmentMap;
		//!\~english	The creation flags.
		//!\~french		Les indicateurs de création.
		PipelineFlags m_flags;
		//!\~english	The count of textures used by the program.
		//!\~french		Le nombre de textures utilisées par le programme.
		uint32_t m_textureCount{ 0u };
		//!\~english	The uniform buffer bindings.
		//!\~french		Les bindings de tampons d'uniformes.
		BindingArray m_bindings;
	};
}

#endif
