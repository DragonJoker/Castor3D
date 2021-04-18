/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TRANSFORMATION_MATRIX_H___
#define ___CASTOR_TRANSFORMATION_MATRIX_H___

#include "CastorUtils/Math/SquareMatrix.hpp"

namespace castor
{
	/**
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
		 *\param[out]	matrix		The matrix that will receive the rotation matrix
		 *\param[in]	orientation	The quaterion used to compute the matrix
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	orientation	Le quaternion contenant la rotation
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & rotate( Matrix4x4< T > & matrix
			, QuaternionT< U > const & orientation );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[out]	matrix		The matrix that will receive the rotation matrix
		 *\param[in]	orientation	The quaterion used to compute the matrix
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	orientation	Le quaternion contenant la rotation
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & setRotate( Matrix4x4< T > & matrix
			, QuaternionT< U > const & orientation );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[in]	matrix		The matrix holding the scale transformation
		 *\param[out]	orientation	Receives the rotation
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[in]	matrix		La matrice qui contient les transformations
		 *\param[out]	orientation	Reçoit la rotation
		 */
		template< typename T, typename U >
		static void getRotate( Matrix4x4< T > const & matrix
			, QuaternionT< U > & orientation );
		/**
		 *\~english
		 *\brief		Rotation function, around Y axis. Builds a rotation matrix from the angle in parameter.
		 *\param[out]	matrix	The matrix that will receive the rotation matrix
		 *\param[in]	angle	The rotation angle
		 *\~french
		 *\brief		Fonction de rotation autour de l'axe Y, construit une matrice de rotation à partir d'un angle
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	angle	L'angle de rotation
		 */
		template< typename T >
		static Matrix4x4< T > & yaw( Matrix4x4< T > & matrix
			, Angle const & angle );
		/**
		 *\~english
		 *\brief		Rotation function, around X axis. Builds a rotation matrix from the angle in parameter.
		 *\param[out]	matrix	The matrix that will receive the rotation matrix
		 *\param[in]	angle	The rotation angle
		 *\~french
		 *\brief		Fonction de rotation autour de l'axe X, construit une matrice de rotation à partir d'un angle
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	angle	L'angle de rotation
		 */
		template< typename T >
		static Matrix4x4< T > & pitch( Matrix4x4< T > & matrix
			, Angle const & angle );
		/**
		 *\~english
		 *\brief		Rotation function, around Z axis. Builds a rotation matrix from the angle in parameter.
		 *\param[out]	matrix	The matrix that will receive the rotation matrix
		 *\param[in]	angle	The rotation angle
		 *\~french
		 *\brief		Fonction de rotation autour de l'axe Z, construit une matrice de rotation à partir d'un angle
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	angle	L'angle de rotation
		 */
		template< typename T >
		static Matrix4x4< T > & roll( Matrix4x4< T > & matrix
			, Angle const & angle );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the three values in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	x, y, z	The scale values
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & scale( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the value in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	scaling	The scale value
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	scaling	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & scale( Matrix4x4< T > & matrix
			, Point3< U > const & scaling );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the three values in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	x, y, z	The scale values
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & setScale( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the value in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	scaling	The scale value
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	scaling	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & setScale( Matrix4x4< T > & matrix
			, Point3< U > const & scaling );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the three values in parameter.
		 *\param[out]	matrix			The matrix that will be translated
		 *\param[in]	x, y, z	The translate values
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix			La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & translate( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the value in parameter.
		 *\param[out]	matrix		The matrix that will be translated
		 *\param[in]	translation	The translate value
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	translation	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & translate( Matrix4x4< T > & matrix
			, Point3< U > const & translation );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the three values in parameter.
		 *\param[out]	matrix	The matrix that will be translated
		 *\param[in]	x, y, z	The translate values
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & setTranslate( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the value in parameter.
		 *\param[out]	matrix		The matrix that will be translated
		 *\param[in]	translation	The translate value
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	translation	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & setTranslate( Matrix4x4< T > & matrix
			, Point3< U > const & translation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	matrix		Receives the transformation
		 *\param[in]	position	The translation value
		 *\param[in]	scaling		The scaling value
		 *\param[in]	orientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	matrix		Reçoit la transformation
		 *\param[in]	position	La valeur de translation
		 *\param[in]	scaling		La valeur de mise à l'échelle
		 *\param[in]	orientation	La valeur de rotation
		 */
		template< typename T, typename U, typename V >
		static Matrix4x4< T > & setTransform( Matrix4x4< T > & matrix
			, Point3< U > const & position
			, Point3< U > const & scaling
			, QuaternionT< V > const & orientation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	matrix		Receives the transformation
		 *\param[in]	position	The translation value
		 *\param[in]	scaling		The scaling value
		 *\param[in]	orientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	matrix		Reçoit la transformation
		 *\param[in]	position	La valeur de translation
		 *\param[in]	scaling		La valeur de mise à l'échelle
		 *\param[in]	orientation	La valeur de rotation
		 */
		template< typename T, typename U, typename V >
		static Matrix4x4< T > & transform( Matrix4x4< T > & matrix
			, Point3< U > const & position
			, Point3< U > const & scaling
			, QuaternionT< V > const & orientation );
		/**
		 *\~english
		 *\brief		Transforms the position/scale through a transformation matrix.
		 *\param[out]	matrix	The transformation matrix
		 *\param[in]	value	The position/scale.
		 *\return		The transformed position.
		 *\~french
		 *\brief		Transforme une position/mise à l'échelle via une matrice de transformation.
		 *\param[out]	matrix	La matrice de transformation.
		 *\param[in]	value	La position/mise à l'échelle.
		 *\return		La position transformée.
		 */
		template< typename T, typename U >
		static Point3< U > getTransformed( Matrix4x4< T > const & matrix
			, Point3< U > const & value );
		/**
		 *\~english
		 *\brief		Transforms the orientation through a transformation matrix.
		 *\param[out]	matrix	The transformation matrix.
		 *\param[in]	value	The orientation.
		 *\return		The transformed orientation.
		 *\~french
		 *\brief		Transforme une orientation via une matrice de transformation.
		 *\param[out]	matrix	La matrice de transformation.
		 *\param[in]	value	L'orientation.
		 *\return		La position transformée.
		 */
		template< typename T, typename U >
		static Quaternion getTransformed( castor::Matrix4x4< T > const & matrix
			, castor::Quaternion const & value );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[out]	matrix	The matrix that will receive the perspective matrix
		 *\param[in]	fovy	Y Field of View
		 *\param[in]	aspect	Width / Height ratio
		 *\param[in]	zNear	Near clipping plane value
		 *\param[in]	zFar	Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	fovy	Angle de vision Y
		 *\param[in]	aspect	Ratio Largeur / Hauteur
		 *\param[in]	zNear	Position du plan proche
		 *\param[in]	zFar	Position du plan lointain
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & perspective( Matrix4x4< T > & matrix
			, Angle const & fovy
			, U aspect
			, U zNear
			, U zFar );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered infinite perspective projection from the given parameters
		 *\param[out]	matrix	The matrix that will receive the perspective matrix
		 *\param[in]	fovy	Y Field of View
		 *\param[in]	aspect	Width / Height ratio
		 *\param[in]	zNear	Near clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective infinie centrée
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	fovy	Angle de vision Y
		 *\param[in]	aspect	Ratio Largeur / Hauteur
		 *\param[in]	zNear	Position du plan proche
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & perspective( Matrix4x4< T > & matrix
			, Angle const & fovy
			, U aspect
			, U zNear );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[in]	fovy	Y Field of View
		 *\param[in]	aspect	Width / Height ratio
		 *\param[in]	nearVal	Near clipping plane value
		 *\param[in]	farVal	Far clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[in]	fovy	Angle de vision Y
		 *\param[in]	aspect	Ratio Largeur / Hauteur
		 *\param[in]	nearVal	Position du plan proche
		 *\param[in]	farVal	Position du plan lointain
		 *\return		La matrice résultante
		 */
		template< typename T >
		static Matrix4x4< T > perspective( Angle const & fovy
			, T aspect
			, T nearVal
			, T farVal );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered infinite perspective projection from the given parameters
		 *\param[in]	fovy	Y Field of View
		 *\param[in]	aspect	Width / Height ratio
		 *\param[in]	nearVal	Near clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection infinie en perspective centrée
		 *\param[in]	fovy	Angle de vision Y
		 *\param[in]	aspect	Ratio Largeur / Hauteur
		 *\param[in]	nearVal	Position du plan proche
		 *\return		La matrice résultante
		 */
		template< typename T >
		static Matrix4x4< T > perspective( Angle const & fovy
			, T aspect
			, T nearVal );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters
		 *\param[out]	matrix	The matrix that will receive the perspective matrix
		 *\param[in]	left	Left clipping plane value
		 *\param[in]	right	Right clipping plane value
		 *\param[in]	bottom	Bottom clipping plane value
		 *\param[in]	top		Top clipping plane value
		 *\param[in]	nearVal	Near clipping plane value
		 *\param[in]	farVal	Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	left	Position du plan gauche
		 *\param[in]	right	Position du plan droit
		 *\param[in]	bottom	Position du plan bas
		 *\param[in]	top		Position du plan haut
		 *\param[in]	nearVal	Position du plan proche
		 *\param[in]	farVal	Position du plan lointain
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & frustum( Matrix4x4< T > & matrix
			, U left
			, U right
			, U bottom
			, U top
			, U nearVal
			, U farVal );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters
		 *\param[in]	left	Left clipping plane value
		 *\param[in]	right	Right clipping plane value
		 *\param[in]	bottom	Bottom clipping plane value
		 *\param[in]	top		Top clipping plane value
		 *\param[in]	nearVal	Near clipping plane value
		 *\param[in]	farVal	Far clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée
		 *\param[in]	left	Position du plan gauche
		 *\param[in]	right	Position du plan droit
		 *\param[in]	bottom	Position du plan bas
		 *\param[in]	top		Position du plan haut
		 *\param[in]	nearVal	Position du plan proche
		 *\param[in]	farVal	Position du plan lointain
		 *\return		La matrice résultante
		 */
		template< typename T >
		static Matrix4x4< T > frustum( T left
			, T right
			, T bottom
			, T top
			, T nearVal
			, T farVal );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[out]	matrix	The matrix that will receive the perspective matrix
		 *\param[in]	left	Left clipping plane value
		 *\param[in]	right	Right clipping plane value
		 *\param[in]	bottom	Bottom clipping plane value
		 *\param[in]	top		Top clipping plane value
		 *\param[in]	zNear	Near clipping plane value
		 *\param[in]	zFar	Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	left	Position du plan gauche
		 *\param[in]	right	Position du plan droit
		 *\param[in]	bottom	Position du plan bas
		 *\param[in]	top		Position du plan haut
		 *\param[in]	zNear	Position du plan proche
		 *\param[in]	zFar	Position du plan lointain
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & ortho( Matrix4x4< T > & matrix
			, U left
			, U right
			, U bottom
			, U top
			, U zNear
			, U zFar );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[in]	left	Left clipping plane value
		 *\param[in]	right	Right clipping plane value
		 *\param[in]	bottom	Bottom clipping plane value
		 *\param[in]	top		Top clipping plane value
		 *\param[in]	zNear	Near clipping plane value
		 *\param[in]	zFar	Far clipping plane value
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[in]	left	Position du plan gauche
		 *\param[in]	right	Position du plan droit
		 *\param[in]	bottom	Position du plan bas
		 *\param[in]	top		Position du plan haut
		 *\param[in]	zNear	Position du plan proche
		 *\param[in]	zFar	Position du plan lointain
		 *\return		La matrice résultante
		 */
		template< typename T >
		static Matrix4x4< T > ortho( T left
			, T right
			, T bottom
			, T top
			, T zNear
			, T zFar );
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point
		 *\param[out]	matrix	The matrix that will receive the perspective matrix
		 *\param[in]	eye		The eye position
		 *\param[in]	center	The point to look at
		 *\param[in]	up		The up direction
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	eye		La position de l'oeil
		 *\param[in]	center	Le point à regarder
		 *\param[in]	up		La direction vers le haut
		 */
		template< typename T, typename U >
		static Matrix4x4< T > & lookAt( Matrix4x4< T > & matrix
			, Point3< U > const & eye
			, Point3< U > const & center
			, Point3< U > const & up );
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point
		 *\param[in]	eye		The eye position
		 *\param[in]	center	The point to look at
		 *\param[in]	up		The up direction
		 *\return		The resulting matrix
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné
		 *\param[in]	eye		La position de l'oeil
		 *\param[in]	center	Le point à regarder
		 *\param[in]	up		La direction vers le haut
		 *\return		La matrice résultante
		 */
		template< typename T >
		static Matrix4x4< T > lookAt( Point3< T > const & eye
			, Point3< T > const & center
			, Point3< T > const & up );
		/**
		 *\~english
		 *\brief			Switches the handness of a matrix (from right to left or left to right)
		 *\param[in,out]	matrix	The matrix to switch, receives the switched matrix
		 *\return			The switched matrix
		 *\~french
		 *\brief			Change la main de la matrice (de main gauche à droite et inversement)
		 *\param[in,out]	matrix	La matrice à changer, reçoit la matrice changée
		 *\return			La matrice changée
		 */
		template< typename T >
		static Matrix4x4< T > & switchHand( Matrix4x4< T > & matrix );
		/**
		 *\~english
		 *\brief		Switches the handness of a matrix (from right to left or left to right)
		 *\param[in]	matrix	The matrix to switch
		 *\return		The switched matrix
		 *\~french
		 *\brief		Change la main de la matrice (de main gauche à droite et inversement)
		 *\param[in]	matrix	La matrice à changer
		 *\return		La matrice changée
		 */
		template< typename T >
		static Matrix4x4< T > getSwitchHand( Matrix4x4< T > const & matrix );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[out]	matrix		The matrix that will receive the rotation matrix
		 *\param[in]	orientation	The quaterion used to compute the matrix
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	orientation	Le quaternion contenant la rotation
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & rotate( Matrix3x3< T > & matrix
			, Angle orientation );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[out]	matrix		The matrix that will receive the rotation matrix
		 *\param[in]	orientation	The quaterion used to compute the matrix
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	orientation	Le quaternion contenant la rotation
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & setRotate( Matrix3x3< T > & matrix
			, Angle orientation );
		/**
		 *\~english
		 *\brief		Rotation function, builds a rotation matrix from the quaternion in parameter.
		 *\param[in]	matrix		The matrix holding the scale transformation
		 *\param[out]	orientation	Receives the rotation
		 *\~french
		 *\brief		Fonction de rotation, construit une matrice de rotation à partir d'un quaternion
		 *\param[in]	matrix		La matrice qui contient les transformations
		 *\param[out]	orientation	Reçoit la rotation
		 */
		template< typename T, typename U >
		static void getRotate( Matrix3x3< T > const & matrix
			, Angle orientation );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the three values in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	x, y, z	The scale values
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & scale( Matrix3x3< T > & matrix
			, U x
			, U y );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the value in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	scaling	The scale value
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	scaling	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & scale( Matrix3x3< T > & matrix
			, Point2< U > const & scaling );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the three values in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	x, y, z	The scale values
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & setScale( Matrix3x3< T > & matrix
			, U x
			, U y );
		/**
		 *\~english
		 *\brief		Scale function. Scales matrix from the value in parameter.
		 *\param[out]	matrix	The matrix that will be scaled
		 *\param[in]	scaling	The scale value
		 *\~french
		 *\brief		Fonction de mise a l'échelle.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	scaling	Les valeurs d'échelle sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & setScale( Matrix3x3< T > & matrix
			, Point2< U > const & scaling );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the three values in parameter.
		 *\param[out]	matrix			The matrix that will be translated
		 *\param[in]	x, y, z	The translate values
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix			La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & translate( Matrix3x3< T > & matrix
			, U x
			, U y );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the value in parameter.
		 *\param[out]	matrix		The matrix that will be translated
		 *\param[in]	translation	The translate value
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	translation	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & translate( Matrix3x3< T > & matrix
			, Point2< U > const & translation );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the three values in parameter.
		 *\param[out]	matrix	The matrix that will be translated
		 *\param[in]	x, y, z	The translate values
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix	La matrice qui contiendra le résultat
		 *\param[in]	x, y, z	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & setTranslate( Matrix3x3< T > & matrix
			, U x
			, U y );
		/**
		 *\~english
		 *\brief		Translate function. Translates a matrix from the value in parameter.
		 *\param[out]	matrix		The matrix that will be translated
		 *\param[in]	translation	The translate value
		 *\~french
		 *\brief		Fonction de construction de matrice de translation.
		 *\param[out]	matrix		La matrice qui contiendra le résultat
		 *\param[in]	translation	Les valeurs de translation sur les 3 axes
		 */
		template< typename T, typename U >
		static Matrix3x3< T > & setTranslate( Matrix3x3< T > & matrix
			, Point2< U > const & translation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	matrix		Receives the transformation
		 *\param[in]	position	The translation value
		 *\param[in]	scaling		The scaling value
		 *\param[in]	orientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	matrix		Reçoit la transformation
		 *\param[in]	position	La valeur de translation
		 *\param[in]	scaling		La valeur de mise à l'échelle
		 *\param[in]	orientation	La valeur de rotation
		 */
		template< typename T, typename U, typename V >
		static Matrix3x3< T > & setTransform( Matrix3x3< T > & matrix
			, Point2< U > const & position
			, Point2< U > const & scaling
			, Angle orientation );
		/**
		 *\~english
		 *\brief		Retrieves transformation matrix from translation, scaling and rotation
		 *\param[out]	matrix		Receives the transformation
		 *\param[in]	position	The translation value
		 *\param[in]	scaling		The scaling value
		 *\param[in]	orientation	The rotation value
		 *\~french
		 *\brief		Récupère la matrice de transformation à partir de la translation, la mise à l'échelle et la rotation
		 *\param[out]	matrix		Reçoit la transformation
		 *\param[in]	position	La valeur de translation
		 *\param[in]	scaling		La valeur de mise à l'échelle
		 *\param[in]	orientation	La valeur de rotation
		 */
		template< typename T, typename U, typename V >
		static Matrix3x3< T > & transform( Matrix3x3< T > & matrix
			, Point2< U > const & position
			, Point2< U > const & scaling
			, Angle orientation );
	}
}

#include "TransformationMatrix.inl"

#endif
