#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "verilated.h"
#include "testb.h"
#include "Vtop.h"

#ifdef NVBOARD_ENABLE
    #include <nvboard.h>
#endif

#ifdef NVBOARD_ENABLE
    void nvboard_bind_all_pins(TOP_NAME* top);
#endif

#define	TOPASSERT(A) do { if (!(A)) { closetrace(); } assert(A); } while(0)

class TOP_TB: public TESTB<TOP_NAME> {
    public:
        TOP_TB(void) {
            #ifdef NVBOARD_ENABLE
                nvboard_bind_all_pins(m_core);
                nvboard_init();
            #endif
        }

        ~TOP_TB(void) {
            #ifdef NVBOARD_ENABLE
                nvboard_quit();
            #endif
        }

        void reset(void) {
            TESTB<TOP_NAME>::reset();
        }

        void tick(void) {
            //combinational code
            //
            int a = rand() & 1;
            int b = rand() & 1;
            m_core->a = a;
            m_core->b = b;

            #ifdef NVBOARD_ENABLE
                nvboard_update();
            #endif

            TESTB<TOP_NAME>::tick();

            //other code 
            //
            printf("[%4lld]a = %d, b = %d, f = %d, data = %d\n",(unsigned long long)TESTB<TOP_NAME>::m_tickcount, a, b, m_core->f, m_core->data);
            TOPASSERT(m_core->f == (a ^ b));
        }

};


int main(int argc, char** argv, char** env) {
    //set up
    Verilated::commandArgs(argc, argv);
    TOP_TB	*tb = new TOP_TB();

    #ifdef TRACE_ENABLE
        tb->opentrace("waveform.vcd");
    #endif

    #ifdef NVBOARD_ENABLE
        while(1) {
    #else
        while(tb->m_tickcount < MAX_SIM_TIME) {
    #endif
        //circuit update
        tb->tick();
    }

    delete tb;

    // Return good completion status
    exit(EXIT_SUCCESS);
}
