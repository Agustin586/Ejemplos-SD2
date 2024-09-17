//$file${src::qf::qep_hsm.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qpc.qm
// File:  ${src::qf::qep_hsm.c}
//
// This code has been generated by QM 6.1.1 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This code is covered by the following QP license:
// License #    : LicenseRef-QL-dual
// Issued to    : Any user of the QP/C real-time embedded framework
// Framework(s) : qpc
// Support ends : 2024-12-31
// License scope:
//
// Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${src::qf::qep_hsm.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#define QP_IMPL           // this is QP implementation
#include "qp_port.h"      // QP port
#include "qp_pkg.h"       // QP package-scope interface
#include "qsafe.h"        // QP Functional Safety (FuSa) Subsystem
#ifdef Q_SPY              // QS software tracing enabled?
    #include "qs_port.h"  // QS port
    #include "qs_pkg.h"   // QS facilities for pre-defined trace records
#else
    #include "qs_dummy.h" // disable the QS software tracing
#endif // Q_SPY

Q_DEFINE_THIS_MODULE("qep_hsm")

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QEP::QP_versionStr[8]} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${QEP::QP_versionStr[8]} ...................................................
char const QP_versionStr[8] = QP_VERSION_STR;
//$enddef${QEP::QP_versionStr[8]} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
//! @cond INTERNAL

//$define${QEP::QEvt::reserved_[4]} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
QEvt const QEvt_reserved_[4] = {
    QEVT_INITIALIZER(Q_EMPTY_SIG),
    QEVT_INITIALIZER(Q_ENTRY_SIG),
    QEVT_INITIALIZER(Q_EXIT_SIG),
    QEVT_INITIALIZER(Q_INIT_SIG)
};

//$enddef${QEP::QEvt::reserved_[4]} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

enum {
    // maximum depth of state nesting in a QHsm (including the top level),
    // must be >= 3
    QHSM_MAX_NEST_DEPTH_ = 6
};

// helper macro to handle reserved event in an QHsm
#define QHSM_RESERVED_EVT_(state_, sig_) \
    ((*(state_))(me, &QEvt_reserved_[(sig_)]))

// helper macro to trace state entry
#define QS_STATE_ENTRY_(state_, qsId_)         \
    QS_CRIT_ENTRY();                           \
    QS_MEM_SYS();                              \
    QS_BEGIN_PRE_(QS_QEP_STATE_ENTRY, (qsId_)) \
        QS_OBJ_PRE_(me);                       \
        QS_FUN_PRE_(state_);                   \
    QS_END_PRE_()                              \
    QS_MEM_APP();                              \
    QS_CRIT_EXIT()

// helper macro to trace state exit
#define QS_STATE_EXIT_(state_, qsId_)          \
    QS_CRIT_ENTRY();                           \
    QS_MEM_SYS();                              \
    QS_BEGIN_PRE_(QS_QEP_STATE_EXIT, (qsId_))  \
        QS_OBJ_PRE_(me);                       \
        QS_FUN_PRE_(state_);                   \
    QS_END_PRE_()                              \
    QS_MEM_APP();                              \
    QS_CRIT_EXIT()

//! @endcond

//$define${QEP::QHsm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${QEP::QHsm} ...............................................................

//${QEP::QHsm::ctor} .........................................................
//! @protected @memberof QHsm
void QHsm_ctor(QHsm * const me,
    QStateHandler const initial)
{
    static struct QAsmVtable const vtable = { // QAsm virtual table
        &QHsm_init_,
        &QHsm_dispatch_,
        &QHsm_isIn_
    #ifdef Q_SPY
        ,&QHsm_getStateHandler_
    #endif
    };
    // do not call the QAsm_ctor() here
    me->super.vptr      = &vtable;
    me->super.state.fun = Q_STATE_CAST(&QHsm_top);
    me->super.temp.fun  = initial;
}

