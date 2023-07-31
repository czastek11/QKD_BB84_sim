#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include "F-losujace.h"
#include <math.h>
using namespace std;

void GenerateFromQTlist(int* bits, int size, bool* QTlist, int QTlistsize) //generacja losowej tablicy bitów z ząładowanej tablicy bitów
{
    int start = rand3(0, QTlistsize-1-size);
    //while (start + size-1 > QTlistsize)
    //    start = rand3(0, QTlistsize - 1);
    for (int i = 0; i < size; i++)
    {
        bits[i] = QTlist[start + i];
    }
}

bool* load_QTlist(int& give_size)
{
    int id;
    id = rand3(0,10); //losowanie pliku
    string filename;
    if (id == 0)
    {
        filename = "QNGFile.dat";
    }
    else
    {
        filename = "QNGFile" + to_string(id)  + ".dat";
    }
    // cout << filename << endl;
    ifstream kwantowy_ciag;
    kwantowy_ciag.open(filename, ios::binary); //robimy w binarnym wiec musi byc w ten sposob
    if (kwantowy_ciag.good())
    {
        //Get the size of the file in bytes
        kwantowy_ciag.seekg(0, std::ios::end);
        int file_size =kwantowy_ciag.tellg();
        give_size = file_size * 8;
        kwantowy_ciag.seekg(0, std::ios::beg);

        // Allocate memory for loaded data
        char* buffer= new char[file_size];
        //cout << file_size << endl;

        // Read data from file
        kwantowy_ciag.read(buffer, file_size);
        //bity w bool
        bool* bits = new bool[file_size*8];
        for (int i = 0; i < file_size; i++) //przepisuje bity z kazdego bajta
        {
            for (int j = 0; j < 8; j++)
            {
                bits[i * 8 + j] = buffer[i] & (1 << j);
            }
        }
        delete[] buffer;
        kwantowy_ciag.close();
        return bits;
    }
    cout << "error loading a file" << endl;
    return NULL;
}

// Funkcja symuluj¹ca b³êdy detektora
double symulujBledy(int tablica[], int n, double prawdopodobienstwo)
{
    int ile_bledow=0;
    double x;
    if (prawdopodobienstwo!= 0)
    {
        for (int i = 0; i < n; i++)
        {
            x=rand1();
            if (x <= prawdopodobienstwo)
            {
                tablica[i] = (tablica[i] + 1) % 2;
                ile_bledow++;
            }
        }
    }
    return ile_bledow;
}

int State_error(int* table_base,int * table_state, int size, double rate) //blad zrodla i kanalu
{
    int ile_bledow=0;
    if (rate != 0)
    {
        double r;
        for (int i = 0; i < size; i++)
        {
            r = rand1();
            if (r <= rate)
            {
                table_base[i] = (table_base[i] + 1) % 2;
                table_state[i] = rand3(0, 1);
                ile_bledow++;
            }
        }
    }
    return ile_bledow;
}

int odczyt(int tablica_E1[],int tablica_E2[],int tablica_A1[],int tablica_A2[],int n)
{
    int k=0;
    for (int i = 0; i < n; ++i) // Tworzenie tablicy wyników pomiarów w bazach z tablicy_E1
    {
        if (tablica_E1[i] == tablica_A1[i] )
        {
            tablica_E2[i] = tablica_A2[i]; // Jeśli baza pomiaru jest taka sama jak baza Alicji, wynik jest bez zmian
            k++;
        }
        else
        {
            tablica_E2[i] = rand3(0,1); // Jeśli baza pomiaru jest przeciwna do bazy w tablicy_B1, wynik jest losowy
        }
    }
    return k;
}

void kopiuj(int tablica1[], int tablica2[], int n)
{
    for (int i = 0; i < n; i++)
    {
        tablica1[i] = tablica2[i];
    }
}

void printArray(int *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        cout << arr[i] << " ";
    }
    cout <<endl;
}

