/* -*- mode:cc; tab-width:4; coding:Windows-31J; -*- */
/* vi:set ft=cpp ts=4 fenc=Windows-31J : */

/*
	winshebang

	2011/08/05 opa
*/

#include <string>
#include <algorithm>
#include <dir>

#include <windows.h>

#define PGM					"winshebang"
#define PGM_DEBUG			PGM ": "
#define PGM_INFO			PGM ": "
#define PGM_WARN			PGM " warning: "
#define PGM_ERR				PGM " error: "
#define VERSTR				"1.01"

#define CREDIT2011			"Copyright (c) 2011 by opa"

typedef signed char schar;
typedef unsigned char uchar;
typedef signed int sint;
typedef unsigned int uint;
typedef signed long slong;
typedef unsigned long ulong;

using namespace std;

char
	credit[] = PGM " version " VERSTR " " CREDIT2011;
bool
	error = false;
sint
	rcode = 0;

////////////////////////////////////////////////////////////////////////

template <class BidirectionalIterator, class T>
BidirectionalIterator rfind(BidirectionalIterator first, BidirectionalIterator last, const T &value)
{
	while(first != last){
		--last;
		if(*last == value)
			break;
	}

	return last;
}

template <class BidirectionalIterator, class Predicate>
BidirectionalIterator rfind_if(BidirectionalIterator first, BidirectionalIterator last, Predicate pred)
{
	while(first != last){
		--last;
		if(pred(*last))
			break;
	}

	return last;
}

inline bool isnotwspace(wchar_t c)
{
	return !iswspace(c);
}

inline bool isbackslash(wchar_t c)
{
	return c == L'\\' || c == L'/';
}

bool file_is_exist(const wchar_t *filename)
{
#if 1
	_wffblk ff;
	sint r;

	r = _wfindfirst(filename, &ff, FA_NORMAL + FA_HIDDEN + FA_SYSTEM);
	_wfindclose(&ff);

	return r == 0;
#else
	return _waccess(filename, 0) == 0;
#endif
}

inline bool file_is_exist(const wstring &filename)
{
	return file_is_exist(filename.c_str());
}

////////////////////////////////////////////////////////////////////////

class String : public wstring {
	typedef String Self;
	typedef wstring Super;

public:
	String();
	String(const Super &s);
	String(const wchar_t *s);
	String(const_iterator b, const_iterator e);
	String(const char *s);
	~String();

	string to_ansi() const;
	Self to_upper() const;
	Self trim() const;
	bool isdoublequote() const;
	Self doublequote() const;
	Self doublequote_del() const;

	Self &operator=(const Self &s);
	Self &operator=(const wchar_t *s);
	Self &operator=(const char *s);

	Self &operator+=(const Self &s)				{ append(s); return *this; }
	Self &operator+=(const wchar_t *s)			{ append(s); return *this; }
	Self &operator+=(const char *s);

	Self &assign_from_ansi(const char *s);
	Self &assign_from_ansi(const uchar *s)		{ return assign_from_ansi((const char*)s); }
	Self &assign_from_ansi(const string &s)		{ return assign_from_ansi(s.c_str()); }
	Self &assign_from_utf8(const char *s);
	Self &assign_from_utf8(const string &s)		{ return assign_from_utf8(s.c_str()); }
	Self &assign_from_env(const Self &name);
	Self &printf(Self format, ...);

	// filename operator
	bool have_path() const;
	bool have_ext() const;
	bool isbackslash() const;
	Self backslash() const;
	Self subext(const Self &ext) const;
	Self drivename() const;
	Self dirname() const;
	Self basename() const;
};

String::String()									{}
String::String(const String::Super &s)				: Super(s) {}
String::String(const wchar_t *s)					: Super(s) {}
String::String(const_iterator b, const_iterator e)	: Super(b, e) {}
String::String(const char *s)						{ assign_from_ansi(s); }
String::~String() {}
String &String::operator=(const String &s)			{ assign(s); return *this; }
String &String::operator=(const wchar_t *s)			{ assign(s); return *this; }
String &String::operator=(const char *s)			{ assign_from_ansi(s); return *this; }
String &String::operator+=(const char *s)			{ append(String(s)); return *this; }
String operator+(const String &s1, const char *s2)	{ return s1 + String(s2); }
String operator+(const char *s1, const String &s2)	{ return String(s1) + s2; }
bool operator==(const String &s1, const char *s2)	{ return s1 == String(s2); }
bool operator==(const char *s1, const String &s2)	{ return String(s1) == s2; }

