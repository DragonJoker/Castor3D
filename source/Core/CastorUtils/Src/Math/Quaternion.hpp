/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_QUATERNION_H___
#define ___CASTOR_QUATERNION_H___

#include "Math/Angle.hpp"
#include "Math/Point.hpp"
#include "Math/SquareMatrix.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1.0.0
	\date		09/02/2010
	\~english
	\brief		Quaternion representation class
	\remark		A quaternion is an axis and an angle, it's one of the best ways to represent orientations and rotations
	\~french
	\brief		Représentation d'un quaternion
	\remark		Un quaternion se compose d'un axe et d'un angle
	*/
	template< typename T >
	class QuaternionT
		: public Coords4< T >
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief QuaternionT< T > loader
		\~french
		\brief Loader de QuaternionT< T >
		*/
		class TextLoader
			: public Castor::TextLoader< QuaternionT< T > >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader();
			/**
			 *\~english
			 *\brief		Loads a Coords< T, Count > object from a text file
			 *\param[out]	p_object	The Coords< T, Count > object to read
			 *\param[in]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Charge un objet Coords< T, Count > à partir d'un fichier texte
			 *\param[out]	p_object	L'objet Coords< T, Count >
			 *\param[in]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( QuaternionT< T > & p_object, TextFile & p_file );
		};
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief QuaternionT< T > Writer
		\~french
		\brief Writer de QuaternionT< T >
		*/
		class TextWriter
			: public Castor::TextWriter< QuaternionT< T > >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextWriter( String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a Point2f object into a text file
			 *\param[in]	p_object	The Point2f object to write
			 *\param[out]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Ecrit un objet Point2f dans un fichier texte
			 *\param[in]	p_object	L'objet Point2f
			 *\param[out]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( QuaternionT< T > const & p_object, TextFile & p_file );
		};

	private:
		typedef Coords4< T > BaseType;

	private:
		QuaternionT( NoInit const & );
		QuaternionT( T p_x, T p_y, T p_z, T p_w );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline QuaternionT();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_rhs	The Quaternion object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_rhs	L'objet Quaternion à copier
		 */
		inline QuaternionT( QuaternionT< T > const & p_rhs );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_rhs	The Quaternion object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_rhs	L'objet Quaternion à déplacer
		 */
		inline QuaternionT( QuaternionT< T > && p_rhs );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_values	The Quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_values	Les valeurs du Quaternion
		 */
		inline QuaternionT( double const * p_values );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_values	The Quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_values	Les valeurs du Quaternion
		 */
		inline QuaternionT( float const * p_values );
		/**
		 *\~english
		 *\brief		Constructor from axis and angle
		 *\param[in]	p_vector	A point containing the axis values
		 *\param[in]	p_angle		The angle
		 *\~french
		 *\brief		Constructeur à partir d'un axe et d'un angle
		 *\param[in]	p_vector	Un point contenant les valeurs de l'axe
		 *\param[in]	p_angle		L'angle
		 */
		inline QuaternionT( Point3f const & p_vector, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Constructor from axis and angle
		 *\param[in]	p_vector	A point containing the axis values
		 *\param[in]	p_angle		The angle
		 *\~french
		 *\brief		Constructeur à partir d'un axe et d'un angle
		 *\param[in]	p_vector	Un point contenant les valeurs de l'axe
		 *\param[in]	p_angle		L'angle
		 */
		inline QuaternionT( Point3d const & p_vector, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Constructor from Euler angles.
		 *\param[in]	p_pitch	The X axis angle.
		 *\param[in]	p_yaw	The Y axis angle.
		 *\param[in]	p_roll	The Z axis angle.
		 *\~french
		 *\brief		Constructeur à partir d'angles d'Euler.
		 *\param[in]	p_pitch	L'angle de rotation autour de l'axe X.
		 *\param[in]	p_yaw	L'angle de rotation autour de l'axe Y.
		 *\param[in]	p_roll	L'angle de rotation autour de l'axe Z.
		 */
		inline QuaternionT( Angle const & p_pitch, Angle const & p_yaw, Angle const & p_roll );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_values	The quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_values	Les valeurs du quaternion
		 */
		inline QuaternionT( Point4f const & p_values );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_values	The quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_values	Les valeurs du quaternion
		 */
		inline QuaternionT( Point4d const & p_values );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~QuaternionT();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_rhs	The Quaternion object to copy
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_rhs	L'objet Quaternion à copier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator=( QuaternionT< T > const & p_rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_rhs	The Quaternion object to move
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_rhs	L'objet Quaternion à déplacer
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator=( QuaternionT< T > && p_rhs );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_rhs	The Quaternion object to add
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_rhs	L'objet Quaternion à ajouter
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator+=( QuaternionT< T > const & p_rhs );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_rhs	The Quaternion object to substract
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_rhs	L'objet Quaternion à soustraire
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator-=( QuaternionT< T > const & p_rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_rhs	The Quaternion object to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_rhs	L'objet Quaternion à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator*=( QuaternionT< T > const & p_rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_rhs	The scalar to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_rhs	Le scalaire à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator*=( double p_rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_rhs	The scalar to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_rhs	Le scalaire à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator*=( float p_rhs );
		/**
		 *\~english
		 *\brief		Transforms a vector and gives the result
		 *\param[in]	p_vector	The vector to transform
		 *\param[out]	p_result	Receives the result
		 *\return		A reference to p_result
		 *\~french
		 *\brief		Transforme un vecteur et donne le résultat
		 *\param[in]	p_vector	Le vecteur à transformer
		 *\param[out]	p_result	Reçoit le résultat
		 *\return		Une référence sur p_result
		 */
		inline Point3f & transform( Point3f const & p_vector, Point3f & p_result )const;
		/**
		 *\~english
		 *\brief		Transforms a vector and gives the result
		 *\param[in]	p_vector	The vector to transform
		 *\param[out]	p_result	Receives the result
		 *\return		A reference to p_result
		 *\~french
		 *\brief		Transforme un vecteur et donne le résultat
		 *\param[in]	p_vector	Le vecteur à transformer
		 *\param[out]	p_result	Reçoit le résultat
		 *\return		Une référence sur p_result
		 */
		inline Point3d & transform( Point3d const & p_vector, Point3d & p_result )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		inline void to_matrix( float * p_matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		inline void to_matrix( double * p_matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		inline void to_matrix( Matrix4x4f & p_matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		inline void to_matrix( Matrix4x4d & p_matrix )const;
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		inline void from_matrix( Matrix4x4f const & p_matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		inline void from_matrix( Matrix4x4d const & p_matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		inline void from_matrix( float const * p_matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		inline void from_matrix( double const * p_matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from an axis and an angle
		 *\param[in]	p_vector	The axis
		 *\param[in]	p_angle		The angle
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'un axe et d'un angle
		 *\param[in]	p_vector	L'axe
		 *\param[in]	p_angle		L'angle
		 */
		inline void from_axis_angle( Point3f const & p_vector, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from an axis and an angle
		 *\param[in]	p_vector	The axis
		 *\param[in]	p_angle		The angle
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'un axe et d'un angle
		 *\param[in]	p_vector	L'axe
		 *\param[in]	p_angle		L'angle
		 */
		inline void from_axis_angle( Point3d const & p_vector, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Gives the axis and the angle from this Quaternion
		 *\param[out]	p_vector	Receives the axis
		 *\param[out]	p_angle		Receives the angle
		 *\~french
		 *\brief		Calcule l'axe et l'angle de ce Quaternion
		 *\param[out]	p_vector	Reçoit l'axe
		 *\param[out]	p_angle		Reçoit l'angle
		 */
		inline void to_axis_angle( Point3f & p_vector, Angle & p_angle )const;
		/**
		 *\~english
		 *\brief		Gives the axis and the angle from this Quaternion
		 *\param[out]	p_vector	Receives the axis
		 *\param[out]	p_angle		Receives the angle
		 *\~french
		 *\brief		Calcule l'axe et l'angle de ce Quaternion
		 *\param[out]	p_vector	Reçoit l'axe
		 *\param[out]	p_angle		Reçoit l'angle
		 */
		inline void to_axis_angle( Point3d & p_vector, Angle & p_angle )const;
		/**
		 *\~english
		 *\brief		Updates this Quaternion from 3 axes
		 *\param[in]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir de 3 axes
		 *\param[in]	p_x, p_y, p_z	Les axes
		 */
		inline void from_axes( Point3f const & p_x, Point3f const & p_y, Point3f const & p_z );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from 3 axes
		 *\param[in]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir de 3 axes
		 *\param[in]	p_x, p_y, p_z	Les axes
		 */
		inline void from_axes( Point3d const & p_x, Point3d const & p_y, Point3d const & p_z );
		/**
		 *\~english
		 *\brief		Computes 3 axes from this quaternion
		 *\param[out]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Calcule 3 axes à partir de ce quaternion
		 *\param[out]	p_x, p_y, p_z	Les axes
		 */
		inline void to_axes( Point3f & p_x, Point3f & p_y, Point3f & p_z )const;
		/**
		 *\~english
		 *\brief		Computes 3 axes from this quaternion
		 *\param[out]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Calcule 3 axes à partir de ce quaternion
		 *\param[out]	p_x, p_y, p_z	Les axes
		 */
		inline void to_axes( Point3d & p_x, Point3d & p_y, Point3d & p_z )const;
		/**
		 *\~english
		 *\brief		Updates this Quaternion from Euler angles
		 *\param[in]	p_yaw	The Y axis angle
		 *\param[in]	p_pitch	The X axis angle
		 *\param[in]	p_roll	The Z axis angle
		 *\~french
		 *\brief		Met àjour ce Quaternion à partir d'angles d'Euler
		 *\param[in]	p_yaw	L'angle de rotation autour de l'axe Y
		 *\param[in]	p_pitch	L'angle de rotation autour de l'axe X
		 *\param[in]	p_roll	L'angle de rotation autour de l'axe Z
		 */
		inline void from_euler( Angle const & p_pitch, Angle const & p_yaw, Angle const & p_roll );
		/**
		 *\~english
		 *\brief		Computes Euler angles from this Quaternion
		 *\param[out]	p_yaw	The Y axis angle
		 *\param[out]	p_pitch	The X axis angle
		 *\param[out]	p_roll	The Z axis angle
		 *\~french
		 *\brief		Calcule des angles d'Euler à partir de ce Quaternion
		 *\param[out]	p_yaw	L'angle de rotation autour de l'axe Y
		 *\param[out]	p_pitch	L'angle de rotation autour de l'axe X
		 *\param[out]	p_roll	L'angle de rotation autour de l'axe Z
		 */
		inline void to_euler( Angle & p_pitch, Angle & p_yaw, Angle & p_roll );
		/**
		 *\~english
		 *\brief		Retrieves the Y axis rotation value
		 *\return		The Y axis rotation value
		 *\~french
		 *\brief		Récupère la valeur de la rotation sur l'axe Y
		 *\return		La valeur de la rotation autour de l'axe Y
		 */
		inline Angle get_yaw()const;
		/**
		 *\~english
		 *\brief		Retrieves the Y axis rotation value
		 *\return		The Y axis rotation value
		 *\~french
		 *\brief		Récupère la valeur de la rotation sur l'axe Y
		 *\return		La valeur de la rotation autour de l'axe Y
		 */
		inline Angle get_pitch()const;
		/**
		 *\~english
		 *\brief		Retrieves the Y axis rotation value
		 *\return		The Y axis rotation value
		 *\~french
		 *\brief		Récupère la valeur de la rotation sur l'axe Y
		 *\return		La valeur de la rotation autour de l'axe Y
		 */
		inline Angle get_roll()const;
		/**
		 *\~english
		 *\brief		Sets this Quaternion to its conjugate
		 *\~french
		 *\brief		Définit ce Quaternion à son conjugué
		 */
		inline void conjugate();
		/**
		 *\~english
		 *\brief		Retrieves the conjugate of this Quaternion
		 *\return		The conjugate
		 *\~french
		 *\brief		Récupère le conjugué de ce Quaternion
		 *\return		Le conjugué
		 */
		inline QuaternionT< T > get_conjugate()const;
		/**
		 *\~english
		 *\brief		Retrieves the magnitude of this Quaternion
		 *\return		The magnitude
		 *\~french
		 *\brief		Récupère la magnitude de ce Quaternion
		 *\return		La magnitude
		 */
		inline double get_magnitude()const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation another quaternions.
		 *\remarks		The interpolation is oriented and the rotation is performed at constant speed.
		 *\param[in]	p_target	The target quaternion
		 *\param[in]	p_factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation est orientée et le rotation est effectuée à vitesse constante.
		 *\param[in]	p_target	Le quaternion cible
		 *\param[in]	p_factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > mix( QuaternionT< T > const & p_target, double p_factor )const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation another quaternions.
		 *\remarks		The interpolation is oriented and the rotation is performed at constant speed.
		 *\param[in]	p_target	The target quaternion
		 *\param[in]	p_factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation est orientée et le rotation est effectuée à vitesse constante.
		 *\param[in]	p_target	Le quaternion cible
		 *\param[in]	p_factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > mix( QuaternionT< T > const & p_target, float p_factor )const;
		/**
		 *\~english
		 *\brief		Linear interpolation of two quaternions.
		 *\param[in]	p_target	The target quaternion
		 *\param[in]	p_factor	The interpolation factor. The interpolation is defined in the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation linéaire de deux quaternions.
		 *\param[in]	p_target	Le quaternion cible
		 *\param[in]	p_factor	Le facteur d'interpolation. L'interpolation est définie à l'intérieur de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > lerp( QuaternionT< T > const & p_target, double p_factor )const;
		/**
		 *\~english
		 *\brief		Linear interpolation of two quaternions.
		 *\param[in]	p_target	The target quaternion
		 *\param[in]	p_factor	The interpolation factor. The interpolation is defined in the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation linéaire de deux quaternions.
		 *\param[in]	p_target	Le quaternion cible
		 *\param[in]	p_factor	Le facteur d'interpolation. L'interpolation est définie à l'intérieur de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > lerp( QuaternionT< T > const & p_target, float p_factor )const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation with another quaternions.
		 *\remarks		The interpolation always take the short path and the rotation is performed at constant speed.
		 *\param[in]	p_target	The target quaternion
		 *\param[in]	p_factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation prend toujours le chemin le plus court, et la rotation est effectuée à vitesse constante.
		 *\param[in]	p_target	Le quaternion cible
		 *\param[in]	p_factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > slerp( QuaternionT< T > const & p_target, double p_factor )const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation with another quaternions.
		 *\remarks		The interpolation always take the short path and the rotation is performed at constant speed.
		 *\param[in]	p_target	The target quaternion
		 *\param[in]	p_factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation prend toujours le chemin le plus court, et la rotation est effectuée à vitesse constante.
		 *\param[in]	p_target	Le quaternion cible
		 *\param[in]	p_factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > slerp( QuaternionT< T > const & p_target, float p_factor )const;
		/**
		 *\~english
		 *\brief		Retrieves the Quaternion identity
		 *\~french
		 *\brief		Récupère le Quaternion identité
		 */
		inline static QuaternionT< T > identity();
		/**
		 *\~english
		 *\brief		Retrieves the null Quaternion
		 *\~french
		 *\brief		Récupère le Quaternion nul
		 */
		inline static QuaternionT< T > null();

	public:
		union
		{
			struct
			{
				T x;
				T y;
				T z;
				T w;
			} quat;
			T buffer[4];
		};
	};
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_lhs	The first Quaternion object to add
	 *\param[in]	p_rhs	The second Quaternion object to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_lhs	Le premier objet Quaternion à ajouter
	 *\param[in]	p_rhs	Le second objet Quaternion à ajouter
	 *\return		Le résultat de l'addtion
	 */
	template< typename T >
	inline QuaternionT< T > operator+( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_lhs	The first Quaternion object to substract
	 *\param[in]	p_rhs	The second Quaternion object to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_lhs	Le premier objet Quaternion à soustraire
	 *\param[in]	p_rhs	Le second objet Quaternion à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T >
	inline QuaternionT< T > operator-( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	The first Quaternion object to multiply
	 *\param[in]	p_rhs	The second Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	Le premier objet Quaternion à multiplier
	 *\param[in]	p_rhs	Le second objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, QuaternionT< T > const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	The Quaternion object to multiply
	 *\param[in]	p_rhs	The scalar to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	L'objet Quaternion à multiplier
	 *\param[in]	p_rhs	Le scalaire à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, double p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	The Quaternion object to multiply
	 *\param[in]	p_rhs	The scalar to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	L'objet Quaternion à multiplier
	 *\param[in]	p_rhs	Le scalaire à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( QuaternionT< T > const & p_lhs, float p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	The scalar to multiply
	 *\param[in]	p_rhs	The Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	Le scalaire à multiplier
	 *\param[in]	p_rhs	L'objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( double p_lhs, QuaternionT< T > const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	The scalar to multiply
	 *\param[in]	p_rhs	The Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	Le scalaire à multiplier
	 *\param[in]	p_rhs	L'objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( float p_lhs, QuaternionT< T > const & p_rhs );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_quat		The Quaternion object to negate
	 *\return		The negation result
	 *\~french
	 *\brief		Opérateur de négation
	 *\param[in]	p_quat		L'objet Quaternion à négativer
	 *\return		Le résultat de la négation
	 */
	template< typename T >
	inline QuaternionT< T > operator-( QuaternionT< T > const & p_quat );
	/**
	 *\~english
	 *\brief			Stream operator.
	 *\param[in,out]	p_stream	The stream.
	 *\param[in]		p_quat		The Quaternion object to put in stream.
	 *\return			The stream.
	 *\~french
	 *\brief			Opérateur de flux.
	 *\param[in,out]	p_stream	Le flux.
	 *\param[in]		p_quat		L'objet Quaternion à mettre dans le flux
	 *\return			Le flux.
	 */
	template< typename CharT, typename T >
	inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & p_stream, QuaternionT< T > const & p_quat )
	{
		p_stream << p_quat.quat.x << ", " << p_quat.quat.y << ", " << p_quat.quat.z << ", " << p_quat.quat.w;
		return p_stream;
	}
}

#include "Quaternion.inl"

#endif
