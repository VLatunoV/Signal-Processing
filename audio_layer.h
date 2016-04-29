/*************************
*  Valentin Latunov 2016 *
*      Audio Layer       *
*************************/
#pragma once

#include <stdio.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <functiondiscoverykeys_devpkey.h>

/*
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IDD_IAudioClient = __uuidof(IAudioClient);
*/

namespace AudioLayer
{
	class AudioEndpoint;
	class CaptureEndpoint;
	class RenderEndpoint;

	static IMMDeviceEnumerator* pIDeviceEnumerator = nullptr;
	static IMMDeviceCollection* pIDeviceCollection = nullptr;
	CaptureEndpoint* GetDefaultCaptureEndpoint();
	RenderEndpoint* GetDefaultRenderEndpoint();
	bool Initialize();
	void Cleanup();

	class AudioEndpoint
	{
	protected:
		IMMDevice* pIDevice;
		IAudioClient* pIAudioClient;
		IPropertyStore* pIPropStore;
		WAVEFORMATEX* pWF;
		UINT32 timeInterval;
		PROPVARIANT pv;

		AudioEndpoint(const AudioEndpoint&);
		AudioEndpoint& operator= (const AudioEndpoint&);
		void Initialize(UINT32);
	public:
		AudioEndpoint() : pIDevice(nullptr), pIAudioClient(nullptr), pIPropStore(nullptr), pWF(nullptr), timeInterval(0) {}
		~AudioEndpoint();

		const WAVEFORMATEX* GetWaveFormat() const;
		void SetWaveFormat(const WAVEFORMATEX*);
		const LPWSTR GetName();

		void Start();
		void Stop();
		void Reset();
	};

	class CaptureEndpoint : public AudioEndpoint
	{
		IAudioCaptureClient* pICaptureClient;

		friend CaptureEndpoint* GetDefaultCaptureEndpoint();
		CaptureEndpoint(const CaptureEndpoint&);
		CaptureEndpoint& operator= (const CaptureEndpoint&);
	public:
		CaptureEndpoint() : AudioEndpoint(), pICaptureClient(nullptr) {}
		~CaptureEndpoint();
		void Initialize(UINT32);
		void Capture(UINT32, BYTE*);
	};

	class RenderEndpoint : public AudioEndpoint
	{
		IAudioRenderClient* pIRenderClient;

		friend RenderEndpoint* GetDefaultRenderEndpoint();
		RenderEndpoint(const RenderEndpoint&);
		RenderEndpoint& operator= (const RenderEndpoint&);
	public:
		RenderEndpoint() : AudioEndpoint(), pIRenderClient(nullptr) {}
		~RenderEndpoint();
		void Initialize(UINT32);
		void Render(const BYTE*, UINT32);
	};

	void PrintWaveFormatInfo(const WAVEFORMATEX* kasdkadk);
}
