#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

class archiver
{
private:
    int* repetitions=NULL;
    char* characters=NULL;
    string* codes = NULL;
    int size=0;
    class node
    {
    public:
        int weight = 0;
        char character = '\0';
        class node* left = NULL;
        class node* right = NULL;
    };
    node* treehead=NULL;

    void dict_init(string input)//эта функция составляет список встречающихся символов и колво их повторений
    {
        for (int j = 0; j < input.size();j++)// чтение входных данных
        {
            int i = 0;
            while (i < size and characters[i] != input[j])i++; // цикл пока не дойдем до конца массива букв или не найдем совпадение
            if (i != size) repetitions[i]++;//случай если нашли совпадение
            else//нет совпадений - добавляем букву в словарь
            {
                int* rep_clone = new int[size+1];//массив на 1 больше
                char* ch_clone = new char[size + 1];//массив на 1 больше
                for (i = 0; i < size; i++)//клонируем значения которые есть в нынешнем массиве в оба
                {
                    rep_clone[i] = repetitions[i];
                    ch_clone[i] = characters[i];
                }
                rep_clone[i] = 1;//добавляем новое в оба. Изначально 1 повторение (логично)
                ch_clone[i] = input[j];
                size++;
                delete[] repetitions;
                delete[] characters;//в помойку старый массив
                repetitions = rep_clone;//присваиваем указатель на новый массив
                characters = ch_clone;
            }
        }
        codes = new string[size];
    }

    void build_tree()
    {
        node** ptrs = new node* [size];
        int tempsize = size;
        for(int i = 0; i<tempsize;i++)
        {
            ptrs[i] = new node;
            ptrs[i]->weight=repetitions[i];
            ptrs[i]->character=characters[i];
        }

        while(tempsize>1)
        {
            node* min1 = ptrs[0];
            node* min2 = ptrs[0];
            for(int i = 0;i<tempsize;i++)
            {
                if(min1->weight>ptrs[i]->weight) min1=ptrs[i];
            }
            int i = 0;
            while(min1==min2) min2 = ptrs[i++];
            for(int i = 0;i<tempsize;i++)
            {
                if( (min2->weight>=ptrs[i]->weight) and (ptrs[i]!=min1)) min2=ptrs[i];
            }
            node** copy = new node* [tempsize-1];
            int j = 0;
            for(int i = 0;i<tempsize;i++)
            {
                if(ptrs[i]!=min1 and ptrs[i]!=min2) copy[j++]=ptrs[i];
            }
            tempsize--;
            copy[j]=new node;
            copy[j]->weight = min1->weight + min2->weight;
            copy[j]->left = min1;
            copy[j]->right = min2;
            delete[] ptrs;
            ptrs = copy;
        }
        treehead = *ptrs;
    }

    void traverse_tree_char(node* node, string code)
    {
        if (!node) return;
        if (node->character)
        {
            int i = 0;
            for(i;i<size and characters[i]!=node->character;i++);
            codes[i]=code;
        }
        traverse_tree_char(node->left, code+'1');
        traverse_tree_char(node->right, code+'0');
    }

    string encode(string input)
    {
        unsigned char wroot= 0;
        int lecounter = 7;
        string output;
        output+=size;
        for(int i = 0;i<size;i++)
        {
            output+=repetitions[i];
            output+=characters[i];
        }
        for(int j = 0;j<input.size();j++)
        {
            int u = 0;
            while(input[j]!=characters[u])u++;
            for(int i = 0;i<codes[u].length();i++)
            {
                if(lecounter==-1)
                {
                    output+=wroot;
                    wroot=0;
                    lecounter = 7;
                }
                if(codes[u][i]=='0')
                {
                    lecounter--;
                }
                if(codes[u][i]=='1')
                {
                    wroot=wroot|(1<<lecounter);
                    lecounter--;
                }
            }
        }
        output+=wroot;
        return output;
    }

