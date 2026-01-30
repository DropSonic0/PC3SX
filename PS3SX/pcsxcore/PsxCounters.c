/***************************************************************************
 *   Copyright (C) 2010 by Blade_Arma                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
 * Internal PSX counters.
 */

#include "PsxCounters.h"
#include "R3000A.h"

/******************************************************************************/

enum
{
    Rc0Gate           = 0x0001, // 0    not implemented
    Rc1Gate           = 0x0001, // 0    not implemented
    Rc2Disable        = 0x0001, // 0    partially implemented
    RcUnknown1        = 0x0002, // 1    ?
    RcUnknown2        = 0x0004, // 2    ?
    RcCountToTarget   = 0x0008, // 3
    RcIrqOnTarget     = 0x0010, // 4
    RcIrqOnOverflow   = 0x0020, // 5
    RcIrqRegenerate   = 0x0040, // 6
    RcUnknown7        = 0x0080, // 7    ?
    Rc0PixelClock     = 0x0100, // 8    fake implementation
    Rc1HSyncClock     = 0x0100, // 8
    Rc2Unknown8       = 0x0100, // 8    ?
    Rc0Unknown9       = 0x0200, // 9    ?
    Rc1Unknown9       = 0x0200, // 9    ?
    Rc2OneEighthClock = 0x0200, // 9
    RcUnknown10       = 0x0400, // 10   ?
    RcCountEqTarget   = 0x0800, // 11
    RcOverflow        = 0x1000, // 12
    RcUnknown13       = 0x2000, // 13   ? (always zero)
    RcUnknown14       = 0x4000, // 14   ? (always zero)
    RcUnknown15       = 0x8000, // 15   ? (always zero)
};

#define CounterQuantity           ( 4 )

static const u32 CountToOverflow  = 0;
static const u32 CountToTarget    = 1;

static const u32 FrameRate[]      = { 60, 50 };
static const u32 VBlankStart[]    = { 243, 256 };
static const u32 HSyncTotal[]     = { 263, 313 };
static const u32 SpuUpdInterval[] = { 23, 22 };

/******************************************************************************/

psxCounter psxCounters[ CounterQuantity ];

static u32 hSyncCount = 0;
static u32 spuSyncCount = 0;

u32 psxNextCounter = 0, psxNextsCounter = 0;

/******************************************************************************/

static inline
void setIrq( u32 irq )
{
    psxHu32ref(0x1070) |= SWAPu32(irq);
}

/******************************************************************************/

static inline
void _psxRcntWcount( u32 index, u32 value )
{
    if( value > 0xffff )
    {
        value &= 0xffff;
    }

    psxCounters[index].cycleStart  = psxRegs.cycle;
    psxCounters[index].cycleStart -= value * psxCounters[index].rate;

    if( value < psxCounters[index].target )
    {
        psxCounters[index].cycle = psxCounters[index].target * psxCounters[index].rate;
        psxCounters[index].counterState = CountToTarget;
    }
    else
    {
        psxCounters[index].cycle = 0xffff * psxCounters[index].rate;
        psxCounters[index].counterState = CountToOverflow;
    }
}

static inline
u32 _psxRcntRcount( u32 index )
{
    u32 count;

    count  = psxRegs.cycle;
    count -= psxCounters[index].cycleStart;
    count /= psxCounters[index].rate;

    if( count > 0xffff )
    {
        count &= 0xffff;
    }

    return count;
}

/******************************************************************************/

static
void psxRcntSet(void)
{
    s32 countToUpdate;
    u32 i;

    psxNextsCounter = psxRegs.cycle;
    psxNextCounter  = 0x7fffffff;

    for( i = 0; i < CounterQuantity; ++i )
    {
        countToUpdate = psxCounters[i].cycle - (psxNextsCounter - psxCounters[i].cycleStart);

        if( countToUpdate < 0 )
        {
            psxNextCounter = 0;
            break;
        }

        if( countToUpdate < (s32)psxNextCounter )
        {
            psxNextCounter = countToUpdate;
        }
    }
}

/******************************************************************************/

static
void psxRcntReset( u32 index )
{
    u32 count;

    if( psxCounters[index].counterState == CountToTarget )
    {
        if( psxCounters[index].mode & RcCountToTarget )
        {
            count  = psxRegs.cycle;
            count -= psxCounters[index].cycleStart;
            count /= psxCounters[index].rate;
            count -= psxCounters[index].target;
        }
        else
        {
            count = _psxRcntRcount( index );
        }

        _psxRcntWcount( index, count );

        if( psxCounters[index].mode & RcIrqOnTarget )
        {
            if( (psxCounters[index].mode & RcIrqRegenerate) || (!psxCounters[index].irqState) )
            {
                setIrq( psxCounters[index].irq );
                psxCounters[index].irqState = 1;
                psxRegs.interrupt |= 0x80000000;
            }
        }

        psxCounters[index].mode |= RcCountEqTarget;
    }
    else if( psxCounters[index].counterState == CountToOverflow )
    {
        count  = psxRegs.cycle;
        count -= psxCounters[index].cycleStart;
        count /= psxCounters[index].rate;
        count -= 0xffff;

        _psxRcntWcount( index, count );

        if( psxCounters[index].mode & RcIrqOnOverflow )
        {
            if( (psxCounters[index].mode & RcIrqRegenerate) || (!psxCounters[index].irqState) )
            {
                setIrq( psxCounters[index].irq );
                psxCounters[index].irqState = 1;
                psxRegs.interrupt |= 0x80000000;
            }
        }

        psxCounters[index].mode |= RcOverflow;
    }

    psxCounters[index].mode |= RcUnknown10;

    psxRcntSet();
}

