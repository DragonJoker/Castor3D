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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		The rendering pipeline
	\remark		Defines the various matrices, applies the transformations it can support
	\~french
	\brief		Le pipeline de rendu
	\remark		Définit les diverses matrices, applique les transformations supportées
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
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		Pipeline( RenderSystem * p_pRenderSystem );

	public:
		/**
		 *\~english
		 *\brief		Denstructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Pipeline();
		/**
		 *\~english
		 *\brief		Initialises the pipeline
		 *\~french
		 *\brief		Initialise le pipeline
		 */
		virtual void Initialise() = 0;
		/**
		 *\~english
		 *\brief		Updates the used implementation
		 *\param[in,opt]	p_pProgram	The shader
		 *\~french
		 *\brief		Met à jour l'implémentation utilisée
		 *\param[in,opt]	p_pProgram	Le shader
		 */
		virtual void UpdateFunctions( Castor3D::ShaderProgramBase * p_pProgram ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the render system
		 *\return		The render system
		 *\~french
		 *\brief		Récupère le render system
		 *\return		Le render system
		 */
		inline RenderSystem * GetRenderSystem()const
		{
			return m_pRenderSystem;
		}
		/**
		 *\~english
		 *\brief		Retrieves the matrix for given mode
		 *\param[in@	p_eMode	The matrix mode
		 *\return		The matrix
		 *\~french
		 *\brief		Récupère la matrice pour le mode donné
		 *\param[in@	p_eMode	Le mode
		 *\return		La matrice
		 */
		matrix4x4 const & GetMatrix( eMTXMODE p_eMode )const;
		/**
		 *\~english
		 *\brief		Retrieves the matrix for given mode
		 *\param[in@	p_eMode	The matrix mode
		 *\return		The matrix
		 *\~french
		 *\brief		Récupère la matrice pour le mode donné
		 *\param[in@	p_eMode	Le mode
		 *\return		La matrice
		 */
		matrix4x4 & GetMatrix( eMTXMODE p_eMode );
		/**
		 *\~english
		 *\brief		Tells if there is a matrix for the given mode
		 *\param[in]	p_eMode	The matrix mode
		 *\return		\p true id there is a matrix
		 *\~french
		 *\brief		Dit s'il y a une matrice pour le moce donné
		 *\param[in]	p_eMode	Le mode
		 *\return		\p true s'il y a une matrice
		 */
		bool HasMatrix( eMTXMODE p_eMode )const;
		/**
		 *\~english
		 *\brief		Sets the current mode for the matrix operations
		 *\param[in]	p_eMode	The matrix mode
		 *\return		The previous mode
		 *\~french
		 *\brief		Définit le mode courant, pour les opérations de matrices
		 *\param[in]	p_eMode	Le mode
		 *\return		Le mode précédent
		 */
		eMTXMODE MatrixMode( eMTXMODE p_eMode );
		/**
		 *\~english
		 *\brief		Sets the current matrix to the identity matrix
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Définit la matrice courante à la matrice identité
		 *\return		\p false si le mode courant est invalide
		 */
		bool LoadIdentity();
		/**
		 *\~english
		 *\brief		Pushes the current matrice in the current mode's matrix stack
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Met la matrice courante dans la pile de matrices du mode courant
		 *\return		\p false si le mode courant est invalide
		 */
		bool PushMatrix();
		/**
		 *\~english
		 *\brief		Pops the current matrice from the current mode's matrix stack
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Retire la matrice courante de la pile de matrices du mode courant
		 *\return		\p false si le mode courant est invalide
		 */
		bool PopMatrix();
		/**
		 *\~english
		 *\brief		Multiplies the current matrice from the current mode's with the given matrix
		 *\param[in]	p_matrix	The matrix to multiply
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Multiplie la matrice courante du mode courant par la mamtrice donnée
		 *\param[in]	p_matrix	La matrice à multiplier
		 *\return		\p false si le mode courant est invalide
		 */
		bool MultMatrix( Castor::Matrix4x4r const & p_matrix );
		/**
		 *\~english
		 *\brief		Multiplies the current matrice from the current mode's with the given matrix
		 *\param[in]	p_matrix	The matrix to multiply
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Multiplie la matrice courante du mode courant par la mamtrice donnée
		 *\param[in]	p_matrix	La matrice à multiplier
		 *\return		\p false si le mode courant est invalide
		 */
		bool MultMatrix( real const * p_matrix );
		/**
		 *\~english
		 *\brief		Creates a perspective matrix for current mode
		 *\param[in]	p_aFOVY		Specifies the field of view angle, in the y direction.
		 *\param[in]	p_rRatio	Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
		 *\param[in]	p_rNear		Specifies the distance from the viewer to the near clipping plane (always positive).
		 *\param[in]	p_rFar		Specifies the distance from the viewer to the far clipping plane (always positive).
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Crée une matrice de perspective pour le mode courant
		 *\param[in]	p_aFOVY		Définit l'angle d'ouverture en Y
		 *\param[in]	p_rRatio	Définit le ratio qui détermine l'angle de vue en X. L'aspet est le ratio de la largeur sur la hauteur.
		 *\param[in]	p_rNear		Définit la distance de l'observateur au plan de clipping proche (toujours positif).
		 *\param[in]	p_rFar		Définit la distnace de l'observateur au plan de clipping éloigné (toujours positif).
		 *\return		\p false si le mode courant est invalide
		 */
		bool Perspective( Castor::Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar );
		/**
		 *\~english
		 *\brief		Creates a frustum matrix for current mode
		 *\param[in]	p_rLeft, p_rRight	Specify the coordinates for the left and right vertical clipping planes.
		 *\param[in]	p_rBottom, p_rTop	Specify the coordinates for the bottom and top horizontal clipping planes.
		 *\param[in]	p_rNear, p_rFar		Specify the distances to the near and far depth clipping planes. Both distances must be positive.
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Crée une matrice de frustum pour le mode courant
		 *\param[in]	p_rLeft, p_rRight	Spécifie les coordonnées pour les plans de clipping verticaux, gauche et droite.
		 *\param[in]	p_rBottom, p_rTop	Spécifie les coordonnées pour les plans de clipping horizontaux, bas et haut.
		 *\param[in]	p_rNear, p_rFar		Spécifie les distance des plans de clipping de profondeur, proche et éloigné. Les deux distances doivent être positives
		 *\return		\p false si le mode courant est invalide
		 */
		bool Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		/**
		 *\~english
		 *\brief		Creates an orthographic matrix for current mode
		 *\param[in]	p_rLeft, p_rRight	Specify the coordinates for the left and right vertical clipping planes.
		 *\param[in]	p_rBottom, p_rTop	Specify the coordinates for the bottom and top horizontal clipping planes.
		 *\param[in]	p_rNear, p_rFar		Specify the distances to the near and far depth clipping planes. Both distances must be positive.
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Crée une matrice orthographique pour le mode courant
		 *\param[in]	p_rLeft, p_rRight	Spécifie les coordonnées pour les plans de clipping verticaux, gauche et droite.
		 *\param[in]	p_rBottom, p_rTop	Spécifie les coordonnées pour les plans de clipping horizontaux, bas et haut.
		 *\param[in]	p_rNear, p_rFar		Spécifie les distance des plans de clipping de profondeur, proche et éloigné. Les deux distances doivent être positives
		 *\return		\p false si le mode courant est invalide
		 */
		bool Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		/**
		 *\~english
		 *\brief		Projects the given screen point to 3D scene point
		 *\param[in]	p_ptObj			The screen coordinates
		 *\param[in]	p_ptViewport	The viewport
		 *\param[out]	p_ptResult		Receives the scene coordinates
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Projette le point écran donné en point 3D
		 *\param[in]	p_ptObj			Les coordonnées écran
		 *\param[in]	p_ptViewport	Le viewport
		 *\param[in]	p_ptResult		Reçoit coordonnées dans la scène
		 *\return		\p false si le mode courant est invalide
		 */
		bool Project( Castor::Point3r const & p_ptObj, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		/**
		 *\~english
		 *\brief		Unprojects the given scene point to screen point
		 *\param[in]	p_ptObj			The scene coordinates
		 *\param[in]	p_ptViewport	The viewport
		 *\param[out]	p_ptResult		Receives the screen coordinates
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Dé-projette le point dans la scène donné en point écran
		 *\param[in]	p_ptObj			Les coordonnées dans la scène
		 *\param[in]	p_ptViewport	Le viewport
		 *\param[in]	p_ptResult		Reçoit les coordonnées écran
		 *\return		\p false si le mode courant est invalide
		 */
		bool UnProject( Castor::Point3i const & p_ptWin, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		bool PickMatrix( real x, real y, real width, real height, int viewport[4] );
		/**
		 *\~english
		 *\brief		Puts the current projection matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de projection dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyProjection( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current model matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de modèle dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyModel( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current vieww matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de vue dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current normals matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de normales dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyNormal( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current model/view matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice modèle/vue dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyModelView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current projection/view matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice projection/vue dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyProjectionView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current projection/model/view matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice projection/modèle/vue dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyProjectionModelView( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current texture 0 matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de texture 0 dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyTexture0( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current texture 1 matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de texture 1 dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyTexture1( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current texture 2 matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de texture 2 dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyTexture2( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts the current texture 3 matrix into the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met la matrice de texture 3 dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		void ApplyTexture3( FrameVariableBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief		Puts all the matrices in the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\param[in]	p_matricesMask	A bitwise OR combination of MASK_MTXMODE, to select the matrices to apply
		 *\~french
		 *\brief		Met toutes les matrices dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 *\param[in]	p_matricesMask	Une combinaison en OU binaire de MASK_MTXMODE, pour filtrer les matrices à appliquer
		 */
		void ApplyMatrices( FrameVariableBuffer & p_matrixBuffer, uint64_t p_matricesMask );
		/**
		 *\~english
		 *\brief		Applies the given viewport dimension
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	The dimensions
		 *\~french
		 *\brief		Applique les dimensions de viewport données
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	Les dimensions
		 */
		void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight );

	private:
		void DoApplyMatrix( eMTXMODE p_eMatrix, Castor::String const & p_strName, FrameVariableBuffer & p_matrixBuffer );
		void DoApplyMatrix( matrix4x4 const & p_matrix, Castor::String const & p_strName, FrameVariableBuffer & p_matrixBuffer );

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
		//!\~english The identity matrix	\~french La matrice identité
		matrix4x4 m_mtxIdentity;

	protected:
		//!<
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
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Implementation of the rendering pipeline
	\remark		Defines the various matrices, applies the transformations it can support
	\~french
	\brief		Implémentation du pipeline de rendu
	\remark		Définit les diverses matrices, applique les transformations supportées
	*/
	class C3D_API IPipelineImpl
	{
	protected:
		typedef Pipeline::matrix4x4 matrix4x4;
		typedef Pipeline::matrix3x3 matrix3x3;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pPipeline	The parent pipeline
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pPipeline	Le pipeline parent
		 */
		IPipelineImpl( Pipeline * p_pPipeline );
		/**
		 *\~english
		 *\brief		Denstructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~IPipelineImpl();
		/**
		 *\~english
		 *\brief		Initialises the pipeline
		 *\~french
		 *\brief		Initialise le pipeline
		 */
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Sets the current mode for the matrix operations
		 *\param[in]	p_eMode	The matrix mode
		 *\return		The previous mode
		 *\~french
		 *\brief		Définit le mode courant, pour les opérations de matrices
		 *\param[in]	p_eMode	Le mode
		 *\return		Le mode précédent
		 */
		virtual eMTXMODE MatrixMode( eMTXMODE p_eMode );
		/**
		 *\~english
		 *\brief		Sets the current matrix to the identity matrix
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Définit la matrice courante à la matrice identité
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool LoadIdentity();
		/**
		 *\~english
		 *\brief		Pushes the current matrice in the current mode's matrix stack
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Met la matrice courante dans la pile de matrices du mode courant
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool PushMatrix();
		/**
		 *\~english
		 *\brief		Pops the current matrice from the current mode's matrix stack
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Retire la matrice courante de la pile de matrices du mode courant
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool PopMatrix();
		/**
		 *\~english
		 *\brief		Multiplies the current matrice from the current mode's with the given matrix
		 *\param[in]	p_matrix	The matrix to multiply
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Multiplie la matrice courante du mode courant par la mamtrice donnée
		 *\param[in]	p_matrix	La matrice à multiplier
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool MultMatrix( Castor::Matrix4x4r const & p_matrix );
		/**
		 *\~english
		 *\brief		Multiplies the current matrice from the current mode's with the given matrix
		 *\param[in]	p_matrix	The matrix to multiply
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Multiplie la matrice courante du mode courant par la mamtrice donnée
		 *\param[in]	p_matrix	La matrice à multiplier
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool MultMatrix( real const * p_matrix );
		/**
		 *\~english
		 *\brief		Creates a perspective matrix for current mode
		 *\param[in]	p_aFOVY		Specifies the field of view angle, in the y direction.
		 *\param[in]	p_rRatio	Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
		 *\param[in]	p_rNear		Specifies the distance from the viewer to the near clipping plane (always positive).
		 *\param[in]	p_rFar		Specifies the distance from the viewer to the far clipping plane (always positive).
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Crée une matrice de perspective pour le mode courant
		 *\param[in]	p_aFOVY		Définit l'angle d'ouverture en Y
		 *\param[in]	p_rRatio	Définit le ratio qui détermine l'angle de vue en X. L'aspet est le ratio de la largeur sur la hauteur.
		 *\param[in]	p_rNear		Définit la distance de l'observateur au plan de clipping proche (toujours positif).
		 *\param[in]	p_rFar		Définit la distnace de l'observateur au plan de clipping éloigné (toujours positif).
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool Perspective( Castor::Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar );
		/**
		 *\~english
		 *\brief		Creates a frustum matrix for current mode
		 *\param[in]	p_rLeft, p_rRight	Specify the coordinates for the left and right vertical clipping planes.
		 *\param[in]	p_rBottom, p_rTop	Specify the coordinates for the bottom and top horizontal clipping planes.
		 *\param[in]	p_rNear, p_rFar		Specify the distances to the near and far depth clipping planes. Both distances must be positive.
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Crée une matrice de frustum pour le mode courant
		 *\param[in]	p_rLeft, p_rRight	Spécifie les coordonnées pour les plans de clipping verticaux, gauche et droite.
		 *\param[in]	p_rBottom, p_rTop	Spécifie les coordonnées pour les plans de clipping horizontaux, bas et haut.
		 *\param[in]	p_rNear, p_rFar		Spécifie les distance des plans de clipping de profondeur, proche et éloigné. Les deux distances doivent être positives
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		/**
		 *\~english
		 *\brief		Creates an orthographic matrix for current mode
		 *\param[in]	p_rLeft, p_rRight	Specify the coordinates for the left and right vertical clipping planes.
		 *\param[in]	p_rBottom, p_rTop	Specify the coordinates for the bottom and top horizontal clipping planes.
		 *\param[in]	p_rNear, p_rFar		Specify the distances to the near and far depth clipping planes. Both distances must be positive.
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Crée une matrice orthographique pour le mode courant
		 *\param[in]	p_rLeft, p_rRight	Spécifie les coordonnées pour les plans de clipping verticaux, gauche et droite.
		 *\param[in]	p_rBottom, p_rTop	Spécifie les coordonnées pour les plans de clipping horizontaux, bas et haut.
		 *\param[in]	p_rNear, p_rFar		Spécifie les distance des plans de clipping de profondeur, proche et éloigné. Les deux distances doivent être positives
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar );
		/**
		 *\~english
		 *\brief		Projects the given screen point to 3D scene point
		 *\param[in]	p_ptObj			The screen coordinates
		 *\param[in]	p_ptViewport	The viewport
		 *\param[out]	p_ptResult		Receives the scene coordinates
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Projette le point écran donné en point 3D
		 *\param[in]	p_ptObj			Les coordonnées écran
		 *\param[in]	p_ptViewport	Le viewport
		 *\param[in]	p_ptResult		Reçoit coordonnées dans la scène
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool Project( Castor::Point3r const & p_ptObj, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		/**
		 *\~english
		 *\brief		Unprojects the given scene point to screen point
		 *\param[in]	p_ptObj			The scene coordinates
		 *\param[in]	p_ptViewport	The viewport
		 *\param[out]	p_ptResult		Receives the screen coordinates
		 *\return		\p false if the current mode is not valid
		 *\~french
		 *\brief		Dé-projette le point dans la scène donné en point écran
		 *\param[in]	p_ptObj			Les coordonnées dans la scène
		 *\param[in]	p_ptViewport	Le viewport
		 *\param[in]	p_ptResult		Reçoit les coordonnées écran
		 *\return		\p false si le mode courant est invalide
		 */
		virtual bool UnProject( Castor::Point3i const & p_ptWin, Castor::Point4r const & p_ptViewport, Castor::Point3r & p_ptResult );
		virtual bool PickMatrix( real x, real y, real width, real height, int viewport[4] );
		/**
		 *\~english
		 *\brief		Puts all the matrices in the given frame variables buffer
		 *\param[in]	p_matrixBuffer	The matrix variables buffer
		 *\~french
		 *\brief		Met toutes les matrices dans le buffer de variables donné
		 *\param[in]	p_matrixBuffer	Le buffer de variables de matrices
		 */
		virtual void ApplyMatrices( FrameVariableBuffer & CU_PARAM_UNUSED( p_matrixBuffer ) ) {}
		/**
		 *\~english
		 *\brief		Applies the given viewport dimension
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	The dimensions
		 *\~french
		 *\brief		Applique les dimensions de viewport données
		 *\param[in]	p_iWindowWidth, p_iWindowHeight	Les dimensions
		 */
		virtual void ApplyViewport( int CU_PARAM_UNUSED( p_iWindowWidth ), int CU_PARAM_UNUSED( p_iWindowHeight ) ) {}
		/**
		 *\~english
		 *\brief		Retrieves the current matrix mode.
		 *\return		The mode
		 *\~french
		 *\brief		Récupère le mode de matrice courant.
		 *\return		Le mode
		 */
		inline eMTXMODE GetCurrentMode()
		{
			return m_pPipeline->m_eCurrentMode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the current matrix for the current mode.
		 *\return		The matrix
		 *\~french
		 *\brief		Récupère la matrice courant du mode de matrice courant.
		 *\return		La matrice
		 */
		inline matrix4x4 & GetCurrentMatrix()
		{
			CASTOR_ASSERT( m_pPipeline->m_eCurrentMode >= 0 && m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT );
			return m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top();
		}
		/**
		 *\~english
		 *\brief		Retrieves the current matrix for the current mode.
		 *\return		The matrix
		 *\~french
		 *\brief		Récupère la matrice courant du mode de matrice courant.
		 *\return		La matrice
		 */
		inline matrix4x4 const & GetCurrentMatrix()const
		{
			CASTOR_ASSERT( m_pPipeline->m_eCurrentMode >= 0 && m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT );
			return m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top();
		}

	protected:
		//!\~english The parent pipeline	\~french Le pipeline parent
		Pipeline * m_pPipeline;
	};
}

#pragma warning( pop )

#endif