    string decode(string input)
    {
        int k = 0;
        string output="";
        unsigned char read;
        unsigned char bytes;//это размер словаря в байтах
        bytes = input[0];
        size = bytes;
        characters = new char [size];
        repetitions = new int [size];
        int j = 1;
        for (int i =0;i<bytes;i++)
        {
            repetitions[i] = (int)input[j++];
            characters[i] = (int)input[j++];
        }
        build_tree();
        int lecounter;
        node* node=treehead;
        for(j;j<input.size();j++)
        {
            lecounter=7;
            while(lecounter>-1)
            {
                if(node->character)
                {
                    output+=node->character;
                    if((node->character)=='O')
                    {
                    }
                    node=treehead;
                    
                }
                if(input[j] >> lecounter & 1)
                {
                    node=node->left;
                }
                else
                {
                    node=node->right;
                }
                lecounter--;
            }
            if(node->character)
            {
                output+=node->character;
                node=treehead;
            }
        }
        return output;
    }
    int comparesizes(bool do_logs)
    {
        int uncompsize = 0;
        int compsize=0;
        for(int i = 0; i < size; i++)
        {
            compsize+=repetitions[i]*codes[i].length();// размер сжатого файла в битах
        }
        compsize+= size*8*2+8;//размер словаря в начале файла в битах
        for (int i = 0;i<size;i++)
        {
            uncompsize+=repetitions[i]*8;//размер несжатого файла в битах
        }
        if (do_logs)
        {
            ofstream LOGS;
            LOGS.open("logs.txt", ios_base::app); 
            LOGS << to_string(uncompsize)<<" "<<to_string(compsize)<<endl;
        }
        return(compsize<uncompsize);
    }

    //LZW

