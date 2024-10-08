// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  STARTUPINFO StartInfo; //атрибуты запуска
  ZeroMemory(&StartInfo, sizeof(STARTUPINFO)); //инициализация
  PROCESS_INFORMATION ProcessInfo; //данные о процессе (заполняется функцией CreateProcess)
  ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION)); //инициализация (только чтобы компилятор не ругался)
  if(CreateProcess(TEXT("C:\\Windows\\notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo))
  { //запуск блокнота со стандартными параметрами. макрос TEXT позволяет запускать проект и в Unicode, и в ASCII
    Sleep(5000); //ожидание 5 сек
    MessageBox(NULL, TEXT("Hello!"), TEXT("Message"), 0); //стандартное окно сообщения 
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE); //бесконечное ожидание завершения процесса
    MessageBox(NULL, TEXT("Bye"), TEXT("Message"), 0);
  }
  CloseHandle(ProcessInfo.hProcess); //завершаясь, процесс не закрывает дескрипторы, необходимо вручную
  CloseHandle(ProcessInfo.hThread); //это не завершает ни процесс, ни поток!
  return 0;
}