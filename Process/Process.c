// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  STARTUPINFO StartInfo; //�������� �������
  ZeroMemory(&StartInfo, sizeof(STARTUPINFO)); //�������������
  PROCESS_INFORMATION ProcessInfo; //������ � �������� (����������� �������� CreateProcess)
  ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION)); //������������� (������ ����� ���������� �� �������)
  if(CreateProcess(TEXT("C:\\Windows\\notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo))
  { //������ �������� �� ������������ �����������. ������ TEXT ��������� ��������� ������ � � Unicode, � � ASCII
    Sleep(5000); //�������� 5 ���
    MessageBox(NULL, TEXT("Hello!"), TEXT("Message"), 0); //����������� ���� ��������� 
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE); //����������� �������� ���������� ��������
    MessageBox(NULL, TEXT("Bye"), TEXT("Message"), 0);
  }
  CloseHandle(ProcessInfo.hProcess); //����������, ������� �� ��������� �����������, ���������� �������
  CloseHandle(ProcessInfo.hThread); //��� �� ��������� �� �������, �� �����!
  return 0;
}