//
// Created by wangzhizun on 2025/9/27.
//

#ifndef GEM5_SIM_FETCH_HH
#define GEM5_SIM_FETCH_HH

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "base/types.hh"

#include <optional>


namespace gem5
{
namespace o3
{

enum class BrType {
    NotCfi,
    Branch,
    Jal,
    Jalr
};

struct RiscvInstructionInfo {
    bool isRVC;
    BrType brType;
    bool isCall;
    bool isRet;

    // TODO At present, we still have one problem
    // If the branch instruction points to the next sequential program counter, then issues may arise during processing.
    // However, in theory, the compiler will not generate such instructions.
    bool isJump;

    uint32_t rd;
    uint32_t rs1;
};

struct TraceInfo {
    Addr pc;
    Addr npc;
    uint32_t inst;
    RiscvInstructionInfo instInfo;
    uint64_t ftqIdx;
    bool isLastFtqEntry;
    bool isJumpOrTaken;
};

struct FTQEntry {
    uint64_t firstTraceIdx;
    uint32_t numInsts;
};


class SimFetch
{
public:
    SimFetch(const std::string& trace_path);

    bool fetchFinish;

    bool fetchSingleInst (Addr& pc, uint32_t& inst);

    bool fillNextFTQ();

    std::map<uint64_t, TraceInfo> traceQueue;
    std::map<uint64_t, FTQEntry> ftq;

    std::optional<TraceInfo> getTraceInfo(bool is_peek = false);
    FTQEntry getFTQEntry(bool is_peek = false);

    void advanceFtqReadIdx(uint32_t count = 1);

    uint64_t getTraceQueueIdx()
    {
        return traceQueueReadIdx;
    }

    uint64_t getFtqReadIdx()
    {
        return ftqReadIdx;
    };


    bool redirect(uint64_t ftq_idx, uint64_t trace_queue_idx, Addr pc, bool type);

    bool commit(uint64_t trace_queue_idx);
private:
    TraceInfo readerAndParseNextTrace(bool is_peek = false);

    void fillTraceQueue();

    bool isRvC(uint32_t instr);
    BrType getBranchType(uint32_t instr, bool rvc);
    RiscvInstructionInfo analyzeInstr(uint32_t inst);

    void traceQueuEnq(TraceInfo trace_info);
    void ftqEnq(FTQEntry ftq_entry);

    std::istream& peek_line(std::istream& is, std::string& line);

    // var

    std::ifstream traceStream;

    uint64_t traceReaderIdx;

    uint64_t traceQueueEnqIdx;
    uint64_t traceQueueDeqIdx;
    uint64_t traceQueueReadIdx;

    uint64_t ftqEnqIdx;
    uint64_t ftqDeqIdx;
    uint64_t ftqReadIdx;

    uint64_t fetchHead;

    bool needNewFtqEntry;
    const uint64_t maxTraceQueueSize = 1024;
    const uint64_t maxFtqSize = 64;

    const uint32_t maxFtqBytes = 32;
};

}
}
#endif  // GEM5_SIM_FETCH_HH
