//HUFFMAN ENCODING AND DECODING
//STEPS 1: GET THE FREQUNECY OF CHARACTERS FROM THE INPUT
//STEPS 2: BUILD THE HUFF TREE AND AND ASSIGN BINARY CODE TO EACH CHARACTER
//STEPS 3: REPLACE THE CHARACTER WITH BINARY CODE
//STEPS 4: DECOMPRESS THE TEXT BY DEECODING IT
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;
//create a class for Node
//left edges will get 0 and right will get 1 
class Node
{
	//required data items
	public:
	char charac; int f; Node* left; Node* right;
};
//ASSIGN A NODE
Node* ass(char c, int fr, Node* left, Node* right)
{
	Node* n = new Node();
	n->charac=c; n->f=fr; n->left=left; n->right=right;
	return n;
}
//get the lowest freq from priority , for that we will use operaotr overloading
class lowest
{
	public:
	bool operator()(Node* lt, Node* rt)
	{
		return lt->f  >  rt->f;
	}
};
//encode the text
void encode(Node* r, string s, unordered_map<char, string> &h)
{
	if(r==nullptr)
	{
		return;
	}
	if(!r->left && !r->right)
	{
		h[r->charac]=s;
	}
	//rec call to append 0 to left and 1 to right
	encode(r->left, s+"0", h); encode(r->right, s+"1", h);
}
void decode(Node* r, int &i, string st, ofstream& outputfile)
{
	if(r==nullptr)
	{
		return;
	}
	if (!r->left && !r->right)
	{
//		h[r->charac]=s; 
		outputfile<<r->charac;
		cout<<r->charac; return;
	}
	//i iterated to +1 as it will b initialized to -1
	i=i+1;
	if(st[i]!='0')
	{
		decode(r->right, i, st, outputfile);
	}
	else
	{
		decode(r->left, i, st, outputfile);
	}
}
void HUFFMAN(string i)
{
	int total;
	//we willuse unordermap to create a space which will store value for a key and its freq
	unordered_map<char, int> fy;
	//use a range based for loop, getting the fre of characters
	for(char c: i)
	{
		fy[c]++;
	}
	//store the leaf nodes using priority queue
	priority_queue<Node* , vector <Node* >, lowest> PQ; //now we will create a leaf node for our text characters
	for(auto p: fy)
	{
		PQ.push(ass(p.first, p.second, nullptr, nullptr));
	}
	//we have to remove two lowest lowest freq charac and add them tog
	while(PQ.size()!=1)
	{
		Node* left = PQ.top(); PQ.pop(); Node* right = PQ.top(); PQ.pop();
		total = left->f + right->f; PQ.push(ass('\0', total, left, right));
	}
	Node* r = PQ.top();
	//now game starts, we have to store HUFFMAN codes in unordered map
	unordered_map<char, string> HC;
	// call encode
	encode(r, "", HC);
	cout<<"The [HUFF CODES] of your text are as follows: "<<endl;

	ofstream outputfile;
    outputfile.open ("Encoding File.txt");
	outputfile<<"The [HUFF CODES] of your text are as follows: "<<endl<<endl;
	for(auto p: HC)
	{
		cout<<p.first<<" -> "<<p.second<< endl;
        
		outputfile<<p.first;
        outputfile<<"->"<<p.second<<endl;

	}
	outputfile.close();

	cout<<endl<<endl<<"And your original text was: "<<i<< endl;
	string sr = "";
	for(char c: i)
	{
		sr= sr+HC[c];
	}
	cout <<endl<<endl<< "COMPRESSED STRING IS: " << sr << '\n';
	int j = -1;
	cout<<endl<<endl<<"THE DECOMPRESSED STRING IS: "<<endl; 
	

	outputfile.open ("Decoding File.txt");
	outputfile<<"THE DECOMPRESSED STRING IS: "<<endl<<endl;
	
	int size_ = sr.size();
	while(j<(size_-2))
	{
		decode(r, j, sr, outputfile);
	}
	outputfile.close();
}
int main()
{
	cout<<"\t\t\tWELCOME PALS ON HUFFMAN ALGORITHM"<<endl;
	string input, myText;
	ifstream a("Myfile.txt");


	if(a.is_open())
	{
		while (getline (a, myText)) 
	{
	  // Output the text from the file
	  cout << myText;
	}
	
	
	//input = "LIFE IS IN THE HANDS OF ALLAH";
	HUFFMAN(myText);
	a.close();
	}
	else cout << "Unable to open file";
	
}




