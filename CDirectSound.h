#pragma once

#include <mmsystem.h>
#include <dsound.h>
#include "Wav/WaveFile.h"

#define MAX_AUDIO_BUF 4
#define BUFFERNOTIFYSIZE 8192

class CDirectSound {
public:
	LPDIRECTSOUND8 m_ppDS;
	WAVEFORMATEX   m_wfx;
	DSBUFFERDESC   m_dsbdesc;
	BYTE*          m_pBuffer;
	CWaveFile*     m_wav;
	LPDIRECTSOUNDBUFFER m_lpdsbBuff;
	IDirectSoundBuffer8* m_pDSBuffer8;
	IDirectSoundNotify8* m_pDSNotify;

	DSBPOSITIONNOTIFY m_pDSPosNotify[MAX_AUDIO_BUF];
	HANDLE m_event[MAX_AUDIO_BUF];
public:
	CDirectSound(HWND hwnd);
};