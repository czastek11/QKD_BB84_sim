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

void GenerateFromQTlist(int* bits, int size, bool* QTlist, int QTlistsize) //generation of a random array of bits from the loaded array of bits
{
    int start = rand3(0, QTlistsize-1-size);
    //while (start + size-1 > QTlistsize)
    //    start = rand3(0, QTlistsize - 1);
    for (int i = 0; i < size; i++)
    {
        bits[i] = QTlist[start + i];
    }
}

bool* load_QTlist(int& give_size)//function froam loading all bits from random list (which is trurly random as they were genereted with real quantum number generator)
{
    int id;
    id = rand3(0,10); //drawing file
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
    kwantowy_ciag.open(filename, ios::binary); //converting bytes read from file into bool representation of bits
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

// function that simulates detector's error
double symulujBledy(int tablica[], int n, double prawdopodobienstwo)
{
    int ile_bledow=0;
    double x;
    if (prawdopodobienstwo!= 0)
    {
        for (int i = 0; i < n; i++)
        {
            x=rand1();//using rng function to simualte error - chaning bits with certain probability
            if (x <= prawdopodobienstwo)
            {
                tablica[i] = (tablica[i] + 1) % 2;//fliping bit
                ile_bledow++;//counting errors
            }
        }
    }
    return ile_bledow;
}

int State_error(int* table_base,int * table_state, int size, double rate) //function that simulates source and channel error
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
                table_base[i] = (table_base[i] + 1) % 2;//this changes with rate,hovewer because here error can change hole state, when chaning base the state is flipped randomly
                table_state[i] = rand3(0, 1);//because the state in BB84 is ideally simetrical - psi=1/sqrt(2)*(|0>+|1>)
                ile_bledow++;
            }
        }
    }
    return ile_bledow;
}

//making quantum measurment with given set of bases. A1 - bases in which state was emmited and E1 - bases chosen for measurment
int odczyt(int tablica_E1[],int tablica_E2[],int tablica_A1[],int tablica_A2[],int n)
{
    int k=0;
    for (int i = 0; i < n; ++i) // Creating a table of measurement results in databases from table_E1
    {
        if (tablica_E1[i] == tablica_A1[i] )
        {
            tablica_E2[i] = tablica_A2[i]; // If the measurement base is the same as Alice's base, the result is unchanged
            k++;
        }
        else
        {
            tablica_E2[i] = rand3(0,1); // If the measurement base is opposite to the base in tablica_A1, the result is random
        }
    }
    return k;
}

void kopiuj(int tablica1[], int tablica2[], int n)//simplce function to copy arrays
{
    for (int i = 0; i < n; i++)
    {
        tablica1[i] = tablica2[i];
    }
}

