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
extern vector<inst>instruction;
extern int clk;	

/**** Use the result id to index the value in 1: ROB, 2: RS ***/
void writeback()
{
	
	if(m_w.empty==0&&w_c.empty==1){
		m_w.time=clk;
		cout<<"write_back"<<instruction[m_w.icount].name<< "value "<<m_w.d_r.value<<endl;

		/************ rewrite ROB ************/
		for(int j=0;j<num_ROB;j++){
			if(ROB->table[j].id==m_w.d_r.id)ROB->table[j].value=m_w.d_r.value;
		}
		/************ rewrite addi RS *********/
		for(int j=0;j<num_addiRS;j++){
			if(addiRS->table[j].s_r.id==m_w.d_r.id){
				addiRS->table[j].s_r.value=m_w.d_r.value;
				addiRS->table[j].s_r.ready=1;
			}
			if(addiRS->table[j].t_r.id==m_w.d_r.id){
				addiRS->table[j].t_r.value=m_w.d_r.value;
				addiRS->table[j].t_r.ready=1;
			}
		}
		for(int j=0;j<num_addfRS;j++){
			if(addfRS->table[j].s_r.id==m_w.d_r.id){
				addfRS->table[j].s_r.value=m_w.d_r.value;
				addfRS->table[j].s_r.ready=1;
			}
			if(addfRS->table[j].t_r.id==m_w.d_r.id){
				addfRS->table[j].t_r.value=m_w.d_r.value;
				addfRS->table[j].t_r.ready=1;
			}
		}
		for(int j=0;j<num_mulfRS;j++){
			if(mulfRS->table[j].s_r.id==m_w.d_r.id){
				mulfRS->table[j].s_r.value=m_w.d_r.value;
				mulfRS->table[j].s_r.ready=1;
			}
			if(mulfRS->table[j].t_r.id==m_w.d_r.id){
				mulfRS->table[j].t_r.value=m_w.d_r.value;
				mulfRS->table[j].t_r.ready=1;
			}
		}
		for(int j=0;j<num_memRS;j++){
			if(memRS->table[j].s_r.id==m_w.d_r.id){
				memRS->table[j].s_r.value=m_w.d_r.value;
				memRS->table[j].s_r.ready=1;
			}
			if(memRS->table[j].t_r.id==m_w.d_r.id){
				memRS->table[j].t_r.value=m_w.d_r.value;
				memRS->table[j].t_r.ready=1;
			}
		}
/***********************************************************************/

			for(int j=0;j<36;j++){

				if(FpRat->table[j].alias==w_c.d_r.id){
					FpRat->table[j].alias=j+101;
					FpRat->table[j].value=w_c.d_r.value;
					for(int i=0;i<instruction.size();i++){
					if(instruction[i].rd.id==m_w.d_r.id) instruction[i].rd.id=j+101;
					if(instruction[i].rs.id==m_w.d_r.id) instruction[i].rs.id=j+101;
					if(instruction[i].rt.id==m_w.d_r.id) instruction[i].rt.id=j+101;
					}

				
				}
			}
		//for(int j=0;j<num_iR;j++){
		for(int j=0;j<36;j++){

				if(IntRat->table[j].alias==m_w.d_r.id){
					IntRat->table[j].alias=j+1;
					IntRat->table[j].value=m_w.d_r.value;
					for(int i=0;i<instruction.size();i++){
					if(instruction[i].rd.id==m_w.d_r.id) instruction[i].rd.id=j+1;
					if(instruction[i].rs.id==m_w.d_r.id) instruction[i].rs.id=j+1;
					if(instruction[i].rt.id==m_w.d_r.id) instruction[i].rt.id=j+1;
					}
				}
			
		}
		//for(int j=0;j<num_fpR;j++){
	

		/************ haven't memory access!!! ***********/
		w_c=m_w;
		instruction[m_w.icount].t_wb=clk;
		w_c.empty=0;
		m_w.empty=1;
	}
}