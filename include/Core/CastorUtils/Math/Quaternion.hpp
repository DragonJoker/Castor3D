/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_QUATERNION_H___
#define ___CASTOR_QUATERNION_H___

#include "CastorUtils/Math/Angle.hpp"
#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Math/SquareMatrix.hpp"

namespace castor
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
			: public castor::TextLoader< QuaternionT< T > >
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
			 *\param[out]	object	The Coords< T, Count > object to read
			 *\param[in]	file	The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Charge un objet Coords< T, Count > à partir d'un fichier texte
			 *\param[out]	object	L'objet Coords< T, Count >
			 *\param[in]	file	Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( QuaternionT< T > & object, TextFile & file );
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
			: public castor::TextWriter< QuaternionT< T > >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			explicit TextWriter( String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a Point2f object into a text file
			 *\param[in]	object	The Point2f object to write
			 *\param[out]	file	The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Ecrit un objet Point2f dans un fichier texte
			 *\param[in]	object	L'objet Point2f
			 *\param[out]	file	Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( QuaternionT< T > const & object, TextFile & file );
		};

	private:
		typedef Coords4< T > BaseType;

	private:
		explicit QuaternionT( NoInit const & );
		QuaternionT( T x, T y, T z, T w );

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
		 *\param[in]	rhs	The Quaternion object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	rhs	L'objet Quaternion à copier
		 */
		inline QuaternionT( QuaternionT< T > const & rhs );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	rhs	The Quaternion object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	rhs	L'objet Quaternion à déplacer
		 */
		inline QuaternionT( QuaternionT< T > && rhs );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	values	The Quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	values	Les valeurs du Quaternion
		 */
		inline explicit QuaternionT( double const * values );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	values	The Quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	values	Les valeurs du Quaternion
		 */
		inline explicit QuaternionT( float const * values );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	values	The quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	values	Les valeurs du quaternion
		 */
		inline explicit QuaternionT( Point4f const & values );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	values	The quaternion values
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	values	Les valeurs du quaternion
		 */
		inline explicit QuaternionT( Point4d const & values );
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
		 *\param[in]	rhs	The Quaternion object to copy
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	rhs	L'objet Quaternion à copier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator=( QuaternionT< T > const & rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	rhs	The Quaternion object to move
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	rhs	L'objet Quaternion à déplacer
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator=( QuaternionT< T > && rhs );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	rhs	The Quaternion object to add
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	rhs	L'objet Quaternion à ajouter
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator+=( QuaternionT< T > const & rhs );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	rhs	The Quaternion object to subtract
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	rhs	L'objet Quaternion à soustraire
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator-=( QuaternionT< T > const & rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	rhs	The Quaternion object to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	rhs	L'objet Quaternion à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator*=( QuaternionT< T > const & rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	rhs	The scalar to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	rhs	Le scalaire à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator*=( double rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	rhs	The scalar to multiply
		 *\return		A reference to this Quaternion object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	rhs	Le scalaire à multiplier
		 *\return		Une référence sur cet objet Quaternion
		 */
		inline QuaternionT< T > & operator*=( float rhs );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	matrix	La matrice de rotation
		 */
		static inline QuaternionT< T > fromMatrix( Matrix4x4f const & matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	matrix	La matrice de rotation
		 */
		static inline QuaternionT< T > fromMatrix( Matrix4x4d const & matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	matrix	La matrice de rotation
		 */
		static inline QuaternionT< T > fromMatrix( float const * matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from a rotation matrix
		 *\param[in]	matrix	The rotation matrix
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'une matrice de rotation
		 *\param[in]	matrix	La matrice de rotation
		 */
		static inline QuaternionT< T > fromMatrix( double const * matrix );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from an axis and an angle
		 *\param[in]	vector	The axis
		 *\param[in]	angle	The angle
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'un axe et d'un angle
		 *\param[in]	vector	L'axe
		 *\param[in]	angle	L'angle
		 */
		static inline QuaternionT< T > fromAxisAngle( Point3f const & vector, Angle const & angle );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from an axis and an angle
		 *\param[in]	vector	The axis
		 *\param[in]	angle	The angle
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir d'un axe et d'un angle
		 *\param[in]	vector	L'axe
		 *\param[in]	angle	L'angle
		 */
		static inline QuaternionT< T > fromAxisAngle( Point3d const & vector, Angle const & angle );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from 3 axes
		 *\param[in]	x, y, z	The axes
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir de 3 axes
		 *\param[in]	x, y, z	Les axes
		 */
		static inline QuaternionT< T > fromAxes( Point3f const & x, Point3f const & y, Point3f const & z );
		/**
		 *\~english
		 *\brief		Updates this Quaternion from 3 axes
		 *\param[in]	x, y, z	The axes
		 *\~french
		 *\brief		Met à jour ce Quaternion à partir de 3 axes
		 *\param[in]	x, y, z	Les axes
		 */
		static inline QuaternionT< T > fromAxes( Point3d const & x, Point3d const & y, Point3d const & z );
		/**
		 *\~english
		 *\brief		Transforms a vector and gives the result
		 *\param[in]	vector	The vector to transform
		 *\param[out]	result	Receives the result
		 *\return		A reference to result
		 *\~french
		 *\brief		Transforme un vecteur et donne le résultat
		 *\param[in]	vector	Le vecteur à transformer
		 *\param[out]	result	Reçoit le résultat
		 *\return		Une référence sur result
		 */
		inline Point3f & transform( Point3f const & vector, Point3f & result )const;
		/**
		 *\~english
		 *\brief		Transforms a vector and gives the result
		 *\param[in]	vector	The vector to transform
		 *\param[out]	result	Receives the result
		 *\return		A reference to result
		 *\~french
		 *\brief		Transforme un vecteur et donne le résultat
		 *\param[in]	vector	Le vecteur à transformer
		 *\param[out]	result	Reçoit le résultat
		 *\return		Une référence sur result
		 */
		inline Point3d & transform( Point3d const & vector, Point3d & result )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	matrix	La matrice à remplir
		 */
		inline void toMatrix( float * matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	matrix	La matrice à remplir
		 */
		inline void toMatrix( double * matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	matrix	La matrice à remplir
		 */
		inline void toMatrix( Matrix4x4f & matrix )const;
		/**
		 *\~english
		 *\brief		Fills a rotation matrix from this Quaternion
		 *\param[out]	matrix	The rotation matrix to fill
		 *\~french
		 *\brief		Remplit une matrice de rotation à partir de ce Quaternion
		 *\param[out]	matrix	La matrice à remplir
		 */
		inline void toMatrix( Matrix4x4d & matrix )const;
		/**
		 *\~english
		 *\brief		Gives the axis and the angle from this Quaternion
		 *\param[out]	vector	Receives the axis
		 *\param[out]	angle		Receives the angle
		 *\~french
		 *\brief		Calcule l'axe et l'angle de ce Quaternion
		 *\param[out]	vector	Reçoit l'axe
		 *\param[out]	angle		Reçoit l'angle
		 */
		inline void toAxisAngle( Point3f & vector, Angle & angle )const;
		/**
		 *\~english
		 *\brief		Gives the axis and the angle from this Quaternion
		 *\param[out]	vector	Receives the axis
		 *\param[out]	angle		Receives the angle
		 *\~french
		 *\brief		Calcule l'axe et l'angle de ce Quaternion
		 *\param[out]	vector	Reçoit l'axe
		 *\param[out]	angle		Reçoit l'angle
		 */
		inline void toAxisAngle( Point3d & vector, Angle & angle )const;
		/**
		 *\~english
		 *\brief		Computes 3 axes from this quaternion
		 *\param[out]	x, y, z	The axes
		 *\~french
		 *\brief		Calcule 3 axes à partir de ce quaternion
		 *\param[out]	x, y, z	Les axes
		 */
		inline void toAxes( Point3f & x, Point3f & y, Point3f & z )const;
		/**
		 *\~english
		 *\brief		Computes 3 axes from this quaternion
		 *\param[out]	x, y, z	The axes
		 *\~french
		 *\brief		Calcule 3 axes à partir de ce quaternion
		 *\param[out]	x, y, z	Les axes
		 */
		inline void toAxes( Point3d & x, Point3d & y, Point3d & z )const;
		/**
		 *\~english
		 *\brief		sets this Quaternion to its conjugate
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
		inline QuaternionT< T > getConjugate()const;
		/**
		 *\~english
		 *\brief		Retrieves the magnitude of this Quaternion
		 *\return		The magnitude
		 *\~french
		 *\brief		Récupère la magnitude de ce Quaternion
		 *\return		La magnitude
		 */
		inline double getMagnitude()const;
		/**
		 *\~english
		 *\return		The pitch angle.
		 *\~french
		 *\return		L'angle de pitch.
		 */
		inline AngleT< T > getPitch()const;
		/**
		 *\~english
		 *\return		The yaw angle.
		 *\~french
		 *\return		L'angle de yaw.
		 */
		inline AngleT< T > getYaw()const;
		/**
		 *\~english
		 *\return		The roll angle.
		 *\~french
		 *\return		L'angle de roll.
		 */
		inline AngleT< T > getRoll()const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation another quaternions.
		 *\remarks		The interpolation is oriented and the rotation is performed at constant speed.
		 *\param[in]	target	The target quaternion
		 *\param[in]	factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation est orientée et le rotation est effectuée à vitesse constante.
		 *\param[in]	target	Le quaternion cible
		 *\param[in]	factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > mix( QuaternionT< T > const & target, double factor )const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation another quaternions.
		 *\remarks		The interpolation is oriented and the rotation is performed at constant speed.
		 *\param[in]	target	The target quaternion
		 *\param[in]	factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation est orientée et le rotation est effectuée à vitesse constante.
		 *\param[in]	target	Le quaternion cible
		 *\param[in]	factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > mix( QuaternionT< T > const & target, float factor )const;
		/**
		 *\~english
		 *\brief		Linear interpolation of two quaternions.
		 *\param[in]	target	The target quaternion
		 *\param[in]	factor	The interpolation factor. The interpolation is defined in the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation linéaire de deux quaternions.
		 *\param[in]	target	Le quaternion cible
		 *\param[in]	factor	Le facteur d'interpolation. L'interpolation est définie à l'intérieur de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > lerp( QuaternionT< T > const & target, double factor )const;
		/**
		 *\~english
		 *\brief		Linear interpolation of two quaternions.
		 *\param[in]	target	The target quaternion
		 *\param[in]	factor	The interpolation factor. The interpolation is defined in the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation linéaire de deux quaternions.
		 *\param[in]	target	Le quaternion cible
		 *\param[in]	factor	Le facteur d'interpolation. L'interpolation est définie à l'intérieur de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > lerp( QuaternionT< T > const & target, float factor )const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation with another quaternions.
		 *\remarks		The interpolation always take the short path and the rotation is performed at constant speed.
		 *\param[in]	target	The target quaternion
		 *\param[in]	factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation prend toujours le chemin le plus court, et la rotation est effectuée à vitesse constante.
		 *\param[in]	target	Le quaternion cible
		 *\param[in]	factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > slerp( QuaternionT< T > const & target, double factor )const;
		/**
		 *\~english
		 *\brief		Spherical linear interpolation with another quaternions.
		 *\remarks		The interpolation always take the short path and the rotation is performed at constant speed.
		 *\param[in]	target	The target quaternion
		 *\param[in]	factor	The interpolation factor. The interpolation is defined beyond the range [0, 1].
		 *\return		The result of the interpolation
		 *\~french
		 *\brief		Interpolation sphérique linéaire de deux quaternions.
		 *\remarks		L'interpolation prend toujours le chemin le plus court, et la rotation est effectuée à vitesse constante.
		 *\param[in]	target	Le quaternion cible
		 *\param[in]	factor	Le facteur d'interpolation. L'interpolation est définie hors de l'intervalle [0, 1].
		 *\return		Le résultat de l'interpolation.
		 */
		inline QuaternionT< T > slerp( QuaternionT< T > const & target, float factor )const;
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
	 *\brief		addition operator
	 *\param[in]	lhs	The first Quaternion object to add
	 *\param[in]	rhs	The second Quaternion object to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	lhs	Le premier objet Quaternion à ajouter
	 *\param[in]	rhs	Le second objet Quaternion à ajouter
	 *\return		Le résultat de l'addtion
	 */
	template< typename T >
	inline QuaternionT< T > operator+( QuaternionT< T > const & lhs, QuaternionT< T > const & rhs );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	lhs	The first Quaternion object to subtract
	 *\param[in]	rhs	The second Quaternion object to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	lhs	Le premier objet Quaternion à soustraire
	 *\param[in]	rhs	Le second objet Quaternion à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T >
	inline QuaternionT< T > operator-( QuaternionT< T > const & lhs, QuaternionT< T > const & rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	The first Quaternion object to multiply
	 *\param[in]	rhs	The second Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	Le premier objet Quaternion à multiplier
	 *\param[in]	rhs	Le second objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( QuaternionT< T > const & lhs, QuaternionT< T > const & rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	The Quaternion object to multiply
	 *\param[in]	rhs	The scalar to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	L'objet Quaternion à multiplier
	 *\param[in]	rhs	Le scalaire à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( QuaternionT< T > const & lhs, double rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	The Quaternion object to multiply
	 *\param[in]	rhs	The scalar to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	L'objet Quaternion à multiplier
	 *\param[in]	rhs	Le scalaire à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( QuaternionT< T > const & lhs, float rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	The scalar to multiply
	 *\param[in]	rhs	The Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	Le scalaire à multiplier
	 *\param[in]	rhs	L'objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( double lhs, QuaternionT< T > const & rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	The scalar to multiply
	 *\param[in]	rhs	The Quaternion object to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	Le scalaire à multiplier
	 *\param[in]	rhs	L'objet Quaternion à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T >
	inline QuaternionT< T > operator*( float lhs, QuaternionT< T > const & rhs );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	quat	The Quaternion object to negate
	 *\return		The negation result
	 *\~french
	 *\brief		Opérateur de négation
	 *\param[in]	quat	L'objet Quaternion à négativer
	 *\return		Le résultat de la négation
	 */
	template< typename T >
	inline QuaternionT< T > operator-( QuaternionT< T > const & quat );
	/**
	 *\~english
	 *\brief			Stream operator.
	 *\param[in,out]	stream	The stream.
	 *\param[in]		quat	The Quaternion object to put in stream.
	 *\return			The stream.
	 *\~french
	 *\brief			Opérateur de flux.
	 *\param[in,out]	stream	Le flux.
	 *\param[in]		quat	L'objet Quaternion à mettre dans le flux
	 *\return			Le flux.
	 */
	template< typename CharT, typename T >
	inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, QuaternionT< T > const & quat )
	{
		stream << quat.quat.x << ", " << quat.quat.y << ", " << quat.quat.z << ", " << quat.quat.w;
		return stream;
	}
}

#include "Quaternion.inl"

#endif