int main()
{
    srand (time(NULL));
    int n = 5000; //liczba bitów w kluczu
    int m;          //dlugosc ciagu w pliku
    bool* test = load_QTlist(m);
    double srednia1=0,srednia2=0,srednia3=0,srednia4=0,srednia5=0,srednia6=0,srednia7=0,srednia8=0;
    const double alpha1 = 0.05; //blad zrodla dla Alicji
    const double alpha2 = 0.05; //blad kanalu kwantowego dla Alicji
    const double alpha3 = 0.05; //blad kanalu kwantowego dla Ewy
    const double alpha4 = 0.05; //blad detektora dla Boba

    double ile_bledow1=0, ile_bledow2=0, ile_bledow3=0, ile_bledow4=0, ile_zna=0;
    int key_errors, key_errors_noE;

    int* tablica_A1= new int[n]; // tablica z losowymi bazami Alicji
    int* tablica_A2= new int[n]; // tablica z losowymi bitami Alicji
    int* tablica_A12= new int[n]; //tablica z bazami z bledem zrodla i kanalu
    int* tablica_A21= new int[n]; // tablica z bitami z uwzględnionymi bledu zrodla i kanalu
    int* tablica_A12c= new int[n]; //kopia tablicy z bazami z bledem
    int* tablica_A21c= new int[n]; //kopia tablica z bitami z bledu zrodla
    int* tablica_B1= new int[n]; //tablica z loswymi bazami pomiarowych Boba
    int* tablica_B2= new int[n]; //tablica z odczytanymi stanami pomiarowymi Boba
    int* tablica_E1= new int[n]; //tablica baz Ewy do odczytu
    int* tablica_E2= new int[n]; //tablica wyników pomiarów w bazach z tablicy_E1 - dla jednakowej bazy z bazą Alicji stan bez zmian; dla przeciwnej bazy idealne losowanie 50%/50%
    int* tablica_E3= new int[n]; //tablica baz do nadania do boba; wypełniana na podstawie wybranej strategii
    int* tablica_E4= new int[n]; //tablica stanów do nadania do boba; wypełniana na podstawie wybranej strategii
    int* tablica_noE= new int[n];
    vector<int> key1A;
    vector<int> key1B;

    cout<<"Wybierz scenariusz:"<<endl<<"bez Ewy-1  z Ewa-2"<<endl;
    int x;
    cin>>x;
    char w;
    if(x==2)
    {
        cout << "Wybierz strategie Ewy: " << endl << " a ) wysyla baze pionowa i same 1 " << endl << " b ) wysyla losowa baze i losowe bity " << endl;
        cout<< " c ) wysyla bazy wylosowane do pomiaru i bity takie same co zmierzyla" << endl;
        cout << " d ) wysyla bazy wylosowane do pomiaru i bity z dyskret. sin^2(x) " << endl;
        cin>>w;
        cout<<"QBER    "<<"QBER   "<<" QBER     "<<"QBER     "<<"%bledow "<<" % inf Ewy"<<"  # bledow wpr."<<endl;
        cout<<"zrodla: "<<"kanalu: "<<"detekt1: "<<"detekt2: "<<"klucza:"<<"  o kluczu: "<<" przez Ewe:"<<endl;
    }
    if(x==1)
    {
        cout<<"QBER    "<<"QBER   "<<" QBER    "<<"% bledow"<<endl;
        cout<<"zrodla: "<<"kanalu: "<<"detekt: "<<"klucza:"<<endl;
    }


    int sim_time=2000;
    for(int j=0; j<sim_time; j++)
    {
        GenerateFromQTlist(tablica_A1, n,test, m); // generowanie losowych baz Alicji
        GenerateFromQTlist(tablica_A2, n,test, m); // generowanie losowych bitów Alicji
        kopiuj(tablica_A21,tablica_A2,n);
        kopiuj(tablica_A12,tablica_A1,n);
        ile_bledow1=State_error(tablica_A12, tablica_A21, n, alpha1);   //dodanie bledu zrodla
        ile_bledow2=State_error(tablica_A12, tablica_A21, n, alpha2);   //dodanie bledu kanalu

        if(x==2)
        {
            kopiuj(tablica_A12c,tablica_A12,n); //  kopia baz
            kopiuj(tablica_A21c,tablica_A21,n); //kopia  bitow
            GenerateFromQTlist(tablica_E1, n,test, m); //wybor losowy bazy pomiarowej Ewy do odczytu
            ile_zna=odczyt(tablica_E1,tablica_E2,tablica_A12,tablica_A21,n);

            if (w == 'a')//bazy E3 i stany E4 wysylane przez Ewe -rozne strategie
            {
                for (int i = 0; i < n; ++i)
                {
                    tablica_E3[i] = 1;
                    tablica_E4[i] = 1;
                }

            }
            if (w == 'b')
            {
                GenerateFromQTlist(tablica_E3, n, test, m);
                GenerateFromQTlist(tablica_E4, n, test, m);
            }
            if (w == 'c')
            {
                for (int i = 0; i < n; ++i)
                {
                    tablica_E3[i] = tablica_E1[i];
                    tablica_E4[i] = tablica_E2[i];
                }

            }
            if (w == 'd')
            {
                for (int i = 0; i < n; ++i)
                {
                    tablica_E3[i] = tablica_E1[i];
                    //cout << int(round(abs(sin(i * 50.265482*2.0 / n)))) << endl;
                    tablica_E4[i] = int(round(abs(sin(i * 50.265482 * 2.0 / n))));
                }

            }
            kopiuj(tablica_A12,tablica_E3,n);
            kopiuj(tablica_A21,tablica_E4,n);
            ile_bledow3=State_error(tablica_A12, tablica_A21, n, alpha2);
        }

        GenerateFromQTlist(tablica_B1,n,test,m);            // generowanie losowych baz Boba
        odczyt(tablica_B1,tablica_B2,tablica_A12,tablica_A21,n);
        ile_bledow4=symulujBledy(tablica_B2, n, alpha4);     // Symulacja bledów w detektorach na odebranych bitach Boba B2

        key_errors=0;
        key1A.clear();
        key1B.clear();
        for(int i=0; i<n; i++)
        {
            if(tablica_A1[i]==tablica_B1[i])
            {
                if(tablica_A2[i]!=tablica_B2[i])
                {
                    key_errors++;
                }
                key1A.push_back(tablica_A2[i]);
                key1B.push_back(tablica_B2[i]);
            }
        }
        if(x==1)
        {
            cout << fixed << setprecision(3);
            //cout<<ile_bledow1/n<<"   "<<ile_bledow2/n<<"   "<<ile_bledow4/n<<"   "<<key_errors*100/double(key1A.size())<<endl;
            srednia1+=ile_bledow1/n;
            srednia2+=ile_bledow2/n;
            srednia4+=ile_bledow4/n;
            srednia5+=key_errors*100/double(key1A.size());
        }
        else
        {
            ile_zna = 0;
            for (int i = 0; i < n; i++)
            {
                if (tablica_A1[i] == tablica_B1[i] && tablica_A1[i] == tablica_E1[i]) ile_zna++;
            }
            key_errors_noE=0; //bity bez Ewy
            State_error(tablica_A12c, tablica_A21c, n, alpha3);
            odczyt(tablica_B1,tablica_noE,tablica_A12c,tablica_A21c,n);
            ile_bledow4=symulujBledy(tablica_noE,n,alpha4);
            for(int i=0; i<n; i++)
            {
                if(tablica_A1[i]==tablica_B1[i])
                {
                    if(tablica_A2[i]!=tablica_noE[i])
                        key_errors_noE++;
                }
            }
            cout << fixed << setprecision(3);
            //cout << double(ile_bledow1 / n) << "   " << ile_bledow2 / n << "   " << ile_bledow3 / n << "    ";
            //cout << ile_bledow4 / n << "    " << key_errors * 100 / double(key1A.size()) << "   " << ile_zna * 100 / n << "     " << (key_errors - key_errors_noE) << endl;
            srednia1 += ile_bledow1 / n;
            srednia2 += ile_bledow2 / n;
            srednia3 += ile_bledow3 / n;
            srednia4 += ile_bledow4 / n;
            srednia5 += key_errors * 100.0 / double(key1A.size());
            srednia6 += ile_zna * 100.0 / key1A.size();
            srednia7 += (key_errors - key_errors_noE) * 100.0 / key_errors;
            srednia8 += (ile_bledow1 + ile_bledow2 + ile_bledow3 + ile_bledow4) / n;
        }
    }
    cout<<endl;
    cout<<"Srednie:"<<endl<<endl<<"QBER bledow zrodla:         "<<srednia1/sim_time<<endl<<"QBER bledow kanalu:         "<<srednia2/sim_time<<endl;
    cout<<"QBER bledow detektora1:     "<<srednia3/sim_time<<endl<<"QBER bledow detektora2:     "<<srednia4/sim_time<<endl;
    cout<<"% bledow klucza:                       "<<srednia5/sim_time<<endl<<"% posiadanej inf o kluczu przez Ewe:   "<<srednia6/sim_time<<endl;
    cout<<"% bledow wprowadzonych przez Ewe:      "<<srednia7/sim_time<<endl; 
    cout << "calkowtity QBER:" << srednia8 / sim_time << endl;
}
