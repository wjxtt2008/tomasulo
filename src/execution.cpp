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
extern ADDIER *addier, *memer;
extern ADDFER *addfer;
extern MULFER *mulfer;
extern vector<mem_unit> memory;

extern RS_entry e_m;
extern RS_entry m_w;
extern RS_entry w_c;
extern vector<inst> instruction;
extern vector<inst> rom;

extern int clk;	
extern bool memaccess;
extern int pc;
void execution()
{
		Load();
		Mulf();
		Addf();
		Addi();
}

void Addi()
{
	for(int j=0;j<num_addi;j++){
		int temp_offset;
		
		//if(addiRS->head!=addiRS->tail){
		if(!addiRS->empty()){
			if((addier[j].empty==1)&&(addiRS->table[addiRS->head].s_r.ready==1)&&(addiRS->table[addiRS->head].t_r.ready==1)){
				addier[j].line=addiRS->table[addiRS->head];
				addier[j].empty=0;
				addier[j].line.time=clk;
				addiRS->head=(addiRS->head+1)%num_addiRS;
				addiRS->size--;
				
				if(instruction[addier[j].line.icount].opcode==addi)addier[j].line.d_r.value=addier[j].line.s_r.value+addier[j].line.t_r.value;

				else if(instruction[addier[j].line.icount].opcode==subi)addier[j].line.d_r.value=addier[j].line.s_r.value-addier[j].line.t_r.value;

				else if(instruction[addier[j].line.icount].opcode==bne){
					cout<<"s_r"<< addier[j].line.s_r.value<< "t_r"<< addier[j].line.t_r.value<<endl;

					if(addier[j].line.s_r.value-addier[j].line.t_r.value!=0) {
						cout<<"branch taken!"<<endl;
						temp_offset = atoi(instruction[addier[j].line.icount].sd_offset.c_str());
///*
						if(temp_offset<0){
							temp_offset = -temp_offset;
							//vector<inst>instruction;

							vector<inst> tempv_inst(temp_offset+1);
							//vector<inst> tempv_inst;
							vector<inst>::iterator it_instruction = instruction.begin();
							vector<inst>::iterator it_rom = rom.begin();

							//copy(it_instruction+pc-temp_offset,it_instruction+pc+1,tempv_inst.begin());
							copy(it_rom+pc-temp_offset,it_rom+pc+1,tempv_inst.begin());
							
							//for(int i = 0; i<tempv_inst.size();i++){
							//	tempv_inst[i].reset();
							//}
							rom.insert(it_rom+pc+1,tempv_inst.begin(),tempv_inst.end());
							instruction.insert(it_instruction+pc+1,tempv_inst.begin(),tempv_inst.end());
							pc++;
						}

						else pc += temp_offset;
//*/

						//pc=pc+atoi(instruction[addier[j].line.icount].sd_offset.c_str());
					}
					else pc++;
				}
				else if(instruction[addier[j].line.icount].opcode==beq){
					cout<<"s_r"<< addier[j].line.s_r.value<< "t_r"<< addier[j].line.t_r.value<<endl;

					if(addier[j].line.s_r.value-addier[j].line.t_r.value==0) {
						cout<<"branch taken!"<<endl;
						temp_offset = atoi(instruction[addier[j].line.icount].sd_offset.c_str());
///*
						if(temp_offset<0){
							temp_offset = -temp_offset;
							//vector<inst>instruction;

							vector<inst> tempv_inst(temp_offset+1);
							//vector<inst> tempv_inst;
							vector<inst>::iterator it_instruction = instruction.begin();
							vector<inst>::iterator it_rom = rom.begin();

							//copy(it_instruction+pc-temp_offset,it_instruction+pc+1,tempv_inst.begin());
							copy(it_rom+pc-temp_offset,it_rom+pc+1,tempv_inst.begin());
							
							//for(int i = 0; i<tempv_inst.size();i++){
							//	tempv_inst[i].reset();
							//}
							rom.insert(it_rom+pc+1,tempv_inst.begin(),tempv_inst.end());
							instruction.insert(it_instruction+pc+1,tempv_inst.begin(),tempv_inst.end());
							pc++;
						}

						else pc += temp_offset;
//*/
					//	pc=pc+atoi(instruction[addier[j].line.icount].sd_offset.c_str());
					}
					else pc++;
				}
				instruction[addier[j].line.icount].t_ex=clk;
			}
		}
		if(m_w.empty==1&&(addier[j].empty!=1)&&(clk>=(addier[j].line.time+cycle_addi-1))){
				m_w=addier[j].line;
				m_w.empty=0;
				instruction[addier[j].line.icount].t_mem=0;
				memaccess = 0;
				addier[j].empty=1;
				//cout<<e_m.d_r.id <<": "<<e_m.d_r.value<<endl;
		}
	}
}

