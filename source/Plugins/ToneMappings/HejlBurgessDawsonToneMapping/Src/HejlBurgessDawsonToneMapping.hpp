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
#ifndef ___C3D_HBD_TONE_MAPPING_H___
#define ___C3D_HBD_TONE_MAPPING_H___

#include <HDR/ToneMapping.hpp>

namespace HejlBurgessDawson
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Implements Jim Hejl and Richard Burgess-Dawson tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Jim Hejl et Richard Burgess-Dawson.
	*/
	class ToneMapping
		: public Castor3D::ToneMapping
	{
	public:
		/**
		 *\~english
		 * Specified constructor
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 * Constructeur spécifié
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		ToneMapping( Castor3D::Engine & p_engine, Castor3D::Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ToneMapping();
		/**
		 *\~english
		 *\brief		Creator function, for the factory.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Fonction de création, pour la fabrique.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		static Castor3D::ToneMappingSPtr Create( Castor3D::Engine & p_engine, Castor3D::Parameters const & p_parameters );

	private:
		/**
		 *\copydoc		Castor3D::ToneMapping::DoCreate
		 */
		Castor::String DoCreate( Castor3D::UniformBuffer & p_ubo )override;
		/**
		 *\copydoc		Castor3D::ToneMapping::DoDestroy
		 */
		void DoDestroy()override;
		/**
		 *\copydoc		Castor3D::ToneMapping::DoUpdate
		 */
		void DoUpdate()override;
		/**
		 *\copydoc		Castor3D::ToneMapping::DoWriteInto
		 */
		bool DoWriteInto( Castor::TextFile & p_file )override;

	public:
		static Castor::String Name;
	};
}

#endif

