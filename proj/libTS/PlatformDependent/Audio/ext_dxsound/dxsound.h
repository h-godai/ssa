// dxsound.h     direct sound class -*-C++-*-

#ifndef __DXSOUND_H__
#define __DXSOUND_H__

#include "wavread.h"

#define DXSND_3D 0
#define DXSND_2D 1

// DirectSoundバッファ

class CDXSoundBuffer
{
  LPDIRECTSOUND m_pDirectSound; // DirectSoundデバイス
  char* m_LastFn;	// ファイル名
  int m_BufferCnt;	// 同時発生数
  int m_Cur;
  int m_Freq;
  int m_Mode;
public:
  vector<LPDIRECTSOUNDBUFFER> m_pBuffers;
  vector<LPDIRECTSOUND3DBUFFER> m_p3DBuffers;

  CDXSoundBuffer(LPDIRECTSOUND pds=NULL, char* fn=NULL, int n = 2, int mode=DXSND_3D)
  : m_pDirectSound(pds),
    m_LastFn(NULL),
    m_BufferCnt(n),
    m_Cur(0),
    m_Freq(0),
    m_Mode(mode)
  {
    if(fn)
      {
	Load(fn, mode, true);
      }
  }
  CDXSoundBuffer(const CDXSoundBuffer& d)
  : m_pDirectSound(d.m_pDirectSound),
    m_LastFn(d.m_LastFn),
    m_BufferCnt(d.m_BufferCnt),
    m_Cur(d.m_Cur),
    m_Freq(d.m_Freq),
    m_Mode(d.m_Mode)
  {
    m_pBuffers.clear();
    m_p3DBuffers.clear();
    for(unsigned int n = 0; n < d.m_pBuffers.size(); ++n)
      {
	m_pBuffers.push_back(d.m_pBuffers[n]);
	if(m_Mode == DXSND_3D)
	  m_p3DBuffers.push_back(d.m_p3DBuffers[n]);
      }
  }

  ~CDXSoundBuffer()
  {
  }
  void Release()
  {
    for(unsigned int n = 0; n < m_pBuffers.size(); ++n)
      {
	m_pBuffers[n]->Release();
	if(m_Mode == DXSND_3D)
	  m_p3DBuffers[n]->Release();
      }
  }


  void Play(int freq=0, int vrm=0, int loop=0)
  {
    if(m_pBuffers.size() == 0) return;
    m_pBuffers[m_Cur]->Stop();
    m_pBuffers[m_Cur]->SetCurrentPosition(0);

    if(freq != 0 || m_Freq != 0)
      { // 再生周波数の設定
	if(freq == 0) freq = m_Freq;
	m_pBuffers[m_Cur]->SetFrequency(freq/2);
      }
    if(vrm != 0)
      {
	m_pBuffers[m_Cur]->SetVolume(vrm);
      }

    DWORD priolity = 0;
    DWORD flags =  DSBPLAY_TERMINATEBY_TIME;
    if(loop) flags |= DSBPLAY_LOOPING;
    HRESULT hr =  m_pBuffers[m_Cur]->Play(0,priolity, flags);
    if(hr == DSERR_BUFFERLOST)
      {
	Load(m_LastFn, m_Mode, true);
      }
    m_Cur = (m_Cur + 1) % m_pBuffers.size();
  }

  void SetMinMax(float mn, float mx)
  {
    if(m_p3DBuffers.size() == 0) return;
    LPDIRECTSOUND3DBUFFER p3DBuffer = m_p3DBuffers[m_Cur];
    p3DBuffer->SetMinDistance(mn, DS3D_IMMEDIATE);
    p3DBuffer->SetMaxDistance(mx, DS3D_IMMEDIATE);
  }

  void SetVolume(int vol)
  {
    vector<LPDIRECTSOUNDBUFFER>::iterator m;
    for(m = m_pBuffers.begin(); m != m_pBuffers.end(); ++m)
      (*m)->SetVolume(vol);
  }