    string serialize(vector<int>& inp)
    {
        vector<unsigned char> input;
        for (int i = 0; i < inp.size(); i++)
        {
            input.push_back((int)inp[i] + (INT_MIN*-1));
        }
        string output;
        unsigned char sebuf = 0;
        char bitsize = 9;
        int bufpos = 7;
        int towrite = bitsize - 1;

        for (int i = 0; i < input.size(); i++)
        {

            while (towrite > -1)
            {
                if (input[i] & (int)pow(2, towrite))
                {
                    sebuf += (int)pow(2, bufpos);
                    bufpos--;
                }
                else
                {
                    bufpos--;
                }
                if (bufpos == -1)
                {
                    output.push_back(sebuf);
                    sebuf = 0;
                    bufpos = 7;
                }
                towrite--;
            }
            if (input[i] == 256)
            {
                bitsize++;
            }
            towrite = bitsize - 1;
        }
        output.push_back(sebuf);

        cout << "serialized:" << endl;

        for (int i = 0; i < output.size(); i++)
        {
            cout << (int)output[i] << "-";
        }

        cout << endl;
        cout << "serialized size: " << output.size() * 8 << endl;
        return output;
    }
    vector<int> deserialize(string& inp)
    {
        vector<unsigned char> input;
        for (int i = 0; i < inp.length(); i++)
        {
            input.push_back((int)inp[i] + 128);
        }
        vector<int> output;
        int bitsize = 9;

        int bits = bitsize - 1;

        int desbuf = 0;

        for (int i = 0; i < input.size(); i++)
        {
            int toread = 7;

            while (toread > -1)
            {
                if (input[i] & (int)pow(2, toread))
                {
                    desbuf += (int)pow(2, bits);
                    bits--;
                }
                else
                {
                    bits--;
                }
                if (bits == -1)
                {
                    if (desbuf == 256)
                    {
                        bitsize++;
                    }
                    bits = bitsize - 1;
                    output.push_back(desbuf);

                    desbuf = 0;
                }
                toread--;
            }
        }

        cout << "DEserialized:" << endl;

        for (int i = 0; i < output.size(); i++)
        {
            cout << (int)output[i] << "-";
        }
        cout << endl;
        return output;
    }
    vector<int> pr_compressLZW(string& input)
    {
        vector<int> result;
        vector<string> dict;
        int bitsize;
        for (int i = 0; i < input.length(); i++)
        {
            int j = dict.size() - 1;
            for (j; j > -1 and (input.compare(i, dict[j].length(), dict[j]) != 0); j--);
            if (j == -1)
            {
                dict.push_back(input.substr(i, 2));
                result.push_back(input[i]);
            }
            else
            {
                dict.push_back(dict[j] + input[i + dict[j].length()]);
                /*if (j + 257 > 511)
                {
                    result.push_back(256);
                }*/
                result.push_back(j + 257);
                i = i + dict[j].length() - 1;
            }

        }
        for (int i = 0; i < result.size(); i++)
        {
            cout << result[i] << "-";
        }
        cout << endl;
        return result;
    }
    string pr_decompressLZW(vector<int>& input)
    {
        string outp;
        vector<vector<int>> dickt;
        vector<int>buf;


        outp.push_back(input[0]);
        for (int i = 1; i < input.size(); i++)
        {
            if (input[i] < 257)
            {
                buf.push_back(input[i - 1]);
                buf.push_back(input[i]);
                outp.push_back(input[i]);
                dickt.push_back(buf);
                buf.clear();
            }
            else
            {
                if (input[i] - 257 == dickt.size())
                {
                    if (input[i - 1] < 257)
                    {
                        buf.push_back(input[i - 1]);
                        buf.push_back(input[i - 1]);
                        dickt.push_back(buf);
                    }
                    else
                    {
                        buf = dickt[input[i - 1] - 257];
                        buf.push_back(dickt[input[i - 1] - 257][0]);
                        dickt.push_back(buf);
                    }
                    buf.clear();
                }
                else
                {
                    buf.push_back(input[i - 1]);
                    buf.push_back(dickt[input[i] - 257][0]);
                    dickt.push_back(buf);
                    buf.clear();
                }
                vector<int> outpbuf;
                outpbuf.push_back(input[i]);
                bool flag;
                do
                {
                    flag = false;
                    for (int j = 0; j < outpbuf.size(); j++)
                    {
                        if (outpbuf[j] > 256)
                        {
                            outpbuf.insert(outpbuf.begin() + j + 1, dickt[outpbuf[j] - 257].begin(), dickt[outpbuf[j] - 257].end());
                            outpbuf.erase(outpbuf.begin() + j);
                            flag = true;
                        }
                    }
                } while (flag);
                outp.insert(outp.end(), outpbuf.begin(), outpbuf.end());
                outpbuf.clear();
            }
        }
        for (int i = 0; i < outp.size(); i++)
        {
            cout << char(outp[i]);
        }
        return outp;
    }

public:
    string compressHUFF (string input,bool do_logs)
    {
        dict_init(input);
        build_tree();
        string code = "";
        traverse_tree_char(treehead,code);
        if (comparesizes(do_logs))
        {
            return encode(input);
        }
        else
        {
            cout<<"compression is not possible!"<<endl;
            return "";
        }
    }
    string decompressHUFF(string input)
    {
        string output = decode(input);
        return output;
    }
    string compressLZW(string& input)
    {
        vector<int> result=pr_compressLZW(input);
        string compressed = serialize(result);
        return compressed;
    }
    string decompressLZW(string& input)
    {
        vector<int> desd = deserialize(input);
        string decompressed = pr_decompressLZW(desd);
        return decompressed;


    }

};
int main()
{

    archiver test;
    string message = "FAAAAAK DIIIIICK SUSUSUSUSUUSUS";
    message = test.compressHUFF(message,true);
    cout << endl;
    message = test.compressLZW(message);
    archiver test2;
    message = test2.decompressLZW(message);
    message = test2.decompressHUFF(message);
    cout << message;
    
}
