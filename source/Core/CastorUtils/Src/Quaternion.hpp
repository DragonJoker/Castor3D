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
#ifndef ___CASTOR_QUATERNION_H___
#define ___CASTOR_QUATERNION_H___

#include "Point.hpp"
#include "SquareMatrix.hpp"
#include "Loader.hpp"

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
	class Quaternion
		: public Coords4d
	{
	private:
		typedef Coords4d BaseType;

	private:
		Quaternion( double p_x, double p_y, double p_z, double p_w );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		CU_API Quaternion();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_q	The Quaternion object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_q	L'objet Quaternion à copier
		 */
		CU_API Quaternion( Quaternion const & p_q );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_q	The Quaternion object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_q	L'objet Quaternion à déplacer
		 */
		CU_API Quaternion( Quaternion && p_q );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_q	The Quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_q	Les valeurs du Quaternion
		 */
		CU_API Quaternion( double const * p_q );
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
		CU_API Quaternion( Point3f const & p_vector, Angle const & p_angle );
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
		CU_API Quaternion( Point3d const & p_vector, Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_ptValues	The quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ptValues	Les valeurs du quaternion
		 */
		CU_API Quaternion( Point4f const & p_ptValues );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_ptValues	The quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ptValues	Les valeurs du quaternion
		 */
		CU_API Quaternion( Point4d const & p_ptValues );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~Quaternion();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_q	The Quaternion object to copy
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_q	L'objet Quaternion à copier
		 *\return		Une référence sur cet objet Quaternion
		 */
		CU_API Quaternion & operator =( Quaternion const & p_q );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_q	The Quaternion object to move
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_q	L'objet Quaternion à déplacer
		 *\return		Une référence sur cet objet Quaternion
		 */
		CU_API Quaternion & operator =( Quaternion && p_q );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_q	The Quaternion object to add
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_q	L'objet Quaternion à ajouter
		 *\return		Une référence sur cet objet Quaternion
		 */
		CU_API Quaternion & operator +=( Quaternion const & p_q );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_q	The Quaternion object to substract
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_q	L'objet Quaternion à soustraire
		 *\return		Une référence sur cet objet Quaternion
		 */
		CU_API Quaternion & operator -=( Quaternion const & p_q );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_q	The Quaternion object to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_q	L'objet Quaternion à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		CU_API Quaternion & operator *=( Quaternion const & p_q );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_fScalar	The scalar to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_fScalar	Le scalaire à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		CU_API Quaternion & operator *=( double p_fScalar );
		/**
		 *\~english
		 *\brief		Transforms a vector and gives the result
		 *\param[in]	p_vector	The vector to transform
		 *\param[out]	p_ptResult	Receives the result
		 *\return		A reference to p_ptResult
		 *\~french
		 *\brief		Transforme un vecteur et donne le résultat
		 *\param[in]	p_vector	Le vecteur à transformer
		 *\param[out]	p_ptResult	Reçoit le résultat
		 *\return		Une référence sur p_ptResult
		 */
		CU_API Point3f & Transform( Point3f const & p_vector, Point3f & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Transforms a vector and gives the result
		 *\param[in]	p_vector	The vector to transform
		 *\param[out]	p_ptResult	Receives the result
		 *\return		A reference to p_ptResult
		 *\~french
		 *\brief		Transforme un vecteur et donne le résultat
		 *\param[in]	p_vector	Le vecteur à transformer
		 *\param[out]	p_ptResult	Reçoit le résultat
		 *\return		Une référence sur p_ptResult
		 */
		CU_API Point3d & Transform( Point3d const & p_vector, Point3d & p_ptResult )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		CU_API void ToRotationMatrix( float * p_matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		CU_API void ToRotationMatrix( double * p_matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		inline void ToRotationMatrix( Matrix4x4f & p_matrix )const
		{
			ToRotationMatrix( p_matrix.ptr() );
		}
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	p_matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	p_matrix	La matrice à remplir
		 */
		inline void ToRotationMatrix( Matrix4x4d & p_matrix )const
		{
			ToRotationMatrix( p_matrix.ptr() );
		}
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		CU_API void FromRotationMatrix( Matrix4x4f const & p_matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		CU_API void FromRotationMatrix( Matrix4x4d const & p_matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		inline void FromRotationMatrix( float const * p_matrix )
		{
			FromRotationMatrix( Matrix4x4f( p_matrix ) );
		}
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	p_matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	p_matrix	La matrice de rotation
		 */
		inline void FromRotationMatrix( double const * p_matrix )
		{
			FromRotationMatrix( Matrix4x4d( p_matrix ) );
		}
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
		CU_API void FromAxisAngle( Point3f const & p_vector, Angle const & p_angle );
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
		CU_API void FromAxisAngle( Point3d const & p_vector, Angle const & p_angle );
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
		CU_API void ToAxisAngle( Point3f & p_vector, Angle & p_angle )const;
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
		CU_API void ToAxisAngle( Point3d & p_vector, Angle & p_angle )const;
		/**
		 *\~english
		 *\brief		Updates this Quaternion from 3 axes
		 *\param[in]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir de 3 axes
		 *\param[in]	p_vector	L'axe
		 *\param[in]	p_angle		L'angle
		 */
		CU_API void FromAxes( Point3f const & p_x, Point3f const & p_y, Point3f const & p_z );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from 3 axes
		 *\param[in]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir de 3 axes
		 *\param[in]	p_vector	L'axe
		 *\param[in]	p_angle		L'angle
		 */
		CU_API void FromAxes( Point3d const & p_x, Point3d const & p_y, Point3d const & p_z );
		/**
		 *\~english
		 *\brief		Computes 3 axes from this quaternion
		 *\param[out]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Calcule 3 axes à partir de ce quaternion
		 *\param[out]	p_x, p_y, p_z	Les axes
		 */
		CU_API void ToAxes( Point3f & p_x, Point3f & p_y, Point3f & p_z )const;
		/**
		 *\~english
		 *\brief		Computes 3 axes from this quaternion
		 *\param[out]	p_x, p_y, p_z	The axes
		 *\~french
		 *\brief		Calcule 3 axes à partir de ce quaternion
		 *\param[out]	p_x, p_y, p_z	Les axes
		 */
		CU_API void ToAxes( Point3d & p_x, Point3d & p_y, Point3d & p_z )const;
		/**
		 *\~english
		 *\brief		Retrieves the Y axis rotation value
		 *\return		The Y axis rotation value
		 *\~french
		 *\brief		Récupère la valeur de la rotation sur l'axe Y
		 *\return		La valeur de la rotation autour de l'axe Y
		 */
		CU_API Angle GetYaw()const;
		/**
		 *\~english
		 *\brief		Retrieves the Y axis rotation value
		 *\return		The Y axis rotation value
		 *\~french
		 *\brief		Récupère la valeur de la rotation sur l'axe Y
		 *\return		La valeur de la rotation autour de l'axe Y
		 */
		CU_API Angle GetPitch()const;
		/**
		 *\~english
		 *\brief		Retrieves the Y axis rotation value
		 *\return		The Y axis rotation value
		 *\~french
		 *\brief		Récupère la valeur de la rotation sur l'axe Y
		 *\return		La valeur de la rotation autour de l'axe Y
		 */
		CU_API Angle GetRoll()const;
		/**
		 *\~english
		 *\brief		Sets this Quaternion to its conjugate
		 *\~french
		 *\brief		Définit ce Quaternion à son conjugué
		 */
		CU_API void Conjugate();
		/**
		 *\~english
		 *\brief		Retrieves the conjugate of this Quaternion
		 *\return		The conjugate
		 *\~french
		 *\brief		Récupère le conjugué de ce Quaternion
		 *\return		Le conjugué
		 */
		CU_API Quaternion GetConjugate()const;
		/**
		 *\~english
		 *\brief		Retrieves the magnitude of this Quaternion
		 *\return		The magnitude
		 *\~french
		 *\brief		Récupère la magnitude de ce Quaternion
		 *\return		La magnitude
		 */
		CU_API double GetMagnitude()const;
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
		CU_API Quaternion Mix( Quaternion const & p_target, double p_factor )const;
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
		CU_API Quaternion Lerp( Quaternion const & p_target, double p_factor )const;
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
		CU_API Quaternion Slerp( Quaternion const & p_target, double p_factor )const;
		/**
		 *\~english
		 *\brief		Retrieves the x component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante x
		 *\return		La composante
		 */
		inline double const & x()const
		{
			return m_data.quaternion.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the y component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante y
		 *\return		La composante
		 */
		inline double const & y()const
		{
			return m_data.quaternion.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the z component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante z
		 *\return		La composante
		 */
		inline double const & z()const
		{
			return m_data.quaternion.z;
		}
		/**
		 *\~english
		 *\brief		Retrieves the w component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante w
		 *\return		La composante
		 */
		inline double const & w()const
		{
			return m_data.quaternion.w;
		}
		/**
		 *\~english
		 *\brief		Retrieves the x component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante x
		 *\return		La composante
		 */
		inline double & x()
		{
			return m_data.quaternion.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the y component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante y
		 *\return		La composante
		 */
		inline double & y()
		{
			return m_data.quaternion.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the z component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante z
		 *\return		La composante
		 */
		inline double & z()
		{
			return m_data.quaternion.z;
		}
		/**
		 *\~english
		 *\brief		Retrieves the w component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante w
		 *\return		La composante
		 */
		inline double & w()
		{
			return m_data.quaternion.w;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Quaternion identity
		 *\~french
		 *\brief		Récupère le Quaternion identité
		 */
		CU_API static Quaternion Identity();
		/**
		 *\~english
		 *\brief		Retrieves the null Quaternion
		 *\~french
		 *\brief		Récupère le Quaternion nul
		 */
		CU_API static Quaternion Null();

	private:
		union
		{
			struct
			{
				double x;
				double y;
				double z;
				double w;
			}		quaternion;
			double	buffer[4];
		}	m_data;
	};
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_qA	The first Quaternion object to add
	 *\param[in]	p_qB	The second Quaternion object to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_qA	Le premier objet Quaternion à ajouter
	 *\param[in]	p_qB	Le second objet Quaternion à ajouter
	 *\return		Le résultat de l'addtion
	 */
	CU_API Quaternion	operator +( Quaternion const & p_qA, Quaternion const & p_qB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_qA	The first Quaternion object to substract
	 *\param[in]	p_qB	The second Quaternion object to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_qA	Le premier objet Quaternion à soustraire
	 *\param[in]	p_qB	Le second objet Quaternion à soustraire
	 *\return		Le résultat de la soustraction
	 */
	CU_API Quaternion	operator -( Quaternion const & p_qA, Quaternion const & p_qB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_qA	The first Quaternion object to multiply
	 *\param[in]	p_qB	The second Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_qA	Le premier objet Quaternion à multiplier
	 *\param[in]	p_qB	Le second objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	CU_API Quaternion	operator *( Quaternion const & p_qA, Quaternion const & p_qB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_quat		The Quaternion object to multiply
	 *\param[in]	p_fScalar	The scalar to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_quat		L'objet Quaternion à multiplier
	 *\param[in]	p_fScalar	Le scalaire à multiplier
	 *\return		Le résultat de la multiplication
	 */
	CU_API Quaternion	operator *( Quaternion const & p_quat, double p_fScalar );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_fScalar	The scalar to multiply
	 *\param[in]	p_quat		The Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_fScalar	Le scalaire à multiplier
	 *\param[in]	p_quat		L'objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	CU_API Quaternion	operator *( double p_fScalar, Quaternion const & p_quat );
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
	CU_API Quaternion	operator -( Quaternion const & p_quat );
}

#endif
