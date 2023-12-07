#ifndef DLGNEWJOB_H_INCLUDED
#define DLGNEWJOB_H_INCLUDED

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

class CDlgNewJob : public wxDialog
{
private:

protected:
	wxStaticBitmap* m_bitmap1;
	wxStaticText* m_staticText10;
	wxStaticText* m_staticText11;
	wxTextCtrl* m_txtOpen;
	wxButton* m_btnConfirm;
	wxButton* m_btnCancel;
	wxButton* m_btnFind;

	// Virtual event handlers, override them in your derived class
	virtual void OnBtnConfirm( wxCommandEvent& event );
	virtual void OnBtnCancel( wxCommandEvent& event );
	virtual void OnBtnFind( wxCommandEvent& event );
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnTextEnter( wxCommandEvent& event );

private:
	void DoExecNewJob();

public:
	CDlgNewJob( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("새 작업 실행"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 480,153 ), long style = wxDEFAULT_DIALOG_STYLE );
	~CDlgNewJob();

};



#endif // DLGNEWJOB_H_INCLUDED
