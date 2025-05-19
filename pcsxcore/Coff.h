/*
 * Copyright (c) 1995
 *	Ted Lemon (hereinafter referred to as the author)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __ECOFF_H__
#define __ECOFF_H__

/*
 * Some ECOFF definitions.
 */
struct external_filehdr {
	unsigned short f_magic;		/* magic number			*/
	unsigned short f_nscns;		/* number of sections		*/
	unsigned long f_timdat;	/* time & date stamp		*/
	unsigned long f_symptr;	/* file pointer to symtab	*/
	unsigned long f_nsyms;		/* number of symtab entries	*/
	unsigned short f_opthdr;	/* sizeof(optional hdr)		*/
	unsigned short f_flags;		/* flags			*/
};

#define	FILHDR	struct external_filehdr
#define	FILHSZ	sizeof(FILHDR)

struct external_scnehdr {
	char            s_name[8];      /* section name */
	unsigned long s_paddr;        /* physical address, aliased s_nlib */
	unsigned long s_vaddr;        /* virtual address */
	unsigned long s_size;         /* section size */
	unsigned long s_scnptr;       /* file ptr to raw data for section */
	unsigned long s_relptr;       /* file ptr to relocation */
	unsigned long s_lnnoptr;      /* file ptr to gp histogram */
	unsigned short s_nreloc;       /* number of relocation entries */
	unsigned short s_nlnno;        /* number of gp histogram entries */
	unsigned long s_flags;        /* flags */
};

#define	SCNHDR	struct external_scnehdr
#define	SCNHSZ	sizeof(SCNHDR)

struct external_aoutehdr {
	unsigned short magic;          /* magic */
	unsigned short vstamp;         /* version stamp */
	unsigned long tsize;          /* text size in bytes, padded to DW bdry */
	unsigned long dsize;          /* initialized data */
	unsigned long bsize;          /* uninitialized data */
	unsigned long entry;          /* entry pt. */
	unsigned long text_start;     /* base of text used for this file */
	unsigned long data_start;     /* base of data used for this file */
};

#define	AOUTHDR	struct external_aoutehdr
#define	AOUTHSZ	sizeof(AOUTHDR)

#endif