//${QEP::QHsm::init_} ........................................................
//! @private @memberof QHsm
void QHsm_init_(
    QAsm * const me,
    void const * const e,
    uint_fast8_t const qsId)
{
    QF_CRIT_STAT

    #ifdef Q_SPY
    QS_CRIT_ENTRY();
    QS_MEM_SYS();
    if ((QS_priv_.flags & 0x01U) == 0U) {
        QS_priv_.flags |= 0x01U;
        QS_MEM_APP();
        QS_CRIT_EXIT();
        QS_FUN_DICTIONARY(&QHsm_top);
    }
    else {
        QS_MEM_APP();
        QS_CRIT_EXIT();
    }
    #else
    Q_UNUSED_PAR(qsId);
    #endif

    QStateHandler t = me->state.fun;

    QF_CRIT_ENTRY();
    Q_REQUIRE_INCRIT(200, (me->vptr != (struct QAsmVtable *)0)
                      && (me->temp.fun != Q_STATE_CAST(0))
                      && (t == Q_STATE_CAST(&QHsm_top)));
    QF_CRIT_EXIT();

    // execute the top-most initial tran.
    QState r = (*me->temp.fun)(me, Q_EVT_CAST(QEvt));

    QF_CRIT_ENTRY();
    // the top-most initial tran. must be taken
    Q_ASSERT_INCRIT(210, r == Q_RET_TRAN);

    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_STATE_INIT, qsId)
        QS_OBJ_PRE_(me);           // this state machine object
        QS_FUN_PRE_(t);            // the source state
        QS_FUN_PRE_(me->temp.fun); // the target of the initial tran.
    QS_END_PRE_()
    QS_MEM_APP();

    QF_CRIT_EXIT();

    // drill down into the state hierarchy with initial transitions...
    int_fast8_t limit = QHSM_MAX_NEST_DEPTH_; // loop hard limit
    do {
        QStateHandler path[QHSM_MAX_NEST_DEPTH_]; // tran entry path array
        int_fast8_t ip = 0; // tran entry path index

        path[0] = me->temp.fun;
        (void)QHSM_RESERVED_EVT_(me->temp.fun, Q_EMPTY_SIG);
        while ((me->temp.fun != t) && (ip < (QHSM_MAX_NEST_DEPTH_ - 1))) {
            ++ip;
            path[ip] = me->temp.fun;
            (void)QHSM_RESERVED_EVT_(me->temp.fun, Q_EMPTY_SIG);
        }
        QF_CRIT_ENTRY();
        // The initial transition source state must be reached
        // Too many state nesting levels or "malformed" HSM.
        Q_ASSERT_INCRIT(220, me->temp.fun == t);
        QF_CRIT_EXIT();

        me->temp.fun = path[0];

        // retrace the entry path in reverse (desired) order...
        do {
            // enter path[ip]
            if (QHSM_RESERVED_EVT_(path[ip], Q_ENTRY_SIG)
                == Q_RET_HANDLED)
            {
                QS_STATE_ENTRY_(path[ip], qsId);
            }
            --ip;
        } while (ip >= 0);

        t = path[0]; // current state becomes the new source

        r = QHSM_RESERVED_EVT_(t, Q_INIT_SIG); // execute initial tran.

    #ifdef Q_SPY
        if (r == Q_RET_TRAN) {
            QS_CRIT_ENTRY();
            QS_MEM_SYS();
            QS_BEGIN_PRE_(QS_QEP_STATE_INIT, qsId)
                QS_OBJ_PRE_(me);           // this state machine object
                QS_FUN_PRE_(t);            // the source state
                QS_FUN_PRE_(me->temp.fun); // the target of the initial tran.
            QS_END_PRE_()
            QS_MEM_APP();
            QS_CRIT_EXIT();
        }
    #endif // Q_SPY

        --limit;
    } while ((r == Q_RET_TRAN) && (limit > 0));

    QF_CRIT_ENTRY();
    // Loop limit must not be reached.
    // Too many state nesting levels or likely "malformed" HSM
    Q_ENSURE_INCRIT(290, limit > 0);

    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_INIT_TRAN, qsId)
        QS_TIME_PRE_();    // time stamp
        QS_OBJ_PRE_(me);   // this state machine object
        QS_FUN_PRE_(t);    // the new active state
    QS_END_PRE_()
    QS_MEM_APP();

    QF_CRIT_EXIT();

    me->state.fun = t;   // change the current active state
    #ifndef Q_UNSAFE
    me->temp.uint = ~me->state.uint;
    #endif
}

