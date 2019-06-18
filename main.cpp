#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

//data structures used in implementation
map <string, string> vars; //map of the variable names and their encodings
map <string, int> bin; //map of the number of bits of variables
map <string, vector <string>> symbols; //map of the variables and their values

vector <int> arr;
int GCD;

//functions used in implementation
void openFile (string filename);
void parse (ifstream & inp);
void output (string filename);
int scanFile (string filename);
void lastOne (int max);

//taken from geeks for geeks
//used to find gcd
int gcd(int a, int b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

//taken from geeks for geeks
// Function to find gcd of array of number
int findGCD(vector <int> arr)
{
    int result = arr[0];
    for (int i = 1; i < arr.size(); i++)
        result = gcd(arr[i], result);
    
    return result;
}

//taken from geeks for geeks.. converts from binary to decimal number
int binaryToDecimal(long long n)
{
    long long num = n;
    int dec_value = 0;
    
    // Initializing base value to 1, i.e 2^0
    int base = 1;
    
    long long temp = num;
    while (temp)
    {
        int last_digit = temp % 10;
        temp = temp/10;
        
        dec_value += last_digit*base;
        
        base = base*2;
    }
    
    return dec_value;
}


int main(){

    string in, out;
    cout << "Please input a vcd file name\n";
    cin >> in;
    cout << "Please input the name of the html output file\n";
    cin >> out;
    
    openFile(in);
    output(out);
    
//    openFile("DualPortedMem_tb.vcd");
//    output("dualPorted_out.html");

//    openFile("RegisterFile_tb.vcd");
//    output("RF_out.html");

//    openFile("adder_tb.vcd");
//    output("adder_out.html");
    
//    openFile("counter_tb.vcd");
//    output("counter_out.html");
    
//    openFile("carrySaveAdder_tb.vcd");
//    output("CSAadder_out.html");
    
//    openFile("multiplier_tb.vcd");
//    output("multiplier_out.html");
    
    return 0;
}

void openFile (string filename){
    GCD = scanFile(filename); //finding GCD of time
    ifstream inp;
    inp.open(filename.c_str());
    if(inp.is_open()){
        parse(inp);
    }
    else {
        cout << "Error opening file\n";
    }
}

void parse (ifstream & inp){
    string t;
    inp >> t;
    int hash = 1;
    int max = 0;
    while (!inp.eof()){
        if (t=="$var"){ //start of defining each variable
            inp >> t;
            inp >> t;
            int bits;
            bits = stoi(t); //number of bits of variable
            string symbol; //encoding for variable
            inp >> symbol;
            inp >> t; //variable name
            
            vars.insert(pair<string,string>(symbol,t));
            bin.insert(pair<string,int>(symbol,bits));
            vector <string> temp;
            symbols[symbol] = temp;
        }
        if (t=="#0"){ //starting the waveform values
            inp >> t;
            inp >> t;
            while (!inp.eof()){
                if (t[0]!='#' && t!="$end"){
                    if (t[0]!='b'){ //1 bit variables
                        string c;
                        for (int i=1; i<t.length(); i++){ //getting the encoding
                            c += t[i];
                        }
                        string temp = "";
                        temp += t[0]; //value of variable
                        symbols[c].push_back(temp); //pushing to vector of values
                        if (symbols[c].size() > max) //getting maximum size of vector of a variable
                        {
                            max = symbols[c].size();
                        }
                    }
                    else if (t[0]=='b'){ //more than 1 bit variables
                        string value = "";
                        for (int i=1; i<t.length(); i++){ //getting value
                            value += t[i];
                        }
                        int v;
                        if (value != "x" && value!= "z"){
                            v = binaryToDecimal(stol(value)); //converting value to decimal
                            inp >> t; //getting encoding
                            symbols[t].push_back(to_string(v));
                        } else {
                            inp >> t; //if x or z
                            symbols[t].push_back(value);
                        }
                        if (symbols[t].size() > max) //getting max size of vector of a variable
                        {
                            max = symbols[t].size();
                        }
                    }

                }
                if (t[0]=='#' && t!="#"){ //delay
                    
                    max = arr[hash] / GCD;
                    
                    //makes all vectors of all variables the same size according to the ratio of the delay
                    map<char, vector <int>>::iterator it;
                    for ( auto it = symbols.begin(); it != symbols.end(); ++it  )
                    {
                        while(it->second.size() < max){
                            if (it->second.size() == 0)
                                it->second.push_back("x");
                            else
                                it->second.push_back(it->second[it->second.size()-1]);
                        }
                        
                    }
                    hash++;
                }
                inp >> t;
            }
        }
        inp >> t;
    }
    lastOne(max); //makes all vectors of all variables the same size
}


void output (string filename){
    ofstream out;
    out.open(filename.c_str());
    if (out.is_open()){
        //output format of HTML file
        out<< "<html>\n<head>\n<script src=\"http://wavedrom.com/skins/default.js\" type=\"text/javascript\"></script>\n<script src=\"http://wavedrom.com/wavedrom.min.js\" type=\"text/javascript\"></script>\n</head>\n<body onload=\"WaveDrom.ProcessAll()\">\n<script type=\"WaveDrom\">\n{ signal : [ "<< endl;
        //looping over map of variables to output the JSON format
        map<char, vector <int>>::iterator it;
        for ( auto it = symbols.begin(); it != symbols.end(); ++it  )
        {
            string name;
            name = vars[it->first];
            string values="";
            string data="";
            for (int i=0; i<it->second.size(); i++){
                if (bin[it->first] == 1){ //if 1 bit
                    if (i==0){
                        values = it->second[i];
                    }
                    else if (it->second[i] == it->second[i-1]){
                        values += ".";
                    }
                    else {
                        values += it->second[i];
                    }
                }
                else if (bin[it->first] > 1) //if more than 1 bit
                {
                    int count = 3;
                    if (i==0){
                        if (it->second[i] == "x"){
                            values = "x";
                        }
                        else {
                            values = to_string(count);
                            data += it->second[i];
                            count ++;
                        }
                    }
                    else if (it->second[i] == it->second[i-1]){
                        values += ".";
                    }
                    else {
                        if (it->second[i] == "x"){
                            values += "x";
                        }
                        else {
                            values += to_string(count);
                            if (data!="")
                                data += " ";
                            data += it->second[i];
                            count++;
                        }
                    }
                }
            }
            if (bin[it->first] == 1){
                out << "\n{name: \"" << name <<"\", wave: \"" << values << "\" },";
            }
            else if (bin[it->first] > 1)
            {
                out << "\n{name: \"" << name <<"\", wave: \"" << values << "\", data: \""<< data << "\" },";
            }
        }
        
        out << "]}\n</script>\n</body>\n</html>"<<endl;
    } else {
        cout << "Error opening output file" << endl;
    }
    
}

//scan input file to get GCD of time
int scanFile (string filename){
    ifstream inp;
    inp.open(filename.c_str());
    if (inp.is_open()){
        while (!inp.eof()){
            string t;
            inp >> t;
            if (t[0] == '#' && t!= "#"){
                t.erase(t.begin());
                arr.push_back(stoi(t));
            }
        }
        return findGCD(arr);
    }
    else {
        cout << "error opening file" << endl;
        return -1;
    }
}

//makes all vectors of all variables the same size
void lastOne(int max){
    map<char, vector <int>>::iterator it;
    for ( auto it = symbols.begin(); it != symbols.end(); ++it  )
    {
        while(it->second.size() < max){
            if (it->second.size() == 0)
                it->second.push_back("x");
            else
                it->second.push_back(it->second[it->second.size()-1]);
        }
        
    }
}
