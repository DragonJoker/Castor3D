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
	\brief		Implementation of the rendering pipeline.
	\remark		Defines the various matrices, applies the transformations it can support.
	\~french
	\brief		Implémentation du pipeline de rendu.
	\remark		Définit les diverses matrices, applique les transformations supportées.
	*/
	class C3D_API IPipelineImpl
	{
	protected:
		friend class Pipeline;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_pipeline		The parent pipeline.
		 *\param[in]	p_rightHanded	Tells if the pipeline is right handed (true) or left handed (false).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_pipeline		Le pipeline parent.
		 *\param[in]	p_rightHanded	Dit si le pipeline utilise la main droite (true) ou la main gauche (false).
		 */
		IPipelineImpl( Pipeline & p_pipeline, bool p_rightHanded );
		/**
		 *\~english
		 *\brief		Denstructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~IPipelineImpl();
		/**
		 *\~english
		 *\brief		Puts the given matrix in the given frame variables buffer.
		 *\param[in]	p_matrix		The matrix.
		 *\param[in]	p_name			The shader variable name.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice donnée dans le buffer de variables donné.
		 *\param[in]	p_matrix		La matrice.
		 *\param[in]	p_name			Le nom de la variable shader.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		virtual void ApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Applies the given viewport dimension.
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	The dimensions.
		 *\~french
		 *\brief		Applique les dimensions de viewport données.
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	Les dimensions.
		 */
		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight ) = 0;
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters.
		 *\param[out]	p_result	The matrix that will receive the result.
		 *\param[in]	p_aFOVY		Y Field of View.
		 *\param[in]	p_aspect	Width / Height ratio.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée.
		 *\param[out]	p_result	La matrice qui contiendra le résultat.
		 *\param[in]	p_aFOVY		Angle de vision Y.
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 */
		virtual void Perspective( Castor::Matrix4x4r & p_result, Castor::Angle const & p_aFOVY, real p_aspect, real p_near, real p_far ) = 0;
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters.
		 *\param[out]	p_result	The matrix that will receive the result.
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée.
		 *\param[out]	p_result	La matrice qui contiendra le résultat.
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 */
		virtual void Frustum( Castor::Matrix4x4r & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far ) = 0;
		/**
		 *\~english
		 *\brief		Builds a matrix that sets an orthogonal projection.
		 *\param[out]	p_result	The matrix that will receive the result.
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection orthographique.
		 *\param[out]	p_result	La matrice qui contiendra le résultat.
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 */
		virtual void Ortho( Castor::Matrix4x4r & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far ) = 0;
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point.
		 *\param[out]	p_result	The matrix that will receive the result.
		 *\param[in]	p_eye		The eye position.
		 *\param[in]	p_center	The point to look at.
		 *\param[in]	p_up		The up direction..
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné.
		 *\param[out]	p_result	La matrice qui contiendra le résultat.
		 *\param[in]	p_eye		La position de l'oeil.
		 *\param[in]	p_center	Le point à regarder.
		 *\param[in]	p_up		La direction vers le haut.
		 */
		virtual void LookAt( Castor::Matrix4x4r & p_result, Castor::Point3r const & p_eye, Castor::Point3r const & p_center, Castor::Point3r const & p_up ) = 0;

	protected:
		//!\~english The parent pipeline	\~french Le pipeline parent
		Pipeline & m_pipeline;
		//!\~english Tells if the pipeline is right handed (true) or left handed (false).	\~french Dit si le pipeline utilise la main droite (true) ou la main gauche (false).
		bool m_rightHanded;
	};
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
	class C3D_API Pipeline
		: public Castor::OwnedBy< RenderSystem >
	{
	protected:
		friend class IPipelineImpl;

	private:
		typedef std::stack< Castor::Matrix4x4r > MatrixStack;
		typedef std::set< ShaderObjectBaseSPtr > ShaderObjectSet;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The render system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le render system.
		 */
		Pipeline( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Denstructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Pipeline();
		/**
		 *\~english
		 *\brief		Initialises the pipeline.
		 *\~french
		 *\brief		Initialise le pipeline.
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Projects the given screen point to 3D scene point.
		 *\param[in]	p_ptObj			The screen coordinates.
		 *\param[in]	p_ptViewport	The viewport.
		 *\param[out]	p_ptResult		Receives the scene coordinates.
		 *\return		\p false if the current mode is not valid.
		 *\~french
		 *\brief		Projette le point écran donné en point 3D.
		 *\param[in]	p_ptObj			Les coordonnées écran.
		 *\param[in]	p_ptViewport	Le viewport.
		 *\param[in]	p_ptResult		Reçoit coordonnées dans la scène.
		 *\return		\p false si le mode courant est invalide.
		 */
		bool Project( Castor::Point3r const & p_ptObj, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		/**
		 *\~english
		 *\brief		Unprojects the given scene point to screen point.
		 *\param[in]	p_ptObj			The scene coordinates.
		 *\param[in]	p_ptViewport	The viewport.
		 *\param[out]	p_ptResult		Receives the screen coordinates.
		 *\return		\p false if the current mode is not valid.
		 *\~french
		 *\brief		Dé-projette le point dans la scène donné en point écran.
		 *\param[in]	p_ptObj			Les coordonnées dans la scène.
		 *\param[in]	p_ptViewport	Le viewport.
		 *\param[in]	p_ptResult		Reçoit les coordonnées écran.
		 *\return		\p false si le mode courant est invalide.
		 */
		bool UnProject( Castor::Point3i const & p_ptWin, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		/**
		 *\~english
		 *\brief		Puts the current projection matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de projection dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyProjection( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current model matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de modèle dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyModel( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current vieww matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de vue dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current normals matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice de normales dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyNormal( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current model/view matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice modèle/vue dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyModelView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current projection/view matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice projection/vue dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyProjectionView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current projection/model/view matrix into the given frame variables buffer.
		 *\param[in]	p_matrixBuffer	The matrix variables buffer.
		 *\~french
		 *\brief		Met la matrice projection/modèle/vue dans le buffer de variables donné.
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices.
		 */
		void ApplyProjectionModelView( FrameVariableBuffer & p_matrixBuffer );
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
		void ApplyTexture( uint32_t p_index, FrameVariableBuffer & p_matrixBuffer );
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
		void ApplyMatrices( FrameVariableBuffer & p_matrixBuffer, uint64_t p_matricesMask );
		/**
		 *\~english
		 *\brief		Applies the given viewport dimension.
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	The dimensions.
		 *\~french
		 *\brief		Applique les dimensions de viewport données.
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	Les dimensions.
		 */
		void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[in]	p_aFOVY		Y Field of View
		 *\param[in]	p_aspect	Width / Height ratio
		 *\param[in]	p_near		Near clipping plane value
		 *\param[in]	p_far		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[in]	p_aFOVY		Angle de vision Y
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur
		 *\param[in]	p_near		Position du plan proche
		 *\param[in]	p_far		Position du plan lointain
		 */
		void Perspective( Castor::Angle const & p_aFOVY, real p_aspect, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters
		 *\param[in]	p_left		Left clipping plane value
		 *\param[in]	p_right		Right clipping plane value
		 *\param[in]	p_bottom	Bottom clipping plane value
		 *\param[in]	p_top		Top clipping plane value
		 *\param[in]	p_near		Near clipping plane value
		 *\param[in]	p_far		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée
		 *\param[in]	p_left		Position du plan gauche
		 *\param[in]	p_right		Position du plan droit
		 *\param[in]	p_bottom	Position du plan bas
		 *\param[in]	p_top		Position du plan haut
		 *\param[in]	p_near		Position du plan proche
		 *\param[in]	p_far		Position du plan lointain
		 */
		void Frustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[in]	p_left		Left clipping plane value
		 *\param[in]	p_right		Right clipping plane value
		 *\param[in]	p_bottom	Bottom clipping plane value
		 *\param[in]	p_top		Top clipping plane value
		 *\param[in]	p_near		Near clipping plane value
		 *\param[in]	p_far		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[in]	p_left		Position du plan gauche
		 *\param[in]	p_right		Position du plan droit
		 *\param[in]	p_bottom	Position du plan bas
		 *\param[in]	p_top		Position du plan haut
		 *\param[in]	p_near		Position du plan proche
		 *\param[in]	p_far		Position du plan lointain
		 */
		void Ortho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point.
		 *\param[in]	p_eye		The eye position.
		 *\param[in]	p_center	The point to look at.
		 *\param[in]	p_up		The up direction..
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné.
		 *\param[in]	p_eye		La position de l'oeil.
		 *\param[in]	p_center	Le point à regarder.
		 *\param[in]	p_up		La direction vers le haut.
		 */
		void LookAt( Castor::Point3r const & p_eye, Castor::Point3r const & p_center, Castor::Point3r const & p_up );
		/**
		 *\~english
		 *\brief		Updates the used implementation.
		 *\~french
		 *\brief		Met à jour l'implémentation utilisée.
		 */
		virtual void UpdateImpl();
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
			CASTOR_ASSERT( p_index < C3D_MAX_TEXTURE_MATRICES );
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
			CASTOR_ASSERT( p_index < C3D_MAX_TEXTURE_MATRICES );
			return m_mtxTexture[p_index];
		}

	private:
		void DoApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, FrameVariableBuffer & p_matrixBuffer );
		IPipelineImplSPtr DoGetImpl()const
		{
			return m_impl.lock();
		}

	public:
		static const Castor::String MtxProjection;
		static const Castor::String MtxModel;
		static const Castor::String MtxView;
		static const Castor::String MtxModelView;
		static const Castor::String MtxProjectionView;
		static const Castor::String MtxProjectionModelView;
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
		//!\~english The model-view matrix	\~french La matrice modèle-vue
		Castor::Matrix4x4r m_mtxModelView;
		//!\~english The projection-view matrix	\~french La matrice projection-vue
		Castor::Matrix4x4r m_mtxProjectionView;
		//!\~english The projection-model-view matrix	\~french La matrice projection-modèle-vue
		Castor::Matrix4x4r m_mtxProjectionModelView;
		//!\~english The normals matrix	\~french La matrice des normales
		Castor::Matrix4x4r m_mtxNormal;
		//!\~english The texture matrices	\~french Les matrices de texture
		Castor::Matrix4x4r m_mtxTexture[C3D_MAX_TEXTURE_MATRICES];
		//!\~english The driver specific Pipeline implementation	\~french L'implémentation du Pipeline, fournie par le driver
		IPipelineImplWPtr m_impl;
	};
}

#endif