string String::to_ansi() const
{
	sint
		siz = WideCharToMultiByte(CP_ACP, 0, c_str(), size(), NULL, 0, NULL, NULL);
	char
		*buf = new char[siz+1];

	fill(buf, buf + siz+1, 0);

	WideCharToMultiByte(CP_ACP, 0, c_str(), size(), buf, siz, NULL, NULL);

	string
		r(buf);

	delete [] buf;

	return r;
}

String String::trim() const
{
	const_iterator
		b = begin(),
		e = end();

	b = ::find_if(b, e, isnotwspace);
	e = ::rfind_if(b, e, isnotwspace);

	if(e != end() && isnotwspace(*e))
		++e;

	return Self(b, e);
}

String &String::assign_from_ansi(const char *s)
{
	sint
		size = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	wchar_t
		*buf = new wchar_t[size+1];

	fill(buf, buf + size+1, 0);

	MultiByteToWideChar(CP_ACP, 0, s, -1, buf, size);

	assign(buf);

	delete [] buf;

	return *this;
}

String String::subext(const String &ext) const
{
	const_iterator
		e = ::rfind(begin(), end(), L'.');

	if(e != end() && *e != L'.')
		e = end();

	return Self(begin(), e).append(ext);
}

String String::basename() const
{
	const_iterator
		b = begin(),
		e = end();

	b = ::rfind_if(b, e, ::isbackslash);
	if(b != end() && ::isbackslash(*b))
		++b;

	e = ::rfind(b, e, L'.');
	if(e != end() && *e != L'.')
		e = end();

	return Self(b, e);
}

////////////////////////////////////////////////////////////////////////

#if defined(__CONSOLE__)

void _putcxxx(const char *s, FILE *fp)
{
	fputs(s, fp);
	fputc('\n', fp);
}

#else // __CONSOLE__

String
	consolebuf;

void _putcxxx(const char *s, FILE *)
{
	if(consolebuf.size() > 0)
		consolebuf += "\r\n";
	consolebuf += s;
}

#endif // __CONSOLE__

inline void putcout(const char *s)
{
	_putcxxx(s, stdout);
}

inline void putcerr(const char *s)
{
	_putcxxx(s, stderr);
}

void _putcxxx(const String &s, FILE *fp)
{
	_putcxxx(s.to_ansi().c_str(), fp);
}

inline void putcout(const String &s)
{
	_putcxxx(s, stdout);
}

inline void putcerr(const String &s)
{
	_putcxxx(s, stderr);
}

void _putcxxx(const char *s1, const String &s2, FILE *fp)
{
	_putcxxx(String().assign_from_ansi(s1) + s2, fp);
}

inline void putcout(const char *s1, const String &s2)
{
	_putcxxx(s1, s2, stdout);
}

inline void putcerr(const char *s1, const String &s2)
{
	_putcxxx(s1, s2, stderr);
}

////////////////////////////////////////////////////////////////////////

class WindowsAPI {
public:
	static bool CreateProcess(const String &cmd, DWORD CreationFlags, const String &wd, LPSTARTUPINFO si, LPPROCESS_INFORMATION pi);
	static String GetClipboardText();
	static String GetCommandLine()		{ return ::GetCommandLine(); }
	static String GetComputerName();
	static String GetModuleFileName(HMODULE Module = 0);
	static String GetTempPath();
	static String GetUserName();
	static String SHGetSpecialFolder(sint nFolder);
};

bool WindowsAPI::CreateProcess(const String &cmd, DWORD CreationFlags, const String &wd, LPSTARTUPINFO si, LPPROCESS_INFORMATION pi)
{
	bool
		r;
	wchar_t
		*cmd_c_str;

	cmd_c_str = new wchar_t[cmd.size()+1];
	copy(cmd.begin(), cmd.end(), cmd_c_str);
	cmd_c_str[cmd.size()] = 0;

	r = ::CreateProcess(NULL, cmd_c_str, NULL, NULL, TRUE, CreationFlags, NULL, (wd.size()>0 ? wd.c_str() : NULL), si, pi);

	delete [] cmd_c_str;

	return r;
}