  void SetPos(C3DVector& pos, C3DVector& vel)
  {
    if(m_p3DBuffers.size() == 0) return;
    LPDIRECTSOUND3DBUFFER p3DBuffer = m_p3DBuffers[m_Cur];
    p3DBuffer->SetPosition(pos.x, pos.y, pos.z, DS3D_IMMEDIATE);
    p3DBuffer->SetVelocity(vel.x, vel.y, vel.z, DS3D_IMMEDIATE);
  }

  void Play3D(C3DVector& pos, C3DVector& vel, int freq=0, bool loop=false, bool setflag=true)
  {
    if(m_p3DBuffers.size() == 0) return;
    LPDIRECTSOUNDBUFFER pBuffer = m_pBuffers[m_Cur];
    LPDIRECTSOUND3DBUFFER p3DBuffer = m_p3DBuffers[m_Cur];

    pBuffer->Stop();
    pBuffer->SetCurrentPosition(0);
    if(freq)
      {
	pBuffer->SetFrequency(freq);
      }

    if(setflag)
      {
	p3DBuffer->SetPosition(pos.x, pos.y, pos.z, DS3D_IMMEDIATE);
	p3DBuffer->SetVelocity(vel.x, vel.y, vel.z, DS3D_IMMEDIATE);
      }

    // 距離を優先順位とする
//    int l = 10000 - int(pos.Length());
//    if(l < 0) l = 0;
//    DWORD priolity = l;
//    DWORD flags =  DSBPLAY_TERMINATEBY_TIME | DSBPLAY_TERMINATEBY_PRIORITY;
    DWORD priolity = 0;
    DWORD flags =  0; //DSBPLAY_TERMINATEBY_TIME;
    if(loop) flags |= DSBPLAY_LOOPING;
    HRESULT hr =  pBuffer->Play(0,priolity, flags);
    if(hr == DSERR_BUFFERLOST)
      {
	Load(m_LastFn, m_Mode, true);
      }
    m_Cur = (m_Cur + 1) % m_pBuffers.size();
  }


  void Stop(int n=-1)
  {
    if(m_pBuffers.size() == 0) return;
    if(n == -1)
      { // all
	vector<LPDIRECTSOUNDBUFFER>::iterator m;
	for(m = m_pBuffers.begin(); m != m_pBuffers.end(); ++m)
	  (*m)->Stop();
      }
    else
      m_pBuffers[n]->Stop();
  }


  
  bool Load(char* fn, int mode, bool force=false)
  {
    if(force == false && strcmp(m_LastFn, fn) == 0) return true; // 同じファイルは読まない
    m_LastFn = fn;
    if(m_pDirectSound == NULL)
      {
	m_BufferCnt = 0;
	m_Cur = 0;
	return false;
      }

    Release();
    m_pBuffers.clear();
    m_p3DBuffers.clear();


    LPDIRECTSOUNDBUFFER m_pBuffer;
    LPDIRECTSOUND3DBUFFER m_p3DBuffer;

    if(m_LastFn == NULL) return false;

    HRESULT hr;
    CWaveSoundRead* pWSR = new CWaveSoundRead();
    hr = pWSR->Open(fn);
    if(FAILED(hr))
      {
#ifdef _DEBUG
	CString mes;
	mes.Format("can't open %s", fn);
	AfxMessageBox(mes);
#endif
	delete pWSR;
	return false;
    }
    pWSR->Reset();
    UINT datasize = pWSR->m_ckIn.cksize;
    BYTE* wavedata = new BYTE[datasize];
    UINT readsize;
    pWSR->Read(datasize, wavedata, &readsize);

    //セカンダリバッファの作成
    DSBUFFERDESC dsbdesc;
    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCDEFER
      | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
    if(mode == DXSND_3D)
      dsbdesc.dwFlags |= DSBCAPS_CTRL3D;

    dsbdesc.dwBufferBytes = datasize;
    dsbdesc.lpwfxFormat = pWSR->m_pwfx;

    hr = m_pDirectSound->CreateSoundBuffer(&dsbdesc, &m_pBuffer, NULL);
    if(FAILED(hr))
      {
#ifdef _DEBUG
	AfxMessageBox("can't create secondary sound buffer");
#endif
	m_pBuffer = NULL;
	delete [] wavedata;
	pWSR->Close();
	delete pWSR;
	return false;
      }
      
    //バッファに書き込む
    LPVOID pMem1, pMem2;
    DWORD dwSize1, dwSize2;
    hr = m_pBuffer->Lock(0,datasize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);
    if(FAILED(hr))
      {
#ifdef _DEBUG
	AfxMessageBox("can't lock secondary sound buffer");
#endif
	m_pBuffer->Release();
	m_pBuffer = NULL;
	pWSR->Close();
	delete pWSR;
	return false;
      }
      
    CopyMemory(pMem1, wavedata, dwSize1);
    if(dwSize2)
      CopyMemory(pMem2, wavedata+dwSize1, dwSize2);
    m_pBuffer->Unlock(pMem1, dwSize1, pMem2, dwSize2);
    delete [] wavedata;
    
    pWSR->Close();
    delete pWSR;
    
    if(mode == DXSND_3D)
      {
	m_pBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&m_p3DBuffer);
	m_p3DBuffers.push_back(m_p3DBuffer);
      }

