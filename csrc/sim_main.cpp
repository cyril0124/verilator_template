#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include <nvboard.h>

#include "Vtop.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
TOP_NAME* dut = NULL;

void nvboard_bind_all_pins(Vtop* top);

static void single_cycle(void) {
    contextp->timeInc(1);

    #ifdef NVBOARD_ENABLE
        nvboard_update();
    #endif

        dut->eval();

    #ifdef TRACE_ENABLE
        if(tfp) tfp->dump((vluint64_t)(10*contextp->time()-2));
    #endif

        dut->clk = 1;
        dut->eval();
    #ifdef TRACE_ENABLE
        if(tfp) tfp->dump((vluint64_t)(10*contextp->time()));
    #endif

        dut->clk = 0;
        dut->eval();
    #ifdef TRACE_ENABLE
        if(tfp) {
            tfp->dump((vluint64_t)(10*contextp->time()+5));
            tfp->flush();
        }
    #endif
}

static void reset(int n) {
    dut->rst_n = 0;
    while (n -- > 0) single_cycle();
    dut->rst_n = 1;
}

static void sim_init(void) {
    contextp = new VerilatedContext;
    dut = new TOP_NAME;

    #ifdef TRACE_ENABLE
        tfp = new VerilatedVcdC;
        contextp->traceEverOn(true);
        dut->trace(tfp,99);
        tfp->open("waveform.vcd");
    #endif

    #ifdef NVBOARD_ENABLE
        nvboard_bind_all_pins(dut);
        nvboard_init();
    #endif

}

static void sim_exit(void) {
    dut->final();
    single_cycle();

    delete contextp;
    delete dut;

    #ifdef TRACE_ENABLE
        tfp->close();
        delete tfp;
    #endif

    #ifdef NVBOARD_ENABLE
        nvboard_quit();
    #endif
}




int main(int argc, char** argv, char** env) {
    sim_init();

    //reset dut
    reset(10);

#ifdef NVBOARD_ENABLE
    while(1) {
#else
    while(contextp->time()<MAX_SIM_TIME) {
#endif
        //combinational logic code
        int a = rand() & 1;
        int b = rand() & 1;
        dut->a = a;
        dut->b = b;

        //circuit update
        single_cycle();

        //other code
        printf("[%g]a = %d, b = %d, f = %d, data = %d\n",(double)contextp->time(), a, b, dut->f, dut->data);
        assert(dut->f == (a ^ b));
    }

    sim_exit();

    // Return good completion status
    return 0;
}
