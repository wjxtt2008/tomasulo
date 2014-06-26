#include"tomasulo.h"
#include<vector>
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
extern vector<inst> instruction;
extern int clk;	
int pc=0;
int last_pc=-1;
bool issue_flag=false;

/************** First, read inst for ib. Name dst after ROB **********/
/************** Rewrite IntRat or FpRat ******************************/
/************** Based IntRat or FpRat, Name src and tgt **************/
/************** Based opcode, push inst into RS **********************/
void rename()
{
	issue_flag = true;
	/********************** rename src **********************/
		if(instruction[pc].rs.imme_flag==0){
			int tmp=instruction[pc].rs.id;
			/*** integral register ***/
			if(instruction[pc].rs.id>=1&&instruction[pc].rs.id<=100){ 
				instruction[pc].rs.id=IntRat->table[instruction[pc].rs.id-1].alias;
				
				if(instruction[pc].rs.id==tmp){ 
					//instruction[pc].rs.value=IntArf->table[instruction[pc].rs.id-1].value;
					instruction[pc].rs.value=IntRat->table[instruction[pc].rs.id-1].value;
					instruction[pc].rs.ready=1;
					//cout<<"111111111111111111111111"<<endl;
				}
			}
			/*** float register ***/
			else if(instruction[pc].rs.id>=101&&instruction[pc].rs.id<=200){
				instruction[pc].rs.id=FpRat->table[instruction[pc].rs.id-101].alias;
				if(instruction[pc].rs.id==tmp){
					//instruction[pc].rs.value=FpArf->table[instruction[pc].rs.id-101].value;
					instruction[pc].rs.value=FpRat->table[instruction[pc].rs.id-101].value;
					instruction[pc].rs.ready=1;
				}
			}
		}
	/********************** rename tgt **********************/
		if(instruction[pc].rt.imme_flag==0){
			int tmp=instruction[pc].rt.id;
			/*** integral register ***/
			if(instruction[pc].rt.id>=1&&instruction[pc].rt.id<=100){ 
				instruction[pc].rt.id=IntRat->table[instruction[pc].rt.id-1].alias;
				if(instruction[pc].rt.id==tmp){ 
					//instruction[pc].rt.value=IntArf->table[instruction[pc].rt.id-1].value;
					instruction[pc].rt.value=IntRat->table[instruction[pc].rt.id-1].value;
					instruction[pc].rt.ready=1;
				}
			}
			/*** float register ***/
			else if(instruction[pc].rt.id>=101&&instruction[pc].rt.id<=200){
				instruction[pc].rt.id=FpRat->table[instruction[pc].rt.id-101].alias;
				if(instruction[pc].rt.id==tmp){
					//instruction[pc].rt.value=FpArf->table[instruction[pc].rt.id-101].value;
					instruction[pc].rt.value=FpRat->table[instruction[pc].rt.id-101].value;
					instruction[pc].rt.ready=1;
				}
			}
		}
	/********************** write ROB **********************/
		ROB->table[ROB->tail].dst_id=instruction[pc].rd.id;
		int tmp=instruction[pc].rd.id;
		instruction[pc].rd.id=ROB->table[ROB->tail].id;
		ROB->tail=(ROB->tail+1)%num_ROB;
		ROB->size++;
		
		//for(int j=0;j<num_ROB;j++) cout<<ROB->table[j].id<<":	"<<ROB->table[j].dst_id<<endl;
	/********************** rewrite IntRat FpRat ************/
		if(tmp>=1&&tmp<=100)IntRat->table[tmp-1].alias=instruction[pc].rd.id;
		else if(tmp>=101&&tmp<=200) FpRat->table[tmp-101].alias=instruction[pc].rd.id;
		//for(int j=0;j<36;j++) cout<<j+1<<": "<<IntRat->table[j].alias<<"	"<<IntRat->table[j].value<<endl;
		//cout<<endl;
		//cout<<endl;
		//for(int j=0;j<36;j++) cout<<j+1<<": "<<FpRat->table[j].alias<<endl;
}


