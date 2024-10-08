#include <windows.h>
enum //ID для дочерних окон, любые уникальные в пределах родительского окна
{
  MenuDefault,
  MenuNew,
};

static int MemoryCreate(HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew,  int **ArrayNew)
{
  *hHeapDefault = GetProcessHeap();                                                 //получить дескриптор кучи по умолчанию
  *ArrayDefault = (int*)HeapAlloc(*hHeapDefault, HEAP_ZERO_MEMORY, sizeof(int));    //выделить память в куче по умолчанию под один int и занулить
  *hHeapNew = HeapCreate(0, sizeof(int), 0);                                        //создать новую кучу размера int без ограничения макс размера
  *ArrayNew = (int*)HeapAlloc(*hHeapNew, HEAP_ZERO_MEMORY, sizeof(int));            //выделить память в новой куче
  if(!*ArrayDefault || !*ArrayNew)                                                  //если хоть один из массивов не существует
    return 1;
  return 0;
}

static void MemoryFree(HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew)
{
  HeapFree(*hHeapDefault, 0, *ArrayDefault);    //освободить массив в куче по умолчанию
  HeapDestroy(*hHeapNew);                       //уничтожить новую кучу (освобождать массив необязательно)
}

static int MemoryProcess(HWND hWindow, HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew, int **ArrayNew)
{
  BOOL Translated = FALSE;                                                          //флаг для GetDlgItemInt
  int SizeDefault = (int)GetDlgItemInt(hWindow, MenuDefault, &Translated, FALSE);   //переводит текст в int, не проверяя на минус (его невозможно ввести)
  if(!Translated)
    return 1;
  int SizeNew = (int)GetDlgItemInt(hWindow, MenuNew, &Translated, FALSE);
  if(!Translated)
    return 1;
  //используются только realloc, что позволяет не перезапускать программу
  int* ReAllArray = (int*)HeapReAlloc(*hHeapDefault, HEAP_ZERO_MEMORY, *ArrayDefault, SizeDefault * sizeof(int)); //перевыделение памяти в куче по умолчанию
  if(!ReAllArray)
    return 1;
  else
    *ArrayDefault = ReAllArray; //если успешно, сохранить указатель на новый массив
  ReAllArray = (int*)HeapReAlloc(*hHeapNew, HEAP_ZERO_MEMORY, *ArrayNew, SizeNew * sizeof(int));
  if(!ReAllArray)
    return 1;
  else
    *ArrayNew = ReAllArray;
  return 0;
}

