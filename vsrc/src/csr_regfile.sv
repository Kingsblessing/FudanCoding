`ifndef __CSR_REGFILE_SV
`define __CSR_REGFILE_SV

`ifdef VERILATOR
`include "include/common.sv"
`include "include/csr.sv"
`else
`include "common.sv"
`include "csr.sv"
`endif

import common::*;
import csr_pkg::*;

// Lab4：CSR 寄存器与读写（mhartid 只读；mcycle 每周期自增，写覆盖）
// dbg_* 为组合下一拍状态，供 Difftest 在 posedge 采样（与 NEMU 提交后一致）
module csr_regfile import common::*; import csr_pkg::*; (
    input  logic       clk,
    input  logic       reset,
    input  logic       csr_we,
    input  u12         csr_waddr,
    input  word_t      csr_wdata,
    input  u12         csr_raddr,
    output word_t      csr_rdata,
    output word_t      dbg_mstatus,
    output word_t      dbg_mtvec,
    output word_t      dbg_mip,
    output word_t      dbg_mie,
    output word_t      dbg_mscratch,
    output word_t      dbg_mcause,
    output word_t      dbg_mtval,
    output word_t      dbg_mepc,
    output word_t      dbg_mcycle,
    output word_t      dbg_mhartid,
    output word_t      dbg_satp
);
    word_t mstatus_q, mtvec_q, mip_q, mie_q, mscratch_q, mcause_q, mtval_q, mepc_q, mcycle_q, satp_q;

    word_t mstatus_d, mtvec_d, mip_d, mie_d, mscratch_d, mcause_d, mtval_d, mepc_d, mcycle_d, satp_d;

    assign dbg_mstatus  = mstatus_d;
    assign dbg_mtvec    = mtvec_d;
    assign dbg_mip      = mip_d;
    assign dbg_mie      = mie_d;
    assign dbg_mscratch = mscratch_d;
    assign dbg_mcause   = mcause_d;
    assign dbg_mtval    = mtval_d;
    assign dbg_mepc     = mepc_d;
    assign dbg_mcycle   = mcycle_d;
    assign dbg_mhartid  = 64'd0;
    assign dbg_satp     = satp_d;

    function automatic word_t apply_wmask(word_t wdata, word_t oldv, word_t wmask);
        return (wdata & wmask) | (oldv & ~wmask);
    endfunction

    always_comb begin
        csr_rdata = 64'b0;
        unique case (csr_raddr)
            CSR_MSTATUS:  csr_rdata = mstatus_q;
            CSR_MTVEC:    csr_rdata = mtvec_q;
            CSR_MIP:      csr_rdata = mip_q;
            CSR_MIE:      csr_rdata = mie_q;
            CSR_MSCRATCH: csr_rdata = mscratch_q;
            CSR_MCAUSE:   csr_rdata = mcause_q;
            CSR_MTVAL:    csr_rdata = mtval_q;
            CSR_MEPC:     csr_rdata = mepc_q;
            CSR_MCYCLE:   csr_rdata = mcycle_q;
            CSR_MHARTID:  csr_rdata = 64'd0;
            CSR_SATP:     csr_rdata = satp_q;
            default:      csr_rdata = 64'b0;
        endcase
    end

    always_comb begin
        mstatus_d  = mstatus_q;
        mtvec_d    = mtvec_q;
        mip_d      = mip_q;
        mie_d      = mie_q;
        mscratch_d = mscratch_q;
        mcause_d   = mcause_q;
        mtval_d    = mtval_q;
        mepc_d     = mepc_q;
        satp_d     = satp_q;
        mcycle_d   = mcycle_q;

        if (reset) begin
            mstatus_d  = 64'b0;
            mtvec_d    = 64'b0;
            mip_d      = 64'b0;
            mie_d      = 64'b0;
            mscratch_d = 64'b0;
            mcause_d   = 64'b0;
            mtval_d    = 64'b0;
            mepc_d     = 64'b0;
            mcycle_d   = 64'b0;
            satp_d     = 64'b0;
        end else begin
            if (csr_we && csr_waddr == CSR_MCYCLE)
                mcycle_d = csr_wdata;
            else
                mcycle_d = mcycle_q + 64'd1;

            if (csr_we && (csr_waddr != CSR_MHARTID) && (csr_waddr != CSR_MCYCLE)) begin
                unique case (csr_waddr)
                    CSR_MSTATUS:  mstatus_d  = apply_wmask(csr_wdata, mstatus_q, MSTATUS_MASK);
                    CSR_MTVEC:    mtvec_d    = apply_wmask(csr_wdata, mtvec_q, MTVEC_MASK);
                    CSR_MIP:      mip_d      = apply_wmask(csr_wdata, mip_q, MIP_MASK);
                    CSR_MIE:      mie_d      = csr_wdata;
                    CSR_MSCRATCH: mscratch_d = csr_wdata;
                    CSR_MCAUSE:   mcause_d   = csr_wdata;
                    CSR_MTVAL:    mtval_d    = csr_wdata;
                    CSR_MEPC:     mepc_d     = csr_wdata;
                    CSR_SATP:     satp_d     = csr_wdata;
                    default:      ;
                endcase
            end
        end
    end

    always_ff @(posedge clk) begin
        if (reset) begin
            mstatus_q  <= 64'b0;
            mtvec_q    <= 64'b0;
            mip_q      <= 64'b0;
            mie_q      <= 64'b0;
            mscratch_q <= 64'b0;
            mcause_q   <= 64'b0;
            mtval_q    <= 64'b0;
            mepc_q     <= 64'b0;
            mcycle_q   <= 64'b0;
            satp_q     <= 64'b0;
        end else begin
            mstatus_q  <= mstatus_d;
            mtvec_q    <= mtvec_d;
            mip_q      <= mip_d;
            mie_q      <= mie_d;
            mscratch_q <= mscratch_d;
            mcause_q   <= mcause_d;
            mtval_q    <= mtval_d;
            mepc_q     <= mepc_d;
            mcycle_q   <= mcycle_d;
            satp_q     <= satp_d;
        end
    end
endmodule

`endif
