#include "slang-ir-lower-reinterpret.h"
#include "slang-ir.h"
#include "slang-ir-insts.h"
#include "slang-ir-layout.h"
#include "slang-ir-any-value-marshalling.h"
#include "slang-ir-any-value-inference.h"

namespace Slang
{

struct ReinterpretLoweringContext
{
    TargetRequest* targetReq;
    DiagnosticSink* sink;
    IRModule* module;
    OrderedHashSet<IRInst*> workList;

    void addToWorkList(IRInst* inst)
    {
        if (workList.contains(inst))
            return;

        workList.add(inst);
    }

    void processInst(IRInst* inst)
    {
        switch (inst->getOp())
        {
        case kIROp_Reinterpret:
            processReinterpret(inst);
            break;
        default:
            break;
        }
    }

    void processModule()
    {
        addToWorkList(module->getModuleInst());

        while (workList.getCount() != 0)
        {
            IRInst* inst = workList.getLast();

            workList.removeLast();

            processInst(inst);

            for (auto child = inst->getLastChild(); child; child = child->getPrevInst())
            {
                addToWorkList(child);
            }
        }
    }

    void processReinterpret(IRInst* inst)
    {
        auto operand = inst->getOperand(0);
        auto fromType = operand->getDataType();
        auto toType = inst->getDataType();
        SlangInt fromTypeSize = getAnyValueSize(fromType);
        if (fromTypeSize < 0)
        {
            sink->diagnose(inst->sourceLoc, Slang::Diagnostics::typeCannotBePackedIntoAnyValue, fromType);
        }
        SlangInt toTypeSize = getAnyValueSize(toType);
        if (toTypeSize < 0)
        {
            sink->diagnose(inst->sourceLoc, Slang::Diagnostics::typeCannotBePackedIntoAnyValue, toType);
        }
        SlangInt anyValueSize = Math::Max(fromTypeSize, toTypeSize);

        IRBuilder builder(module);
        builder.setInsertBefore(inst);
        auto anyValueType = builder.getAnyValueType(builder.getIntValue(builder.getUIntType(), anyValueSize));
        auto packInst = builder.emitPackAnyValue(
            anyValueType,
            operand);
        auto unpackInst = builder.emitUnpackAnyValue(toType, packInst);
        inst->replaceUsesWith(unpackInst);
        inst->removeAndDeallocate();
    }
};

void lowerReinterpret(TargetRequest* targetReq, IRModule* module, DiagnosticSink* sink)
{
    // Before processing reinterpret insts, ensure that existential types without 
    // user-defined sizes have inferred sizes where possible.
    // 
    inferAnyValueSizeWhereNecessary(module);

    ReinterpretLoweringContext context;
    context.module = module;
    context.targetReq = targetReq;
    context.sink = sink;
    context.processModule();
}

}
