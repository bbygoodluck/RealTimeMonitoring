#include "../ginc.h"
#include "SpectrumImpl.h"

CSpectrumImpl::CSpectrumImpl()
{
	m_lstSpectrum.reserve(100);
}

CSpectrumImpl::~CSpectrumImpl()
{
	Clear();
	wxVector<SPECTRUM_INFO *>().swap(m_lstSpectrum);
	m_lstSpectrum.reserve(0);
}

void CSpectrumImpl::AddSpectrum(int iCount)
{

}

void CSpectrumImpl::MakeSpectrum()
{
	Clear();

	int iXStartPos = 0;
	int iXEndPos = m_Gap;
	int iWidth = 3;
	int iHeight = m_Rect.GetHeight() - 4;
	int iRed = 0;
	int iGreen = 0;
	int iItemCount = 0;

	while (iXStartPos < m_Rect.GetRight())
	{
		if (iItemCount == 100)
			break;

		iXStartPos = iXEndPos;
		wxRect rc(iXStartPos, 2, iWidth, iHeight);

		iRed = (2 * iItemCount) + 55;
		iGreen = 255 - (iItemCount * 3);
		iGreen = iGreen < 0 ? 0 : iGreen;
		wxColour col(iRed, iGreen, 0);

		SPECTRUM_INFO* pSpectrumInfo = new SPECTRUM_INFO();
		pSpectrumInfo->m_rc = rc;
		pSpectrumInfo->m_Colour = col;

		m_lstSpectrum.push_back(pSpectrumInfo);
		iXEndPos = rc.GetRight() + m_Gap;

		iItemCount++;
	}

	m_Size = m_lstSpectrum.size();
}

void CSpectrumImpl::Clear()
{
	for (auto item : m_lstSpectrum)
	{
		SPECTRUM_INFO * stInfo = item;
		delete stInfo;

		stInfo = nullptr;
	}

	m_lstSpectrum.clear();
}