void psxRcntUpdate(void)
{
    u32 cycle;

    cycle = psxRegs.cycle;

    // rcnt 0.
    if( cycle - psxCounters[0].cycleStart >= psxCounters[0].cycle )
    {
        psxRcntReset( 0 );
    }

    // rcnt 1.
    if( cycle - psxCounters[1].cycleStart >= psxCounters[1].cycle )
    {
        psxRcntReset( 1 );
    }

    // rcnt 2.
    if( cycle - psxCounters[2].cycleStart >= psxCounters[2].cycle )
    {
        psxRcntReset( 2 );
    }

    // rcnt base.
    if( cycle - psxCounters[3].cycleStart >= psxCounters[3].cycle )
    {
        psxRcntReset( 3 );

        spuSyncCount++;
        hSyncCount++;

        // Update spu.
        if( spuSyncCount >= SpuUpdInterval[Config.PsxType] )
        {
            spuSyncCount = 0;

            if( SPU_async )
            {
                SPU_async( SpuUpdInterval[Config.PsxType] * psxCounters[3].target );
            }
        }

        // VSync irq.
        if( hSyncCount == VBlankStart[Config.PsxType] )
        {
            // GPU_vBlank( 1 ); // missing in PS3SX
        }

        // Update lace. (with InuYasha fix)
        if( hSyncCount >= (Config.VSyncWA ? HSyncTotal[Config.PsxType] / BIAS : HSyncTotal[Config.PsxType]) )
        {
            hSyncCount = 0;

            // GPU_vBlank( 0 ); // missing in PS3SX
            setIrq( 0x01 );
            psxRegs.interrupt |= 0x80000000;

            GPU_updateLace();
            SysUpdate();
        }
    }
}

/******************************************************************************/

void psxRcntWcount( u32 index, u32 value )
{
    psxRcntUpdate();

    _psxRcntWcount( index, value );
    psxRcntSet();
}

void psxRcntWmode( u32 index, u32 value )
{
    psxRcntUpdate();

    psxCounters[index].mode = value;
    psxCounters[index].irqState = 0;

    switch( index )
    {
        case 0:
            if( value & Rc0PixelClock )
            {
                psxCounters[index].rate = 5;
            }
            else
            {
                psxCounters[index].rate = 1;
            }
        break;
        case 1:
            if( value & Rc1HSyncClock )
            {
                psxCounters[index].rate = (PSXCLK / (FrameRate[Config.PsxType] * HSyncTotal[Config.PsxType]));
            }
            else
            {
                psxCounters[index].rate = 1;
            }
        break;
        case 2:
            if( value & Rc2OneEighthClock )
            {
                psxCounters[index].rate = 8;
            }
            else
            {
                psxCounters[index].rate = 1;
            }

            if( value & Rc2Disable )
            {
                psxCounters[index].rate = 0xffffffff;
            }
        break;
    }

    _psxRcntWcount( index, 0 );
    psxRcntSet();
}

void psxRcntWtarget( u32 index, u32 value )
{
    psxRcntUpdate();

    psxCounters[index].target = value;

    _psxRcntWcount( index, _psxRcntRcount( index ) );
    psxRcntSet();
}

/******************************************************************************/

u32 psxRcntRcount( u32 index )
{
    u32 count;

    psxRcntUpdate();

    count = _psxRcntRcount( index );

    // Parasite Eve 2 fix.
    if( Config.RCntFix )
    {
        if( index == 2 )
        {
            if( psxCounters[index].counterState == CountToTarget )
            {
                count /= BIAS;
            }
        }
    }

    return count;
}

u32 psxRcntRmode( u32 index )
{
    u32 mode;

    psxRcntUpdate();

    mode = psxCounters[index].mode;
    psxCounters[index].mode &= 0xe7ff;

    return mode;
}

u32 psxRcntRtarget( u32 index )
{
    return psxCounters[index].target;
}

/******************************************************************************/

void psxUpdateVSyncRate(void)
{
    psxCounters[3].target = (PSXCLK / (FrameRate[Config.PsxType] * HSyncTotal[Config.PsxType]));
    _psxRcntWcount( 3, _psxRcntRcount( 3 ) );
}

void psxRcntInit(void)
{
    s32 i;

    // rcnt 0.
    psxCounters[0].rate   = 1;
    psxCounters[0].irq    = 0x10;

    // rcnt 1.
    psxCounters[1].rate   = 1;
    psxCounters[1].irq    = 0x20;

    // rcnt 2.
    psxCounters[2].rate   = 1;
    psxCounters[2].irq    = 0x40;

    // rcnt base.
    psxCounters[3].rate   = 1;
    psxCounters[3].mode   = RcCountToTarget;
    psxCounters[3].target = (PSXCLK / (FrameRate[Config.PsxType] * HSyncTotal[Config.PsxType]));

    for( i = 0; i < CounterQuantity; ++i )
    {
        _psxRcntWcount( i, 0 );
    }

    hSyncCount = 0;
    spuSyncCount = 0;

    psxRcntSet();
}

/******************************************************************************/

int psxRcntFreeze( gzFile f, int Mode )
{
    gzfreeze( psxCounters, sizeof(psxCounters) );
    gzfreeze( &hSyncCount, sizeof(hSyncCount) );
    gzfreeze( &spuSyncCount, sizeof(spuSyncCount) );
    gzfreeze( &psxNextCounter, sizeof(psxNextCounter) );
    gzfreeze( &psxNextsCounter, sizeof(psxNextsCounter) );

    return 0;
}

/******************************************************************************/
