#ifndef _R3000ATABLE_H_
#define _R3000ATABLE_H_

void psxBGEZ();
void psxBGEZAL();
void psxBGTZ();
void psxBLEZ();
void psxBLTZ();
void psxBLTZAL();
void psxBEQ();
void psxBNE();

void psxJ();
void psxJAL();
void psxJR();
void psxJALR();

void psxADDI();
void psxADDIU();
void psxANDI();
void psxORI();
void psxXORI();
void psxSLTI();
void psxSLTIU();

void psxADD();
void psxADDU();
void psxSUB();
void psxSUBU();
void psxAND();
void psxOR();
void psxXOR();
void psxNOR();
void psxSLT();
void psxSLTU();

void psxDIV();
void psxDIVU();
void psxMULT();
void psxMULTU();

void psxSLL();
void psxSRA();
void psxSRL();
void psxSLLV();
void psxSRAV();
void psxSRLV();
void psxLUI();

void psxMFHI();
void psxMFLO();
void psxMTHI();
void psxMTLO();

void psxBREAK();
void psxSYSCALL();
void psxRFE();

void psxLB();
void psxLBU();
void psxLH();
void psxLHU();
void psxLW();
void psxLWL();
void psxLWR();

void psxSB();
void psxSH();
void psxSW();
void psxSWL();
void psxSWR();

void psxMFC0();
void psxCFC0();
void psxMTC0();
void psxCTC0();
void psxNULL();
void psxHLE();

#endif
