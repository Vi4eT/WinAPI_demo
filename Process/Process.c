#include <windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  STARTUPINFO StartInfo;                                        //�������� �������
  ZeroMemory(&StartInfo, sizeof(STARTUPINFO));                  //�������������
  StartInfo.cb = sizeof(StartInfo);                             //��� ���������� ������ CreateProcess
  PROCESS_INFORMATION ProcessInfo;                              //������ � �������� (����������� CreateProcess)
  ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));        //�������������
  if(CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo))
  { //������ cmd �� ������������ �����������. ������ TEXT �������������� ������ � ������� ���������
    Sleep(1000);                                                //�������� 1 ���
    MessageBox(NULL, TEXT("Hello"), TEXT("Message"), MB_OK);    //����������� ���� ��������� 
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE);        //����������� �������� ���������� ��������
    MessageBox(NULL, TEXT("Bye"), TEXT("Message"), MB_OK);
    CloseHandle(ProcessInfo.hProcess);                          //����������� ���������� ������� �������
    CloseHandle(ProcessInfo.hThread);                           //��� �� ��������� �� �������, �� �����!
    return 0;
  }
  return 1;
}