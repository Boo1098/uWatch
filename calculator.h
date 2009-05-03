#ifndef __CALCULATOR_H
#define __CALCULATOR_H

int EnterNumber(int Key);

void RPNcalculator(void);
void ALGcalculator(void);

void CompleteXreg(void);
void UpdateDisplayRegs(void);
void PushStackUp(void);
void Operate(int op);
int OperatePrecedence( int op );
void PopStack();
void StoreRecall(void);
void PopOp();
void UpdateYregDisplay(void);
void PushOp(int op);

void Clx();

void ClearAllRegs( void );
void ClearCurrentRegs();

extern double Yregs[];
extern double Zregs[];
extern double Tregs[];

extern double iYregs[];
extern double iZregs[];
extern double iTregs[];

#endif
