#include<fstream>
#include<vector>
#include"tomasulo.h"
using namespace std;

const int num_iR=36;			
const int num_fpR=36;			
extern int num_ROB;			

extern int num_addiRS;
extern int num_addfRS;
extern int num_mulfRS;
extern int num_memRS;

extern int cycle_addi;
extern int cycle_addf;
extern int cycle_mulf;
extern int cycle_mem_exe;
extern int cycle_mem_mem;

extern int num_addi;
extern int num_addf;
extern int num_mulf;
extern int num_mem;

extern Operation opcode;
extern item dst,src,tgt;
extern InstBuf ib;
extern IntARF *IntArf;
extern FpARF *FpArf;
extern IntRAT *IntRat;
extern FpRAT *FpRat;
extern ReOrderBuf *ROB;
extern RS *addiRS,*addfRS,*mulfRS,*memRS;
extern ADDIER *addier, *memer, *memer2;
extern ADDFER *addfer;
extern MULFER *mulfer;

extern int clk;		

const int MAX_BUFFER = 65535;
const char * DELIM0=" \t";
const char * DELIM1=" =,[]";
extern vector<mem_unit> memory;

IntARF::IntARF(int N)
{
	table = new ARF<int>[N];
	for(int j=0;j<N;j++){
		table[j].id=j+1;
		table[j].ready=0;
		table[j].value=0;
	}
	pointer=0;
}

FpARF::FpARF(int N)
{
	table = new ARF<float>[N];
	for(int j=0;j<N;j++){
		table[j].id=(j+101);
		table[j].ready=0;
		table[j].value=0.0;
	}
	pointer=0;
}

IntRAT::IntRAT(int N)
{
	table = new RAT[N];
	for(int j=0;j<N;j++) {
		table[j].alias = IntArf->table[j].id;
		table[j].value = IntArf->table[j].value;
	}
	pointer=0;
}

FpRAT::FpRAT(int N)
{
	table = new RAT[N];
	for(int j=0;j<N;j++) {
		table[j].alias = FpArf->table[j].id;
		table[j].value = FpArf->table[j].value;
	}
	pointer=0;
}

ReOrderBuf::ReOrderBuf(int N)
{
	table = new ReOrderBuf_entry[N];
	for(int j=0;j<N;j++){
		table[j].id=j+201;
		table[j].dst_id=0;
		table[j].value=0.0;
		table[j].cmt_flag=0;
	}
	head=0;
	tail=0;
	n = N;
	size=0;
}
int ReOrderBuf::get_size()
{
	return size;
}
bool ReOrderBuf::empty(){
	if (size== 0) return true;
	else return false;
}
bool ReOrderBuf::full() {
	if(size==n) return true;
	else return false;
}

item::item()
{
	value=0.0;
	id=0;
	imme_flag=0;
	ready=0;
}

RS::RS(int N)
{
	table= new RS_entry[N];
	head=tail=0;
	//size = 0; 
	n = N;
	size=0;
}
int RS::get_size()
{
	return size;
}
bool RS::empty(){
	if (size== 0) return true;
	else return false;
}
bool RS::full() {
	if(size==n) return true;
	else return false;
}
ADDIER::ADDIER()
{
	cycle = cycle_addi;
	empty = 1;
}

