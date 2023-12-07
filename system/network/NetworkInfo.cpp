#include "../../ginc.h"
#include "NetworkInfo.h"

wxBEGIN_EVENT_TABLE(CNetworkInfo, CResourceCommon)
wxEND_EVENT_TABLE()

CNetworkInfo::CNetworkInfo()
{
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CNetworkInfo::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);

	Initialize();
}

CNetworkInfo::~CNetworkInfo()
{
	CResourceCommon::Clear();
	CResourceCommon::Stop();

	this->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CNetworkInfo::OnTimer));
}

void CNetworkInfo::Initialize()
{
	InitAdapterInfo();
	Update();
}

void CNetworkInfo::InitAdapterInfo()
{
#ifdef __WXMSW__
	ULONG            ulOutBufLen;
	//네트워크 어뎁터의 사이즈를 읽어옴
	if (GetAdaptersInfo(NULL, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		//네트워크 어뎁터 정보를 읽을 버퍼를 생성한다.
		char *pAdapterBuffer = new char[ulOutBufLen];
		IP_ADAPTER_INFO* pAdapterInfo;
		//어뎁터 정보를 읽어옴
		if (GetAdaptersInfo((IP_ADAPTER_INFO *)pAdapterBuffer, &ulOutBufLen) != ERROR_SUCCESS)
			return;

		int iAdapterIndex = 0;
		pAdapterInfo = (IP_ADAPTER_INFO *) pAdapterBuffer;

		m_ItemIndex = 0;
		int iIndex = 0;
		while (pAdapterInfo)
		{
			NET_ADAPTER_INFO adapterInfo;

			adapterInfo._adapterName  = pAdapterInfo->AdapterName;
			adapterInfo._description  = pAdapterInfo->Description;
			adapterInfo._ip           = pAdapterInfo->IpAddressList.IpAddress.String;
			adapterInfo._adapterIndex = pAdapterInfo->Index;
			adapterInfo._dhcpEnable   = pAdapterInfo->DhcpEnabled;

			m_adapters.push_back(adapterInfo);

			strNetAdapterName_ = pAdapterInfo->AdapterName;
			strNetDescription_ = pAdapterInfo->Description;
			strNetIP_ = pAdapterInfo->IpAddressList.IpAddress.String;

			//ip가 0.0.0.0 인지 체크
			int chkIPAddr = strNetIP_.CmpNoCase(wxT("0.0.0.0"));
			//VMWare 가 설치되어 있는경우는 VMWare Adapter 가 설치되므로 패스..
			int fndIndex  = strNetDescription_.Find(wxT("VMware "));

			if(chkIPAddr != 0 && fndIndex == -1)
			{
				iAdapterIndex = pAdapterInfo->Index;
				m_ItemIndex = iIndex;
			}

			iIndex++;
			pAdapterInfo = pAdapterInfo->Next;
		}

		m_AdapterIndx = iAdapterIndex;
		delete [] pAdapterBuffer;
	}
#endif
}

void CNetworkInfo::OnTimer(wxTimerEvent& event)
{
	Update();
	SendEvent(wxEVT_RESOURCE_MONITORING);
}

void CNetworkInfo::Update()
{
#ifdef __WXMSW__
	MIB_IFROW ifrow;
	wxZeroMemory(ifrow);

	ifrow.dwIndex = m_AdapterIndx;
	if(GetIfEntry(&ifrow) != NO_ERROR)
		return;

	dwcurrent_sent_ = ifrow.dwOutOctets - dwlast_sent_;
	dwcurrent_recv_ = ifrow.dwInOctets - dwlast_recv_;

	dwlast_sent_ = ifrow.dwOutOctets;
	dwlast_recv_ = ifrow.dwInOctets;

	float diftime = (GetTickCount() - dw_refreshtime_);

	dblrecvspeed_ = dwcurrent_recv_ / diftime;
	dblsentspeed_ = dwcurrent_sent_ / diftime;

	dw_refreshtime_ = GetTickCount();
#else

#endif
}

void CNetworkInfo::SetAdapterIndex(int itemIndex)
{
	if(m_timer.IsRunning())
		m_timer.Stop();

	wxVector<NET_ADAPTER_INFO>::const_iterator cIt = m_adapters.begin() + itemIndex;

	NET_ADAPTER_INFO netInfo = *cIt;
	m_AdapterIndx = netInfo._adapterIndex;

	m_timer.Start(1000);
}
