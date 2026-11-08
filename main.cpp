// S3P11 Структуры. Вариант 14 «Тарификация»
// Исполнитель: Нестеренко М., группа ИКТГ-2521
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

// Тариф оператора связи
struct Tariff
{
    int id;            // уникальный идентификатор
    string name;       // название тарифа
    string provider;   // оператор
    string type;       // вид услуги
    double price;      // абонентская плата, руб./мес.
    int minutes;       // пакет минут
    double traffic;    // пакет интернета, ГБ
};

// ---------- вспомогательные функции ----------

// Длина строки UTF-8 в символах (для выравнивания таблицы)
int utf8Length(const string& s)
{
    int len = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80)
            len++;
    return len;
}

// Дополнение строки пробелами до ширины w символов
string pad(const string& s, int w)
{
    int len = utf8Length(s);
    return len >= w ? s : s + string(w - len, ' ');
}

string trim(const string& s)
{
    size_t b = s.find_first_not_of(" \t\r\n");
    size_t e = s.find_last_not_of(" \t\r\n");
    return b == string::npos ? "" : s.substr(b, e - b + 1);
}

// Разбор целого числа из строки
bool parseInt(const string& s, int& value)
{
    istringstream in(trim(s));
    in >> value;
    return !in.fail() && in.eof();
}

// Разбор вещественного числа из строки
bool parseDouble(const string& s, double& value)
{
    istringstream in(trim(s));
    in >> value;
    return !in.fail() && in.eof();
}

// Ввод целого числа с клавиатуры с проверкой
int readInt(const string& prompt)
{
    string line;
    int value;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line))
            exit(0);
        if (parseInt(line, value))
            return value;
        cout << "Ошибка: введите целое число.\n";
    }
}

// Ввод неотрицательного вещественного числа с проверкой
double readDouble(const string& prompt)
{
    string line;
    double value;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line))
            exit(0);
        if (parseDouble(line, value) && value >= 0)
            return value;
        cout << "Ошибка: введите неотрицательное число.\n";
    }
}

// Ввод непустой строки без символа-разделителя ';'
string readString(const string& prompt)
{
    string line;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line))
            exit(0);
        line = trim(line);
        if (!line.empty() && line.find(';') == string::npos)
            return line;
        cout << "Ошибка: строка не должна быть пустой и содержать ';'.\n";
    }
}

int findById(const Tariff* arr, int n, int id)
{
    for (int i = 0; i < n; i++)
        if (arr[i].id == id)
            return i;
    return -1;
}

// Увеличение ёмкости динамического массива в 2 раза
void grow(Tariff*& arr, int n, int& capacity)
{
    int newCapacity = capacity == 0 ? 4 : capacity * 2;
    Tariff* tmp = new Tariff[newCapacity];
    for (int i = 0; i < n; i++)
        tmp[i] = arr[i];
    delete[] arr;
    arr = tmp;
    capacity = newCapacity;
}

// ---------- функции программы ----------

// Чтение данных из файла в динамический массив
bool readFromFile(const string& fileName, Tariff*& arr, int& n, int& capacity)
{
    ifstream fin(fileName);
    if (!fin)
    {
        cout << "Файл \"" << fileName << "\" не найден!\n";
        return false;
    }
    string line;
    int lineNum = 0;
    while (getline(fin, line))
    {
        lineNum++;
        if (trim(line).empty())
            continue;
        string f[7];
        istringstream ss(line);
        int k = 0;
        while (k < 7 && getline(ss, f[k], ';'))
            k++;
        string rest;
        Tariff t;
        bool ok = (k == 7) && !getline(ss, rest)
            && parseInt(f[0], t.id) && t.id > 0
            && parseDouble(f[4], t.price) && t.price >= 0
            && parseInt(f[5], t.minutes) && t.minutes >= 0
            && parseDouble(f[6], t.traffic) && t.traffic >= 0;
        t.name = trim(f[1]);
        t.provider = trim(f[2]);
        t.type = trim(f[3]);
        if (!ok || t.name.empty() || t.provider.empty() || t.type.empty())
        {
            cout << "Строка " << lineNum << " пропущена: неверный формат.\n";
            continue;
        }
        if (findById(arr, n, t.id) != -1)
        {
            cout << "Строка " << lineNum << " пропущена: id " << t.id << " уже есть.\n";
            continue;
        }
        if (n == capacity)
            grow(arr, n, capacity);
        arr[n++] = t;
    }
    fin.close();
    cout << "Загружено записей: " << n << "\n";
    return true;
}