String WindowsAPI::GetModuleFileName(HMODULE Module)
{
	wchar_t
		buf[MAX_PATH+1];
	DWORD
		size = sizeof buf / sizeof(wchar_t);

	size = ::GetModuleFileName(Module, buf, size);

	if(size == 0)
		return String();

	return String(buf, buf + size);
}

////////////////////////////////////////////////////////////////////////

String
	shebangline;

// コマンドライン文字列から、コマンド名を除いた残りの部分(コマンドに与えるパラメータの部分)を返す
String get_given_option(const String &cmdline)
{
	bool
		in_quote = false;

	for(String::const_iterator s = cmdline.begin() ; s != cmdline.end() ; ++s){
		wchar_t
			c = *s;

		if(!in_quote)
			if(iswspace(c))
				return String(s, cmdline.end());

		if(c == L'"')
			in_quote = in_quote ? false : true;
	}

	return String();
}

sint system(const String &cmdline)
{
	STARTUPINFO
		si;
	PROCESS_INFORMATION
		pi;
	DWORD
		CreationFlags = 0,
		ExitCode = 0;

	memset(&si, 0, sizeof si);
	memset(&pi, 0, sizeof pi);
	si.cb = sizeof si;
	GetStartupInfo(&si);

	si.dwFlags |= STARTF_FORCEOFFFEEDBACK;

	fflush(stdout);
	fflush(stderr);

	if(WindowsAPI::CreateProcess(cmdline, CreationFlags, "", &si, &pi) == 0){
		error = true;
		return -2;
	}

	CloseHandle(pi.hThread);

#if defined(__CONSOLE__)
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &ExitCode);
#endif

	CloseHandle(pi.hProcess);

	return ExitCode;
}

sint system_hide(const String &cmdline)
{
	STARTUPINFO
		si;
	PROCESS_INFORMATION
		pi;
	DWORD
		CreationFlags = 0,
		ExitCode = 0;

	memset(&si, 0, sizeof si);
	memset(&pi, 0, sizeof pi);
	si.cb = sizeof si;
	GetStartupInfo(&si);

	si.wShowWindow = SW_HIDE;					// ← ここが異なる
	si.dwFlags |= STARTF_USESHOWWINDOW;			// ← ここが異なる
	si.dwFlags |= STARTF_FORCEOFFFEEDBACK;

	fflush(stdout);
	fflush(stderr);

	if(WindowsAPI::CreateProcess(cmdline, CreationFlags, "", &si, &pi) == 0){
		error = true;
		return -2;
	}

	CloseHandle(pi.hThread);

#if defined(__CONSOLE__)
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &ExitCode);
#endif

	CloseHandle(pi.hProcess);

	return ExitCode;
}

void execute_batchfile(const String &scriptname, const String &arg)
{
#if defined(__CONSOLE__)
	rcode = system("\"" + scriptname + "\"" + arg);
#else
	rcode = system_hide("\"" + scriptname + "\"" + arg);
#endif
}

void execute_wscript(const String &scriptname, const String &arg)
{
#if defined(__CONSOLE__)
	rcode = system("cscript //Nologo \"" + scriptname + "\"" + arg);
#else
	rcode = system("wscript //Nologo \"" + scriptname + "\"" + arg);
#endif
}

void execute_java_jar(const String &jarname, const String &arg)
{
#if defined(__CONSOLE__)
	rcode = system("java -jar \"" + jarname + "\"" + arg);
#else
	rcode = system("javaw -jar \"" + jarname + "\"" + arg);
#endif
}

void execute_java_class(const String &classname, const String &arg)
{
#if defined(__CONSOLE__)
	rcode = system("java \"" + classname + "\"" + arg);
#else
	rcode = system("javaw \"" + classname + "\"" + arg);
#endif
}

bool file_is_shebang(const String &scriptname)
{
	FILE
		*fp;
	uchar
		buf[1000];

	shebangline.clear();

	if((fp = _wfopen(scriptname.c_str(), L"rb")) == NULL)
		return false;

	fill(buf, buf + sizeof buf, 0);
	fread(buf, 1, sizeof buf-1, fp); // 最後の1バイトには決して読み込まない
	fclose(fp);
	
	// #!で始まっているか判定
	if(!(buf[0] == '#' && buf[1] == '!'))
		return false;
	
	// 先頭行のみ見る
	for(uchar *s = buf ; *s ; ++s){
		if(*s == '\n' || *s == '\r'){
			fill(s, buf + sizeof buf-1, 0);
			break;
		}
	}

	shebangline.assign_from_ansi(buf);

	return true;
}

