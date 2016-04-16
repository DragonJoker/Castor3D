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

#include <SquareMatrix.hpp>
#include <OwnedBy.hpp>

#include <stack>

namespace Castor3D
{
	static const uint32_t C3D_MAX_TEXTURE_MATRICES = 10;
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
		: public Castor::OwnedBy< Context >
	{
	protected:
		friend class IPipelineImpl;

	private:
		typedef std::stack< Castor::Matrix4x4r > MatrixStack;
		typedef std::set< ShaderObjectSPtr > ShaderObjectSet;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_context	The parent context.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_context	Le contexte parent.
		 */
		C3D_API Pipeline( Context & p_context );
		/**
		 *\~english
		 *\brief		Denstructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Pipeline();
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
		C3D_API void ApplyProjection( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current model matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de modèle dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyModel( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current vieww matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de vue dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current normals matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de normales dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		C3D_API void ApplyNormal( FrameVariableBuffer & p_matrixBuffer );
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
		C3D_API void ApplyTexture( uint32_t p_index, FrameVariableBuffer & p_matrixBuffer );
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
		C3D_API void ApplyMatrices( FrameVariableBuffer & p_matrixBuffer, uint64_t p_matricesMask );
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
		void DoApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, FrameVariableBuffer & p_matrixBuffer );

	public:
		static const Castor::String MtxProjection;
		static const Castor::String MtxModel;
		static const Castor::String MtxView;
		static const Castor::String MtxNormal;
		static const Castor::String MtxTexture[C3D_MAX_TEXTURE_MATRICES];
		static const Castor::String MtxBones;

	public:
		//!\~english The identity matrix	\~french La matrice identité
		Castor::Matrix4x4r m_mtxIdentity;

	protected:
		//!\~english The model matrix	\~french La matrice modèle
		Castor::Matrix4x4r m_mtxModel;
		//!\~english The view matrix	\~french La matrice vue
		Castor::Matrix4x4r m_mtxView;
		//!\~english The projection matrix	\~french La matrice projection
		Castor::Matrix4x4r m_mtxProjection;
		//!\~english The normals matrix	\~french La matrice des normales
		Castor::Matrix4x4r m_mtxNormal;
		//!\~english The texture matrices	\~french Les matrices de texture
		Castor::Matrix4x4r m_mtxTexture[C3D_MAX_TEXTURE_MATRICES];
	};
}

#endif
