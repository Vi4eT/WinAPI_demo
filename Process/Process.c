#include <windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  STARTUPINFO StartInfo;                                        //атрибуты запуска
  ZeroMemory(&StartInfo, sizeof(STARTUPINFO));                  //инициализация
  StartInfo.cb = sizeof(StartInfo);                             //для корректной работы CreateProcess
  PROCESS_INFORMATION ProcessInfo;                              //данные о процессе (заполняется CreateProcess)
  ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));        //инициализация
  if(CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo))
  { //запуск cmd со стандартными параметрами. макрос TEXT интерпретирует строку в текущей кодировке
    Sleep(1000);                                                //ожидание 1 сек
    MessageBox(NULL, TEXT("Hello"), TEXT("Message"), MB_OK);    //стандартное окно сообщения 
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE);        //бесконечное ожидание завершения процесса
    MessageBox(NULL, TEXT("Bye"), TEXT("Message"), MB_OK);
    CloseHandle(ProcessInfo.hProcess);                          //дескрипторы необходимо закрыть вручную
    CloseHandle(ProcessInfo.hThread);                           //это не завершает ни процесс, ни поток!
    return 0;
  }
  return 1;
}