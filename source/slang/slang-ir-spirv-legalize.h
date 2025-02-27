// slang-ir-spirv-legalize.h
#pragma once
#include "../core/slang-basic.h"
#include "slang-ir-spirv-snippet.h"
#include "slang-ir-insts.h"

namespace Slang
{

class DiagnosticSink;

struct IRFunc;
struct IRModule;
class TargetRequest;

struct SPIRVEmitSharedContext
{
    IRModule* m_irModule;
    TargetRequest* m_targetRequest;
    Dictionary<IRTargetIntrinsicDecoration*, RefPtr<SpvSnippet>> m_parsedSpvSnippets;
    DiagnosticSink* m_sink;
    SPIRVEmitSharedContext(IRModule* module, TargetRequest* target, DiagnosticSink* sink)
        : m_irModule(module), m_targetRequest(target), m_sink(sink)
    {}
    SpvSnippet* getParsedSpvSnippet(IRTargetIntrinsicDecoration* intrinsic);
};

void legalizeIRForSPIRV(
    SPIRVEmitSharedContext* context,
    IRModule*               module,
    const List<IRFunc*>& entryPoints,
    CodeGenContext*         codeGenContext);

}
