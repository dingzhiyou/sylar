#include<string>
#include<iostream>
using namespace std;

int main(){
	string str = "123456789123456789";
	str.resize(10);
	cout<<"str.size:"<<str.size()<<endl;
	cout<<"str.capacity:"<<str.capacity()<<endl;
	cout<<str<<endl;

	string str1 = "123456789123456789";
	str1.reserve(10);
	cout<<"str.size:"<<str1.size()<<endl;
	cout<<"str.capacity:"<<str1.capacity()<<endl;
	cout<<str1<<endl;


}