// Просмотр данных, хранящихся в массиве
void printTable(const Tariff* arr, int n)
{
    if (n == 0)
    {
        cout << "База данных пуста.\n";
        return;
    }
    string line = "+-----+-----------------+-------------+------------------+-----------+--------+-----------+";
    cout << line << "\n";
    cout << "| " << pad("id", 3) << " | " << pad("Тариф", 15) << " | " << pad("Оператор", 11)
         << " | " << pad("Вид услуги", 16) << " | " << pad("Плата,руб", 9) << " | "
         << pad("Минуты", 6) << " | " << pad("Трафик,ГБ", 9) << " |\n";
    cout << line << "\n";
    for (int i = 0; i < n; i++)
    {
        cout << "| " << setw(3) << arr[i].id << " | " << pad(arr[i].name, 15) << " | "
             << pad(arr[i].provider, 11) << " | " << pad(arr[i].type, 16) << " | "
             << fixed << setprecision(2) << setw(9) << arr[i].price << " | "
             << setw(6) << arr[i].minutes << " | "
             << setprecision(1) << setw(9) << arr[i].traffic << " |\n";
    }
    cout << line << "\n";
}

// Корректировка данных заданной строки
void editRecord(Tariff* arr, int n)
{
    int id = readInt("Введите id записи для корректировки: ");
    int i = findById(arr, n, id);
    if (i == -1)
    {
        cout << "Запись с id " << id << " не найдена.\n";
        return;
    }
    printTable(arr + i, 1);
    cout << "1 - название, 2 - оператор, 3 - вид услуги,\n"
         << "4 - плата, 5 - минуты, 6 - трафик, 0 - отмена\n";
    int field = readInt("Какое поле изменить: ");
    switch (field)
    {
    case 1: arr[i].name = readString("Новое название: "); break;
    case 2: arr[i].provider = readString("Новый оператор: "); break;
    case 3: arr[i].type = readString("Новый вид услуги: "); break;
    case 4: arr[i].price = readDouble("Новая плата, руб.: "); break;
    case 5:
    {
        int m = readInt("Новый пакет минут: ");
        while (m < 0)
            m = readInt("Число должно быть >= 0: ");
        arr[i].minutes = m;
        break;
    }
    case 6: arr[i].traffic = readDouble("Новый трафик, ГБ: "); break;
    default: cout << "Отмена.\n"; return;
    }
    cout << "Запись изменена:\n";
    printTable(arr + i, 1);
}

// Добавление записи
void addRecord(Tariff*& arr, int& n, int& capacity)
{
    Tariff t;
    t.id = readInt("id: ");
    while (t.id <= 0 || findById(arr, n, t.id) != -1)
        t.id = readInt("id должен быть положительным и уникальным: ");
    t.name = readString("Название тарифа: ");
    t.provider = readString("Оператор: ");
    t.type = readString("Вид услуги: ");
    t.price = readDouble("Абонентская плата, руб.: ");
    t.minutes = readInt("Пакет минут: ");
    while (t.minutes < 0)
        t.minutes = readInt("Число должно быть >= 0: ");
    t.traffic = readDouble("Трафик, ГБ: ");
    if (n == capacity)
        grow(arr, n, capacity);
    arr[n++] = t;
    cout << "Запись добавлена.\n";
}

