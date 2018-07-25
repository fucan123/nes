#include "stdafx.h"
#include "CDirectSound.h"
#include "NES/NES.h"
#include "NES/APU.h"

CDirectSound::CDirectSound(HWND hwnd) {

}

CDirectSound::CDirectSound(HWND hwnd, INT f) {
	HRESULT hr;
	hr = DirectSoundCreate8(NULL, &m_ppDS, NULL);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"创建失败.", L"t", MB_OK);
	}
	hr = m_ppDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"设置级别失败.", L"t", MB_OK);
	}

	m_wav = new CWaveFile;
	hr = m_wav->Open(L"Wav/m.wav", NULL, WAVEFILE_READ);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"打开WAV文件失败.", L"t", MB_OK);
	}

	DWORD dwSize = m_wav->GetSize();
	DWORD dwSizeRead = 0;
	m_pBuffer = new BYTE[dwSize];
	hr = m_wav->Read(m_pBuffer, dwSize, &dwSizeRead);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"读取WAV文件失败.", L"t", MB_OK);
	}
	/*CString readSize;
	readSize.Format(L"读取大小:%d, hr:%d", dwSizeRead, hr);
	//::MessageBox(NULL, readSize, L"T", MB_OK);*/
	if (dwSizeRead > 0) {

		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_wfx.nChannels = 1;
		m_wfx.nSamplesPerSec = 44100;
		m_wfx.wBitsPerSample = 8;
		m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
		m_wfx.nAvgBytesPerSec = m_wfx.nBlockAlign * m_wfx.nSamplesPerSec;
		m_wfx.cbSize = 0;

		memset(&m_dsbdesc, 0, sizeof(DSBUFFERDESC));
		m_dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		m_dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;
		/**DSBCAPS_GLOBALFOCUS               //设置主播
		| DSBCAPS_CTRLFX
		| DSBCAPS_CTRLPOSITIONNOTIFY
		| DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;*/
		m_dsbdesc.dwBufferBytes = MAX_AUDIO_BUF * BUFFERNOTIFYSIZE; dwSizeRead;
		m_dsbdesc.lpwfxFormat = m_wav->m_pwfx;

		hr = m_ppDS->CreateSoundBuffer(&m_dsbdesc, &m_lpdsbBuff, NULL);

		if (!FAILED(hr)) {
			hr = m_lpdsbBuff->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8);

			if (FAILED(hr)) {
				::MessageBox(NULL, L"创建副缓冲区失败.", L"T", MB_OK);
			}
			if (FAILED(m_lpdsbBuff->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify))) {
				::MessageBox(NULL, L"创建通知失败.", L"T", MB_OK);
			}
			
			for (int i = 0; i<MAX_AUDIO_BUF; i++) {
				m_pDSPosNotify[i].dwOffset = i * BUFFERNOTIFYSIZE;
				m_event[i] = ::CreateEvent(NULL, false, false, NULL);
				m_pDSPosNotify[i].hEventNotify = m_event[i];
			}
			/*
			m_pDSPosNotify[0].dwOffset = 882;
			m_event[0] = ::CreateEvent(NULL, false, false, NULL);
			m_pDSPosNotify[0].hEventNotify = m_event[0];
			m_pDSNotify->SetNotificationPositions(0, m_pDSPosNotify); */

			m_pDSBuffer8->SetCurrentPosition(0);
			m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);

			BOOL isPlaying = TRUE;
			LPVOID buf = NULL, buf2 = NULL;
			DWORD  buf_len = 0, buf2_len = 0;
			DWORD res = WAIT_OBJECT_0;
			DWORD offset = 0; BUFFERNOTIFYSIZE;
			DWORD pset = 0;

			while (isPlaying) {
				if ((res >= WAIT_OBJECT_0) && (res <= WAIT_OBJECT_0 + 3)) {
					m_pDSBuffer8->Lock(offset, BUFFERNOTIFYSIZE, &buf, &buf_len, NULL, NULL, 0);
					memcpy(buf, m_pBuffer + pset, buf_len);
					m_pDSBuffer8->Unlock(buf, buf_len, NULL, 0);
					offset += buf_len;
					DWORD old = offset;
					offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
					pset += buf_len;
					CString str;
					str.Format(L"old:%d, offset:%d, buf len:%d, buf2 len:%d", old, offset, buf_len, buf2_len);
					::MessageBox(NULL, str, L"Notify", MB_OK);
					//offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
				}
				res = WaitForMultipleObjects(MAX_AUDIO_BUF, m_event, FALSE, INFINITE);
			}
			/*
			WAIT_OBJECT_0;
		
			LPVOID lpvWrite;
			DWORD dwLength;
			
			if (DS_OK == m_lpdsbBuff->Lock(
				0, // Offset at which to start lock.
				0, // Size of lock; ignored because of flag.
				&lpvWrite, // Gets address of first part of lock.
				&dwLength, // Gets size of first part of lock.
				NULL, // Address of wraparound not needed. 
				NULL, // Size of wraparound not needed.
				DSBLOCK_ENTIREBUFFER)) // Flag.
			{

				memcpy(lpvWrite, m_pBuffer, dwLength);
				m_lpdsbBuff->Unlock(
					lpvWrite, // Address of lock start.
					dwLength, // Size of lock.
					NULL, // No wraparound portion.
					0); // No wraparound size.

				m_lpdsbBuff->SetCurrentPosition(0);
				m_lpdsbBuff->Play(0, 0, DSBPLAY_LOOPING);
			}*/
		}
		else {
			::MessageBox(NULL, L"ERROR.", L"T", MB_OK);
		}
	}
}

