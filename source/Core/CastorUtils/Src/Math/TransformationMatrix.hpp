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
#ifndef ___CASTOR_TRANSFORMATION_MATRIX_H___
#define ___CASTOR_TRANSFORMATION_MATRIX_H___

#include "SquareMatrix.hpp"

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
	namespace matrix
	{
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & rotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & set_rotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat );
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
		template< typename T, typename U >
		static void get_rotate( SquareMatrix< T, 4 > const & p_matrix, QuaternionT< U > & p_quat );
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
		template< typename T >
		static SquareMatrix< T, 4 > & yaw( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle );
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
		template< typename T >
		static SquareMatrix< T, 4 > & pitch( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle );
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
		template< typename T >
		static SquareMatrix< T, 4 > & roll( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & scale( SquareMatrix< T, 4 > & p_matrix, U p_hx, U p_hy, U p_hz );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & scale( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_scale );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & set_scale( SquareMatrix< T, 4 > & p_matrix, U p_hx, U p_hy, U p_hz );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & set_scale( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_scale );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & translate( SquareMatrix< T, 4 > & p_matrix, U p_tx, U p_ty, U p_tz );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & translate( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_translation );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & set_translate( SquareMatrix< T, 4 > & p_matrix, U p_tx, U p_ty, U p_tz );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & set_translate( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_translation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	p_matrix		Receives the transformation
		 *\param[in]	p_position	The translation value
		 *\param[in]	p_scale		The scaling value
		 *\param[in]	p_orientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	p_matrix		Reçoit la transformation
		 *\param[in]	p_position	La valeur de translation
		 *\param[in]	p_scale		La valeur de mise à l'échelle
		 *\param[in]	p_orientation	La valeur de rotation
		 */
		template< typename T, typename U, typename V >
		static SquareMatrix< T, 4 > & set_transform( Castor::SquareMatrix< T, 4 > & p_matrix, Castor::Point< U, 3 > const & p_position, Castor::Point< U, 3 > const & p_scale, Castor::QuaternionT< V > const & p_orientation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	p_matrix		Receives the transformation
		 *\param[in]	p_position	The translation value
		 *\param[in]	p_scale		The scaling value
		 *\param[in]	p_orientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	p_matrix		Reçoit la transformation
		 *\param[in]	p_position	La valeur de translation
		 *\param[in]	p_scale		La valeur de mise à l'échelle
		 *\param[in]	p_orientation	La valeur de rotation
		 */
		template< typename T, typename U, typename V >
		static SquareMatrix< T, 4 > & transform( Castor::SquareMatrix< T, 4 > & p_matrix, Castor::Point< U, 3 > const & p_position, Castor::Point< U, 3 > const & p_scale, Castor::QuaternionT< V > const & p_orientation );
		/**
		 *\~english
		 *\brief		Transforms the position/scale through a transformation matrix.
		 *\param[out]	p_matrix	The transformation matrix
		 *\param[in]	p_value		The position/scale.
		 *\return		The transformed position.
		 *\~french
		 *\brief		Transforme une position/mise à l'échelle via une matrice de transformation.
		 *\param[out]	p_matrix	La matrice de transformation.
		 *\param[in]	p_value		La position/mise à l'échelle.
		 *\return		La position transformée.
		 */
		template< typename T, typename U >
		static Point< U, 3 > get_transformed( Castor::SquareMatrix< T, 4 > const & p_matrix, Castor::Point< U, 3 > const & p_value );
		/**
		 *\~english
		 *\brief		Transforms the orientation through a transformation matrix.
		 *\param[out]	p_matrix	The transformation matrix.
		 *\param[in]	p_value		The orientation.
		 *\return		The transformed orientation.
		 *\~french
		 *\brief		Transforme une orientation via une matrice de transformation.
		 *\param[out]	p_matrix	La matrice de transformation.
		 *\param[in]	p_value		L'orientation.
		 *\return		La position transformée.
		 */
		template< typename T, typename U >
		static Quaternion get_transformed( Castor::SquareMatrix< T, 4 > const & p_matrix, Castor::Quaternion const & p_value );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[out]	p_matrix	The matrix that will receive the perspective matrix
		 *\param[in]	p_fovy		Y Field of View
		 *\param[in]	aspect		Width / Height ratio
		 *\param[in]	zNear		Near clipping plane value
		 *\param[in]	zFar		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[out]	p_matrix	La matrice qui contiendra le résultat
		 *\param[in]	p_fovy		Angle de vision Y
		 *\param[in]	aspect		Ratio Largeur / Hauteur
		 *\param[in]	zNear		Position du plan proche
		 *\param[in]	zFar		Position du plan lointain
		 */
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & perspective( SquareMatrix< T, 4 > & p_matrix, Angle const & p_fovy, U aspect, U zNear, U zFar );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[in]	p_fovy		Y Field of View
		 *\param[in]	aspect		Width / Height ratio
		 *\param[in]	nearVal		Near clipping plane value
		 *\param[in]	farVal		Far clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[in]	p_fovy		Angle de vision Y
		 *\param[in]	aspect		Ratio Largeur / Hauteur
		 *\param[in]	nearVal		Position du plan proche
		 *\param[in]	farVal		Position du plan lointain
		 *\return		La matrice résultante
		 */
		template< typename T >
		static SquareMatrix< T, 4 > perspective( Angle const & p_fovy, T aspect, T nearVal, T farVal );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & frustum( SquareMatrix< T, 4 > & p_matrix, U left, U right, U bottom, U top, U nearVal, U farVal );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters
		 *\param[in]	left		Left clipping plane value
		 *\param[in]	right		Right clipping plane value
		 *\param[in]	bottom		Bottom clipping plane value
		 *\param[in]	top			Top clipping plane value
		 *\param[in]	nearVal		Near clipping plane value
		 *\param[in]	farVal		Far clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée
		 *\param[in]	left		Position du plan gauche
		 *\param[in]	right		Position du plan droit
		 *\param[in]	bottom		Position du plan bas
		 *\param[in]	top			Position du plan haut
		 *\param[in]	nearVal		Position du plan proche
		 *\param[in]	farVal		Position du plan lointain
		 *\return		La matrice résultante
		 */
		template< typename T >
		static SquareMatrix< T, 4 > frustum( T left, T right, T bottom, T top, T nearVal, T farVal );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & ortho( SquareMatrix< T, 4 > & p_matrix, U left, U right, U bottom, U top, U zNear, U zFar );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[in]	left		Left clipping plane value
		 *\param[in]	right		Right clipping plane value
		 *\param[in]	bottom		Bottom clipping plane value
		 *\param[in]	top			Top clipping plane value
		 *\param[in]	zNear		Near clipping plane value
		 *\param[in]	zFar		Far clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[in]	left		Position du plan gauche
		 *\param[in]	right		Position du plan droit
		 *\param[in]	bottom		Position du plan bas
		 *\param[in]	top			Position du plan haut
		 *\param[in]	zNear		Position du plan proche
		 *\param[in]	zFar		Position du plan lointain
		 *\return		La matrice résultante
		 */
		template< typename T >
		static SquareMatrix< T, 4 > ortho( T left, T right, T bottom, T top, T zNear, T zFar );
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
		template< typename T, typename U >
		static SquareMatrix< T, 4 > & look_at( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_ptEye, Point< U, 3 > const & p_ptCenter, Point< U, 3 > const & p_ptUp );
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point
		 *\param[in]	p_ptEye		The eye position
		 *\param[in]	p_ptCenter	The point to look at
		 *\param[in]	p_ptUp		The up direction
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné
		 *\param[in]	p_ptEye		La position de l'oeil
		 *\param[in]	p_ptCenter	Le point à regarder
		 *\param[in]	p_ptUp		La direction vers le haut
		 *\return		La matrice résultante
		 */
		template< typename T >
		static SquareMatrix< T, 4 > look_at( Point< T, 3 > const & p_ptEye, Point< T, 3 > const & p_ptCenter, Point< T, 3 > const & p_ptUp );
		/**
		 *\~english
		 *\brief			Switches the handness of a matrix (from right to left or left to right)
		 *\param[in,out]	p_matrix	The matrix to switch, receives the switched matrix
		 *\return			The switched matrix
		 *\~french
		 *\brief			Change la main de la matrice (de main gauche à droite et inversement)
		 *\param[in,out]	p_matrix	La matrice à changer, reçoit la matrice changée
		 *\return			La matrice changée
		 */
		template< typename T >
		static SquareMatrix< T, 4 > & switch_hand( SquareMatrix< T, 4 > & p_matrix );
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
		template< typename T >
		static SquareMatrix< T, 4 > get_switch_hand( SquareMatrix< T, 4 > const & p_matrix );
	}
}

#include "TransformationMatrix.inl"

#endif
