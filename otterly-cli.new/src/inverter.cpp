#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;

ifstream op;
ofstream out("lf/finalresults.txt", ios::out);
ofstream lfout("lf/lastfile.txt", ios::out);
ifstream lfin("lf/lastfile.txt", ios::in);

class DATACLEANER
{
    public:

        DATACLEANER();
  
        void invert();

        protected:
        int max=3694; //57 less
        int black= 3900;
        int pixel[3694];
        int data[3694];
        string filename;
};

DATACLEANER::DATACLEANER()
{
    cout << "DATACLEANER is cleaning house!"<< endl;
    // out << "I am writing to file" << endl;
    int i;
    system("ls -ltr | grep ^- | tail -1 | awk '{ print $(NF) }' > lf/lastfile.txt");
    lfin >> filename;
    //cout << "The last file created in this directory is " << filename << endl;
    

    op.open(filename ,ios::in); // need to figure out how to get latest output dat file

    for(int i=0; i<max; i++)
    {
        op>>pixel[i]>>data[i];
        
        
    }
    op.close();
    op.clear();
    //cout <<"DATACLEANER! Construct is DATACLEANERing busy!"<<endl;
}

void DATACLEANER::invert()
{
    int i;
    double invertdata[max];
    cout << "DATACLEANER is inverting some things!" << endl;
    for(i=1; i<max; i++)
    {
        invertdata[i]= abs(data[i]-black);
        // cout << "i=" << i << " data[" << i <<"]=" << data[i] << " invertdata[" << i << "]=" << endl;
        out << invertdata[i] << endl;

        

     }
    cout << "DATACLEANER is done.  Check out the file named finalresults.txt in the lf directory." << endl;
}

int main()
    {
        DATACLEANER u;
        u.invert();   

       
        return 0;
    }