void CDirectSound::CDirectSoundOld(HWND hwnd) {
	HRESULT hr;
	hr = DirectSoundCreate8(NULL, &m_ppDS, NULL);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"创建失败.", L"t", MB_OK);
	}
	hr = m_ppDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"设置级别失败.", L"t", MB_OK);
	}

	m_wav = new CWaveFile;
	hr = m_wav->Open(L"Wav/m.wav", NULL, WAVEFILE_READ);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"打开WAV文件失败.", L"t", MB_OK);
	}

	DWORD dwSize = m_wav->GetSize();
	DWORD dwSizeRead = 0;
	m_pBuffer = new BYTE[dwSize];
	hr = m_wav->Read(m_pBuffer, dwSize, &dwSizeRead);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"读取WAV文件失败.", L"t", MB_OK);
	}
	/*CString readSize;
	readSize.Format(L"读取大小:%d, hr:%d", dwSizeRead, hr);
	//::MessageBox(NULL, readSize, L"T", MB_OK);*/
	if (dwSizeRead > 0) {

		memset(&m_dsbdesc, 0, sizeof(DSBUFFERDESC));
		m_dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		m_dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;
		/**DSBCAPS_GLOBALFOCUS               //设置主播
		| DSBCAPS_CTRLFX
		| DSBCAPS_CTRLPOSITIONNOTIFY
		| DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;*/
		m_dsbdesc.dwBufferBytes = MAX_AUDIO_BUF * BUFFERNOTIFYSIZE; dwSizeRead;
		m_dsbdesc.lpwfxFormat = m_wav->m_pwfx;

		hr = m_ppDS->CreateSoundBuffer(&m_dsbdesc, &m_lpdsbBuff, NULL);

		if (!FAILED(hr)) {
			hr = m_lpdsbBuff->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8);

			if (FAILED(hr)) {
				::MessageBox(NULL, L"创建副缓冲区失败.", L"T", MB_OK);
			}
			if (FAILED(m_lpdsbBuff->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify))) {
				::MessageBox(NULL, L"创建通知失败.", L"T", MB_OK);
			}

			for (int i = 0; i<MAX_AUDIO_BUF; i++) {
				m_pDSPosNotify[i].dwOffset = i * BUFFERNOTIFYSIZE;
				m_event[i] = ::CreateEvent(NULL, false, false, NULL);
				m_pDSPosNotify[i].hEventNotify = m_event[i];
			}
			m_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, m_pDSPosNotify);

			//m_pDSBuffer8->SetCurrentPosition(0);
			//m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);

			BOOL isPlaying = TRUE;
			LPVOID buf = NULL, buf2 = NULL;
			DWORD  buf_len = 0, buf2_len = 0;
			DWORD res = WAIT_OBJECT_0;
			DWORD offset = 0; BUFFERNOTIFYSIZE;
			DWORD pset = 0;

			while (isPlaying) {
				if ((res >= WAIT_OBJECT_0) && (res <= WAIT_OBJECT_0 + 3)) {
					m_pDSBuffer8->Lock(offset, BUFFERNOTIFYSIZE, &buf, &buf_len, NULL, NULL, 0);
					memcpy(buf, m_pBuffer + pset, buf_len);
					m_pDSBuffer8->Unlock(buf, buf_len, NULL, 0);
					offset += buf_len;
					DWORD old = offset;
					offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
					pset += buf_len;
					CString str;
					str.Format(L"old:%d, offset:%d, buf len:%d, buf2 len:%d", old, offset, buf_len, buf2_len);
					//::MessageBox(NULL, str, L"Notify", MB_OK);
					//offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
				}
				res = WaitForMultipleObjects(MAX_AUDIO_BUF, m_event, FALSE, INFINITE);
			}
			/*
			WAIT_OBJECT_0;

			LPVOID lpvWrite;
			DWORD dwLength;

			if (DS_OK == m_lpdsbBuff->Lock(
			0, // Offset at which to start lock.
			0, // Size of lock; ignored because of flag.
			&lpvWrite, // Gets address of first part of lock.
			&dwLength, // Gets size of first part of lock.
			NULL, // Address of wraparound not needed.
			NULL, // Size of wraparound not needed.
			DSBLOCK_ENTIREBUFFER)) // Flag.
			{

			memcpy(lpvWrite, m_pBuffer, dwLength);
			m_lpdsbBuff->Unlock(
			lpvWrite, // Address of lock start.
			dwLength, // Size of lock.
			NULL, // No wraparound portion.
			0); // No wraparound size.

			m_lpdsbBuff->SetCurrentPosition(0);
			m_lpdsbBuff->Play(0, 0, DSBPLAY_LOOPING);
			}*/
		}
		else {
			::MessageBox(NULL, L"ERROR.", L"T", MB_OK);
		}
	}
}

