/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TreeModel_HPP___
#define ___CTP_TreeModel_HPP___

#include "CastorTestParser/Prerequisites.hpp"

#include <wx/dataview.h>

namespace test_parser
{
	/*
	TreeModel
	Implements the following data model:
		Category	Name		Renderer	RunDate					Status	Ignored	OutOfCastorDate	OutOfSceneDate
	--------------------------------------------------------------------------------------------------------------
	1:	Common		001-test	vk			2020-11-06 15:35:22		1		1		1				1
	*/
	class TreeModel
		: public wxDataViewModel
	{
	public:
		enum class Column
		{
			eCategory,
			eName,
			eRenderer,
			eRunDate,
			eStatus,
			eCount,
		};

	public:
		TreeModel( Config const & config );
		~TreeModel();

		TreeModelNode * addRenderer( std::string const & renderer );
		TreeModelNode * addCategory( std::string const & renderer
			, std::string const & category );
		TreeModelNode * addTest( Test & test );
		void expandRoots( wxDataViewCtrl * view );

		// helper method for wxLog
		uint32_t getId( wxDataViewItem const & item )const;
		std::string getName( wxDataViewItem const & item )const;
		db::DateTime getRunDate( wxDataViewItem const & item )const;
		TestStatus getStatus( wxDataViewItem const & item )const;
		std::string getRenderer( wxDataViewItem const & item )const;
		std::string getCategory( wxDataViewItem const & item )const;

		TreeModelNode * GetRootNode()const
		{
			return m_root;
		}

		// helper methods to change the model
		void Delete( wxDataViewItem const & item );

		// Custom comparison
		int Compare( wxDataViewItem const & item1
			, wxDataViewItem const & item2
			, unsigned int column
			, bool ascending )const override;

		// implementation of base class virtuals to define model
		unsigned int GetColumnCount()const override;
		wxString GetColumnType( unsigned int col )const override;
		void GetValue( wxVariant & variant
			, wxDataViewItem const & item
			, unsigned int col )const override;
		bool SetValue( wxVariant const & variant
			, wxDataViewItem const & item
			, unsigned int col ) override;
		wxDataViewItem GetParent( wxDataViewItem const & item )const override;
		bool IsContainer( wxDataViewItem const & item )const override;
		unsigned int GetChildren( wxDataViewItem const & parent
			, wxDataViewItemArray & array )const override;

	private:
		TreeModelNode * m_root;
		Config const & m_config;
		std::map< std::string, TreeModelNode * > m_renderers;
		std::map< std::string, TreeModelNode * > m_categories;
	};
}

#endif
