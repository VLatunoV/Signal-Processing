#include "audio_layer.h"
#include <iostream>

namespace AudioLayer
{
	bool Initialize()
	{
		CoInitialize(0);
		CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL,
			CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
			(void**)&pIDeviceEnumerator);
		return true;
	}
	void Cleanup()
	{
		if (pIDeviceCollection != nullptr)
		{
			pIDeviceCollection->Release();
			pIDeviceCollection = nullptr;
		}
		if (pIDeviceEnumerator != nullptr)
		{
			pIDeviceEnumerator->Release();
			pIDeviceEnumerator = nullptr;
		}
	}
	AudioEndpoint::~AudioEndpoint()
	{
		if (pIDevice != nullptr)
		{
			pIDevice->Release();
			pIDevice = nullptr;
		}
		if (pIAudioClient != nullptr)
		{
			pIAudioClient->Release();
			pIAudioClient = nullptr;
		}
		PropVariantClear(&this->pv);
	}
	CaptureEndpoint* GetDefaultCaptureEndpoint()
	{
		if (pIDeviceEnumerator == nullptr)
			return nullptr;
		CaptureEndpoint* result = new CaptureEndpoint();
		PropVariantInit(&result->pv);
		pIDeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &result->pIDevice);
		result->pIDevice->OpenPropertyStore(STGM_READ, &result->pIPropStore);
		result->pIDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&result->pIAudioClient);
		result->pIAudioClient->GetMixFormat(&result->pWF);
		return result;
	}
	RenderEndpoint* GetDefaultRenderEndpoint()
	{
		if (pIDeviceEnumerator == nullptr)
			return nullptr;
		RenderEndpoint* result = new RenderEndpoint();
		PropVariantInit(&result->pv);
		pIDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eCommunications, &result->pIDevice);
		result->pIDevice->OpenPropertyStore(STGM_READ, &result->pIPropStore);
		result->pIDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&result->pIAudioClient);
		result->pIAudioClient->GetMixFormat(&result->pWF);
		return result;
	}
	const WAVEFORMATEX* AudioEndpoint::GetWaveFormat() const
	{
		return pWF;
	}
	void AudioEndpoint::SetWaveFormat(const WAVEFORMATEX* WF)
	{
		*this->pWF = *WF;
	}
	const LPWSTR AudioEndpoint::GetName()
	{
		this->pIPropStore->GetValue(PKEY_Device_FriendlyName, &this->pv);
		return this->pv.pwszVal;
	}
	void AudioEndpoint::Initialize(UINT32 timeIntervalForBuffer)
	{
		if (this->pIDevice == nullptr || this->pIAudioClient == nullptr)
			return;
		this->timeInterval = timeIntervalForBuffer;
		this->pIAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, timeIntervalForBuffer * 10000, 0, this->pWF, NULL);
	}
	void AudioEndpoint::Start()
	{
		this->pIAudioClient->Start();
	}
	void AudioEndpoint::Stop()
	{
		this->pIAudioClient->Stop();
	}
	void AudioEndpoint::Reset()
	{
		this->pIAudioClient->Reset();
	}

	CaptureEndpoint::~CaptureEndpoint()
	{
		if (pICaptureClient != nullptr)
		{
			pICaptureClient->Release();
			pICaptureClient = nullptr;
		}
	}
	void CaptureEndpoint::Initialize(UINT32 timeInterval)
	{
		AudioEndpoint::Initialize(timeInterval);
		this->pIAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&this->pICaptureClient);
	}
	void CaptureEndpoint::Capture(UINT32 milliseconds, BYTE* outBuffer)
	{
		BYTE* pBegin;
		BYTE* readFrom;
		UINT32 framesToRead = milliseconds * (pWF->nAvgBytesPerSec / 1000) / pWF->nBlockAlign;
		UINT32 framesAvailable;
		UINT32 framesRequest;
		DWORD flags;
		
		pBegin = outBuffer;

		while (framesToRead)
		{
			pICaptureClient->GetBuffer(&readFrom, &framesAvailable, &flags, NULL, NULL);
			framesRequest = min(framesAvailable, framesToRead);
			memcpy(pBegin, readFrom, framesRequest * pWF->nBlockAlign);
			pBegin += framesRequest * pWF->nBlockAlign;
			pICaptureClient->ReleaseBuffer(framesAvailable);
			framesToRead -= framesRequest;
			if (framesToRead)
			{
				Sleep(framesRequest * pWF->nBlockAlign * 1000 / pWF->nAvgBytesPerSec);
			}

		}
	}

	RenderEndpoint::~RenderEndpoint()
	{
		if (pIRenderClient != nullptr)
		{
			pIRenderClient->Release();
			pIRenderClient = nullptr;
		}
	}
	void RenderEndpoint::Initialize(UINT32 timeInterval)
	{
		AudioEndpoint::Initialize(timeInterval);
		this->pIAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&this->pIRenderClient);
	}
	void RenderEndpoint::Render(const BYTE* data, UINT32 size)
	{
		const BYTE* pBegin = data;
		BYTE* writeTo;
		UINT32 framesToWrite = size / pWF->nBlockAlign;
		UINT32 framesAvailable;
		UINT32 framesRequest;
		UINT32 bufferSize;
		pIAudioClient->GetBufferSize(&bufferSize);
		while (framesToWrite)
		{
			pIAudioClient->GetCurrentPadding(&framesAvailable);
			framesAvailable = bufferSize - framesAvailable;
			framesRequest = min(framesAvailable, framesToWrite);
			pIRenderClient->GetBuffer(framesRequest, &writeTo);
			memcpy(writeTo, pBegin, framesRequest * pWF->nBlockAlign);
			pBegin += framesRequest * pWF->nBlockAlign;
			pIRenderClient->ReleaseBuffer(framesRequest, 0);
			framesToWrite -= framesRequest;
			if (framesToWrite)
			{
				Sleep(framesRequest * pWF->nBlockAlign * 1000 / pWF->nAvgBytesPerSec);
			}
		}
	}

	void PrintWaveFormatInfo(const WAVEFORMATEX* wf)
	{
		printf("Sample rate: %u Hz\n", wf->nSamplesPerSec);
		printf("Sample size: %u bits\n", wf->wBitsPerSample);
		printf("Size of audio frame: %u bytes\n", wf->nBlockAlign);
		printf("Number of channels: %u\n", wf->nChannels);
		printf("Average bytes per second: %u\n", wf->nAvgBytesPerSec);
	}
}
