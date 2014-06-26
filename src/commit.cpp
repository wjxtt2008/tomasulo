#include<vector>
#include<iostream>
#include"tomasulo.h"
using namespace std;

extern const int num_iR=36;			
extern const int num_fpR=36;			
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
extern RS_entry e_m;
extern RS_entry m_w;
extern RS_entry w_c;

extern int clk;	
extern vector<inst>instruction;
vector<RS_entry> cmt_vector;
void commit()
{
	if(w_c.empty==0){
		w_c.empty=1;
		
		cmt_vector.push_back(w_c);
	}
	if(!cmt_vector.empty()){
		//find earlierest inst
		int min=0;
		for(int j=0;j<cmt_vector.size();j++){
			//if(cmt_vector[min].time>cmt_vector[j].time)
			if(instruction[cmt_vector[min].icount].t_issue >instruction[cmt_vector[j].icount].t_issue)
				min=j;
		}
		//cout<<"cmt_vector "<<cmt_vector[min].d_r.id<<"rob_head "<<ROB->table[ROB->head].id<<endl;
		if(cmt_vector[min].d_r.id==ROB->table[ROB->head].id){
			//commit min
			instruction[cmt_vector[min].icount].t_commit=clk;
			instruction[cmt_vector[min].icount].cmt=true;
			
			cout<<"commit "<<instruction[cmt_vector[min].icount].name<<" value: "<<cmt_vector[min].d_r.value<<endl;

			/*************** reuse ROB **************/
			ROB->table[ROB->head].cmt_flag=1;
			ROB->head=(ROB->head+1)%num_ROB;
			ROB->size--;

			for(int j=0;j<num_iR;j++) IntArf->table[j].value=(int)IntRat->table[j].value;
			for(int j=0;j<num_fpR;j++)  FpArf->table[j].value=FpRat->table[j].value;

			vector<RS_entry>::iterator it_cmt_vector = cmt_vector.begin();
			cmt_vector.erase(it_cmt_vector+min);
		}
	}
}