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
#ifndef ___C3D_PIPELINE_H___
#define ___C3D_PIPELINE_H___

#include "Castor3DPrerequisites.hpp"

#include "State/BlendState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"

#include <Math/SquareMatrix.hpp>
#include <Design/OwnedBy.hpp>

#include <stack>

namespace Castor3D
{
	static const uint32_t C3D_MAX_TEXTURE_MATRICES = 16;
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
	class Pipeline
		: public Castor::OwnedBy< RenderSystem >
	{
	private:
		typedef std::stack< Castor::Matrix4x4r > MatrixStack;
		typedef std::set< ShaderObjectSPtr > ShaderObjectSet;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The parent RenderSystem.
		 *\param[in]	p_rsState		The rateriser state.
		 *\param[in]	p_blState		The blend state.
		 *\param[in]	p_msState		The multisample state.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem parent.
		 *\param[in]	p_rsState		L'état de rastériseur.
		 *\param[in]	p_blState		L'état de mélange.
		 *\param[in]	p_msState		L'état de multi-échantillonnage.
		 */
		C3D_API explicit Pipeline( RenderSystem & p_renderSystem
								   , RasteriserStateUPtr && p_rsState
								   , BlendStateUPtr && p_blState
								   , MultisampleStateUPtr && p_msState );
		/**
		 *\~english
		 *\brief		Denstructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Pipeline();
		/**
		 *\~english
		 *\brief		Applies the pipeline.
		 *\~french
		 *\brief		Applique le pipeline.
		 */
		C3D_API virtual void Apply()const = 0;
		/**
		 *\~english
		 *\brief		Projects the given screen point to 3D scene point.
		 *\param[in]	p_screen	The screen coordinates.
		 *\param[in]	p_viewport	The viewport.
		 *\param[out]	p_result	Receives the scene coordinates.
		 *\return		\p false if the current mode is not valid.
		 *\~french
		 *\brief		Projette le point écran donné en point 3D.
		 *\param[in]	p_screen	Les coordonnées écran.
		 *\param[in]	p_viewport	Le viewport.
		 *\param[out]	p_result	Reçoit coordonnées dans la scène.
		 *\return		\p false si le mode courant est invalide.
		 */
		C3D_API bool Project( Castor::Point3r const & p_screen, Castor::Point4r const & p_viewport, Castor::Point3r & p_result );
		/**
		 *\~english
		 *\brief		Unprojects the given scene point to screen point.
		 *\param[in]	p_scene		The scene coordinates.
		 *\param[in]	p_viewport	The viewport.
		 *\param[out]	p_result	Receives the screen coordinates.
		 *\return		\p false if the current mode is not valid.
		 *\~french
		 *\brief		Dé-projette le point dans la scène donné en point écran.
		 *\param[in]	p_scene		Les coordonnées dans la scène.
		 *\param[in]	p_viewport	Le viewport.
		 *\param[out]	p_result	Reçoit les coordonnées écran.
		 *\return		\p false si le mode courant est invalide.
		 */
		C3D_API bool UnProject( Castor::Point3i const & p_scene, Castor::Point4r const & p_viewport, Castor::Point3r & p_result );
		/**
		 *\~english
		 *\brief		Puts the current projection matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de projection dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyProjection( FrameVariableBuffer const & p_matrixBuffer )const;
		/**
		 *\~english
		 *\brief		Puts the current model matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de modèle dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyModel( FrameVariableBuffer const & p_matrixBuffer )const;
		/**
		 *\~english
		 *\brief		Puts the current vieww matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de vue dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyView( FrameVariableBuffer const & p_matrixBuffer )const;
		/**
		 *\~english
		 *\brief		Puts the current normals matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de normales dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyNormal( FrameVariableBuffer const & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current texture 0 matrix into the given frame variables buffer.
		 *\param[in]	p_index			The texture index.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de texture 0 dans le buffer de variables donné.
		 *\param[in]	p_index			L'indice de la texture.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyTexture( uint32_t p_index, FrameVariableBuffer const & p_matrixBuffer )const;
		/**
		 *\~english
		 *\brief		Puts all the matrices in the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\param[in]	p_matricesMask	A bitwise OR combination of MASK_MTXMODE, to select the matrices to apply.
		 *\~french
		 *\brief		Met toutes les matrices dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 *\param[in]	p_matricesMask	Une combinaison en OU binaire de MASK_MTXMODE, pour filtrer les matrices à appliquer.
		 */
		C3D_API void ApplyMatrices( FrameVariableBuffer const & p_matrixBuffer, uint64_t p_matricesMask );
		/**
		 *\~english
		 *\brief		Sets the model matrix.
		 *\param[in]	p_mtx	The new matrix.
		 *\~french
		 *\brief		Définit la matrice modèle.
		 *\param[in]	p_mtx	La nouvelle matrice.
		 */
		inline void SetModelMatrix( Castor::Matrix4x4r const & p_mtx )
		{
			m_mtxModel = p_mtx;
		}
		/**
		 *\~english
		 *\brief		Sets the view matrix.
		 *\param[in]	p_mtx	The new matrix.
		 *\~french
		 *\brief		Définit la matrice vue.
		 *\param[in]	p_mtx	La nouvelle matrice.
		 */
		inline void SetViewMatrix( Castor::Matrix4x4r const & p_mtx )
		{
			m_mtxView = p_mtx;
		}
		/**
		 *\~english
		 *\brief		Sets the projection matrix.
		 *\param[in]	p_mtx	The new matrix.
		 *\~french
		 *\brief		Définit la matrice projection.
		 *\param[in]	p_mtx	La nouvelle matrice.
		 */
		inline void SetProjectionMatrix( Castor::Matrix4x4r const & p_mtx )
		{
			m_mtxProjection = p_mtx;
		}
		/**
		 *\~english
		 *\brief		Sets the projection matrix.
		 *\param[in]	p_index	The texture index.
		 *\param[in]	p_mtx	The new matrix.
		 *\~french
		 *\brief		Définit la matrice projection.
		 *\param[in]	p_index	L'indice de la texture.
		 *\param[in]	p_mtx	La nouvelle matrice.
		 */
		inline void SetTextureMatrix( uint32_t p_index, Castor::Matrix4x4r const & p_mtx )
		{
			REQUIRE( p_index < C3D_MAX_TEXTURE_MATRICES );
			m_mtxTexture[p_index] = p_mtx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the model matrix.
		 *\return		The matrix.
		 *\~french
		 *\brief		Récupère la matrice modèle.
		 *\return		La matrice.
		 */
		inline Castor::Matrix4x4r const & GetModelMatrix()const
		{
			return m_mtxModel;
		}
		/**
		 *\~english
		 *\brief		Retrieves the view matrix.
		 *\return		The matrix.
		 *\~french
		 *\brief		Récupère la matrice vue.
		 *\return		La matrice.
		 */
		inline Castor::Matrix4x4r const & GetViewMatrix()const
		{
			return m_mtxView;
		}
		/**
		 *\~english
		 *\brief		Retrieves the projection matrix.
		 *\return		The matrix.
		 *\~french
		 *\brief		Récupère la matrice projection.
		 *\return		La matrice.
		 */
		inline Castor::Matrix4x4r const & GetProjectionMatrix()const
		{
			return m_mtxProjection;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture matrix for given index.
		 *\param[in]	p_index	The texture index.
		 *\return		The matrix.
		 *\~french
		 *\brief		Récupère la matrice de texture pour l'indice donné.
		 *\param[in]	p_index	L'indice de la texture.
		 *\return		La matrice.
		 */
		inline Castor::Matrix4x4r const & GetTextureMatrix( uint32_t p_index )const
		{
			REQUIRE( p_index < C3D_MAX_TEXTURE_MATRICES );
			return m_mtxTexture[p_index];
		}

	private:
		void DoApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, FrameVariableBuffer const & p_matrixBuffer )const;

	public:
		C3D_API static const Castor::String MtxProjection;
		C3D_API static const Castor::String MtxModel;
		C3D_API static const Castor::String MtxView;
		C3D_API static const Castor::String MtxNormal;
		C3D_API static const Castor::String MtxTexture[C3D_MAX_TEXTURE_MATRICES];

	protected:
		//!\~english	The model matrix.
		//!\~french		La matrice modèle.
		Castor::Matrix4x4r m_mtxModel;
		//!\~english	The view matrix.
		//!\~french		La matrice vue.
		Castor::Matrix4x4r m_mtxView;
		//!\~english	The projection matrix.
		//!\~french		La matrice projection.
		Castor::Matrix4x4r m_mtxProjection;
		//!\~english	The normals matrix.
		//!\~french		La matrice des normales.
		Castor::Matrix4x4r m_mtxNormal;
		//!\~english	The texture matrices.
		//!\~french		Les matrices de texture.
		Castor::Matrix4x4r m_mtxTexture[C3D_MAX_TEXTURE_MATRICES];
		//!\~english	The rateriser state.
		//!\~french		L'état de rastériseur.
		RasteriserStateUPtr m_rsState;
		//!\~english	The blend state.
		//!\~french		L'état de mélange.
		BlendStateUPtr m_blState;
		//!\~english	The muultisampling state.
		//!\~french		L'état de multi-échantillonnage.
		MultisampleStateUPtr m_msState;
	};
}

#endif