void printArray(int *arr, int size)//simple function to print array
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
    int n = 5000; //the number of bits in the key
    int m;          //the length of the string in the file
    bool* test = load_QTlist(m);
    double srednia1=0,srednia2=0,srednia3=0,srednia4=0,srednia5=0,srednia6=0,srednia7=0,srednia8=0;
    const double alpha1 = 0.05; //source error for Alice
    const double alpha2 = 0.05; //chanel error for Alice
    const double alpha3 = 0.05; //chanel error for Eve 
    const double alpha4 = 0.05; //detector error for Bob

    double ile_bledow1=0, ile_bledow2=0, ile_bledow3=0, ile_bledow4=0, ile_zna=0;
    int key_errors, key_errors_noE;

    int* tablica_A1= new int[n]; // Alice's table with random bits - bases
    int* tablica_A2= new int[n]; // Alice's table with random bits - states
    int* tablica_A12= new int[n]; //table with bases after channel error
    int* tablica_A21= new int[n]; // table with states after channel error
    int* tablica_A12c= new int[n]; //copy of table with bases after chanel error
    int* tablica_A21c= new int[n]; //copy of table with states after chanel error
    int* tablica_B1= new int[n]; //Bob's table with random bits - measurment bases
    int* tablica_B2= new int[n]; //Bob's table with measured states
    int* tablica_E1= new int[n]; //Eve's table with  bits - measurment bases
    int* tablica_E2= new int[n]; //able of measurement results in databases from table_E1 - for the same database as Alice's database,
                                 //the status is unchanged; for opposite base perfect draw 50%/50%
    int* tablica_E3= new int[n]; //table of bases to be sent to bob; populated based on the selected strategy
    int* tablica_E4= new int[n]; //state table to be given to bob; populated based on the selected strategy
    int* tablica_noE= new int[n];
    vector<int> key1A;//vecotr with generetaed key on Alice side
    vector<int> key1B;//vecotr with generetaed key on Bob side

    cout<<"Select a scenario:"<<endl<<"without Eve-1 , with Eve-2"<<endl; //user chooeses if to simulate Eve-an eavesdropper
    int x;
    cin>>x;
    char w;
    if(x==2)
    {
        cout << "Choose Eve's strategy: " << endl << " a ) sends vertical base and only 1 states " << endl << " b ) sends random bases and random states " << endl;
        cout<< " c ) sends chosen bases and measured states" << endl;
        cout << " d ) sends random base and bits from discretization of sin^2(x) " << endl;//user chooses Eve's strategy - what to send after measurment
        cin>>w;
        cout<<"QBER    "<<"QBER   "<<" QBER     "<<"QBER     "<<"%errors "<<" % Eve's inf"<<"  # errors intr."<<endl;
        cout<<"source's: "<<"channel's: "<<"dector1's: "<<"detector2's: "<<"key's:"<<"  about key: "<<" by Eve:"<<endl;
    }
    if(x==1)
    {
        cout<<"QBER    "<<"QBER   "<<" QBER    "<<"% bledow"<<endl;
        cout<<"source's: "<<"channel's: "<<"dector's: "<<"key's:"<<endl;
    }


    int sim_time=2000;
    for(int j=0; j<sim_time; j++)
    {
        GenerateFromQTlist(tablica_A1, n,test, m); // generating random Alice bases
        GenerateFromQTlist(tablica_A2, n,test, m); // generating random Alice bits
        kopiuj(tablica_A21,tablica_A2,n);
        kopiuj(tablica_A12,tablica_A1,n);
        ile_bledow1=State_error(tablica_A12, tablica_A21, n, alpha1);   //adding source error
        ile_bledow2=State_error(tablica_A12, tablica_A21, n, alpha2);   //adding channel error

        if(x==2)
        {
            kopiuj(tablica_A12c,tablica_A12,n); //  copy of bases
            kopiuj(tablica_A21c,tablica_A21,n); //copy of bits
            GenerateFromQTlist(tablica_E1, n,test, m); //choosing random bases for Eve's measurment
            ile_zna=odczyt(tablica_E1,tablica_E2,tablica_A12,tablica_A21,n); // counting how many bits Eve knows

            if (w == 'a')//diffrent strategies for what Eve is sending
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

        GenerateFromQTlist(tablica_B1,n,test,m);            // generating random bases for Bob
        odczyt(tablica_B1,tablica_B2,tablica_A12,tablica_A21,n);
        ile_bledow4=symulujBledy(tablica_B2, n, alpha4);     // Simulating dector error for Bob

        //next we compile keys as per BB84 protocol and then make statistic of errors and keys
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
    cout<<"mean:"<<endl<<endl<<"QBER source's error:         "<<srednia1/sim_time<<endl<<"QBER channel's error:         "<<srednia2/sim_time<<endl;
    cout<<"QBER detector1's error:     "<<srednia3/sim_time<<endl<<"QBER detector2's error:     "<<srednia4/sim_time<<endl;
    cout<<"% key error:                       "<<srednia5/sim_time<<endl<<"% of information Eve knows:   "<<srednia6/sim_time<<endl;
    cout<<"% errors introduced by Eve:      "<<srednia7/sim_time<<endl; 
    cout << "total QBER:" << srednia8 / sim_time << endl;
}
