#pragma once

//��ʱ��uiֱ�ӵ��õ�һЩ����Ժ��ع���io�߳��ϣ�������python�߳����ṩ��
//ui�������ݴ������Ȼ��ѽ����ui. �����������ipc��ipc�յ����ݺ��ui����ʾ
#include <windows.h>
#include <string>

namespace ProfilerCmd{
void DumpGlobalVar(HWND parent);
void WriteMinidump(bool fulldump);
void OpenMinidumpFolder();
void ParserMinidump(HWND parent);
void CrashIt(DWORD pid);

void RunLow(HWND parent,const std::wstring& apppaths_name,const std::wstring& proc_name);
void LimitedJob(HWND parent,const std::wstring& apppaths_name,const std::wstring& proc_name);
void DebugRun(const std::wstring& apppaths_name,const std::wstring& proc_name);
}