// Удаление записи
void deleteRecord(Tariff* arr, int& n)
{
    int id = readInt("Введите id удаляемой записи: ");
    int i = findById(arr, n, id);
    if (i == -1)
    {
        cout << "Запись с id " << id << " не найдена.\n";
        return;
    }
    for (int j = i; j < n - 1; j++)
        arr[j] = arr[j + 1];
    n--;
    cout << "Запись удалена.\n";
}

// Вычисления
void calculate(const Tariff* arr, int n)
{
    if (n == 0)
    {
        cout << "Нет данных для вычислений.\n";
        return;
    }
    double sum = 0;
    int cheapest = 0, expensive = 0, bestGb = -1;
    for (int i = 0; i < n; i++)
    {
        sum += arr[i].price;
        if (arr[i].price < arr[cheapest].price) cheapest = i;
        if (arr[i].price > arr[expensive].price) expensive = i;
        if (arr[i].traffic > 0 && (bestGb == -1 ||
            arr[i].price / arr[i].traffic < arr[bestGb].price / arr[bestGb].traffic))
            bestGb = i;
    }
    cout << fixed << setprecision(2);
    cout << "Количество тарифов: " << n << "\n";
    cout << "Средняя абонентская плата: " << sum / n << " руб.\n";
    cout << "Самый дешёвый: " << arr[cheapest].name << " (" << arr[cheapest].provider
         << ") - " << arr[cheapest].price << " руб.\n";
    cout << "Самый дорогой: " << arr[expensive].name << " (" << arr[expensive].provider
         << ") - " << arr[expensive].price << " руб.\n";
    if (bestGb != -1)
        cout << "Самый выгодный по цене 1 ГБ: " << arr[bestGb].name << " ("
             << arr[bestGb].provider << ") - "
             << arr[bestGb].price / arr[bestGb].traffic << " руб./ГБ\n";

    double budget = readDouble("Введите бюджет, руб./мес.: ");
    cout << "Тарифы с платой не выше " << budget << " руб.:\n";
    int count = 0;
    for (int i = 0; i < n; i++)
        if (arr[i].price <= budget)
        {
            cout << "  " << arr[i].id << ". " << arr[i].name << " (" << arr[i].provider
                 << ") - " << arr[i].price << " руб.\n";
            count++;
        }
    if (count == 0)
        cout << "  таких тарифов нет\n";
}

// Сохранение данных в новом файле
void saveToFile(const Tariff* arr, int n)
{
    string fileName = readString("Введите имя нового файла: ");
    ofstream fout(fileName);
    if (!fout)
    {
        cout << "Не удалось создать файл \"" << fileName << "\".\n";
        return;
    }
    for (int i = 0; i < n; i++)
        fout << arr[i].id << ';' << arr[i].name << ';' << arr[i].provider << ';'
             << arr[i].type << ';' << arr[i].price << ';' << arr[i].minutes << ';'
             << arr[i].traffic << '\n';
    fout.close();
    cout << "Сохранено записей: " << n << " в файл \"" << fileName << "\".\n";
}

int main()
{
    Tariff* tariffs = nullptr;
    int n = 0, capacity = 0;

    string fileName = readString("Введите имя файла с данными: ");
    if (!readFromFile(fileName, tariffs, n, capacity))
        return 1;

    while (true)
    {
        cout << "\n===== Тарификация =====\n"
             << "1. Просмотр данных\n"
             << "2. Корректировка записи\n"
             << "3. Добавление записи\n"
             << "4. Удаление записи\n"
             << "5. Вычисления\n"
             << "0. Выход с сохранением в новый файл\n";
        int choice = readInt("Выберите действие: ");
        switch (choice)
        {
        case 1: printTable(tariffs, n); break;
        case 2: editRecord(tariffs, n); break;
        case 3: addRecord(tariffs, n, capacity); break;
        case 4: deleteRecord(tariffs, n); break;
        case 5: calculate(tariffs, n); break;
        case 0:
            saveToFile(tariffs, n);
            delete[] tariffs;
            tariffs = nullptr;
            cout << "Работа завершена.\n";
            return 0;
        default: cout << "Нет такого пункта меню.\n";
        }
    }
}