//${QEP::QHsm::dispatch_} ....................................................
//! @private @memberof QHsm
void QHsm_dispatch_(
    QAsm * const me,
    QEvt const * const e,
    uint_fast8_t const qsId)
{
    #ifndef Q_SPY
    Q_UNUSED_PAR(qsId);
    #endif

    QStateHandler s = me->state.fun;
    QStateHandler t = s;
    QF_CRIT_STAT

    QF_CRIT_ENTRY();
    Q_REQUIRE_INCRIT(300, (s != Q_STATE_CAST(0))
        && (me->state.uint == (uintptr_t)(~me->temp.uint)));
    Q_REQUIRE_INCRIT(302, QEvt_verify_(e));

    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_DISPATCH, qsId)
        QS_TIME_PRE_();      // time stamp
        QS_SIG_PRE_(e->sig); // the signal of the event
        QS_OBJ_PRE_(me);     // this state machine object
        QS_FUN_PRE_(s);      // the current state
    QS_END_PRE_()
    QS_MEM_APP();

    QF_CRIT_EXIT();

    // process the event hierarchically...
    QState r;
    me->temp.fun = s;
    int_fast8_t limit = QHSM_MAX_NEST_DEPTH_; // loop hard limit
    do {
        s = me->temp.fun;
        r = (*s)(me, e); // invoke state handler s

        if (r == Q_RET_UNHANDLED) { // unhandled due to a guard?

            QS_CRIT_ENTRY();
            QS_MEM_SYS();
            QS_BEGIN_PRE_(QS_QEP_UNHANDLED, qsId)
                QS_SIG_PRE_(e->sig); // the signal of the event
                QS_OBJ_PRE_(me);     // this state machine object
                QS_FUN_PRE_(s);      // the current state
            QS_END_PRE_()
            QS_MEM_APP();
            QS_CRIT_EXIT();

            r = QHSM_RESERVED_EVT_(s, Q_EMPTY_SIG); // superstate of s
        }

        --limit;
    } while ((r == Q_RET_SUPER) && (limit > 0));

    QF_CRIT_ENTRY();
    Q_ASSERT_INCRIT(310, limit > 0);
    QF_CRIT_EXIT();

    if (r >= Q_RET_TRAN) { // regular tran. taken?
        QStateHandler path[QHSM_MAX_NEST_DEPTH_];

        path[0] = me->temp.fun; // tran. target
        path[1] = t; // current state
        path[2] = s; // tran. source

        // exit current state to tran. source s...
        limit = QHSM_MAX_NEST_DEPTH_; // loop hard limit
        for (; (t != s) && (limit > 0); t = me->temp.fun) {
            // exit from t
            if (QHSM_RESERVED_EVT_(t, Q_EXIT_SIG) == Q_RET_HANDLED) {
                QS_STATE_EXIT_(t, qsId);
                // find superstate of t
                (void)QHSM_RESERVED_EVT_(t, Q_EMPTY_SIG);
            }
            --limit;
        }
        QF_CRIT_ENTRY();
        Q_ASSERT_INCRIT(320, limit > 0);
        QF_CRIT_EXIT();

        int_fast8_t ip = QHsm_tran_(me, path, qsId); // take the tran.

    #ifdef Q_SPY
        if (r == Q_RET_TRAN_HIST) {
            QS_CRIT_ENTRY();
            QS_MEM_SYS();
            QS_BEGIN_PRE_(QS_QEP_TRAN_HIST, qsId)
                QS_OBJ_PRE_(me);      // this state machine object
                QS_FUN_PRE_(t);       // the source of the tran.
                QS_FUN_PRE_(path[0]); // the target of the tran. to history
            QS_END_PRE_()
            QS_MEM_APP();
            QS_CRIT_EXIT();
        }
    #endif // Q_SPY

        // execute state entry actions in the desired order...
        for (; ip >= 0; --ip) {
            // enter path[ip]
            if (QHSM_RESERVED_EVT_(path[ip], Q_ENTRY_SIG)
                == Q_RET_HANDLED)
            {
                QS_STATE_ENTRY_(path[ip], qsId);
            }
        }
        t = path[0];      // stick the target into register
        me->temp.fun = t; // update the next state

        // drill into the target hierarchy...
        while (QHSM_RESERVED_EVT_(t, Q_INIT_SIG) == Q_RET_TRAN) {

            QS_CRIT_ENTRY();
            QS_MEM_SYS();
            QS_BEGIN_PRE_(QS_QEP_STATE_INIT, qsId)
                QS_OBJ_PRE_(me);           // this state machine object
                QS_FUN_PRE_(t);            // the source (pseudo)state
                QS_FUN_PRE_(me->temp.fun); // the target of the tran.
            QS_END_PRE_()
            QS_MEM_APP();
            QS_CRIT_EXIT();

            ip = 0;
            path[0] = me->temp.fun;

            // find superstate
            (void)QHSM_RESERVED_EVT_(me->temp.fun, Q_EMPTY_SIG);

            while ((me->temp.fun != t) && (ip < (QHSM_MAX_NEST_DEPTH_ - 1))) {
                ++ip;
                path[ip] = me->temp.fun;
                // find superstate
                (void)QHSM_RESERVED_EVT_(me->temp.fun, Q_EMPTY_SIG);
            }
            QF_CRIT_ENTRY();
            // The initial transition source state must be reached.
            // Too many state nesting levels or "malformed" HSM.
            Q_ASSERT_INCRIT(330, me->temp.fun == t);
            QF_CRIT_EXIT();

            me->temp.fun = path[0];

            // retrace the entry path in reverse (correct) order...
            do {
                // enter path[ip]
                if (QHSM_RESERVED_EVT_(path[ip], Q_ENTRY_SIG)
                    == Q_RET_HANDLED)
                {
                    QS_STATE_ENTRY_(path[ip], qsId);
                }
                --ip;
            } while (ip >= 0);

            t = path[0]; // current state becomes the new source
        }

        QS_CRIT_ENTRY();
        QS_MEM_SYS();
        QS_BEGIN_PRE_(QS_QEP_TRAN, qsId)
            QS_TIME_PRE_();      // time stamp
            QS_SIG_PRE_(e->sig); // the signal of the event
            QS_OBJ_PRE_(me);     // this state machine object
            QS_FUN_PRE_(s);      // the source of the tran.
            QS_FUN_PRE_(t);      // the new active state
        QS_END_PRE_()
        QS_MEM_APP();
        QS_CRIT_EXIT();
    }

    #ifdef Q_SPY
    else if (r == Q_RET_HANDLED) {
        QS_CRIT_ENTRY();
        QS_MEM_SYS();
        QS_BEGIN_PRE_(QS_QEP_INTERN_TRAN, qsId)
            QS_TIME_PRE_();      // time stamp
            QS_SIG_PRE_(e->sig); // the signal of the event
            QS_OBJ_PRE_(me);     // this state machine object
            QS_FUN_PRE_(s);      // the source state
        QS_END_PRE_()
        QS_MEM_APP();
        QS_CRIT_EXIT();
    }
    else {
        QS_CRIT_ENTRY();
        QS_MEM_SYS();
        QS_BEGIN_PRE_(QS_QEP_IGNORED, qsId)
            QS_TIME_PRE_();      // time stamp
            QS_SIG_PRE_(e->sig); // the signal of the event
            QS_OBJ_PRE_(me);     // this state machine object
            QS_FUN_PRE_(me->state.fun); // the current state
        QS_END_PRE_()
        QS_MEM_APP();
        QS_CRIT_EXIT();
    }
    #endif // Q_SPY

    me->state.fun = t; // change the current active state
    #ifndef Q_UNSAFE
    me->temp.uint = ~me->state.uint;
    #endif
}

