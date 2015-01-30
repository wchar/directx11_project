#include <iostream>
#include <hash_map>
#include <functional>

using namespace std;

void f()
{
    cout << "asd" << endl;
}

int main()
{
    function<void(int)> a = [](int b){ f();  };
    a(1);
    //hash_map<const char *, int, hash<const char *>, eqstr> months;
    hash_map<char*, int> IntHash;
    IntHash["asd"] = 1;
    cout << IntHash["asd"] << endl;
    //cout << "march -> " << months["march"] << endl;
	getchar();
	return 0;
}