void issue()
{

	//for(int j=0;j<36;j++) cout<<IntArf->table[j].id<<"	"<<IntArf->table[j].value<<endl;


	if((last_pc!=pc)&&(!ROB->full())&&pc<instruction.size())
	{	


		if((!memRS->full())&&(instruction[pc].opcode == load||instruction[pc].opcode == store)){	
				rename();

				memRS->table[memRS->tail].d_r=instruction[pc].rd;
				memRS->table[memRS->tail].s_r=instruction[pc].rs;
				memRS->table[memRS->tail].t_r=instruction[pc].rt;
				memRS->table[memRS->tail].time=clk;
				memRS->table[memRS->tail].icount=pc;
				instruction[pc].t_issue=clk;
				memRS->tail=(memRS->tail+1)%num_memRS;
				memRS->size++;
				/*
				for(int j=memRS->head;j!=memRS->tail;j++) cout<<memRS->table[j].d_r.id<<": "<<memRS->table[j].d_r.value<<" "<<memRS->table[j].d_r.ready<<"	"<<
				memRS->table[j].s_r.id<<": "<<memRS->table[j].s_r.value<<" "<<memRS->table[j].s_r.ready<<"	"<<
				memRS->table[j].t_r.id<<": "<<memRS->table[j].t_r.value<<" "<<memRS->table[j].t_r.ready<<endl;
				*/
		}
				//MUL 
		if((!mulfRS->full())&&(instruction[pc].opcode == mulf)){
				rename();

				mulfRS->table[mulfRS->tail].d_r=instruction[pc].rd;
				mulfRS->table[mulfRS->tail].s_r=instruction[pc].rs;
				mulfRS->table[mulfRS->tail].t_r=instruction[pc].rt;
				mulfRS->table[mulfRS->tail].time=clk;
				mulfRS->table[mulfRS->tail].icount=pc;
				instruction[pc].t_issue=clk;
				mulfRS->tail=(mulfRS->tail+1)%num_mulfRS;
				mulfRS->size++;
				
				/*
				for(int j=mulfRS->head;j!=mulfRS->tail;j++) cout<<mulfRS->table[j].d_r.id<<": "<<mulfRS->table[j].d_r.value<<" "<<mulfRS->table[j].d_r.ready<<"	"<<
				mulfRS->table[j].s_r.id<<": "<<mulfRS->table[j].s_r.value<<" "<<mulfRS->table[j].s_r.ready<<"	"<<
				mulfRS->table[j].t_r.id<<": "<<mulfRS->table[j].t_r.value<<" "<<mulfRS->table[j].t_r.ready<<endl;
				*/
		}
	/********************** add to RS ***********************/
		//ALU float
		if((!addfRS->full())&&(instruction[pc].opcode == addf||instruction[pc].opcode == subf)){
				rename();

				addfRS->table[addfRS->tail].d_r=instruction[pc].rd;
				addfRS->table[addfRS->tail].s_r=instruction[pc].rs;
				addfRS->table[addfRS->tail].t_r=instruction[pc].rt;
				addfRS->table[addfRS->tail].time=clk;
				addfRS->table[addfRS->tail].icount=pc;
				instruction[pc].t_issue=clk;
				addfRS->tail=(addfRS->tail+1)%num_addfRS;
				addfRS->size++;
			/*
			for(int j=addfRS->head;j!=num_addfRS;j++) cout<<addfRS->table[j].d_r.id<<": "<<addfRS->table[j].d_r.value<<" "<<addfRS->table[j].d_r.ready<<"	"<<
				addfRS->table[j].s_r.id<<": "<<addfRS->table[j].s_r.value<<" "<<addfRS->table[j].s_r.ready<<"	"<<
				addfRS->table[j].t_r.id<<": "<<addfRS->table[j].t_r.value<<" "<<addfRS->table[j].t_r.ready<<endl;
				*/
		}
		//ALU integer
		if((!addiRS->full())&&(instruction[pc].opcode == addi||instruction[pc].opcode == subi||instruction[pc].opcode == beq||instruction[pc].opcode == bne)){
				rename();

				addiRS->table[addiRS->tail].d_r=instruction[pc].rd;
				addiRS->table[addiRS->tail].s_r=instruction[pc].rs;
				addiRS->table[addiRS->tail].t_r=instruction[pc].rt;
				addiRS->table[addiRS->tail].time=clk;
				addiRS->table[addiRS->tail].icount=pc;
				instruction[pc].t_issue=clk;
				addiRS->tail=(addiRS->tail+1)%num_addiRS;
				addiRS->size++;
			/*for(int j=addiRS->head;j!=num_addiRS;j++) cout<<addiRS->table[j].d_r.id<<": "<<addiRS->table[j].d_r.value<<" "<<addiRS->table[j].d_r.ready<<"	"<<
				addiRS->table[j].s_r.id<<": "<<addiRS->table[j].s_r.value<<" "<<addiRS->table[j].s_r.ready<<"	"<<
				addiRS->table[j].t_r.id<<": "<<addiRS->table[j].t_r.value<<" "<<addiRS->table[j].t_r.ready<<endl;
				*/
		}

		




		if(issue_flag){
			cout<<"issue "<<instruction[pc].name<<endl;
			issue_flag = false;
			last_pc = pc;
			if(pc<instruction.size()&&(instruction[pc].opcode!=bne&&instruction[pc].opcode!=beq)) pc++;
		}
	}
}