void execute_shebang(const String &scriptname, const String &arg)
{
	String::const_iterator
		b = shebangline.begin(),
		e = shebangline.end(),
		shebang_cmd_b;
	String
		shebang_cmd,
		shebang_arg;

	// shebangline が #! で始まることは確認済み。
	b += 2;

	// #! のあとの空白をスキップ
	for( ; b != e ; ++b)
		if(!iswspace(*b))
			break;

	// コマンド名の切り出し
	shebang_cmd_b = b;
	for( ; ; ++b){
		if(b == e || iswspace(*b)){
			shebang_cmd = String(shebang_cmd_b, b);
			break;
		}
		if(isbackslash(*b))
			shebang_cmd_b = b + 1;
	}

	// envだった場合は次のトークンが事実上のコマンド名
	if(shebang_cmd == "env"){
		shebang_cmd.clear();

		// 空白をスキップ
		for( ; b != e ; ++b)
			if(!iswspace(*b))
				break;

		// 再度コマンド名の切り出し
		shebang_cmd_b = b;
		for( ; ; ++b){
			if(b == e || iswspace(*b)){
				shebang_cmd = String(shebang_cmd_b, b);
				break;
			}
			if(isbackslash(*b))
				shebang_cmd_b = b + 1;
		}
	}

	// 空白をスキップ
	for( ; b != e ; ++b)
		if(!iswspace(*b))
			break;

	// 残りは引数
	shebang_arg = String(b, e).trim();
	if(shebang_arg.size() > 0)
		shebang_cmd += " " + shebang_arg;

#if defined(__CONSOLE__)
#else
	if(shebang_cmd == "perl")
		shebang_cmd = "wperl";

	if(shebang_cmd == "ruby")
		shebang_cmd = "rubyw";
	
	if(shebang_cmd == "python")
		shebang_cmd = "pythonw";

	if(shebang_cmd == "php")
		shebang_cmd = "php-win";
#endif

//putcout("[" + shebangline + "]");
//putcout("[" + shebang_cmd + " \"" + scriptname + "\"" + arg + "]");

	rcode = system(shebang_cmd + " \"" + scriptname + "\"" + arg);
}

void winshebang_main()
{
	String
		exename = WindowsAPI::GetModuleFileName(),
		arg = get_given_option(WindowsAPI::GetCommandLine()),
		s;

	if(s = exename.subext(""), file_is_shebang(s)){
		execute_shebang(s, arg);

	}else if(s = exename.subext(".bat"), file_is_exist(s)){
		execute_batchfile(s, arg);

	}else if(s = exename.subext(".cmd"), file_is_exist(s)){
		execute_batchfile(s, arg);

	}else if(s = exename.subext(".js"), file_is_exist(s)){
		execute_wscript(s, arg);

	}else if(s = exename.subext(".jse"), file_is_exist(s)){
		execute_wscript(s, arg);

	}else if(s = exename.subext(".vbs"), file_is_exist(s)){
		execute_wscript(s, arg);

	}else if(s = exename.subext(".vbe"), file_is_exist(s)){
		execute_wscript(s, arg);

	}else if(s = exename.subext(".jar"), file_is_exist(s)){
		execute_java_jar(s, arg);

	}else if(s = exename.subext(".class"), file_is_exist(s)){
		execute_java_class(s, arg);

	}else{
		putcout(credit);
		putcout("");
		putcout("Error: shebang target file not found.");
		error = true;
		rcode = 1;
	}
}

#if defined(__CONSOLE__)

extern "C"
sint wmain(sint, wchar_t *[])
{
	winshebang_main();
	return rcode;
}

#else // __CONSOLE__

// ダミーのメッセージを送受信することで、プログラムが動き出したことをOSに伝える
void dummy_message()
{
	MSG msg;
	PostMessage(NULL, WM_APP, 0, 0);
	GetMessage(&msg, NULL, WM_APP, WM_APP);
}

extern "C"
sint WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	dummy_message();
	consolebuf.clear();

	winshebang_main();

	if(consolebuf.size() > 0)
		MessageBox(0, consolebuf.c_str(), WindowsAPI::GetModuleFileName().basename().c_str(),
					MB_ICONINFORMATION + MB_OK);

	return rcode;
}

#endif // __CONSOLE__