//${QEP::QHsm::getStateHandler_} .............................................
#ifdef Q_SPY
//! @private @memberof QHsm
QStateHandler QHsm_getStateHandler_(QAsm * const me) {
    return me->state.fun;
}
#endif // def Q_SPY

//${QEP::QHsm::isIn_} ........................................................
//! @private @memberof QHsm
bool QHsm_isIn_(
    QAsm * const me,
    QStateHandler const state)
{
    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    Q_REQUIRE_INCRIT(602, me->state.uint
                      == (uintptr_t)(~me->temp.uint));
    QF_CRIT_EXIT();

    bool inState = false; // assume that this HSM is not in 'state'

    // scan the state hierarchy bottom-up
    QStateHandler s = me->state.fun;
    int_fast8_t limit = QHSM_MAX_NEST_DEPTH_ + 1; // loop hard limit
    QState r = Q_RET_SUPER;
    for (; (r != Q_RET_IGNORED) && (limit > 0); --limit) {
        if (s == state) { // do the states match?
            inState = true;  // 'true' means that match found
            break; // break out of the for-loop
        }
        else {
            r = QHSM_RESERVED_EVT_(s, Q_EMPTY_SIG);
            s = me->temp.fun;
        }
    }

    QF_CRIT_ENTRY();
    Q_ENSURE_INCRIT(690, limit > 0);
    QF_CRIT_EXIT();

    #ifndef Q_UNSAFE
    me->temp.uint = ~me->state.uint;
    #endif

    return inState; // return the status
}

