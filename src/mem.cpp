#include<vector>
#include"tomasulo.h"
using namespace std;

extern int num_iR;			
extern int num_fpR;			
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
extern bool memaccess;
extern ADDIER *memer2;
void mem()
{
	//for(int j=0;j<36;j++) cout<<IntArf->table[j].id<<"	"<<IntArf->table[j].value<<endl;
	if(e_m.empty==0){
		if(instruction[e_m.icount].opcode == load || instruction[e_m.icount].opcode == store){
			if(memer2->empty==1){
				memer2->line=e_m;
				memer2->empty=0;
				memer2->line.time=clk;
				e_m.empty=1;
				instruction[memer2->line.icount].t_mem=clk;
			}
		}else if(m_w.empty==1){
			m_w=e_m;
			m_w.time=clk;
			instruction[e_m.icount].t_mem=clk;
			m_w.empty=0;
			e_m.empty=1;
		}
	}
	if((memer2->empty!=1)&&(clk>=(memer2->line.time+cycle_mem_mem-1))&&m_w.empty==1){
			m_w=memer2->line;
			m_w.empty=0;
			memaccess = 0;
			memer2->empty=1;
			//cout<<e_m.d_r.id <<": "<<e_m.d_r.value<<"	"<<clk<<endl;
			//for(int j=0;j<num_ROB;j++)cout<<ROB->table[j].id<<"	"<<ROB->table[j].dst_id<<endl;
	}
}