    m_pBuffers.push_back(m_pBuffer);


    // コピーバッファを作成
    for(int n = 1; n < m_BufferCnt; ++n)
      {
	LPDIRECTSOUNDBUFFER pNewBuffer;
	if(m_pDirectSound->DuplicateSoundBuffer(m_pBuffer, &pNewBuffer) != S_OK)
	  break;
	m_pBuffers.push_back(pNewBuffer);
	if(mode == DXSND_3D)
	  {
	    pNewBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&m_p3DBuffer);
	    m_p3DBuffers.push_back(m_p3DBuffer);
	  }
      }
    return true;
  }

};

class CDXSound
{
  CDXSoundBuffer m_NullBuffer;

public:
  LPDIRECTSOUND m_pDirectSound;
  LPDIRECTSOUNDBUFFER m_pPrimaryBuffer;  // プライマリバッファ
  LPDIRECTSOUND3DLISTENER m_pListener;
  vector<CDXSoundBuffer> m_SoundList;
  
  CDXSound()
  : m_pDirectSound(NULL),
    m_pPrimaryBuffer(NULL),
    m_pListener(NULL)
  {
  }
  ~CDXSound()
  {
    Release();
  }
  void Release()
  {
    vector<CDXSoundBuffer>::iterator m;
    for(m = m_SoundList.begin(); m != m_SoundList.end(); ++m)
      m->Release();
    
    if(m_pListener)
      {
	m_pListener->Release();
      }

    if(m_pPrimaryBuffer)
      {
	m_pPrimaryBuffer->Release();
      }
    if(m_pDirectSound)
      {
	m_pDirectSound->Release();
      }

    m_SoundList.clear();
    m_pDirectSound = NULL;
    m_pPrimaryBuffer =NULL;
    m_pListener = NULL;

  }
  LPDIRECTSOUND GetDirectSound()
  {
    return m_pDirectSound;
  }

  void Initialize(CWnd* pWnd, DWORD copper=DSSCL_EXCLUSIVE)
  {
    HRESULT hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL);
    if(FAILED(hr))
      {
	m_pDirectSound = NULL;
#ifdef _DEBUG
	AfxMessageBox("no sound");
#endif
	return;
      }
    
    m_pDirectSound->SetCooperativeLevel(pWnd->m_hWnd,  copper);
//    m_pDirectSound->SetCooperativeLevel(pWnd->m_hWnd, DSSCL_PRIORITY);
//    m_pDirectSound->SetCooperativeLevel(pWnd->m_hWnd, DSSCL_NORMAL);

