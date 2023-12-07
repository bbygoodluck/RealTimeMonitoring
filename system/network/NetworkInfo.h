#ifndef NETWORKINFO_H_INCLUDED
#define NETWORKINFO_H_INCLUDED

#include "../ResourceCommon.h"

class CNetworkInfo : public CResourceCommon
{
public:
	CNetworkInfo();
	~CNetworkInfo();

public:
	unsigned long GetInBytes() { return dwcurrent_recv_; }
	unsigned long GetOutBytes() { return dwcurrent_sent_; }

	double GetDownSpeed() { return dblrecvspeed_; }
	double GetUpSpeed() { return dblsentspeed_; }

	wxString GetAdapterName() const { return strNetAdapterName_; }
	wxString GetDescription() const { return strNetDescription_; }
	wxString GetIP() const { return strNetIP_; }

	wxVector<NET_ADAPTER_INFO>* GetNetAdaptersInfo() {
		return &m_adapters;
	}

	int GetAdapterIndex() const { return m_ItemIndex;}
	void SetAdapterIndex(int itemIndex);

private:
	void Initialize() override;
	void Update() override;
	void InitAdapterInfo();

private:
	wxString strNetAdapterName_ = wxT("");
	wxString strNetDescription_ = wxT("");
	wxString strNetIP_          = wxT("");
	bool bSetNetInfo_           = false;

	wxVector<NET_ADAPTER_INFO> m_adapters;

#ifdef __WXMSW__
	DWORD dwcurrent_sent_ = 0;
	DWORD dwcurrent_recv_ = 0;
	DWORD dwlast_sent_ = 0;
	DWORD dwlast_recv_ = 0;

	DWORD dw_refreshtime_ = 0;
	double dblrecvspeed_ = 0;
	double dblsentspeed_ = 0;

	unsigned long m_AdapterIndx = 0;
	//wxVector<NET_ADAPTER_INFO> 인덱스
	int m_ItemIndex = 0;
#else
#endif
private:
	virtual void OnTimer(wxTimerEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};

#endif // NETWORKINFO_H_INCLUDED
