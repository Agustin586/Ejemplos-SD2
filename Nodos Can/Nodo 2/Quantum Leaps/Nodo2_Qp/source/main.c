//$file${.::main.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: Nodo2.qm
// File:  ${.::main.c}
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
//$endhead${.::main.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"

#include "qpc.h"          // QP/C real-time embedded framework
#include "blinky.h"          // Board Support Package

#include "CanApi.h"
#include "can.h"

#include "Nodo_2.h"

enum BlinkySignals {
    SERIE_SIG = Q_USER_SIG,
    CAN_SIG,
    MAX_SIG,
};


/* Declaraciones de qp. */
//$declare${AOs::SerieEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${AOs::SerieEvt} ...........................................................
typedef struct {
// protected:
    QEvt super;
} SerieEvt;
//$enddecl${AOs::SerieEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${AOs::Blinky} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${AOs::Blinky} .............................................................
typedef struct Blinky {
// protected:
    QActive super;

// private:
    QTimeEvt timeEvt;
} Blinky;

extern Blinky Blinky_inst;

// private:
static void Blinky_ctor(Blinky * const me);

// protected:
static QState Blinky_initial(Blinky * const me, void const * const par);
static QState Blinky_Escritura(Blinky * const me, QEvt const * const e);
//$enddecl${AOs::Blinky} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//............................................................................
QActive * const AO_Blinky = &Blinky_inst.super;
static QF_MPOOL_EL(SerieEvt) poolSto[10];  // Cambia el tamaño según sea necesario

int main() {
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* Inicializacion de pines. */
    BOARD_InitLEDs();
    BOARD_InitButtons();

    /* Configuracion del systick. */
    SysTick_Config(CLOCK_GetCoreSysClkFreq() / 1000U);

    QF_init(); // initialize the framework
    QF_poolInit(poolSto, sizeof(poolSto), sizeof(SerieEvt));

    Blinky_ctor(&Blinky_inst); // explicitly call the "constructor"
    static QEvt const *blinky_queueSto[10];
    QACTIVE_START(AO_Blinky,
                  1U, // priority
                  blinky_queueSto, Q_DIM(blinky_queueSto),
                  (void *)0, 0U, // no stack
                  (void *)0);    // no initialization parameter
    return QF_run(); // run the QF application
}
//..........................................................................
void SysTick_Handler(void) {
    QTIMEEVT_TICK_X(0U, &l_SysTick_Handler); // time events at rate 0

    Nodo2_xtransfer();
}

//================ ask QM to define the Blinky class ================
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${AOs::Blinky} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${AOs::Blinky} .............................................................
Blinky Blinky_inst;

//${AOs::Blinky::ctor} .......................................................
static void Blinky_ctor(Blinky * const me) {
    /* Crea el objet activo. */
    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky_initial));
    /* Crea el evento de tiempo. */
    QTimeEvt_ctorX(&me->timeEvt, &me->super, CAN_SIG, 0U);
}

//${AOs::Blinky::SM} .........................................................
static QState Blinky_initial(Blinky * const me, void const * const par) {
    //${AOs::Blinky::SM::initial}
    (void)par; // unused parameter
    // Inicializo el constructor del timer
    QTimeEvt_armX(&me->timeEvt,
    TIEMPO_DE_MUESTRA_ESTADOS, TIEMPO_DE_MUESTRA_ESTADOS);

    // Inicializacion de Nodo 2
    BSP_initMsg();

    /* Inicialziacion del nodo 2. */
    Nodo2_init();

    QS_FUN_DICTIONARY(&Blinky_Escritura);

    return Q_TRAN(&Blinky_Escritura);
}

//${AOs::Blinky::SM::Escritura} ..............................................
static QState Blinky_Escritura(Blinky * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        //${AOs::Blinky::SM::Escritura}
        case Q_ENTRY_SIG: {
            BSP_ledOff(); // Acciones que se ejecutan en el entry
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Blinky::SM::Escritura::SERIE}
        case SERIE_SIG: {
            /* Escritura en el puerto serie. */
            Nodo2_serialPort();
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Blinky::SM::Escritura::CAN}
        case CAN_SIG: {
            /* Escritura en el modulo can. */
            Nodo2_writeToBus();
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
//$enddef${AOs::Blinky} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::setSerieEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${AOs::setSerieEvt} ........................................................
void setSerieEvt(void) {
    // Crear un evento sin parámetros de tipo EVENTO_SIMPLE_SIG
    //QEvt *evento = Q_NEW(QEvt, SERIE_SIG);
    SerieEvt *evento = Q_NEW(SerieEvt, SERIE_SIG);

    // Postear el evento a la máquina de estados activa
    //QACTIVE_POST((QActive *)&Blinky_inst.super, &Blinky_inst.evento.super, 0);
    QACTIVE_POST(AO_Blinky, &evento->super, 0);
}
//$enddef${AOs::setSerieEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
