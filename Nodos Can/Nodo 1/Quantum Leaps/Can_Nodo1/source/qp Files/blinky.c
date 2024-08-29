//$file${.::blinky.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: Can_Nodo1.qm
// File:  ${.::blinky.c}
//
// This code has been generated by QM 6.1.1 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::blinky.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpc.h"    // QP/C real-time embedded framework
#include "bsp.h"    // Board Support Package interface
#include "Includes/Temp.h"    // Archivo del periferico temperatura

// ask QM to declare the Blinky class
//$declare${AOs::MEF_Temp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${AOs::MEF_Temp} ...........................................................
typedef struct {
// protected:
    QActive super;

// private:
    QTimeEvt timeEvt_ReadTemp;
} MEF_Temp;

// protected:
static QState MEF_Temp_initial(MEF_Temp * const me, void const * const par);
static QState MEF_Temp_Reset(MEF_Temp * const me, QEvt const * const e);
static QState MEF_Temp_Lectura(MEF_Temp * const me, QEvt const * const e);
//$enddecl${AOs::MEF_Temp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

