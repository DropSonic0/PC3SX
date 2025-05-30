/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 2004 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * In addition, as a special exception, Andrea Mazzoleni
 * gives permission to link the code of this program with
 * the MAME library (or with modified versions of MAME that use the
 * same license as MAME), and distribute linked combinations including
 * the two.  You must obey the GNU General Public License in all
 * respects for all of the code used other than MAME.  If you modify
 * this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to
 * do so, delete this exception statement from your version.
 */

/*
 * This effect is a rewritten implementation of the hq effect made by Maxim Stepin
 */

case 0 :
case 1 :
case 4 :
case 5 :
case 32 :
case 33 :
case 36 :
case 37 :
case 128 :
case 129 :
case 132 :
case 133 :
case 160 :
case 161 :
case 164 :
case 165 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);

} break;
case 2 :
case 34 :
case 130 :
case 162 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 3 :
case 35 :
case 131 :
case 163 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 6 :
case 38 :
case 134 :
case 166 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 7 :
case 39 :
case 135 :
case 167 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 8 :
case 12 :
case 136 :
case 140 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 9 :
case 13 :
case 137 :
case 141 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 10 :
case 138 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = I31(4, 0);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 11 :
case 139 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 14 :
case 142 :
{
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = I31(4, 0);
	P1 = IC(4);
	P2 = I31(4,5);
	P3 = IC(4);
} else {
	P0 = I11(1, 3);
	P1 = I31(1, 4);
	P2 = I211(4, 1, 5);
	P3 = I31(4, 3);
}
} break;
case 15 :
case 143 :
{
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P2 = I31(4,5);
	P3 = IC(4);
} else {
	P0 = I11(1, 3);
	P1 = I31(1, 4);
	P2 = I211(4, 1, 5);
	P3 = I31(4, 3);
}
} break;
case 16 :
case 17 :
case 48 :
case 49 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 18 :
case 50 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUR) {
	P1 = IC(4);
	P2 = I31(4, 2);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 19 :
case 51 :
{
P3 = I31(4, 3);
P4 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUR) {
	P0 = I31(4, 3);
	P1 = IC(4);
	P2 = I31(4, 2);
	P5 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P1 = I31(1, 4);
	P2 = I11(1, 5);
	P5 = I31(4,5);
}
} break;
case 20 :
case 21 :
case 52 :
case 53 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 22 :
case 54 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 23 :
case 55 :
{
P3 = I31(4, 3);
P4 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUR) {
	P0 = I31(4, 3);
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P1 = I31(1, 4);
	P2 = I11(1, 5);
	P5 = I31(4,5);
}
} break;
case 24 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 25 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 26 :
case 31 :
{
P1 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUL) {
	P0 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P3 = I71(4, 3);
}
if (MUR) {
	P2 = IC(4);
	P5 = IC(4);
} else {
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 27 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 28 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 29 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 30 :
{
P0 = I31(4, 0);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 40 :
case 44 :
case 168 :
case 172 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 41 :
case 45 :
case 169 :
case 173 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
} break;
case 42 :
case 170 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = I31(4, 0);
	P1 = IC(4);
	P3 = IC(4);
	P6 = I31(4, 7);
} else {
	P0 = I11(1, 3);
	P1 = I31(4,1);
	P3 = I31(3, 4);
	P6 = I211(4, 3, 7);
}
} break;
case 43 :
case 171 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
	P6 = I31(4, 7);
} else {
	P0 = I11(1, 3);
	P1 = I31(4,1);
	P3 = I31(3, 4);
	P6 = I211(4, 3, 7);
}
} break;
case 46 :
case 174 :
{
P1 = IC(4);
P2 = I31(4,5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 47 :
case 175 :
{
P1 = IC(4);
P2 = I31(4,5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I211(4, 5, 7);
if (MUL) {
	P0 = IC(4);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 56 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 57 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 58 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 59 :
{
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 60 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 61 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
} break;
case 62 :
{
P0 = I31(4, 0);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 63 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 8);
if (MUL) {
	P0 = IC(4);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = IC(4);
	P5 = IC(4);
} else {
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 64 :
case 65 :
case 68 :
case 69 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 66 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 67 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 70 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 71 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 72 :
case 76 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = I31(4, 6);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
} break;
case 73 :
case 77 :
{
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P0 = I31(4,1);
	P3 = IC(4);
	P6 = I31(4, 6);
	P7 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P3 = I31(3, 4);
	P6 = I11(3, 7);
	P7 = I31(4, 7);
}
} break;
case 74 :
case 107 :
{
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P6 = IC(4);
	P7 = IC(4);
} else {
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
}
} break;
case 75 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 78 :
{
P1 = IC(4);
P2 = I31(4,5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 79 :
{
P2 = I31(4,5);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 80 :
case 81 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = I31(4, 8);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 82 :
case 214 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P7 = IC(4);
	P8 = IC(4);
} else {
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
}
} break;
case 83 :
{
P0 = I31(4, 3);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = IC(4);
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 84 :
case 85 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P2 = I31(4,1);
	P5 = IC(4);
	P7 = IC(4);
	P8 = I31(4, 8);
} else {
	P2 = I211(4, 1, 5);
	P5 = I31(5, 4);
	P7 = I31(4, 7);
	P8 = I11(5, 7);
}
} break;
case 86 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 87 :
{
P0 = I31(4, 3);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
P7 = IC(4);
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 88 :
case 248 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P4 = IC(4);
P7 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
}
if (MDR) {
	P5 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P8 = I772(5, 7, 4);
}
} break;
case 89 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 90 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 91 :
{
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 92 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 93 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 94 :
{
P3 = IC(4);
P4 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 95 :
{
P1 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4, 8);
if (MUL) {
	P0 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P3 = I71(4, 3);
}
if (MUR) {
	P2 = IC(4);
	P5 = IC(4);
} else {
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 96 :
case 97 :
case 100 :
case 101 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 98 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 99 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 102 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 103 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
} break;
case 104 :
case 108 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
} break;
case 105 :
case 109 :
{
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P0 = I31(4,1);
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P3 = I31(3, 4);
	P6 = I11(3, 7);
	P7 = I31(4, 7);
}
} break;
case 106 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
} break;
case 110 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
} break;
case 111 :
{
P1 = IC(4);
P2 = I31(4,5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4, 8);
if (MDL) {
	P6 = IC(4);
	P7 = IC(4);
} else {
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MUL) {
	P0 = IC(4);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 112 :
case 113 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
if (MDR) {
	P5 = IC(4);
	P6 = I31(4, 3);
	P7 = IC(4);
	P8 = I31(4, 8);
} else {
	P5 = I31(4,5);
	P6 = I211(4, 3, 7);
	P7 = I31(7, 4);
	P8 = I11(5, 7);
}
} break;
case 114 :
{
P0 = I31(4, 0);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 115 :
{
P0 = I31(4, 3);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 116 :
case 117 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 118 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 119 :
{
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4, 8);
if (MUR) {
	P0 = I31(4, 3);
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P1 = I31(1, 4);
	P2 = I11(1, 5);
	P5 = I31(4,5);
}
} break;
case 120 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P4 = IC(4);
P5 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
} break;
case 121 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P4 = IC(4);
P5 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 122 :
{
P1 = IC(4);
P4 = IC(4);
P5 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MDR) {
	P8 = I31(4, 8);
} else {
	P8 = I211(4, 5, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 123 :
{
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P6 = IC(4);
	P7 = IC(4);
} else {
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
}
} break;
case 124 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P4 = IC(4);
P5 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
} break;
case 125 :
{
P1 = I31(4,1);
P2 = I31(4,1);
P4 = IC(4);
P5 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P0 = I31(4,1);
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P3 = I31(3, 4);
	P6 = I11(3, 7);
	P7 = I31(4, 7);
}
} break;
case 126 :
{
P0 = I31(4, 0);
P4 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 127 :
{
P4 = IC(4);
P8 = I31(4, 8);
if (MDL) {
	P6 = IC(4);
	P7 = IC(4);
} else {
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I211(4, 1, 3);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
if (MUR) {
	P2 = IC(4);
	P5 = IC(4);
} else {
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 144 :
case 145 :
case 176 :
case 177 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 146 :
case 178 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
if (MUR) {
	P1 = IC(4);
	P2 = I31(4, 2);
	P5 = IC(4);
	P8 = I31(4, 7);
} else {
	P1 = I31(4,1);
	P2 = I11(1, 5);
	P5 = I31(5, 4);
	P8 = I211(4, 5, 7);
}
} break;
case 147 :
case 179 :
{
P0 = I31(4, 3);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 148 :
case 149 :
case 180 :
case 181 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 150 :
case 182 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
	P8 = I31(4, 7);
} else {
	P1 = I31(4,1);
	P2 = I11(1, 5);
	P5 = I31(5, 4);
	P8 = I211(4, 5, 7);
}
} break;
case 151 :
case 183 :
{
P0 = I31(4, 3);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I211(4, 3, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUR) {
	P2 = IC(4);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 152 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 153 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 154 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 155 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 156 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 157 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 158 :
{
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
	P5 = I71(4, 5);
}
} break;
case 159 :
{
P1 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P3 = I71(4, 3);
}
if (MUR) {
	P2 = IC(4);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 184 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 185 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 186 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 187 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = IC(4);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
	P6 = I31(4, 7);
} else {
	P0 = I11(1, 3);
	P1 = I31(4,1);
	P3 = I31(3, 4);
	P6 = I211(4, 3, 7);
}
} break;
case 188 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 189 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
} break;
case 190 :
{
P0 = I31(4, 0);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
	P8 = I31(4, 7);
} else {
	P1 = I31(4,1);
	P2 = I11(1, 5);
	P5 = I31(5, 4);
	P8 = I211(4, 5, 7);
}
} break;
case 191 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 7);
P7 = I31(4, 7);
P8 = I31(4, 7);
if (MUL) {
	P0 = IC(4);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = IC(4);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 192 :
case 193 :
case 196 :
case 197 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 194 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 195 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 198 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 199 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 200 :
case 204 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P4 = IC(4);
P5 = I31(4,5);
if (MDL) {
	P3 = IC(4);
	P6 = I31(4, 6);
	P7 = IC(4);
	P8 = I31(4,5);
} else {
	P3 = I31(4, 3);
	P6 = I11(3, 7);
	P7 = I31(7, 4);
	P8 = I211(4, 5, 7);
}
} break;
case 201 :
case 205 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4,5);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
} break;
case 202 :
{
P1 = IC(4);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4,5);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 203 :
{
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 206 :
{
P1 = IC(4);
P2 = I31(4,5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4,5);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 207 :
{
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 6);
P7 = IC(4);
P8 = I31(4,5);
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P2 = I31(4,5);
	P3 = IC(4);
} else {
	P0 = I11(1, 3);
	P1 = I31(1, 4);
	P2 = I211(4, 1, 5);
	P3 = I31(4, 3);
}
} break;
case 208 :
case 209 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 210 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 211 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 212 :
case 213 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P2 = I31(4,1);
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P2 = I211(4, 1, 5);
	P5 = I31(5, 4);
	P7 = I31(4, 7);
	P8 = I11(5, 7);
}
} break;
case 215 :
{
P0 = I31(4, 3);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P7 = IC(4);
	P8 = IC(4);
} else {
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUR) {
	P2 = IC(4);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 216 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 217 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 218 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 219 :
{
P2 = I31(4, 2);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
	P3 = I71(4, 3);
}
} break;
case 220 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
if (MDL) {
	P6 = I31(4, 6);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
} break;
case 221 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P2 = I31(4,1);
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P2 = I211(4, 1, 5);
	P5 = I31(5, 4);
	P7 = I31(4, 7);
	P8 = I11(5, 7);
}
} break;
case 222 :
{
P0 = I31(4, 0);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P7 = IC(4);
	P8 = IC(4);
} else {
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
}
} break;
case 223 :
{
P4 = IC(4);
P6 = I31(4, 6);
if (MDR) {
	P7 = IC(4);
	P8 = IC(4);
} else {
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUL) {
	P0 = IC(4);
	P3 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P3 = I71(4, 3);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
	P5 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I211(4, 1, 5);
	P5 = I71(4, 5);
}
} break;
case 224 :
case 225 :
case 228 :
case 229 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 226 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 227 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 230 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 231 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4,5);
P3 = I31(4, 3);
P4 = IC(4);
P5 = I31(4,5);
P6 = I31(4, 3);
P7 = IC(4);
P8 = I31(4,5);
} break;
case 232 :
case 236 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P4 = IC(4);
P5 = I31(4,5);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
	P8 = I31(4,5);
} else {
	P3 = I31(4, 3);
	P6 = I11(3, 7);
	P7 = I31(7, 4);
	P8 = I211(4, 5, 7);
}
} break;
case 233 :
case 237 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I211(4, 1, 5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4,5);
if (MDL) {
	P6 = IC(4);
} else {
	P6 = I211(4, 3, 7);
}
} break;
case 234 :
{
P1 = IC(4);
P2 = I31(4, 2);
P4 = IC(4);
P5 = I31(4,5);
P8 = I31(4,5);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
	P7 = I71(4, 7);
}
if (MUL) {
	P0 = I31(4, 0);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 235 :
{
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4,5);
if (MDL) {
	P6 = IC(4);
} else {
	P6 = I211(4, 3, 7);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
}
} break;
case 238 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4,5);
P4 = IC(4);
P5 = I31(4,5);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
	P8 = I31(4,5);
} else {
	P3 = I31(4, 3);
	P6 = I11(3, 7);
	P7 = I31(7, 4);
	P8 = I211(4, 5, 7);
}
} break;
case 239 :
{
P1 = IC(4);
P2 = I31(4,5);
P3 = IC(4);
P4 = IC(4);
P5 = I31(4,5);
P7 = IC(4);
P8 = I31(4,5);
if (MDL) {
	P6 = IC(4);
} else {
	P6 = I211(4, 3, 7);
}
if (MUL) {
	P0 = IC(4);
} else {
	P0 = I211(4, 1, 3);
}
} break;
case 240 :
case 241 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
if (MDR) {
	P5 = IC(4);
	P6 = I31(4, 3);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I31(4,5);
	P6 = I211(4, 3, 7);
	P7 = I31(7, 4);
	P8 = I11(5, 7);
}
} break;
case 242 :
{
P0 = I31(4, 0);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P6 = I31(4, 3);
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I772(5, 7, 4);
}
if (MUR) {
	P2 = I31(4, 2);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 243 :
{
P0 = I31(4, 3);
P1 = IC(4);
P2 = I31(4, 2);
P3 = I31(4, 3);
P4 = IC(4);
if (MDR) {
	P5 = IC(4);
	P6 = I31(4, 3);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I31(4,5);
	P6 = I211(4, 3, 7);
	P7 = I31(7, 4);
	P8 = I11(5, 7);
}
} break;
case 244 :
case 245 :
{
P0 = I211(4, 1, 3);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
if (MDR) {
	P8 = IC(4);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 246 :
{
P0 = I31(4, 0);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
if (MDR) {
	P8 = IC(4);
} else {
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
}
} break;
case 247 :
{
P0 = I31(4, 3);
P1 = IC(4);
P3 = I31(4, 3);
P4 = IC(4);
P5 = IC(4);
P6 = I31(4, 3);
P7 = IC(4);
if (MDR) {
	P8 = IC(4);
} else {
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P2 = IC(4);
} else {
	P2 = I211(4, 1, 5);
}
} break;
case 249 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4, 2);
P3 = IC(4);
P4 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = IC(4);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P5 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P8 = I772(5, 7, 4);
}
} break;
case 250 :
{
P0 = I31(4, 0);
P1 = IC(4);
P2 = I31(4, 2);
P4 = IC(4);
P7 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
}
if (MDR) {
	P5 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P8 = I772(5, 7, 4);
}
} break;
case 251 :
{
P2 = I31(4, 2);
P4 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
	P7 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I211(4, 3, 7);
	P7 = I71(4, 7);
}
if (MDR) {
	P5 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P8 = I772(5, 7, 4);
}
if (MUL) {
	P0 = IC(4);
	P1 = IC(4);
} else {
	P0 = I772(1, 3, 4);
	P1 = I71(4, 1);
}
} break;
case 252 :
{
P0 = I31(4, 0);
P1 = I31(4,1);
P2 = I31(4,1);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
}
if (MDR) {
	P8 = IC(4);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 253 :
{
P0 = I31(4,1);
P1 = I31(4,1);
P2 = I31(4,1);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = IC(4);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = IC(4);
} else {
	P8 = I211(4, 5, 7);
}
} break;
case 254 :
{
P0 = I31(4, 0);
P4 = IC(4);
if (MDL) {
	P3 = IC(4);
	P6 = IC(4);
} else {
	P3 = I71(4, 3);
	P6 = I772(3, 7, 4);
}
if (MDR) {
	P5 = IC(4);
	P7 = IC(4);
	P8 = IC(4);
} else {
	P5 = I71(4, 5);
	P7 = I71(4, 7);
	P8 = I211(4, 5, 7);
}
if (MUR) {
	P1 = IC(4);
	P2 = IC(4);
} else {
	P1 = I71(4, 1);
	P2 = I772(1, 5, 4);
}
} break;
case 255 :
{
P1 = IC(4);
P3 = IC(4);
P4 = IC(4);
P5 = IC(4);
P7 = IC(4);
if (MDL) {
	P6 = IC(4);
} else {
	P6 = I211(4, 3, 7);
}
if (MDR) {
	P8 = IC(4);
} else {
	P8 = I211(4, 5, 7);
}
if (MUL) {
	P0 = IC(4);
} else {
	P0 = I211(4, 1, 3);
}
if (MUR) {
	P2 = IC(4);
} else {
	P2 = I211(4, 1, 5);
}
} break