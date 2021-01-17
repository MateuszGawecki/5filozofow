#include <iostream>
#include <thread>//wątki
#include <string>
#include <mutex>//synchronizacja
#include <vector>//kontener
#include <chrono>// czas
#include <memory>//smart pointery

using namespace std;

class filozof
{
    private:
        string imie;
        mutex* lewy_widelec;
        mutex* prawy_widelec;

    public:
    filozof(string n,mutex* lw,mutex* pw);
    void jedz();
    void mysl();
    void akcja();
};

filozof::filozof(string n, mutex* lw, mutex* pw)
{
    imie=n;
    lewy_widelec=lw;
    prawy_widelec=pw;
}

void filozof::akcja() //cykl życia filozofa
{
    if(lewy_widelec && prawy_widelec)
    {
        int n=10; // ile razy wykonaja akcje
        for(int i=0;i<n;i++)
        {
            mysl();
            jedz(); 
        }
    }
}

void filozof::jedz()
{
    if(lewy_widelec && prawy_widelec)
    {
        //tworzymy unikatowe blokowanie ale jeszcze tego nie robimy
        unique_lock<mutex> lewy(*lewy_widelec, defer_lock);
        unique_lock<mutex> prawy(*prawy_widelec, defer_lock);

        lock(lewy,prawy); //blokujemy oba na raz , nie dojdzie do zakleszczenia

        cout<<"\n"<<imie<<" Zaczyna jesc\n";
        this_thread::sleep_for(chrono::milliseconds(2000));
        cout<<"\n"<<imie<<" Konczy jesc\n";


        prawy.unlock();
        lewy.unlock();//zwalniamy widelce
    }
}

void filozof::mysl()
{
    cout<<"\n"<<imie<<" Zaczyna myslec\n";
    this_thread::sleep_for(chrono::milliseconds(2000));  //czekamy 2s
    cout<<"\n"<<imie<<" Konczy myslec\n";
}

int main()
{
    int ile=5;
    string t[5]={"A","B","C","D","E"};

    vector< shared_ptr<mutex> > widelce; //tworzymy vector widelców i je dodajemy
    for(int i=0; i<ile;i++)
    {
        widelce.push_back(make_shared<mutex>());
    }

    vector< shared_ptr<filozof> > filozofowie; //tworzymy vector filozofów , dodajemy ich oraz odpowiadajace im widelce

    for(int i=0;i<ile;i++)
    {
        if(i==4)
        {
            filozofowie.push_back(make_shared<filozof>(t[i],widelce[i].get(),widelce[0].get()));
        }
        else
        {
            filozofowie.push_back(make_shared<filozof>(t[i],widelce[i].get(),widelce[i+1].get()));
        }   
    }

    vector<thread> th; //tworzymy vector wątków i je dodajemy każdy wątek wykonuje symulacje kolejnego filozofa
    for(int i=0;i<ile;i++)
    {
        th.push_back(thread(&filozof::akcja,filozofowie[i]));
    }

    for (auto &t : th)
    {
        t.join(); //czekamy aż wątki sie zakończą
    }

    return 0;
}