void Addf()
{
	for(int j=0;j<num_addf;j++){
		//if(addfRS->head!=addfRS->tail){
		if(!addfRS->empty()){

		
			//if(((addfer[j].tail+1)%cycle_addf!=addfer[j].head)&&(addfRS->table[addfRS->head].s_r.ready==1)&&(addfRS->table[addfRS->head].t_r.ready==1)){
			if((!addfer[j].full())&&(addfRS->table[addfRS->head].s_r.ready==1)&&(addfRS->table[addfRS->head].t_r.ready==1)){
				
				//addfer[j].tail=(addfer[j].tail+1)%cycle_addf;

				addfer[j].line[addfer[j].tail]=addfRS->table[addfRS->head];
				addfer[j].line[addfer[j].tail].time=clk;
				
				//remove head from addfRS
				addfRS->head=(addfRS->head+1)%num_addfRS;
				addfRS->size--;
				
				if(instruction[addfer[j].line[addfer[j].tail].icount].opcode==addf) addfer[j].line[addfer[j].tail].d_r.value=addfer[j].line[addfer[j].tail].s_r.value+addfer[j].line[addfer[j].tail].t_r.value;
				else addfer[j].line[addfer[j].tail].d_r.value=addfer[j].line[addfer[j].tail].s_r.value-addfer[j].line[addfer[j].tail].t_r.value;
				
/*				
				if(instruction[addfer[j].head].opcode == subf)  addfer[j].line[addfer[j].head].d_r.value=addfer[j].line[addfer[j].head].s_r.value-addfer[j].line[addfer[j].head].t_r.value;
				else if(instruction[addfer[j].head].opcode == addf) addfer[j].line[addfer[j].tail].d_r.value=addfer[j].line[addfer[j].head].s_r.value+addfer[j].line[addfer[j].head].t_r.value;
				
*/				
				//
				addfer[j].line[addfer[j].tail].d_r.ready=1;
				instruction[addfer[j].line[addfer[j].tail].icount].t_ex=clk;
				addfer[j].tail=(addfer[j].tail+1)%cycle_addf;
				addfer[j].size++;
			}
		}

		//if((m_w.empty==1)&&(addfer[j].head!=addfer[j].tail)&&(clk>=(addfer[j].line[addfer[j].head].time+cycle_addf-1))){
		if((m_w.empty==1)&&(!addfer[j].empty())&&(clk>=(addfer[j].line[addfer[j].head].time+cycle_addf-1))){
			m_w=addfer[j].line[addfer[j].head];
				m_w.empty=0;
				instruction[addier[j].line.icount].t_mem=0;
				memaccess = 0;
				addfer[j].head=(addfer[j].head+1)%cycle_addf;
				addfer[j].size--;
				
				cout<<m_w.d_r.value<<endl;
		}
	}
}

void Mulf()
{
	for(int j=0;j<num_addf;j++){

		//if(mulfRS->head!=mulfRS->tail){
		if(!mulfRS->empty()){
			if(((mulfer[j].tail+1)%cycle_mulf!=mulfer[j].head)&&(mulfRS->table[mulfRS->head].s_r.ready==1)&&(mulfRS->table[mulfRS->head].t_r.ready==1)){
				mulfer[j].line[mulfer[j].tail]=mulfRS->table[mulfRS->head];
				mulfer[j].line[mulfer[j].tail].time=clk;
				mulfRS->head=(mulfRS->head+1)%num_mulfRS;
				mulfRS->size--;

				//mulfer[j].line[mulfer[j].tail].d_r.value=mulfer[j].line[mulfer[j].tail].s_r.value*mulfer[j].line[mulfer[j].tail].t_r.value;
				mulfer[j].line[mulfer[j].head].d_r.value=mulfer[j].line[mulfer[j].head].s_r.value*mulfer[j].line[mulfer[j].head].t_r.value;

				mulfer[j].line[mulfer[j].tail].d_r.ready=1;
				instruction[mulfer[j].line[mulfer[j].tail].icount].t_ex=clk;
				mulfer[j].tail=(mulfer[j].tail+1)%cycle_mulf;
				mulfer[j].size++;
			}
		}

		if((clk>=(mulfer[j].line[mulfer[j].head].time+cycle_mulf-1)&&(m_w.empty==true)&&(mulfer[j].head!=mulfer[j].tail))){
			m_w=mulfer[j].line[mulfer[j].head];
			m_w.empty=0;
			instruction[addier[j].line.icount].t_mem=0;
			memaccess = 0;
			mulfer[j].head=(mulfer[j].head+1)%cycle_mulf;
			mulfer[j].size--;
		}
	}
}

void Load()
{
	//if(memRS->head!=memRS->tail){
	if(!memRS->empty()){
		if((memer->line.empty)&&(memRS->table[memRS->head].s_r.ready==1)&&(memRS->table[addiRS->head].t_r.ready==1)){
				memer->line=memRS->table[memRS->head];
				memer->empty=0;
				memer->line.time=clk;
				memRS->head=(memRS->head+1)%num_memRS;
				memRS->size--;
				
				/******** load ***********/
				if(instruction[memer->line.icount].opcode==load){
					for(int j=0;j<memory.size();j++){
						if(memory[j].first==memer->line.s_r.value+memer->line.t_r.value)memer->line.d_r.value=memory[j].second;
					}
				}
				/******** store **********/
				else{	
					int fg=0;
					mem_unit memtmp=make_pair(atoi(instruction[memer->line.icount].sd_offset.c_str())+memer->line.t_r.value,memer->line.s_r.value);
					for(int i=0;i<memory.size();i++){
						if(memory[i].first==memtmp.first) memory[i].second = memtmp.second;
						fg=1;
					}
						if(fg==0)memory.push_back(memtmp);
						
						
						//for(int j=0;j<memory.size();j++) cout<<memory[j].first<<' '<<memory[j].second<<endl;
				}
				instruction[memer->line.icount].t_ex=clk;
				
			}
		}
	if(e_m.empty==1&&(memer->empty!=1)&&(clk>=(memer->line.time+cycle_mem_exe-1))){
				e_m=memer->line;
				e_m.empty=0;
				memaccess = 1;
				memer->empty=1;
				//cout<<e_m.d_r.id <<": "<<e_m.d_r.value<<endl;
		}
}
