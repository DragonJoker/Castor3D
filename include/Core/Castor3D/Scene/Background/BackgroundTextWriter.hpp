/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundTextWriter_H___
#define ___C3D_BackgroundTextWriter_H___

#include "Castor3D/Scene/Background/Visitor.hpp"

namespace castor3d
{
	class BackgroundTextWriter
		: public BackgroundVisitor
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\~french
		*\brief
		*	Constructeur.
		*/
		C3D_API BackgroundTextWriter( castor::TextFile & file
			, castor::String const & tabs );
		/**
		*\copydoc	castor3d::BackgroundVisitor::visit
		*/
		C3D_API void visit( ColourBackground const & background )override;
		/**
		*\copydoc	castor3d::BackgroundVisitor::visit
		*/
		C3D_API void visit( SkyboxBackground const & background )override;
		/**
		*\copydoc	castor3d::BackgroundVisitor::visit
		*/
		C3D_API void visit( ImageBackground const & background )override;

	private:
		castor::TextFile & m_file;
		castor::String m_tabs;
	};
}

#endif
