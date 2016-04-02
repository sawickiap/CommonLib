/** \file
\brief Klasy wyj¹tków do obs³ugi b³êdów
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Czêœæ biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: Za¿ó³æ gêœl¹ jaŸñ \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Error \n
Elementy modu³u: \ref code_error
*/
#include "Base.hpp"
#include "Error.hpp"

#ifndef WIN32
	#include <errno.h>
#endif

#ifdef USE_DIRECTX
	#include <dxerr.h>
	#include <D3DX9Mesh.h> // Dla ID3DXBuffer
#endif

#ifdef USE_DEVIL
	#include <IL/il.h>
	#include <IL/ilu.h>
#endif

#ifdef USE_AVI_FILE
	#include <vfw.h>
#endif

#ifdef USE_OPENGL
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif


namespace common
{

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Error

void Error::Push(const tstring &msg, const tstring &file, int line)
{
	if (msg.empty()) return;

	tstring file_name;
	size_t pos = file.find_last_of(_T("/\\"));
	if (pos != tstring::npos)
		file_name = file.substr(pos+1);
	else
		file_name = file;

	if (file_name.empty() && line == 0)
		m_Msgs.push_back(msg);
	else if (file_name.empty())
		m_Msgs.push_back(_T("[") + IntToStrR(line) + _T("] ") + msg);
	else if (line == 0)
		m_Msgs.push_back(_T("[") + file_name + _T("] ") + msg);
	else
		m_Msgs.push_back(_T("[") + file_name + _T(",") + IntToStrR(line) + _T("] ") + msg);
}

void Error::GetMessage_(tstring *Msg, const tstring &Indent, const tstring &eol) const
{
	Msg->clear();
	for (
		std::vector<tstring>::const_reverse_iterator crit = m_Msgs.rbegin();
		crit != m_Msgs.rend();
		++crit)
	{
		if (!Msg->empty())
			*Msg += eol;
		*Msg += Indent;
		*Msg += *crit;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ErrnoError

ErrnoError::ErrnoError(int err_code, const tstring &msg, const tstring &file, int line)
{
	#ifdef _UNICODE
		#ifdef _MSC_VER
			wchar_t buf[256];
			_wcserror_s(buf, _countof(buf), err_code);
			Push(_T("(errno,") + IntToStrR(errno) + _T(") ") + buf);
		#else
			Push(_T("(errno,") + IntToStrR(err_code) + _T(") ") + _wcserror(err_code));
		#endif
	#else
		#ifdef _MSC_VER
			char buf[256];
			strerror_s(buf, _countof(buf), err_code);
			Push(_T("(errno,") + IntToStrR(err_code) + _T(") ") + buf);
		#else
			Push(_T("(errno,") + IntToStrR(err_code) + _T(") ") + strerror(err_code));
		#endif
	#endif

	Push(msg, file, line);
}

ErrnoError::ErrnoError(const tstring &msg, const tstring &file, int line)
{
	#ifdef _UNICODE
		#ifdef _MSC_VER
			wchar_t buf[256];
			_wcserror_s(buf, _countof(buf), errno);
			Push(_T("(errno,") + IntToStrR(errno) + _T(") ") + buf);
		#else
			Push(_T("(errno,") + IntToStrR(errno) + _T(") ") + _wcserror(errno));
		#endif
	#else
		#ifdef _MSC_VER
			char buf[256];
			strerror_s(buf, _countof(buf), errno);
			Push(_T("(errno,") + IntToStrR(errno) + _T(") ") + buf);
		#else
			Push(_T("(errno,") + IntToStrR(errno) + _T(") ") + strerror(errno));
		#endif
	#endif

	Push(msg, file, line);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa SDLError

#ifdef USE_SDL
	SDLError::SDLError(const tstring &msg, const tstring &file, int line)
	{
		#ifdef _UNICODE
			wstring Msg;
			ConvertCharsToUnicode(&Msg, SDL_GetError(), CP_ACP);
			Push(_T("(SDLError) ") + Msg);
		#else
			Push("(SDLError) " + string(SDL_GetError()));
		#endif

		Push(msg, file, line);
	}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa OpenGLError

#ifdef USE_OPENGL
	OpenGLError::OpenGLError(const tstring &msg, const tstring &file, int line)
	{
		#ifdef _UNICODE
			wstring Msg;
			ConvertCharsToUnicode(&Msg, (const char*)gluErrorString(glGetError()), CP_ACP);
			Push(_T("(OpenGLError,") + IntToStrR(glGetError()) + _T(") ") + Msg);
		#else
			Push("(OpenGLError,"+ IntToStrR(glGetError()) +") " + string((char*)gluErrorString(glGetError())));
		#endif

		Push(msg, file, line);
	}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa FmodError

#ifdef USE_FMOD
	void FmodError::CodeToMessage(FMOD_RESULT code, tstring *Enum, tstring *Message)
	{
		switch (code)
		{
		case FMOD_OK:
			if (Enum) *Enum = _T("FMOD_OK");
			if (Message) *Message = _T("No errors.");
			break;
		case FMOD_ERR_ALREADYLOCKED:
			if (Enum) *Enum = _T("FMOD_ERR_ALREADYLOCKED");
			if (Message) *Message = _T("Tried to call lock a second time before unlock was called.");
			break;
		case FMOD_ERR_BADCOMMAND:
			if (Enum) *Enum = _T("FMOD_ERR_BADCOMMAND");
			if (Message) *Message = _T("Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound).");
			break;
		case FMOD_ERR_CDDA_DRIVERS:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_DRIVERS");
			if (Message) *Message = _T("Neither NTSCSI nor ASPI could be initialised.");
			break;
		case FMOD_ERR_CDDA_INIT:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_INIT");
			if (Message) *Message = _T("An error occurred while initialising the CDDA subsystem.");
			break;
		case FMOD_ERR_CDDA_INVALID_DEVICE:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_INVALID_DEVICE");
			if (Message) *Message = _T("Couldn't find the specified device.");
			break;
		case FMOD_ERR_CDDA_NOAUDIO:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_NOAUDIO");
			if (Message) *Message = _T("No audio tracks on the specified disc.");
			break;
		case FMOD_ERR_CDDA_NODEVICES:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_NODEVICES");
			if (Message) *Message = _T("No CD/DVD devices were found.");
			break;
		case FMOD_ERR_CDDA_NODISC:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_NODISC");
			if (Message) *Message = _T("No disc present in the specified drive.");
			break;
		case FMOD_ERR_CDDA_READ:
			if (Enum) *Enum = _T("FMOD_ERR_CDDA_READ");
			if (Message) *Message = _T("A CDDA read error occurred.");
			break;
		case FMOD_ERR_CHANNEL_ALLOC:
			if (Enum) *Enum = _T("FMOD_ERR_CHANNEL_ALLOC");
			if (Message) *Message = _T("Error trying to allocate a channel.");
			break;
		case FMOD_ERR_CHANNEL_STOLEN:
			if (Enum) *Enum = _T("FMOD_ERR_CHANNEL_STOLEN");
			if (Message) *Message = _T("The specified channel has been reused to play another sound.");
			break;
		case FMOD_ERR_COM:
			if (Enum) *Enum = _T("FMOD_ERR_COM");
			if (Message) *Message = _T("A Win32 COM related error occured. COM failed to initialize or a QueryInterface failed meaning a Windows codec or driver was not installed properly.");
			break;
		case FMOD_ERR_DMA:
			if (Enum) *Enum = _T("FMOD_ERR_DMA");
			if (Message) *Message = _T("DMA Failure. See debug output for more information.");
			break;
		case FMOD_ERR_DSP_CONNECTION:
			if (Enum) *Enum = _T("FMOD_ERR_DSP_CONNECTION");
			if (Message) *Message = _T("DSP connection error. Connection possibly caused a cyclic dependancy.");
			break;
		case FMOD_ERR_DSP_FORMAT:
			if (Enum) *Enum = _T("FMOD_ERR_DSP_FORMAT");
			if (Message) *Message = _T("DSP Format error. A DSP unit may have attempted to connect to this network with the wrong format.");
			break;
		case FMOD_ERR_DSP_NOTFOUND:
			if (Enum) *Enum = _T("FMOD_ERR_DSP_NOTFOUND");
			if (Message) *Message = _T("DSP connection error. Couldn't find the DSP unit specified.");
			break;
		case FMOD_ERR_DSP_RUNNING:
			if (Enum) *Enum = _T("FMOD_ERR_DSP_RUNNING");
			if (Message) *Message = _T("DSP error. Cannot perform this operation while the network is in the middle of running. This will most likely happen if a connection or disconnection is attempted in a DSP callback.");
			break;
		case FMOD_ERR_DSP_TOOMANYCONNECTIONS:
			if (Enum) *Enum = _T("FMOD_ERR_DSP_TOOMANYCONNECTIONS");
			if (Message) *Message = _T("DSP connection error. The unit being connected to or disconnected should only have 1 input or output.");
			break;
		case FMOD_ERR_FILE_BAD:
			if (Enum) *Enum = _T("FMOD_ERR_FILE_BAD");
			if (Message) *Message = _T("Error loading file.");
			break;
		case FMOD_ERR_FILE_COULDNOTSEEK:
			if (Enum) *Enum = _T("FMOD_ERR_FILE_COULDNOTSEEK");
			if (Message) *Message = _T("Couldn't perform seek operation. This is a limitation of the medium (ie netstreams) or the file format.");
			break;
		case FMOD_ERR_FILE_EOF:
			if (Enum) *Enum = _T("FMOD_ERR_FILE_EOF");
			if (Message) *Message = _T("End of file unexpectedly reached while trying to read essential data (truncated data?).");
			break;
		case FMOD_ERR_FILE_NOTFOUND:
			if (Enum) *Enum = _T("FMOD_ERR_FILE_NOTFOUND");
			if (Message) *Message = _T("File not found.");
			break;
		case FMOD_ERR_FORMAT:
			if (Enum) *Enum = _T("FMOD_ERR_FORMAT");
			if (Message) *Message = _T("Unsupported file or audio format.");
			break;
		case FMOD_ERR_HTTP:
			if (Enum) *Enum = _T("FMOD_ERR_HTTP");
			if (Message) *Message = _T("A HTTP error occurred. This is a catch-all for HTTP errors not listed elsewhere.");
			break;
		case FMOD_ERR_HTTP_ACCESS:
			if (Enum) *Enum = _T("FMOD_ERR_HTTP_ACCESS");
			if (Message) *Message = _T("The specified resource requires authentication or is forbidden.");
			break;
		case FMOD_ERR_HTTP_PROXY_AUTH:
			if (Enum) *Enum = _T("FMOD_ERR_HTTP_PROXY_AUTH");
			if (Message) *Message = _T("Proxy authentication is required to access the specified resource.");
			break;
		case FMOD_ERR_HTTP_SERVER_ERROR:
			if (Enum) *Enum = _T("FMOD_ERR_HTTP_SERVER_ERROR");
			if (Message) *Message = _T("A HTTP server error occurred.");
			break;
		case FMOD_ERR_HTTP_TIMEOUT:
			if (Enum) *Enum = _T("FMOD_ERR_HTTP_TIMEOUT");
			if (Message) *Message = _T("The HTTP request timed out.");
			break;
		case FMOD_ERR_INITIALIZATION:
			if (Enum) *Enum = _T("FMOD_ERR_INITIALIZATION");
			if (Message) *Message = _T("FMOD was not initialized correctly to support this function.");
			break;
		case FMOD_ERR_INITIALIZED:
			if (Enum) *Enum = _T("FMOD_ERR_INITIALIZED");
			if (Message) *Message = _T("Cannot call this command after System::init.");
			break;
		case FMOD_ERR_INTERNAL:
			if (Enum) *Enum = _T("FMOD_ERR_INTERNAL");
			if (Message) *Message = _T("An error occured that wasnt supposed to. Contact support.");
			break;
		case FMOD_ERR_INVALID_HANDLE:
			if (Enum) *Enum = _T("FMOD_ERR_INVALID_HANDLE");
			if (Message) *Message = _T("An invalid object handle was used.");
			break;
		case FMOD_ERR_INVALID_PARAM:
			if (Enum) *Enum = _T("FMOD_ERR_INVALID_PARAM");
			if (Message) *Message = _T("An invalid parameter was passed to this function.");
			break;
		case FMOD_ERR_IRX:
			if (Enum) *Enum = _T("FMOD_ERR_IRX");
			if (Message) *Message = _T("PS2 only. fmodex.irx failed to initialize. This is most likely because you forgot to load it.");
			break;
		case FMOD_ERR_MEMORY:
			if (Enum) *Enum = _T("FMOD_ERR_MEMORY");
			if (Message) *Message = _T("Not enough memory or resources.");
			break;
		case FMOD_ERR_MEMORY_IOP:
			if (Enum) *Enum = _T("FMOD_ERR_MEMORY_IOP");
			if (Message) *Message = _T("PS2 only. Not enough memory or resources on PlayStation 2 IOP ram.");
			break;
		case FMOD_ERR_MEMORY_SRAM:
			if (Enum) *Enum = _T("FMOD_ERR_MEMORY_SRAM");
			if (Message) *Message = _T("Not enough memory or resources on console sound ram.");
			break;
		case FMOD_ERR_NEEDHARDWARE:
			if (Enum) *Enum = _T("FMOD_ERR_NEEDHARDWARE");
			if (Message) *Message = _T("Tried to use a feature that requires hardware support. (ie trying to play a VAG compressed sound in software on PS2).");
			break;
		case FMOD_ERR_NEEDSOFTWARE:
			if (Enum) *Enum = _T("FMOD_ERR_NEEDSOFTWARE");
			if (Message) *Message = _T("Tried to use a feature that requires the software engine. Software engine has either been turned off, or command was executed on a hardware channel which does not support this feature.");
			break;
		case FMOD_ERR_NET_CONNECT:
			if (Enum) *Enum = _T("FMOD_ERR_NET_CONNECT");
			if (Message) *Message = _T("Couldn't connect to the specified host.");
			break;
		case FMOD_ERR_NET_SOCKET_ERROR:
			if (Enum) *Enum = _T("FMOD_ERR_NET_SOCKET_ERROR");
			if (Message) *Message = _T("A socket error occurred. This is a catch-all for socket-related errors not listed elsewhere.");
			break;
		case FMOD_ERR_NET_URL:
			if (Enum) *Enum = _T("FMOD_ERR_NET_URL");
			if (Message) *Message = _T("The specified URL couldn't be resolved.");
			break;
		case FMOD_ERR_NOTREADY:
			if (Enum) *Enum = _T("FMOD_ERR_NOTREADY");
			if (Message) *Message = _T("Operation could not be performed because specified sound is not ready.");
			break;
		case FMOD_ERR_OUTPUT_ALLOCATED:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_ALLOCATED");
			if (Message) *Message = _T("Error initializing output device, but more specifically, the output device is already in use and cannot be reused.");
			break;
		case FMOD_ERR_OUTPUT_CREATEBUFFER:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_CREATEBUFFER");
			if (Message) *Message = _T("Error creating hardware sound buffer.");
			break;
		case FMOD_ERR_OUTPUT_DRIVERCALL:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_DRIVERCALL");
			if (Message) *Message = _T("A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted.");
			break;
		case FMOD_ERR_OUTPUT_FORMAT:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_FORMAT");
			if (Message) *Message = _T("Soundcard does not support the minimum features needed for this soundsystem (16bit stereo output).");
			break;
		case FMOD_ERR_OUTPUT_INIT:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_INIT");
			if (Message) *Message = _T("Error initializing output device.");
			break;
		case FMOD_ERR_OUTPUT_NOHARDWARE:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_NOHARDWARE");
			if (Message) *Message = _T("FMOD_HARDWARE was specified but the sound card does not have the resources nescessary to play it.");
			break;
		case FMOD_ERR_OUTPUT_NOSOFTWARE:
			if (Enum) *Enum = _T("FMOD_ERR_OUTPUT_NOSOFTWARE");
			if (Message) *Message = _T("Attempted to create a software sound but no software channels were specified in System::init.");
			break;
		case FMOD_ERR_PAN:
			if (Enum) *Enum = _T("FMOD_ERR_PAN");
			if (Message) *Message = _T("Panning only works with mono or stereo sound sources.");
			break;
		case FMOD_ERR_PLUGIN:
			if (Enum) *Enum = _T("FMOD_ERR_PLUGIN");
			if (Message) *Message = _T("An unspecified error has been returned from a 3rd party plugin.");
			break;
		case FMOD_ERR_PLUGIN_MISSING:
			if (Enum) *Enum = _T("FMOD_ERR_PLUGIN_MISSING");
			if (Message) *Message = _T("A requested output, dsp unit type or codec was not available.");
			break;
		case FMOD_ERR_PLUGIN_RESOURCE:
			if (Enum) *Enum = _T("FMOD_ERR_PLUGIN_RESOURCE");
			if (Message) *Message = _T("A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback)");
			break;
		case FMOD_ERR_RECORD:
			if (Enum) *Enum = _T("FMOD_ERR_RECORD");
			if (Message) *Message = _T("An error occured trying to initialize the recording device.");
			break;
		case FMOD_ERR_REVERB_INSTANCE:
			if (Enum) *Enum = _T("FMOD_ERR_REVERB_INSTANCE");
			if (Message) *Message = _T("Specified Instance in FMOD_REVERB_PROPERTIES couldn't be set. Most likely because another application has locked the EAX4 FX slot.");
			break;
		case FMOD_ERR_SUBSOUND_ALLOCATED:
			if (Enum) *Enum = _T("FMOD_ERR_SUBSOUND_ALLOCATED");
			if (Message) *Message = _T("This subsound is already being used by another sound, you cannot have more than one parent to a sound. Null out the other parent's entry first.");
			break;
		case FMOD_ERR_TAGNOTFOUND:
			if (Enum) *Enum = _T("FMOD_ERR_TAGNOTFOUND");
			if (Message) *Message = _T("The specified tag could not be found or there are no tags.");
			break;
		case FMOD_ERR_TOOMANYCHANNELS:
			if (Enum) *Enum = _T("FMOD_ERR_TOOMANYCHANNELS");
			if (Message) *Message = _T("The sound created exceeds the allowable input channel count. This can be increased with System::setMaxInputChannels.");
			break;
		case FMOD_ERR_UNIMPLEMENTED:
			if (Enum) *Enum = _T("FMOD_ERR_UNIMPLEMENTED");
			if (Message) *Message = _T("Something in FMOD hasn't been implemented when it should be! contact support!");
			break;
		case FMOD_ERR_UNINITIALIZED:
			if (Enum) *Enum = _T("FMOD_ERR_UNINITIALIZED");
			if (Message) *Message = _T("This command failed because System::init or System::setDriver was not called.");
			break;
		case FMOD_ERR_UNSUPPORTED:
			if (Enum) *Enum = _T("FMOD_ERR_UNSUPPORTED");
			if (Message) *Message = _T("A command issued was not supported by this object. Possibly a plugin without certain callbacks specified.");
			break;
		case FMOD_ERR_UPDATE:
			if (Enum) *Enum = _T("FMOD_ERR_UPDATE");
			if (Message) *Message = _T("On PS2, System::update was called twice in a row when System::updateFinished must be called first.");
			break;
		case FMOD_ERR_VERSION:
			if (Enum) *Enum = _T("FMOD_ERR_VERSION");
			if (Message) *Message = _T("The version number of this file format is not supported.");
			break;
		default:
			if (Enum) *Enum = _T("(other)");
			if (Message) *Message = _T("Unknown error.");
			break;
		}
	}

	FmodError::FmodError(FMOD_RESULT code, const tstring &msg, const tstring &file, int line)
	{
		tstring e, m;
		CodeToMessage(code, &e, &m);
		Push(_T("(FMOD,") + IntToStrR((int)code) + _T(",") + e + _T(") ") + m);
		Push(msg, file, line);
	}
#endif

#ifdef WIN32
	Win32Error::Win32Error(const tstring &msg, const tstring &file, int line)
	{
		DWORD Code = GetLastError();
		tstring s = _T("(Win32Error,") + UintToStrR<uint32>(Code) + _T(") ");
		tchar *Message = 0;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, // dwFlags
			0, // lpSource
			Code, // dwMessageId
			0, // dwLanguageId
			(tchar*)&Message, // lpBuffer (celowe rzutowanie tchar** na tchar* !!!)
			0, // nSize
			0); // Arguments
		Push(s + Message);
		LocalFree(Message);

		Push(msg, file, line);
	}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa DirectXError

#ifdef USE_DIRECTX
	DirectXError::DirectXError(HRESULT hr, const tstring &Msg, const tstring &File, int Line)
	{
		Push( _T("(DirectXError,") + IntToStrR(hr) + _T(") ") + tstring(DXGetErrorString(hr)) + _T(" (") + tstring(DXGetErrorDescription(hr)) + _T(")") );
		Push(Msg, File, Line);
	}

	DirectXError::DirectXError(HRESULT hr, ID3DXBuffer *Buf, const tstring &Msg, const tstring &File, int Line)
	{
		Push( _T("(DirectXError,") + IntToStrR(hr) + _T(") ") + tstring(DXGetErrorString(hr)) + _T(" (") + tstring(DXGetErrorDescription(hr)) + _T(")") );
		Push(Msg, File, Line);

		if (Buf)
		{
			#ifdef _UNICODE
				string Msg;
				Msg.assign((const char*)Buf->GetBufferPointer(), Buf->GetBufferSize());
				wstring WMsg;
				ConvertCharsToUnicode(&WMsg, Msg, CP_ACP);
				Push(WMsg);
			#else
				string M;
				M.assign((const char*)Buf->GetBufferPointer(), Buf->GetBufferSize());
				Push(M);
			#endif
			Buf->Release();
		}
	}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa WinSockError

#ifdef USE_WINSOCK
	void WinSockError::CodeToStr(int Code, tstring *Name, tstring *Message)
	{
		Name->clear();
		Message->clear();

		switch (Code)
		{
		case WSAEINTR: *Name = _T("EINTR"); *Message = _T("Interrupted function call"); break;
		case WSAEACCES: *Name = _T("EACCES"); *Message = _T("Permission denied"); break;
		case WSAEFAULT: *Name = _T("EFAULT"); *Message = _T("Bad address"); break;
		case WSAEINVAL: *Name = _T("EINVAL"); *Message = _T("Invalid argument"); break;
		case WSAEMFILE: *Name = _T("EMFILE"); *Message = _T("Too many open files"); break;
		case WSAEWOULDBLOCK: *Name = _T("EWOULDBLOCK"); *Message = _T("Resource temporarily unavailable"); break;
		case WSAEINPROGRESS: *Name = _T("EINPROGRESS"); *Message = _T("Operation now in progress"); break;
		case WSAEALREADY: *Name = _T("EALREADY"); *Message = _T("Operation already in progress"); break;
		case WSAENOTSOCK: *Name = _T("ENOTSOCK"); *Message = _T("Socket operation on nonsocket"); break;
		case WSAEDESTADDRREQ: *Name = _T("EDESTADDRREQ"); *Message = _T("Destination address required"); break;
		case WSAEMSGSIZE: *Name = _T("EMSGSIZE"); *Message = _T("*Message too long"); break;
		case WSAEPROTOTYPE: *Name = _T("EPROTOTYPE"); *Message = _T("Protocol wrong type for socket"); break;
		case WSAENOPROTOOPT: *Name = _T("ENOPROTOOPT"); *Message = _T("Bad protocol option"); break;
		case WSAEPROTONOSUPPORT: *Name = _T("EPROTONOSUPPORT"); *Message = _T("Protocol not supported"); break;
		case WSAESOCKTNOSUPPORT: *Name = _T("ESOCKTNOSUPPORT"); *Message = _T("Socket type not supported"); break;
		case WSAEOPNOTSUPP: *Name = _T("EOPNOTSUPP"); *Message = _T("Operation not supported"); break;
		case WSAEPFNOSUPPORT: *Name = _T("EPFNOSUPPORT"); *Message = _T("Protocol family not supported"); break;
		case WSAEAFNOSUPPORT: *Name = _T("EAFNOSUPPORT"); *Message = _T("Address family not supported by protocol family"); break;
		case WSAEADDRINUSE: *Name = _T("EADDRINUSE"); *Message = _T("Address already in use"); break;
		case WSAEADDRNOTAVAIL: *Name = _T("EADDRNOTAVAIL"); *Message = _T("Cannot assign requested address"); break;
		case WSAENETDOWN: *Name = _T("ENETDOWN"); *Message = _T("Network is down"); break;
		case WSAENETUNREACH: *Name = _T("ENETUNREACH"); *Message = _T("Network is unreachable"); break;
		case WSAENETRESET: *Name = _T("ENETRESET"); *Message = _T("Network dropped connection on reset"); break;
		case WSAECONNABORTED: *Name = _T("ECONNABORTED"); *Message = _T("Software caused connection abort"); break;
		case WSAECONNRESET: *Name = _T("ECONNRESET"); *Message = _T("Connection reset by peer"); break;
		case WSAENOBUFS: *Name = _T("ENOBUFS"); *Message = _T("No buffer space available"); break;
		case WSAEISCONN: *Name = _T("EISCONN"); *Message = _T("Socket is already connected"); break;
		case WSAENOTCONN: *Name = _T("ENOTCONN"); *Message = _T("Socket is not connected"); break;
		case WSAESHUTDOWN: *Name = _T("ESHUTDOWN"); *Message = _T("Cannot send after socket shutdown"); break;
		case WSAETIMEDOUT: *Name = _T("ETIMEDOUT"); *Message = _T("Connection timed out"); break;
		case WSAECONNREFUSED: *Name = _T("ECONNREFUSED"); *Message = _T("Connection refused"); break;
		case WSAEHOSTDOWN: *Name = _T("EHOSTDOWN"); *Message = _T("Host is down"); break;
		case WSAEHOSTUNREACH: *Name = _T("EHOSTUNREACH"); *Message = _T("No route to host"); break;
		case WSAEPROCLIM: *Name = _T("EPROCLIM"); *Message = _T("Too many processes"); break;
		case WSASYSNOTREADY: *Name = _T("SYSNOTREADY"); *Message = _T("Network subsystem is unavailable"); break;
		case WSAVERNOTSUPPORTED: *Name = _T("VERNOTSUPPORTED"); *Message = _T("Winsock.dll version out of range"); break;
		case WSANOTINITIALISED: *Name = _T("NOTINITIALISED"); *Message = _T("Successful WSAStartup not yet performed"); break;
		case WSAEDISCON: *Name = _T("EDISCON"); *Message = _T("Graceful shutdown in progress"); break;
		case WSATYPE_NOT_FOUND: *Name = _T("TYPE_NOT_FOUND"); *Message = _T("Class type not found"); break;
		case WSAHOST_NOT_FOUND: *Name = _T("HOST_NOT_FOUND"); *Message = _T("Host not found"); break;
		case WSATRY_AGAIN: *Name = _T("TRY_AGAIN"); *Message = _T("Nonauthoritative host not found"); break;
		case WSANO_RECOVERY: *Name = _T("NO_RECOVERY"); *Message = _T("This is a nonrecoverable error"); break;
		case WSANO_DATA: *Name = _T("NO_DATA"); *Message = _T("Valid name, no data record of requested type"); break;
	//	case WSA_INVALID_HANDLE: *Name = _T("_INVALID_HANDLE"); *Message = _T("Specified event object handle is invalid"); break;
	//	case WSA_INVALID_PARAMETER: *Name = _T("_INVALID_PARAMETER"); *Message = _T("One or more parameters are invalid"); break;
	//	case WSA_IO_INCOMPLETE: *Name = _T("_IO_INCOMPLETE"); *Message = _T("Overlapped I/O event object not in signaled state"); break;
	//	case WSA_IO_PENDING: *Name = _T("_IO_PENDING"); *Message = _T("Overlapped operations will complete later"); break;
	//	case WSA_NOT_ENOUGH_MEMORY: *Name = _T("_NOT_ENOUGH_MEMORY"); *Message = _T("Insufficient memory available"); break;
	//	case WSA_OPERATION_ABORTED: *Name = _T("_OPERATION_ABORTED"); *Message = _T("Overlapped operation aborted"); break;
	//	case WSAINVALIDPROCTABLE: *Name = _T("INVALIDPROCTABLE"); *Message = _T("Invalid procedure table from service provider"); break;
	//	case WSAINVALIDPROVIDER: *Name = _T("INVALIDPROVIDER"); *Message = _T("Invalid service provider version number"); break;
	//	case WSAPROVIDERFAILEDINIT: *Name = _T("PROVIDERFAILEDINIT"); *Message = _T("Unable to initialize a service provider"); break;
		case WSASYSCALLFAILURE: *Name = _T("SYSCALLFAILURE"); *Message = _T("System call failure"); break;
		}

		if (!Name->empty()) *Name = _T("WSA") + *Name;
	}

	int WinSockError::WinSockError(int Code, const tstring &Msg, const tstring &File, int Line)
	{
		tstring Name, Text;
		CodeToStr(Code, &Name, &Text);
		Push(_T("(WinSockError,") + IntToStr(Code) + _T(",") + Name + _T(") ") + Text);
		Push(Msg, File, Line);
	}

	int WinSockError::WinSockError(const tstring &Msg, const tstring &File, int Line)
	{
		int Code = WSAGetLastError();
		tstring Name, Text;
		CodeToStr(Code, &Name, &Text);
		Push(_T("(WinSockError,") + IntToStr(Code) + _T(",") + Name + _T(") ") + Text);
		Push(Msg, File, Line);
	}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa DevILError

#ifdef USE_DEVIL
	DevILError::DevILError(unsigned FirstCode, const tstring &Msg, const tstring &File, int Line)
	{
		// Tu jest pêtla, bo w bibliotece DevILError jest stos b³êdów
		ILenum code;
		tstring LocalMsg;
		code = FirstCode;

		do
		{
			StringToTstring(&LocalMsg, string(iluErrorString(code)));
			Push(_T("(DevIL,") + UintToStrR(code) + _T(") ") + LocalMsg);
		}
		while ( (code = ilGetError()) != IL_NO_ERROR );

		Push(Msg, File, Line);
	}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa AVIFileError

#ifdef USE_AVI_FILE
	AVIFileError::AVIFileError(HRESULT hr, const tstring &Msg, const tstring &File, int Line)
	{
		tstring Message;
		switch (hr)
		{
		case AVIERR_OK             : Message = _T("AVIERR_OK"); break;
		case AVIERR_UNSUPPORTED	   : Message = _T("AVIERR_UNSUPPORTED"); break;
		case AVIERR_BADFORMAT	   : Message = _T("AVIERR_BADFORMAT"); break;
		case AVIERR_MEMORY		   : Message = _T("AVIERR_MEMORY"); break;
		case AVIERR_INTERNAL	   : Message = _T("AVIERR_INTERNAL"); break;
		case AVIERR_BADFLAGS	   : Message = _T("AVIERR_BADFLAGS"); break;
		case AVIERR_BADPARAM	   : Message = _T("AVIERR_BADPARAM"); break;
		case AVIERR_BADSIZE		   : Message = _T("AVIERR_BADSIZE"); break;
		case AVIERR_BADHANDLE	   : Message = _T("AVIERR_BADHANDLE"); break;
		case AVIERR_FILEREAD	   : Message = _T("AVIERR_FILEREAD"); break;
		case AVIERR_FILEWRITE	   : Message = _T("AVIERR_FILEWRITE"); break;
		case AVIERR_FILEOPEN	   : Message = _T("AVIERR_FILEOPEN"); break;
		case AVIERR_COMPRESSOR	   : Message = _T("AVIERR_COMPRESSOR"); break;
		case AVIERR_NOCOMPRESSOR   : Message = _T("AVIERR_NOCOMPRESSOR"); break;
		case AVIERR_READONLY	   : Message = _T("AVIERR_READONLY"); break;
		case AVIERR_NODATA		   : Message = _T("AVIERR_NODATA"); break;
		case AVIERR_BUFFERTOOSMALL : Message = _T("AVIERR_BUFFERTOOSMALL"); break;
		case AVIERR_CANTCOMPRESS   : Message = _T("AVIERR_CANTCOMPRESS"); break;
		case AVIERR_USERABORT	   : Message = _T("AVIERR_USERABORT"); break;
		case AVIERR_ERROR		   : Message = _T("AVIERR_ERROR"); break;
		default: Message = _T("Unknown error code"); break;
		}

		Push( _T("(AVIFileError,") + IntToStrR(hr) + _T(") ") + Message);
		Push(Msg, File, Line);
	}
#endif

} // namespace common