    // プライマリバッファの作成
    if(copper != DSSCL_NORMAL)
    {
      DSBUFFERDESC dsbdesc;
      ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
      dsbdesc.dwSize = sizeof(DSBUFFERDESC);
      dsbdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
      dsbdesc.dwBufferBytes = 0;
      dsbdesc.lpwfxFormat = NULL;
      hr = m_pDirectSound->CreateSoundBuffer(&dsbdesc, &m_pPrimaryBuffer,NULL);
      if(FAILED(hr))
	{
	  m_pPrimaryBuffer = NULL;
#ifdef _DEBUG
	  AfxMessageBox("can't create sound buffer");
	  return;
#endif
	}
//      PCMWAVEFORMAT pcmwf;
      WAVEFORMATEX wf;
      ZeroMemory(&wf, sizeof(PCMWAVEFORMAT));
      wf.wFormatTag = WAVE_FORMAT_PCM;
      wf.nChannels = 2;
      wf.nSamplesPerSec = 22050;
      wf.nBlockAlign = 4;
      wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
      wf.wBitsPerSample = 16;
      m_pPrimaryBuffer->SetFormat(&wf);

      hr = m_pPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (LPVOID*)&m_pListener);
      if(FAILED(hr))
	{
#ifdef _DEBUG
	  AfxMessageBox("can't get 3DListener");
#endif
	  m_pListener = NULL;
	  return;
	}

      // リスナーパラメータの設定
      m_pListener->SetDistanceFactor(1.0f, DS3D_IMMEDIATE);
      m_pListener->SetRolloffFactor(0.67f, DS3D_IMMEDIATE);
      m_pListener->SetDopplerFactor(0.67f,  DS3D_IMMEDIATE);

    }
  }

  void SetListenerPosition(C3DVector& mypos, C3DVector& topos, C3DVector& velo)
  {
    if(!m_pListener) return;
    m_pListener->SetPosition(mypos.x, mypos.y, mypos.z, DS3D_IMMEDIATE);
    m_pListener->SetVelocity(velo.x, velo.y, velo.z,  DS3D_IMMEDIATE);
    m_pListener->SetOrientation(topos.x, topos.y, topos.z, 0.0f, 0.0f, 1.0f,  DS3D_IMMEDIATE);
#if 0
    m_pListener->SetPosition(mypos.x, mypos.y, mypos.z, DS3D_DEFERRED);
    m_pListener->SetVelocity(velo.x, velo.y, velo.z, DS3D_DEFERRED);
    m_pListener->SetOrientation(topos.x, topos.y, topos.z, 0.0f, 0.0f, 1.0f, DS3D_DEFERRED);
      _pListener->CommitDeferredSettings();
#endif
  }


  // データの読み込み
  int Load(char* fn, int mul=1, int mode=DXSND_3D)
  {
    if(!m_pDirectSound) return -1;

    m_SoundList.push_back(CDXSoundBuffer(m_pDirectSound, fn, mul, mode));
    return m_SoundList.size() - 1;
  }

  void Play(size_t n, int freq=0, int vrm=0, int loop=0)
  {
    if(static_cast<size_t>(n) < m_SoundList.size())
      m_SoundList[n].Play(freq, vrm, loop);
  }

  void Play3D(size_t n, C3DVector& pos, C3DVector& vel, int freq=0, 
	      bool loop=false, bool setflag=true)
  {
    if(n < m_SoundList.size())
      m_SoundList[n].Play3D(pos, vel, freq, loop, setflag);
  }
  void SetPos(size_t n, C3DVector& pos, C3DVector& vel)
  {
    if(n < m_SoundList.size())
      m_SoundList[n].SetPos(pos, vel);
  }

  void SetVolume(size_t n, int vol)
  {
    if(n < m_SoundList.size())
      m_SoundList[n].SetVolume(vol);
  }
  void StopAll()
  {
    vector<CDXSoundBuffer>::iterator m;
    for(m = m_SoundList.begin(); m != m_SoundList.end(); ++m)
      m->Stop();
  }

  CDXSoundBuffer& operator[](size_t n)
  {
    if(n >= m_SoundList.size()) return m_NullBuffer;
    return m_SoundList[n];
  }


};

#endif
