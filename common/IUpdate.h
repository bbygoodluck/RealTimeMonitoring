#ifndef INFOUPDATELISTENER_H_INCLUDED
#define INFOUPDATELISTENER_H_INCLUDED

class IUpdate
{
public:
	virtual void UpdateData(wxCommandEvent& event) = 0;

	void RefreshWindow(wxWindow* pWindow, const wxRect& rect, bool bUpdate = false)
	{
#ifdef __WXMSW__
		HWND hWnd = pWindow->GetHWND();
		if (rect.GetWidth() != 0)
		{
			RECT mswRect;
			const RECT* pRect;

			wxCopyRectToRECT(rect, mswRect);
			pRect = &mswRect;

			//	INT flags = RDW_INVALIDATE | RDW_NOCHILDREN | RDW_UPDATENOW | RDW_ERASE;
			//	::RedrawWindow(hWnd, pRect, NULL, flags);
			::InvalidateRect(hWnd, pRect, false);
		}
		else
			pWindow->Refresh(false);

		if (bUpdate)
			pWindow->Update();
#else
		pWindow->RefreshRect(rect, false);
		pWindow->Update();
#endif
	}
};

#endif // INFOUPDATELISTENER_H_INCLUDED