ADDFER::ADDFER()
{
	line = new RS_entry[cycle_addf];
	head=tail= 0;
	cycle = cycle_addf;
	n=cycle;
	size=0;
}
int ADDFER::get_size()
{
	return size;
}
bool ADDFER::empty(){
	if (size== 0) return true;
	else return false;
}
bool ADDFER::full() {
	if(size==n) return true;
	else return false;
}
MULFER::MULFER()
{
	line = new RS_entry[cycle_mulf];
	head=tail= 0;
	cycle = cycle_mulf;
	n=cycle;
	size=0;
}
int MULFER::get_size()
{
	return size;
}
bool MULFER::empty(){
	if (size== 0) return true;
	else return false;
}
bool MULFER::full() {
	if(size==n) return true;
	else return false;
}
void initial()
{
	ifstream config;
	//config.open("C:\\Users\\Administrator\\Desktop\\tomasulo\\config.txt");
	//config.open("D:\\VS2008Projects\\tomasulo\\config.txt");
	config.open("config.txt");

	src.value=0.0;
	src.imme_flag=0;
	tgt.value=0;
	tgt.imme_flag=0;
	ib.free=1;

	/* read from config.txt */
	/************************/
	if (config.fail()) {
		cout << "Can't open file: config.txt" << endl;
		exit(-1);
	}

	char buf[MAX_BUFFER];
	config.getline(buf, MAX_BUFFER);//skip the first line
	for (int i = 0; i <= 3; i++) {//read the first table with 4 rows
		config.getline(buf, MAX_BUFFER);
		char * word;
		vector<char *> words;
		string token, token1;
		word = strtok(buf, DELIM0);//DELIM0=" \t"
		while (word) {
			words.push_back(word);
			word = strtok(NULL, DELIM0);
		}

		//parse the first token
		token = words[0];
		if ((token.at(0) == 'I') || (token.at(0) == 'i')) {//Integer adder
			//parse the second token
			token1 = words[1];
			if ((token1.at(0) == 'A') || (token1.at(0) == 'a')) {
				num_addiRS=atoi(words[2]);
				cycle_addi=atoi(words[3]);
				num_addi=atoi(words[4]);
			}
		} else if ((token.at(0) == 'F') || (token.at(0) == 'f')) {
			token1 = words[1];
			if ((token1.at(0) == 'A') || (token1.at(0) == 'a')) {//if FP adder
				num_addfRS=atoi(words[2]);
				cycle_addf=atoi(words[3]);
				num_addf=atoi(words[4]);
			}
			else {//must be FP multiplier
				num_mulfRS=atoi(words[2]);
				cycle_mulf=atoi(words[3]);
				num_mulf=atoi(words[4]);
			}
		} else {//must be load/store unit
			num_memRS=atoi(words[2]);
			cycle_mem_exe=atoi(words[3]);
			cycle_mem_mem=atoi(words[4]);
			num_mem=atoi(words[5]);
		}
	}
	/************************/
	IntArf = new IntARF(num_iR);
	
	
	FpArf = new FpARF(num_fpR);
	FpRat = new FpRAT(num_fpR);


	addiRS = new RS(num_addiRS);
	addfRS = new RS(num_addfRS);
	mulfRS = new RS(num_mulfRS);
	memRS = new RS(num_memRS);
	addier = new ADDIER[num_addi];
	memer= new ADDIER;
	memer2= new ADDIER;
	addfer = new ADDFER[num_addf];
	mulfer = new MULFER[num_mulf];

	/* if there are some initial values, add here. */
	/************************/
	/************************/
	config.getline(buf, MAX_BUFFER);
	//go to second table
	for (int i = 0; i <=2; i++) {//read the second table with 3 rows
		config.getline(buf, MAX_BUFFER);
		char * word;
		vector<char *> words;
		string token, token1;
		word = strtok(buf, DELIM1);//DELIM1=" =,[]"
		while (word) {
			words.push_back(word);
			word = strtok(NULL, DELIM1); //DELIM1
		}
		token = words[0];
		if ((token.at(0) == 'R') && (token.at(1) == 'O') && (token.at(2) == 'B'))  {//ROB
			num_ROB=atoi(words[2]);//Number of ROB entries
		} else if (((token.at(0) == 'R') && (token.at(1) != 'O')) || (token.at(0) == 'F')) {//initial registers
			int k = 0;
			int max = words.size();
			while (token.at(0) == 'R') {//assign initial integers
				int index;
				index = token.at(1) - '0';//get the index of registers
				IntArf->table[index].value = atoi(words[1+k]);//next is the value
				k = k + 2;
				if (k <= max-1)	token = words[k];
				else break;
			}//end while for integers
			//for(int j=0;j<36;j++) cout<<IntArf->table[j].id<<"	"<<IntArf->table[j].value<<endl;
			while (token.at(0) == 'F') {
				int index;
				float fp_value;
				index = token.at(1) - '0';
				if(token.at(2)!=NULL)index=index*10+token.at(2) - '0';
				//cout<<token<<endl;
				FpArf->table[index].value = atof(words[1+k]);
				k = k + 2;
				if (k <= max-1)	token = words[k];
				else break;
			}//end while for floating points
			//for(int j=0;j<36;j++) cout<<FpArf->table[j].id<<"	"<<FpArf->table[j].value<<endl;
		} else {//Memory
			int k = 0;
			int max = words.size();
			while (token.at(0) == 'M') {
				int index;
				index = atoi(words[1+k]);
				mem_unit memtmp=make_pair(index,atof(words[2+k]));
				memory.push_back(memtmp);
				k = k + 3;
				if (k <= max-2) token = words[k];
				else break;
			}
		}
	}
	config.close();

	FpRat = new FpRAT(num_fpR);
	IntRat = new IntRAT(num_iR);
	ROB = new ReOrderBuf(num_ROB);
	//for(int j=0;j<num_ROB;j++) cout<<ROB->table[j].id<<":	"<<ROB->table[j].dst_id<<endl;
}



