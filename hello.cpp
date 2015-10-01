//C:\Users\Maurice\Documents\Visual Studio 2013\Projects\hello.cpp

#include <iostream>
#include <fstream>
using namespace std;

int main() {
	cout << "Hello world!" << "\n";
	ofstream Test("C:\\Users\\Maurice\\Documents\\tester.csv");
	cout << Test.is_open();
	Test << "This,is,a,..." << endl << "testerfile,for,muh,game";
	Test.close();
	return 0;
}