//${QEP::QHsm::childState} ...................................................
//! @public @memberof QHsm
QStateHandler QHsm_childState(QHsm * const me,
    QStateHandler const parent)
{
    QStateHandler child = me->super.state.fun; // start with current state
    bool isFound = false; // start with the child not found

    // establish stable state configuration
    me->super.temp.fun = child;
    QState r;
    do {
        // is this the parent of the current child?
        if (me->super.temp.fun == parent) {
            isFound = true; // child is found
            r = Q_RET_IGNORED; // break out of the loop
        }
        else {
            child = me->super.temp.fun;
            r = QHSM_RESERVED_EVT_(me->super.temp.fun, Q_EMPTY_SIG);
        }
    } while (r != Q_RET_IGNORED); // the top state not reached

    #ifndef Q_UNSAFE
    me->super.temp.uint = ~me->super.state.uint;
    #endif

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    Q_ASSERT_INCRIT(890, isFound);
    QF_CRIT_EXIT();

    return child; // return the child
}

//${QEP::QHsm::tran_} ........................................................
//! @private @memberof QHsm
int_fast8_t QHsm_tran_(
    QAsm * const me,
    QStateHandler * const path,
    uint_fast8_t const qsId)
{
    #ifndef Q_SPY
    Q_UNUSED_PAR(qsId);
    #endif

    int_fast8_t ip = -1; // tran. entry path index
    QStateHandler t = path[0];
    QStateHandler const s = path[2];
    QF_CRIT_STAT

    // (a) check source==target (tran. to self)...
    if (s == t) {
        // exit source s
        if (QHSM_RESERVED_EVT_(s, Q_EXIT_SIG) == Q_RET_HANDLED) {
            QS_STATE_EXIT_(s, qsId);
        }
        ip = 0; // enter the target
    }
    else {
        // find superstate of target
        (void)QHSM_RESERVED_EVT_(t, Q_EMPTY_SIG);

        t = me->temp.fun;

        // (b) check source==target->super...
        if (s == t) {
            ip = 0; // enter the target
        }
        else {
            // find superstate of src
            (void)QHSM_RESERVED_EVT_(s, Q_EMPTY_SIG);

            // (c) check source->super==target->super...
            if (me->temp.fun == t) {
                // exit source s
                if (QHSM_RESERVED_EVT_(s, Q_EXIT_SIG) == Q_RET_HANDLED) {
                    QS_STATE_EXIT_(s, qsId);
                }
                ip = 0; // enter the target
            }
            else {
                // (d) check source->super==target...
                if (me->temp.fun == path[0]) {
                    // exit source s
                    if (QHSM_RESERVED_EVT_(s, Q_EXIT_SIG) == Q_RET_HANDLED) {
                        QS_STATE_EXIT_(s, qsId);
                    }
                }
                else {
                    // (e) check rest of source==target->super->super..
                    // and store the entry path along the way
                    int_fast8_t iq = 0; // indicate that LCA was found
                    ip = 1; // enter target and its superstate
                    path[1] = t;      // save the superstate of target
                    t = me->temp.fun; // save source->super

                    // find target->super->super...
                    QState r = QHSM_RESERVED_EVT_(path[1], Q_EMPTY_SIG);
                    while ((r == Q_RET_SUPER)
                           && (ip < (QHSM_MAX_NEST_DEPTH_ - 1)))
                    {
                        ++ip;
                        path[ip] = me->temp.fun; // store the entry path
                        if (me->temp.fun == s) { // is it the source?
                            iq = 1; // indicate that the LCA found
                            --ip; // do not enter the source
                            r = Q_RET_HANDLED; // terminate the loop
                        }
                        else { // it is not the source, keep going up
                            r = QHSM_RESERVED_EVT_(me->temp.fun, Q_EMPTY_SIG);
                        }
                    }
                    QF_CRIT_ENTRY();
                    // Tran. source must be found within the nesting depth
                    // Too many state nesting levels or "malformed" HSM.
                    Q_ASSERT_INCRIT(510, r != Q_RET_SUPER);
                    QF_CRIT_EXIT();

                    // the LCA not found yet?
                    if (iq == 0) {
                        // exit source s
                        if (QHSM_RESERVED_EVT_(s, Q_EXIT_SIG)
                            == Q_RET_HANDLED)
                        {
                            QS_STATE_EXIT_(s, qsId);
                        }

                        // (f) check the rest of source->super
                        //                  == target->super->super...
                        iq = ip;
                        r = Q_RET_IGNORED; // indicate that the LCA NOT found
                        do {
                            if (t == path[iq]) { // is this the LCA?
                                r = Q_RET_HANDLED; // indicate the LCA found
                                ip = iq - 1; // do not enter the LCA
                                iq = -1; // cause termination of the loop
                            }
                            else {
                                --iq; // try lower superstate of target
                            }
                        } while (iq >= 0);

                        // the LCA not found yet?
                        if (r != Q_RET_HANDLED) {
                            // (g) check each source->super->...
                            // for each target->super...
                            r = Q_RET_IGNORED; // keep looping
                            int_fast8_t limit = QHSM_MAX_NEST_DEPTH_;
                            do {
                                // exit from t
                                if (QHSM_RESERVED_EVT_(t, Q_EXIT_SIG)
                                    == Q_RET_HANDLED)
                                {
                                    QS_STATE_EXIT_(t, qsId);
                                    // find superstate of t
                                    (void)QHSM_RESERVED_EVT_(t, Q_EMPTY_SIG);
                                }
                                t = me->temp.fun; // set to super of t
                                iq = ip;
                                do {
                                    // is this the LCA?
                                    if (t == path[iq]) {
                                        ip = iq - 1; // do not enter the LCA
                                        iq = -1;     // break out of inner loop
                                        r = Q_RET_HANDLED; // break outer loop
                                    }
                                    else {
                                        --iq;
                                    }
                                } while (iq >= 0);

                                --limit;
                            } while ((r != Q_RET_HANDLED) && (limit > 0));
                            QF_CRIT_ENTRY();
                            Q_ASSERT_INCRIT(530, limit > 0);
                            QF_CRIT_EXIT();
                        }
                    }
                }
            }
        }
    }
    QF_CRIT_ENTRY();
    Q_ENSURE_INCRIT(590, ip < QHSM_MAX_NEST_DEPTH_);
    QF_CRIT_EXIT();
    return ip;
}

//${QEP::QHsm::top} ..........................................................
//! @protected @memberof QAsm
QState QHsm_top(QHsm const * const me,
    QEvt const * const e)
{
    Q_UNUSED_PAR(me);
    Q_UNUSED_PAR(e);
    return Q_RET_IGNORED; // the top state ignores all events
}
//$enddef${QEP::QHsm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
