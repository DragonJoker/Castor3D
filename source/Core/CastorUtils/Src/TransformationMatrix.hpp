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
#ifndef ___CASTOR_TRANSFORMATION_MATRIX_H___
#define ___CASTOR_TRANSFORMATION_MATRIX_H___

#include "Matrix.hpp"
#include "Point.hpp"
#include "Angle.hpp"
#include "Quaternion.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Matrix basic operations
	\remark		Holds rotation, translation, scale, multiplication operations over 4x4 matrix.
				<br />Also computes frustum, perspective, ortho, switches left-hand to right-hand and reciprocally
	\~french
	\brief		Opérations basiques sur des matrices 4x4
	\remark		Contient des fonctions de rotation, translation, scale et multiplication (optimisées pour des matrices 4x4)
				<br />Permet aussi de calculer les frustum, persperctive, ortho, le passage main gauche main droite et inversement
	*/
	class matrix
	{
	public:
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the angle and the axis in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_angle		The rotation angle
		 *\param[in]	p_axis		The axis around which the angle is set
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un angle et d'un axe
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_angle		L'angle de rotation
		 *\param[in]	p_axis		L'axe autour duquel l'angle est défini
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & rotate( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle, Point< TypeB, 3 > const & p_axis );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_quat		The quaterion used to compute the matrix
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_quat		Le quaternion contenant la rotation
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > & rotate( SquareMatrix< TypeA, 4 > & p_matrix, Quaternion const & p_quat );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the angle and the axis in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_angle		The rotation angle
		 *\param[in]	p_axis		The axis around which the angle is set
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un angle et d'un axe
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_angle		L'angle de rotation
		 *\param[in]	p_axis		L'axe autour duquel l'angle est défini
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & set_rotate( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle, Point< TypeB, 3 > const & p_axis );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_quat	The quaterion used to compute the matrix
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_quat		Le quaternion contenant la rotation
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > & set_rotate( SquareMatrix< TypeA, 4 > & p_matrix, Quaternion const & p_quat );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[in]	p_matrix	The matrix holding the scale transformation
		 *\param[out]	p_quat		Receives the rotation
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[in]	p_matrix	La matrice qui contient les transformations
		 *\param[out]	p_quat		Reçoit la rotation
		 */
		template< typename TypeA >
		static void get_rotate( SquareMatrix< TypeA, 4 > const & p_matrix, Quaternion & p_quat );
		/**
		 *\~english
		 *\brief		Rotation function, around Y axis. Builds a rotation matrix from the angle in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_angle		The rotation angle
		 *\~french
		 *\brief		Fonction de rotation autour de l'axe Y, construit une matrice de rotation à partir d'un angle
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_angle		L'angle de rotation
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > & yaw( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotation function, around X axis. Builds a rotation matrix from the angle in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_angle		The rotation angle
		 *\~french
		 *\brief		Fonction de rotation autour de l'axe X, construit une matrice de rotation à partir d'un angle
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_angle		L'angle de rotation
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > & pitch( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotation function, around Z axis. Builds a rotation matrix from the angle in parameter.
		 *\param[out]	p_matrix	The matrix that will receive the rotation matrix
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Fonction de rotation autour de l'axe Z, construit une matrice de rotation à partir d'un angle
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_angle		L'angle de rotation
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > & roll( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the three values in parameter.
		 *\param[out]	p_matrix			The matrix that will be scaled
		 *\param[in]	p_hx, p_hy, p_hz	The scale values
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	p_matrix			La matrice qui contiendra le résultat
		 *\param[in]	p_hx, p_hy, p_hz	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & scale( SquareMatrix< TypeA, 4 > & p_matrix, TypeB p_hx, TypeB p_hy, TypeB p_hz );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the value in parameter.
		 *\param[out]	p_matrix	The matrix that will be scaled
		 *\param[in]	p_scale		The scale value
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_scale		Les valeurs d'échelle sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & scale( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_scale );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the three values in parameter.
		 *\param[out]	p_matrix			The matrix that will be scaled
		 *\param[in]	p_hx, p_hy, p_hz	The scale values
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	p_matrix			La matrice qui contiendra le résultat
		 *\param[in]	p_hx, p_hy, p_hz	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & set_scale( SquareMatrix< TypeA, 4 > & p_matrix, TypeB p_hx, TypeB p_hy, TypeB p_hz );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the value in parameter.
		 *\param[out]	p_matrix	The matrix that will be scaled
		 *\param[in]	p_scale		The scale value
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_scale		Les valeurs d'échelle sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & set_scale( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_scale );
		/**
		 *\~english
		 *\brief		Retrieves scale from a transformation matrix.
		 *\param[in]	p_matrix	The matrix holding the scale transformation
		 *\param[out]	p_scale		Receives the scale value
		 *\~french
		 *\brief		Récupère la mise a l'échelle à partir d'une matrice de transformation.
		 *\param[in]	p_matrix	La matrice qui contient les transformations
		 *\param[out]	p_scale		Récupère les valeurs d'échelle sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static void get_scale( SquareMatrix< TypeA, 4 > const & p_matrix, Point< TypeB, 3 > & p_scale );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the three values in parameter.
		 *\param[out]	p_matrix			The matrix that will be translated
		 *\param[in]	p_tx, p_ty, p_tz	The translate values
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	p_matrix			La matrice qui contiendra le résultat
		 *\param[in]	p_tx, p_ty, p_tz	Les valeurs de translation sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & translate( SquareMatrix< TypeA, 4 > & p_matrix, TypeB p_tx, TypeB p_ty, TypeB p_tz );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the value in parameter.
		 *\param[out]	p_matrix		The matrix that will be translated
		 *\param[in]	p_translation	The translate value
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	p_matrix		La matrice qui contiendra le résultat
		 *\param[in]	p_translation	Les valeurs de translation sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & translate( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_translation );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the three values in parameter.
		 *\param[out]	p_matrix			The matrix that will be translated
		 *\param[in]	p_tx, p_ty, p_tz	The translate values
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	p_matrix			La matrice qui contiendra le résultat
		 *\param[in]	p_tx, p_ty, p_tz	Les valeurs de translation sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & set_translate( SquareMatrix< TypeA, 4 > & p_matrix, TypeB p_tx, TypeB p_ty, TypeB p_tz );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the value in parameter.
		 *\param[out]	p_matrix		The matrix that will be translated
		 *\param[in]	p_translation	The translate value
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	p_matrix		La matrice qui contiendra le résultat
		 *\param[in]	p_translation	Les valeurs de translation sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & set_translate( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_translation );
		/**
		 *\~english
		 *\brief		Retrieves translation from a transformation matrix.
		 *\param[in]	p_matrix		The matrix holding the scale transformation
		 *\param[out]	p_translation	Receives the translation value
		 *\~french
		 *\brief		Récupère la translation à partir d'une matrice de transformation.
		 *\param[in]	p_matrix		La matrice qui contient les transformations
		 *\param[out]	p_translation	Récupère les valeurs de translation sur les 3 axes
		 */
		template< typename TypeA, typename TypeB >
		static void get_translate( SquareMatrix< TypeA, 4 > const & p_matrix, Point< TypeB, 3 > & p_translation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	p_matrix		Receives the transformation
		 *\param[in]	p_ptPosition	The translation value
		 *\param[in]	p_ptScale		The scaling value
		 *\param[in]	p_qOrientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	p_matrix		Reçoit la transformation
		 *\param[in]	p_ptPosition	La valeur de translation
		 *\param[in]	p_ptScale		La valeur de mise à l'échelle
		 *\param[in]	p_qOrientation	La valeur de rotation
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & set_transform( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_ptPosition, Castor::Point< TypeB, 3 > const & p_ptScale, Castor::Quaternion const & p_qOrientation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	p_matrix		Receives the transformation
		 *\param[in]	p_ptPosition	The translation value
		 *\param[in]	p_ptScale		The scaling value
		 *\param[in]	p_qOrientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	p_matrix		Reçoit la transformation
		 *\param[in]	p_ptPosition	La valeur de translation
		 *\param[in]	p_ptScale		La valeur de mise à l'échelle
		 *\param[in]	p_qOrientation	La valeur de rotation
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & transform( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_ptPosition, Castor::Point< TypeB, 3 > const & p_ptScale, Castor::Quaternion const & p_qOrientation );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[out]	p_matrix	The matrix that will receive the perspective matrix
		 *\param[in]	p_aFOVY		Y Field of View
		 *\param[in]	aspect		Width / Height ratio
		 *\param[in]	zNear		Near clipping plane value
		 *\param[in]	zFar		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_aFOVY		Angle de vision Y
		 *\param[in]	aspect		Ratio Largeur / Hauteur
		 *\param[in]	zNear		Position du plan proche
		 *\param[in]	zFar		Position du plan lointain
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & perspective( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters
		 *\param[out]	p_matrix	The matrix that will receive the perspective matrix
		 *\param[in]	left		Left clipping plane value
		 *\param[in]	right		Right clipping plane value
		 *\param[in]	bottom		Bottom clipping plane value
		 *\param[in]	top			Top clipping plane value
		 *\param[in]	nearVal		Near clipping plane value
		 *\param[in]	farVal		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	left		Position du plan gauche
		 *\param[in]	right		Position du plan droit
		 *\param[in]	bottom		Position du plan bas
		 *\param[in]	top			Position du plan haut
		 *\param[in]	nearVal		Position du plan proche
		 *\param[in]	farVal		Position du plan lointain
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & frustum( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB nearVal, TypeB farVal );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[out]	p_matrix	The matrix that will receive the perspective matrix
		 *\param[in]	left		Left clipping plane value
		 *\param[in]	right		Right clipping plane value
		 *\param[in]	bottom		Bottom clipping plane value
		 *\param[in]	top			Top clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	left		Position du plan gauche
		 *\param[in]	right		Position du plan droit
		 *\param[in]	bottom		Position du plan bas
		 *\param[in]	top			Position du plan haut
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & ortho( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[out]	p_matrix	The matrix that will receive the perspective matrix
		 *\param[in]	left		Left clipping plane value
		 *\param[in]	right		Right clipping plane value
		 *\param[in]	bottom		Bottom clipping plane value
		 *\param[in]	top			Top clipping plane value
		 *\param[in]	zNear		Near clipping plane value
		 *\param[in]	zFar		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	left		Position du plan gauche
		 *\param[in]	right		Position du plan droit
		 *\param[in]	bottom		Position du plan bas
		 *\param[in]	top			Position du plan haut
		 *\param[in]	zNear		Position du plan proche
		 *\param[in]	zFar		Position du plan lointain
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & ortho( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar );
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point
		 *\param[out]	p_matrix	The matrix that will receive the perspective matrix
		 *\param[in]	p_ptEye		The eye position
		 *\param[in]	p_ptCenter	The point to look at
		 *\param[in]	p_ptUp		The up direction
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_ptEye		La position de l'oeil
		 *\param[in]	p_ptCenter	Le point à regarder
		 *\param[in]	p_ptUp		La direction vers le haut
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > & look_at( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_ptEye, Point< TypeB, 3 > const & p_ptCenter, Point< TypeB, 3 > const & p_ptUp );
		/**
		 *\~english
		 *\brief		Transforms a vector through a transformation matrix
		 *\param[in]	p_matrix	The transformation matrix
		 *\param[in]	p_vertex	The vector to transform
		 *\return		The transformed vector
		 *\~french
		 *\brief		Transforme un vecteur par une matrice de transformation
		 *\param[in]	p_matrix	La matrice
		 *\param[in]	p_vertex	Le vecteur
		 *\return		Le vecteur tranforme
		 */
		template <typename TypeA, typename TypeB, uint32_t Count>
		static Point<TypeB, Count> mult( SquareMatrix< TypeA, 4 > const & p_matrix, Point<TypeB, Count> const & p_vertex );
		/**
		 *\~english
		 *\brief		Multiplies 2 matrix
		 *\param[in]	p_matrixA	The first matrix
		 *\param[in]	p_matrixB	The second matrix
		 *\return		The result matrix
		 *\~french
		 *\brief		Multiplie 2 matrices
		 *\param[in]	p_matrixA	La premiere matrice
		 *\param[in]	p_matrixB	La seconde matrice
		 *\return		La matrice résultat
		 */
		template< typename TypeA, typename TypeB >
		static SquareMatrix< TypeA, 4 > mult( SquareMatrix< TypeA, 4 > const & p_matrixA, SquareMatrix<TypeB, 4> const & p_matrixB );
		/**
		 *\~english
		 *\brief		Switches the handness of a matrix (from right to left or left to right)
		 *\param[in,out]p_matrix	The matrix to switch, receives the switched matrix
		 *\return		The switched matrix
		 *\~french
		 *\brief		Change la main de la matrice (de main gauche à droite et inversement)
		 *\param[in,out]p_matrix	La matrice à changer, reçoit la matrice changée
		 *\return		La matrice changée
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > & switch_hand( SquareMatrix< TypeA, 4 > & p_matrix );
		/**
		 *\~english
		 *\brief		Switches the handness of a matrix (from right to left or left to right)
		 *\param[in]	p_matrix	The matrix to switch
		 *\return		The switched matrix
		 *\~french
		 *\brief		Change la main de la matrice (de main gauche à droite et inversement)
		 *\param[in]	p_matrix	La matrice à changer
		 *\return		La matrice changée
		 */
		template< typename TypeA >
		static SquareMatrix< TypeA, 4 > get_switch_hand( SquareMatrix< TypeA, 4 > const & p_matrix );
	};
}

#include "TransformationMatrix.inl"

#endif
