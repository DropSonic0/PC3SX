/*  PCSX-Revolution - PS Emulator for Nintendo Wii
 *  Copyright (C) 2009-2010  PCSX-Revolution Dev Team
 *
 *  PCSX-Revolution is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public 
 *  License as published by the Free Software Foundation, either 
 *  version 2 of the License, or (at your option) any later version.
 *
 *  PCSX-Revolution is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with PCSX-Revolution.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _pass0R3000ATABLE_H_
#define _pass0R3000ATABLE_H_

// this defines shall be used with the tmp 
// of the next func (instead of _Funct_...)
#define _tFunct_  ((tmp      ) & 0x3F)  // The funct part of the instruction register 
#define _tRd_     ((tmp >> 11) & 0x1F)  // The rd part of the instruction register 
#define _tRt_     ((tmp >> 16) & 0x1F)  // The rt part of the instruction register 
#define _tRs_     ((tmp >> 21) & 0x1F)  // The rs part of the instruction register 
#define _tSa_     ((tmp >>  6) & 0x1F)  // The sa part of the instruction register

void pass0(u32 tmp) {
// 	u32 tmp = code;
	if (tmp == 0) return; // NOP
	switch (_fOp_(tmp)) {
		case 0x00: // SPECIAL
			switch (_tFunct_) {
				case 0x00: // SLL
				case 0x02: case 0x03: // SRL/SRA
					block.regs_to_alloc[_Rd_] = 1;
					block.regs_to_alloc[_Rt_] = 1;
					break;

				case 0x08: case 0x09: // JALR/JR
					block.regs_to_alloc[_Rs_] = 1;
					block.sp_size += 16;
					pass0_branch = 1;
					break;

				case 0x20: case 0x21: case 0x22: case 0x23:
				case 0x24: case 0x25: case 0x26: case 0x27: 
				case 0x2a: case 0x2b: // ADD/ADDU...
					if (!_Rd_) return;
					block.regs_to_alloc[_Rt_] = 1;
					block.regs_to_alloc[_Rs_] = 1;
					block.regs_to_alloc[_Rd_] = 1;
					break;
				
				case 0x04: case 0x06: case 0x07: // SLLV...
					if (!_Rd_) return;

					block.regs_to_alloc[_Rd_] = 1;
					block.regs_to_alloc[_Rt_] = 1;
					block.regs_to_alloc[_Rs_] = 1;
					break;

				case 0x10: case 0x12: // MFHI/MFLO
					if (!_Rd_) return;
					block.regs_to_alloc[_Rd_] = 1;
					break;

				case 0x11: case 0x13: // MTHI/MTLO
					block.regs_to_alloc[_Rs_] = 1;
					break;

				case 0x18: case 0x19:
				case 0x1a: case 0x1b: // MULT/DIV...
					block.regs_to_alloc[_Rt_] = 1;
					block.regs_to_alloc[_Rs_] = 1;
					break;
			}
			break;

		case 0x01: // REGIMM
			switch (_tRt_) {
				case 0x00: case 0x02:
				case 0x10: case 0x12: // BLTZ/BGEZ...
					block.regs_to_alloc[_Rs_] = 1;
					block.sp_size += 16;
					pass0_branch = 1;
					break;
			}
			break;

		case 0x04: case 0x05: // BEQ/BNE
			block.regs_to_alloc[_Rs_] = 1;
			block.regs_to_alloc[_Rt_] = 1;
			block.sp_size += 16;
			pass0_branch = 1;
			break;

		case 0x06: case 0x07: // BLEZ/BGTZ
			block.regs_to_alloc[_Rs_] = 1;
			block.sp_size += 16;
			pass0_branch = 1;
			break;

		case 0x08: case 0x09: case 0x0a: case 0x0b:
		case 0x0c: case 0x0d: case 0x0e: // ADDI/ADDIU...
			if (!_Rt_) return;
			block.regs_to_alloc[_Rt_] = 1;
			block.regs_to_alloc[_Rs_] = 1;
			break;

		case 0x10: // COP0
			switch (_tRs_) {
				case 0x00: case 0x02: // MFC0/CFC0
					if (!_Rt_) return;
					block.regs_to_alloc[_Rt_] = 1;
					break;
				case 0x04: case 0x06: // MTC0/CTC0
					block.regs_to_alloc[_Rt_] = 1;
					if(pass0_branch == 0) pass0_branch = 2;
					break;
					
				case 0x0a: // RFE
					if(pass0_branch == 0) pass0_branch = 2;
			}
			break;

		case 0x12: // COP2
			switch (_tFunct_) {
				case 0x00: 
					switch (_tRs_) {
						case 0x00: case 0x02: // CFC2/MFC2
							if (!_Rt_) return;
// 							block.regs_to_alloc[_Rt_] = 1;
// 							break;
						case 0x04: case 0x06: // CTC2/MTC2
							block.regs_to_alloc[_Rt_] = 1;
							break;
					}
					break;
			}
			break;

		case 0x22: case 0x26: // LWL/LWR
		case 0x20: case 0x21: case 0x23:
		case 0x24: case 0x25: // LB/LH/LW/LBU/LHU
			block.regs_to_alloc[_Rs_] = 1;
			block.regs_to_alloc[_Rt_] = 1;
			block.sp_size += 16;
			break;

		case 0x28: case 0x29: case 0x2b: // SB/SH/SW
		  	block.regs_to_alloc[_Rs_] = 1;
			block.regs_to_alloc[_Rt_] = 1;
			block.sp_size += 8;
			break;
			
		case 0x2a: case 0x2e: // SWL/SWR
			block.regs_to_alloc[_Rs_] = 1;
			block.sp_size += 24;
			break;
			
		case 0x32: case 0x3a: // LWC2/SWC2
			block.sp_size += 16;
			break;
	}

	return;
}

#endif