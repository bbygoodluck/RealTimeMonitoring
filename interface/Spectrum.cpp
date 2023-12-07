#include "../ginc.h"
#include "SpectrumImpl.h"
#include "Spectrum.h"

CSpectrum::CSpectrum()
	: m_pSpectrumImpl(new CSpectrumImpl())
{
}

CSpectrum::~CSpectrum()
{

}

void CSpectrum::SetSpectrumRect(const wxRect& rc)
{
	m_pSpectrumImpl->m_Rect = rc;
	m_pSpectrumImpl->MakeSpectrum();
}

void CSpectrum::AddSpectrum(int iCount)
{
	m_pSpectrumImpl->AddSpectrum(iCount);
}

SPECTRUM_INFO* CSpectrum::GetItemPtr(int iIndex)
{
	return m_pSpectrumImpl->m_lstSpectrum.at(iIndex);
}

wxVector<SPECTRUM_INFO *>::const_iterator CSpectrum::GetItemIter(int iIndex)
{
	return (m_pSpectrumImpl->m_lstSpectrum.begin() + iIndex);
}

size_t CSpectrum::GetSize()
{
	return m_pSpectrumImpl->m_lstSpectrum.size();
}