void CDirectSound::Play(HWND hwnd, NES* nes) {
	HRESULT hr;
	hr = DirectSoundCreate8(NULL, &m_ppDS, NULL);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"创建失败.", L"t", MB_OK);
	}
	hr = m_ppDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"设置级别失败.", L"t", MB_OK);
	}

	m_wav = new CWaveFile;
	hr = m_wav->Open(L"Wav/m.wav", NULL, WAVEFILE_READ);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"打开WAV文件失败.", L"t", MB_OK);
	}

	DWORD dwSize = m_wav->GetSize();
	DWORD dwSizeRead = 0;
	m_pBuffer = new BYTE[dwSize];
	hr = m_wav->Read(m_pBuffer, dwSize, &dwSizeRead);
	if (FAILED(hr)) {
		::MessageBox(NULL, L"读取WAV文件失败.", L"t", MB_OK);
	}
	/*CString readSize;
	readSize.Format(L"读取大小:%d, hr:%d", dwSizeRead, hr);
	//::MessageBox(NULL, readSize, L"T", MB_OK);*/
	if (dwSizeRead > 0) {

		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_wfx.nChannels = 1;
		m_wfx.nSamplesPerSec = 44100;
		m_wfx.wBitsPerSample = 16;
		m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
		m_wfx.nAvgBytesPerSec = m_wfx.nBlockAlign * m_wfx.nSamplesPerSec;
		m_wfx.cbSize = 0;

		memset(&m_dsbdesc, 0, sizeof(DSBUFFERDESC));
		m_dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		m_dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;
		/**DSBCAPS_GLOBALFOCUS               //设置主播
		| DSBCAPS_CTRLFX
		| DSBCAPS_CTRLPOSITIONNOTIFY
		| DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;*/
		m_dsbdesc.dwBufferBytes = MAX_AUDIO_BUF * BUFFERNOTIFYSIZE; dwSizeRead;
		m_dsbdesc.lpwfxFormat = &m_wfx;

		hr = m_ppDS->CreateSoundBuffer(&m_dsbdesc, &m_lpdsbBuff, NULL);

		if (!FAILED(hr)) {
			hr = m_lpdsbBuff->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8);

			if (FAILED(hr)) {
				::MessageBox(NULL, L"创建副缓冲区失败.", L"T", MB_OK);
			}
			if (FAILED(m_lpdsbBuff->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify))) {
				::MessageBox(NULL, L"创建通知失败.", L"T", MB_OK);
			}

			for (int i = 0; i<MAX_AUDIO_BUF; i++) {
				m_pDSPosNotify[i].dwOffset = i * BUFFERNOTIFYSIZE;
				m_event[i] = ::CreateEvent(NULL, false, false, NULL);
				m_pDSPosNotify[i].hEventNotify = m_event[i];
			}
			m_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, m_pDSPosNotify);

			m_pDSBuffer8->SetCurrentPosition(0);
			m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);

			BOOL isPlaying = TRUE;
			LPVOID buf = NULL, buf2 = NULL;
			DWORD  buf_len = 0, buf2_len = 0;
			DWORD res = WAIT_OBJECT_0;
			DWORD offset = 0; BUFFERNOTIFYSIZE;
			DWORD pset = 0;

			while (isPlaying) {
				if ((res >= WAIT_OBJECT_0) && (res <= WAIT_OBJECT_0 + 3)) {
					m_pDSBuffer8->Lock(offset, BUFFERNOTIFYSIZE, &buf, &buf_len, NULL, NULL, 0);
					memcpy(buf,nes->apu->sound_data, buf_len);
					m_pDSBuffer8->Unlock(buf, buf_len, NULL, 0);
					offset += buf_len;
					DWORD old = offset;
					offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
					pset += buf_len;
					CString str;
					str.Format(L"old:%d, offset:%d, buf len:%d, buf2 len:%d", old, offset, buf_len, buf2_len);
					//::MessageBox(NULL, str, L"Notify", MB_OK);
					//offset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);
				}
				res = WaitForMultipleObjects(MAX_AUDIO_BUF, m_event, FALSE, INFINITE);
			}
		}
		else {
			::MessageBox(NULL, L"ERROR.", L"T", MB_OK);
		}
	}
}
