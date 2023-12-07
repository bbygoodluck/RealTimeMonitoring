#include "../ginc.h"
#include "DlgNewJob.h"

CDlgNewJob::CDlgNewJob( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bxMain;
	bxMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bxMessage;
	bxMessage = new wxBoxSizer( wxHORIZONTAL );

	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxArtProvider::GetBitmap( wxART_CDROM, wxART_CMN_DIALOG ), wxDefaultPosition, wxDefaultSize, 0 );
	bxMessage->Add( m_bitmap1, 0, wxALL, 5 );

	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("프로그램, 폴더, 문서 또는 인터넷 주소를 입력하여  해당 목록을 열수 있습니다."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bxMessage->Add( m_staticText10, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	bxMain->Add( bxMessage, 0, wxEXPAND, 5 );

	wxBoxSizer* bxOpen;
	bxOpen = new wxBoxSizer( wxHORIZONTAL );

	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("열기 : "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bxOpen->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_txtOpen = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bxOpen->Add( m_txtOpen, 1, wxALL|wxEXPAND, 5 );


	bxMain->Add( bxOpen, 0, wxEXPAND, 5 );

	wxBoxSizer* bxButton;
	bxButton = new wxBoxSizer( wxHORIZONTAL );

	m_btnConfirm = new wxButton( this, wxID_ANY, wxT("확인"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnConfirm->SetBitmap( wxArtProvider::GetBitmap( wxART_TICK_MARK, wxART_TOOLBAR ) );
	bxButton->Add( m_btnConfirm, 0, wxALL, 5 );

	m_btnCancel = new wxButton( this, wxID_ANY, wxT("취소"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnCancel->SetBitmap( wxArtProvider::GetBitmap( wxART_CLOSE, wxART_TOOLBAR ) );
	bxButton->Add( m_btnCancel, 0, wxALL, 5 );

	m_btnFind = new wxButton( this, wxID_ANY, wxT("찾아보기"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnFind->SetBitmap( wxArtProvider::GetBitmap( wxART_FIND, wxART_TOOLBAR ) );
	bxButton->Add( m_btnFind, 0, wxALL, 5 );


	bxMain->Add( bxButton, 0, wxALIGN_RIGHT|wxTOP, 5 );


	this->SetSizer( bxMain );
	this->Layout();

	this->Centre( wxBOTH );

	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( CDlgNewJob::OnInitDialog ) );
	m_txtOpen->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CDlgNewJob::OnTextEnter ), NULL, this );

	// Connect Events
	m_btnConfirm->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CDlgNewJob::OnBtnConfirm ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CDlgNewJob::OnBtnCancel ), NULL, this );
	m_btnFind->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CDlgNewJob::OnBtnFind ), NULL, this );
}

CDlgNewJob::~CDlgNewJob()
{
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( CDlgNewJob::OnInitDialog ) );
	m_txtOpen->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CDlgNewJob::OnTextEnter ), NULL, this );

	// Disconnect Events
	m_btnConfirm->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CDlgNewJob::OnBtnConfirm ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CDlgNewJob::OnBtnCancel ), NULL, this );
	m_btnFind->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CDlgNewJob::OnBtnFind ), NULL, this );
}

void CDlgNewJob::OnInitDialog( wxInitDialogEvent& event )
{
//	m_btnConfirm->Enable(false);
}

void CDlgNewJob::OnBtnConfirm( wxCommandEvent& event )
{
	DoExecNewJob();
}

void CDlgNewJob::OnBtnCancel( wxCommandEvent& event )
{
	EndDialog(wxID_CLOSE);
}

void CDlgNewJob::OnBtnFind( wxCommandEvent& event )
{

}

void CDlgNewJob::OnTextEnter( wxCommandEvent& event )
{
	DoExecNewJob();
}

void CDlgNewJob::DoExecNewJob()
{
	wxString strOpen = m_txtOpen->GetLineText(0);
	if(strOpen.IsEmpty())
	{
		wxMessageBox(wxT("실행할 작업을 입력하세요!"), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		m_txtOpen->SetFocus();
		return;
	}

	wxExecute(strOpen);
	EndDialog(wxID_CLOSE);
}
