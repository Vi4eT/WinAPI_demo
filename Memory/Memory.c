// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <windows.h>
enum //ID для дочерних окон, могут быть любыми удобными, главное уникальными в пределах одного родительского окна
{
  MenuDefault,
  MenuNew,
};
int MemoryCreate(HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew,  int **ArrayNew)
{
  *hHeapDefault = GetProcessHeap(); //получить дескриптор кучи по умолчанию
  *ArrayDefault = (int*)HeapAlloc(*hHeapDefault, HEAP_ZERO_MEMORY, sizeof(int)); //выделить память в куче по умолчанию под один элемент типа int (условный минимум) и обнулить её
  *hHeapNew = HeapCreate(0, sizeof(int), 0); //создать новую кучу начального размера int без параметров и ограничения макс размера
  *ArrayNew = (int*)HeapAlloc(*hHeapNew, HEAP_ZERO_MEMORY, sizeof(int)); //выделить память в новой куче
  if(!*ArrayDefault || !*ArrayNew) //если хоть один из массивов не существует - ошибка
    return 1;
  return 0;
}
void MemoryFree(HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew)
{
  HeapFree(*hHeapDefault, 0, *ArrayDefault); //освободить массив в куче по умолчанию
  HeapDestroy(*hHeapNew); //уничтожить новую кучу (освобождать массив в ней необязательно)
}
int MemoryProcess(HWND hWindow, HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew, int **ArrayNew)
{
  BOOL Translated = FALSE; //индикатор успеха для GetDlgItemInt
  int SizeDefault = (int)GetDlgItemInt(hWindow, MenuDefault, &Translated, FALSE); //извлекает текст окна и переводит его в int, не проверяя на знак минус (его невозможно ввести)
  if(!Translated) //если не переведено - ошибка
    return 1;
  int SizeNew = (int)GetDlgItemInt(hWindow, MenuNew, &Translated, FALSE);
  if(!Translated)
    return 1;
  //в процессе работы программы используются только realloc, что позволяет не перезапускать программу после каждого нажатия кнопки
  int* ReAllArray = (int*)HeapReAlloc(*hHeapDefault, HEAP_ZERO_MEMORY, *ArrayDefault, SizeDefault * sizeof(int)); //перевыделение памяти в куче по умолчанию
  if(!ReAllArray) //если не выделено - ошибка
    return 1;
  else
    *ArrayDefault = ReAllArray; //если выделено - сохранить указатель на новый массив
  ReAllArray = (int*)HeapReAlloc(*hHeapNew, HEAP_ZERO_MEMORY, *ArrayNew, SizeNew * sizeof(int));
  if(!ReAllArray)
    return 1;
  else
    *ArrayNew = ReAllArray;
  return 0;
}
LRESULT CALLBACK WindowProcess(_In_ HWND hWindow, _In_ UINT Message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{//static позволяет не терять значения переменных между вызовами данной функции
  static HWND hEditDefault, hEditNew, hButton, hStatic; //дескрипторы дочерних окон
  static HANDLE hHeapDefault = NULL, hHeapNew = NULL; //дескрипторы куч
  static int *ArrayDefault = NULL, *ArrayNew = NULL; //указатели на массивы
  switch(Message) //обработка полученного сообщения
  {
    case WM_CREATE: //первичное создание окна
    {
      HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance; //получение дескриптора экземпляра приложения
      hEditDefault = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | ES_NUMBER, //окно дочернее, видимое, только для цифр
                                    30, 80, 70, 20, hWindow, (HMENU)MenuDefault, hInstance, NULL);
      ShowWindow(hEditDefault, SW_SHOWNORMAL);
      hEditNew = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | ES_NUMBER,
                                180, 80, 70, 20, hWindow, (HMENU)MenuNew, hInstance, NULL);
      ShowWindow(hEditNew, SW_SHOWNORMAL);
      hButton = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Button"), TEXT("Start"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, //кнопка с обводкой "выбор по умолчанию", посылающая WM_COMMAND при нажатии
                               90, 120, 100, 30, hWindow, NULL, hInstance, NULL);
      ShowWindow(hButton, SW_SHOWNORMAL);
      hStatic = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Static"), TEXT("Ready"), WS_CHILD | WS_VISIBLE | SS_CENTER, //выравнивание текста по центру
                               90, 165, 100, 20, hWindow, NULL, hInstance, NULL);
      ShowWindow(hStatic, SW_SHOWNORMAL);
      if(MemoryCreate(&hHeapDefault, &ArrayDefault, &hHeapNew, &ArrayNew)) //подготовка областей памяти
      { //при любой ошибке: вывести сообщение Error и сделать кнопку неактивной, юзеру остается только перезапуск
        SetWindowText(hStatic, TEXT("Error")); //установить текст в окне
        EnableWindow(hButton, FALSE); //деактивировать окно
      } 
      break;
    }
    case WM_PAINT: //отрисовка элементов окна
    {
      PAINTSTRUCT PaintStruct; //структура информации об отрисовке
      HDC hDeviceContext = BeginPaint(hWindow, &PaintStruct); //подготовка к отрисовке, получение дескриптора устройства отображения
      TextOut(hDeviceContext, 110, 20, TEXT("Data size"), 9); //вывод текста: устройство отображения, координаты x и y начала, сам текст (необязательно с /0), длина строки
      TextOut(hDeviceContext, 30, 50, TEXT("Default heap"), 12);
      TextOut(hDeviceContext, 180, 50, TEXT("New heap"), 8);
      EndPaint(hWindow, &PaintStruct); //отметка конца отрисовки
      break;
    }
    case WM_COMMAND: //обработка команд
      if(lParam == (LPARAM)hButton) //если команда поступила от кнопки
      {
        if(MemoryProcess(hWindow, &hHeapDefault, &ArrayDefault, &hHeapNew, &ArrayNew)) //работа с памятью
        {
          SetWindowText(hStatic, TEXT("Error"));
          EnableWindow(hButton, FALSE);
        }
        else
          SetWindowText(hStatic, TEXT("Done")); //Done выводится при каждом успешном выполнении
      }
      break;
    case WM_DESTROY: //закрытие окна
      MemoryFree(&hHeapDefault, &ArrayDefault, &hHeapNew); //освобождение всей памяти
      PostQuitMessage(0); //выход из программы с кодом 0
      break;
    default:
      return DefWindowProc(hWindow, Message, wParam, lParam); //если поступило сообщение, обработка которого не прописана, обработать его стандартно
  }
  return 0;
}
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  WNDCLASSEX WindowClass; //структура информации о классе окна
  ZeroMemory(&WindowClass, sizeof(WNDCLASSEX)); //инициализация (изменению подвергнутся не все поля)
  WindowClass.cbSize = sizeof(WNDCLASSEX); //обязательная установка размера
  WindowClass.style = CS_HREDRAW | CS_VREDRAW; //перерисовка окна при изменении горизонтальных и вертикальных размеров
  WindowClass.lpfnWndProc = WindowProcess; //функция обработки событий окна
  WindowClass.hInstance = hInstance; //дескриптор экземпляра приложения (идентификатор, отличающий этот исполняемый файл от любых других, в том числе подключаемых dll при их наличии)
  WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //цвет фона окна (получая его таким способом, всегда нужно +1)
  WindowClass.lpszClassName = TEXT("WindowClass"); //название класса окна
  RegisterClassEx(&WindowClass); //регистрация класса окна
  //создание основного окна
  HWND hWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, //дополнительный (EXtended) стиль окна, стандартный для окон windows
                                TEXT("WindowClass"), //имя класса окна
                                TEXT("Memory"), //заголовок окна
                                WS_OVERLAPPEDWINDOW, //основной стиль окна, стандартный для окон windows
                                600, //абсцисса верхнего левого угла
                                300, //ордината верхнего левого угла
                                300, //ширина окна
                                250, //высота окна
                                NULL, //дескриптор родительского окна
                                NULL, //дескриптор меню или, в случае дочернего окна, идентификатор дочернего окна
                                hInstance, //приложение, связанное с окном
                                NULL); //дополнительные параметры
  ShowWindow(hWindow, nCmdShow); //изменение состояния показа окна
  UpdateWindow(hWindow); //обновление окна: посылает сообщение WM_PAINT в напрямую в WindowProcess
  MSG Message; //структура информации о сообщениях
  while(GetMessage(&Message, NULL, 0, 0)) //цикл обработки всех сообщений из очереди сообщений приложения
  {
    TranslateMessage(&Message); //перевод сообщений клавиш в сообщения символов (например "нажата клавиша" (WM_KEYDOWN) -> "введён символ" (WM_CHAR))
    DispatchMessage(&Message); //посылает информацию о сообщении в WindowProcess
  }
  return Message.wParam; //возрат значения выхода последнего сообщения
}