static LRESULT CALLBACK WindowProcess(_In_ HWND hWindow, _In_ UINT Message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{ //static переменные сохраняют значения между вызовами функции
  static HWND hEditDefault, hEditNew, hButton, hStatic;     //дескрипторы дочерних окон
  static HANDLE hHeapDefault = NULL, hHeapNew = NULL;       //дескрипторы куч
  static int *ArrayDefault = NULL, *ArrayNew = NULL;        //указатели на массивы
  switch(Message)       //обработка полученного сообщения
  {
    case WM_CREATE:     //первичное создание окна
    {
      HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;    //получение дескриптора экземпляра приложения
      hEditDefault = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | ES_NUMBER,     //окно дочернее, видимое, только для цифр
                                    30, 80, 70, 20, hWindow, (HMENU)MenuDefault, hInstance, NULL);
      ShowWindow(hEditDefault, SW_SHOWNORMAL);
      hEditNew = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | ES_NUMBER,
                                180, 80, 70, 20, hWindow, (HMENU)MenuNew, hInstance, NULL);
      ShowWindow(hEditNew, SW_SHOWNORMAL);
      hButton = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Button"), TEXT("Start"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,   //кнопка, выбираемая по умолчанию, посылает WM_COMMAND
                               90, 120, 100, 30, hWindow, NULL, hInstance, NULL);
      ShowWindow(hButton, SW_SHOWNORMAL);
      hStatic = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Static"), TEXT("Ready"), WS_CHILD | WS_VISIBLE | SS_CENTER,          //выравнивание текста по центру
                               90, 165, 100, 20, hWindow, NULL, hInstance, NULL);
      ShowWindow(hStatic, SW_SHOWNORMAL);
      if(MemoryCreate(&hHeapDefault, &ArrayDefault, &hHeapNew, &ArrayNew))  //подготовка областей памяти
      { //при ошибке вывести Error и деактивировать кнопку
        SetWindowText(hStatic, TEXT("Error"));  //установить текст в окне
        EnableWindow(hButton, FALSE);           //деактивировать кнопку
      } 
      break;
    }
    case WM_PAINT:      //отрисовка элементов окна
    {
      PAINTSTRUCT PaintStruct;                                  //структура информации об отрисовке
      HDC hDeviceContext = BeginPaint(hWindow, &PaintStruct);   //получение дескриптора устройства отображения
      TextOut(hDeviceContext, 110, 20, TEXT("Data size"), 9);   //вывод текста: устройство отображения, X и Y начала, текст, длина строки
      TextOut(hDeviceContext, 30, 50, TEXT("Default heap"), 12);
      TextOut(hDeviceContext, 180, 50, TEXT("New heap"), 8);
      EndPaint(hWindow, &PaintStruct);                          //конец отрисовки
      break;
    }
    case WM_COMMAND:    //обработка команд
      if(lParam == (LPARAM)hButton) //если команда от кнопки
      {
        if(MemoryProcess(hWindow, &hHeapDefault, &ArrayDefault, &hHeapNew, &ArrayNew)) //работа с памятью
        {
          SetWindowText(hStatic, TEXT("Error"));
          EnableWindow(hButton, FALSE);
        }
        else
          SetWindowText(hStatic, TEXT("Done")); //вывод при каждом успешном выполнении
      }
      break;
    case WM_DESTROY:    //закрытие окна
      MemoryFree(&hHeapDefault, &ArrayDefault, &hHeapNew);      //освобождение памяти
      PostQuitMessage(0);                                       //выход с кодом 0
      break;
    default:
      return DefWindowProc(hWindow, Message, wParam, lParam);   //стандартная обработка
  }
  return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  WNDCLASSEX WindowClass;                                   //структура информации о классе окна
  ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));             //инициализация
  WindowClass.cbSize = sizeof(WNDCLASSEX);                  //обязательная установка размера
  WindowClass.style = CS_HREDRAW | CS_VREDRAW;              //перерисовка окна при изменении размеров
  WindowClass.lpfnWndProc = WindowProcess;                  //функция обработки событий окна
  WindowClass.hInstance = hInstance;                        //дескриптор экземпляра приложения (идентификатор, отличающий исполняемый файл от других, в том числе dll)
  WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);   //цвет фона окна (+1 необходимо)
  WindowClass.lpszClassName = TEXT("WindowClass");          //название класса окна
  RegisterClassEx(&WindowClass);                            //регистрация класса окна
  //создание основного окна
  HWND hWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, //доп стиль окна, стандартный для windows
                                TEXT("WindowClass"),    //имя класса окна
                                TEXT("Memory"),         //заголовок окна
                                WS_OVERLAPPEDWINDOW,    //основной стиль окна, стандартный для windows
                                600,                    //абсцисса верхнего левого угла
                                300,                    //ордината верхнего левого угла
                                300,                    //ширина окна
                                250,                    //высота окна
                                NULL,                   //дескриптор родительского окна
                                NULL,                   //дескриптор меню или идентификатор дочернего окна
                                hInstance,              //приложение, связанное с окном
                                NULL);                  //дополнительные параметры
  ShowWindow(hWindow, nCmdShow);            //изменение состояния показа окна
  UpdateWindow(hWindow);                    //обновление окна, посылает WM_PAINT напрямую в WindowProcess
  MSG Message;                              //структура информации о сообщениях
  while(GetMessage(&Message, NULL, 0, 0))   //обработка всех сообщений из очереди
  {
    TranslateMessage(&Message);             //перевод сообщений клавиш в сообщения символов (WM_KEYDOWN -> WM_CHAR)
    DispatchMessage(&Message);              //посылает информацию о сообщении в WindowProcess
  }
  return Message.wParam;                    //возрат значения выхода последнего сообщения
}