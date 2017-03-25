/*
 * Copyright 2017 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "logging-binary-reader.h"

#include <inttypes.h>

#include "stream.h"

namespace wabt {

#define INDENT_SIZE 2

#define LOGF_NOINDENT(...) writef(stream, __VA_ARGS__)

#define LOGF(...)               \
  do {                          \
    WriteIndent();              \
    LOGF_NOINDENT(__VA_ARGS__); \
  } while (0)

namespace {

void sprint_limits(char* dst, size_t size, const Limits* limits) {
  int result;
  if (limits->has_max) {
    result = wabt_snprintf(dst, size, "initial: %" PRIu64 ", max: %" PRIu64,
                           limits->initial, limits->max);
  } else {
    result = wabt_snprintf(dst, size, "initial: %" PRIu64, limits->initial);
  }
  WABT_USE(result);
  assert(static_cast<size_t>(result) < size);
}

}  // namespace

LoggingBinaryReader::LoggingBinaryReader(Stream* stream, BinaryReader* forward)
    : stream(stream), reader(forward), indent(0) {}

void LoggingBinaryReader::Indent() {
  indent += INDENT_SIZE;
}

void LoggingBinaryReader::Dedent() {
  indent -= INDENT_SIZE;
  assert(indent >= 0);
}

void LoggingBinaryReader::WriteIndent() {
  static char s_indent[] =
      "                                                                       "
      "                                                                       ";
  static size_t s_indent_len = sizeof(s_indent) - 1;
  size_t i = indent;
  while (i > s_indent_len) {
    write_data(stream, s_indent, s_indent_len, nullptr);
    i -= s_indent_len;
  }
  if (i > 0) {
    write_data(stream, s_indent, indent, nullptr);
  }
}

void LoggingBinaryReader::LogTypes(uint32_t type_count, Type* types) {
  LOGF_NOINDENT("[");
  for (uint32_t i = 0; i < type_count; ++i) {
    LOGF_NOINDENT("%s", get_type_name(types[i]));
    if (i != type_count - 1)
      LOGF_NOINDENT(", ");
  }
  LOGF_NOINDENT("]");
}

bool LoggingBinaryReader::OnError(const State& state, const char* message) {
  return reader->OnError(state, message);
}

Result LoggingBinaryReader::BeginModule(const State& state, uint32_t version) {
  LOGF("BeginModule(version: %u)\n", version);
  Indent();
  return reader->BeginModule(state, version);
}


Result LoggingBinaryReader::BeginSection(const State& state,
                                         BinarySection section_type,
                                         uint32_t size) {
  return reader->BeginSection(state, section_type, size);
}

Result LoggingBinaryReader::BeginCustomSection(const State& state,
                                               uint32_t size,
                                               StringSlice section_name) {
  LOGF("BeginCustomSection('" PRIstringslice "', size: %d)\n",
       WABT_PRINTF_STRING_SLICE_ARG(section_name), size);
  Indent();
  return reader->BeginCustomSection(state, size, section_name);
}

Result LoggingBinaryReader::OnType(const State& state,
                                   uint32_t index,
                                   uint32_t param_count,
                                   Type* param_types,
                                   uint32_t result_count,
                                   Type* result_types) {
  LOGF("OnType(index: %u, params: ", index);
  LogTypes(param_count, param_types);
  LOGF_NOINDENT(", results: ");
  LogTypes(result_count, result_types);
  LOGF_NOINDENT(")\n");
  return reader->OnType(state, index, param_count, param_types, result_count,
                        result_types);
}

Result LoggingBinaryReader::OnImport(const State& state,
                                     uint32_t index,
                                     StringSlice module_name,
                                     StringSlice field_name) {
  LOGF("OnImport(index: %u, module: \"" PRIstringslice
       "\", field: \"" PRIstringslice "\")\n",
       index, WABT_PRINTF_STRING_SLICE_ARG(module_name),
       WABT_PRINTF_STRING_SLICE_ARG(field_name));
  return reader->OnImport(state, index, module_name, field_name);
}

Result LoggingBinaryReader::OnImportFunc(const State& state,
                                         uint32_t import_index,
                                         StringSlice module_name,
                                         StringSlice field_name,
                                         uint32_t func_index,
                                         uint32_t sig_index) {
  LOGF("OnImportFunc(import_index: %u, func_index: %u, sig_index: %u)\n",
       import_index, func_index, sig_index);
  return reader->OnImportFunc(state, import_index, module_name, field_name,
                              func_index, sig_index);
}

Result LoggingBinaryReader::OnImportTable(const State& state,
                                          uint32_t import_index,
                                          StringSlice module_name,
                                          StringSlice field_name,
                                          uint32_t table_index,
                                          Type elem_type,
                                          const Limits* elem_limits) {
  char buf[100];
  sprint_limits(buf, sizeof(buf), elem_limits);
  LOGF("OnImportTable(import_index: %u, table_index: %u, elem_type: %s, %s)\n",
       import_index, table_index, get_type_name(elem_type), buf);
  return reader->OnImportTable(state, import_index, module_name, field_name,
                               table_index, elem_type, elem_limits);
}

Result LoggingBinaryReader::OnImportMemory(const State& state,
                                           uint32_t import_index,
                                           StringSlice module_name,
                                           StringSlice field_name,
                                           uint32_t memory_index,
                                           const Limits* page_limits) {
  char buf[100];
  sprint_limits(buf, sizeof(buf), page_limits);
  LOGF("OnImportMemory(import_index: %u, memory_index: %u, %s)\n", import_index,
       memory_index, buf);
  return reader->OnImportMemory(state, import_index, module_name, field_name,
                                memory_index, page_limits);
}

Result LoggingBinaryReader::OnImportGlobal(const State& state,
                                           uint32_t import_index,
                                           StringSlice module_name,
                                           StringSlice field_name,
                                           uint32_t global_index,
                                           Type type,
                                           bool mutable_) {
  LOGF(
      "OnImportGlobal(import_index: %u, global_index: %u, type: %s, mutable: "
      "%s)\n",
      import_index, global_index, get_type_name(type),
      mutable_ ? "true" : "false");
  return reader->OnImportGlobal(state, import_index, module_name, field_name,
                                global_index, type, mutable_);
}

Result LoggingBinaryReader::OnTable(const State& state,
                                    uint32_t index,
                                    Type elem_type,
                                    const Limits* elem_limits) {
  char buf[100];
  sprint_limits(buf, sizeof(buf), elem_limits);
  LOGF("OnTable(index: %u, elem_type: %s, %s)\n", index,
       get_type_name(elem_type), buf);
  return reader->OnTable(state, index, elem_type, elem_limits);
}

Result LoggingBinaryReader::OnMemory(const State& state,
                                     uint32_t index,
                                     const Limits* page_limits) {
  char buf[100];
  sprint_limits(buf, sizeof(buf), page_limits);
  LOGF("OnMemory(index: %u, %s)\n", index, buf);
  return reader->OnMemory(state, index, page_limits);
}

Result LoggingBinaryReader::BeginGlobal(const State& state,
                                        uint32_t index,
                                        Type type,
                                        bool mutable_) {
  LOGF("BeginGlobal(index: %u, type: %s, mutable: %s)\n", index,
       get_type_name(type), mutable_ ? "true" : "false");
  return reader->BeginGlobal(state, index, type, mutable_);
}

Result LoggingBinaryReader::OnExport(const State& state,
                                     uint32_t index,
                                     ExternalKind kind,
                                     uint32_t item_index,
                                     StringSlice name) {
  LOGF("OnExport(index: %u, kind: %s, item_index: %u, name: \"" PRIstringslice
       "\")\n",
       index, get_kind_name(kind), item_index,
       WABT_PRINTF_STRING_SLICE_ARG(name));
  return reader->OnExport(state, index, kind, item_index, name);
}

Result LoggingBinaryReader::OnLocalDecl(const State& state,
                                        uint32_t decl_index,
                                        uint32_t count,
                                        Type type) {
  LOGF("OnLocalDecl(index: %u, count: %u, type: %s)\n", decl_index, count,
       get_type_name(type));
  return reader->OnLocalDecl(state, decl_index, count, type);
}

Result LoggingBinaryReader::OnBlockExpr(const State& state,
                                        uint32_t num_types,
                                        Type* sig_types) {
  LOGF("OnBlockExpr(sig: ");
  LogTypes(num_types, sig_types);
  LOGF_NOINDENT(")\n");
  return reader->OnBlockExpr(state, num_types, sig_types);
}

Result LoggingBinaryReader::OnBrExpr(const State& state, uint32_t depth) {
  LOGF("OnBrExpr(depth: %u)\n", depth);
  return reader->OnBrExpr(state, depth);
}

Result LoggingBinaryReader::OnBrIfExpr(const State& state, uint32_t depth) {
  LOGF("OnBrIfExpr(depth: %u)\n", depth);
  return reader->OnBrIfExpr(state, depth);
}

Result LoggingBinaryReader::OnBrTableExpr(const State& state,
                                          uint32_t num_targets,
                                          uint32_t* target_depths,
                                          uint32_t default_target_depth) {
  LOGF("OnBrTableExpr(num_targets: %u, depths: [", num_targets);
  for (uint32_t i = 0; i < num_targets; ++i) {
    LOGF_NOINDENT("%u", target_depths[i]);
    if (i != num_targets - 1)
      LOGF_NOINDENT(", ");
  }
  LOGF_NOINDENT("], default: %u)\n", default_target_depth);
  return reader->OnBrTableExpr(state, num_targets, target_depths,
                               default_target_depth);
}

Result LoggingBinaryReader::OnF32ConstExpr(const State& state,
                                           uint32_t value_bits) {
  float value;
  memcpy(&value, &value_bits, sizeof(value));
  LOGF("OnF32ConstExpr(%g (0x04%x))\n", value, value_bits);
  return reader->OnF32ConstExpr(state, value_bits);
}

Result LoggingBinaryReader::OnF64ConstExpr(const State& state,
                                           uint64_t value_bits) {
  double value;
  memcpy(&value, &value_bits, sizeof(value));
  LOGF("OnF64ConstExpr(%g (0x08%" PRIx64 "))\n", value, value_bits);
  return reader->OnF64ConstExpr(state, value_bits);
}

Result LoggingBinaryReader::OnI32ConstExpr(const State& state, uint32_t value) {
  LOGF("OnI32ConstExpr(%u (0x%x))\n", value, value);
  return reader->OnI32ConstExpr(state, value);
}

Result LoggingBinaryReader::OnI64ConstExpr(const State& state, uint64_t value) {
  LOGF("OnI64ConstExpr(%" PRIu64 " (0x%" PRIx64 "))\n", value, value);
  return reader->OnI64ConstExpr(state, value);
}

Result LoggingBinaryReader::OnIfExpr(const State& state,
                                     uint32_t num_types,
                                     Type* sig_types) {
  LOGF("OnIfExpr(sig: ");
  LogTypes(num_types, sig_types);
  LOGF_NOINDENT(")\n");
  return reader->OnIfExpr(state, num_types, sig_types);
}

Result LoggingBinaryReader::OnLoadExpr(const State& state,
                                       Opcode opcode,
                                       uint32_t alignment_log2,
                                       uint32_t offset) {
  LOGF("OnLoadExpr(opcode: \"%s\" (%u), align log2: %u, offset: %u)\n",
       get_opcode_name(opcode), static_cast<unsigned>(opcode), alignment_log2,
       offset);
  return reader->OnLoadExpr(state, opcode, alignment_log2, offset);
}

Result LoggingBinaryReader::OnLoopExpr(const State& state,
                                       uint32_t num_types,
                                       Type* sig_types) {
  LOGF("OnLoopExpr(sig: ");
  LogTypes(num_types, sig_types);
  LOGF_NOINDENT(")\n");
  return reader->OnLoopExpr(state, num_types, sig_types);
}

Result LoggingBinaryReader::OnStoreExpr(const State& state,
                                        Opcode opcode,
                                        uint32_t alignment_log2,
                                        uint32_t offset) {
  LOGF("OnStoreExpr(opcode: \"%s\" (%u), align log2: %u, offset: %u)\n",
       get_opcode_name(opcode), static_cast<unsigned>(opcode), alignment_log2,
       offset);
  return reader->OnStoreExpr(state, opcode, alignment_log2, offset);
}

Result LoggingBinaryReader::OnDataSegmentData(const State& state,
                                              uint32_t index,
                                              const void* data,
                                              uint32_t size) {
  LOGF("OnDataSegmentData(index:%u, size:%u)\n", index, size);
  return reader->OnDataSegmentData(state, index, data, size);
}

Result LoggingBinaryReader::OnFunctionNameSubsection(const State& state,
                                                     uint32_t index,
                                                     uint32_t name_type,
                                                     uint32_t subsection_size) {
  LOGF("OnFunctionNameSubsection(index:%u, nametype:%u, size:%u)\n", index,
       name_type, subsection_size);
  return reader->OnFunctionNameSubsection(state, index, name_type,
                                          subsection_size);
}

Result LoggingBinaryReader::OnFunctionName(const State& state,
                                           uint32_t index,
                                           StringSlice name) {
  LOGF("OnFunctionName(index: %u, name: \"" PRIstringslice "\")\n", index,
       WABT_PRINTF_STRING_SLICE_ARG(name));
  return reader->OnFunctionName(state, index, name);
}

Result LoggingBinaryReader::OnLocalNameSubsection(const State& state,
                                                  uint32_t index,
                                                  uint32_t name_type,
                                                  uint32_t subsection_size) {
  LOGF("OnLocalNameSubsection(index:%u, nametype:%u, size:%u)\n", index,
       name_type, subsection_size);
  return reader->OnLocalNameSubsection(state, index, name_type,
                                       subsection_size);
}

Result LoggingBinaryReader::OnLocalName(const State& state,
                                        uint32_t func_index,
                                        uint32_t local_index,
                                        StringSlice name) {
  LOGF("OnLocalName(func_index: %u, local_index: %u, name: \"" PRIstringslice
       "\")\n",
       func_index, local_index, WABT_PRINTF_STRING_SLICE_ARG(name));
  return reader->OnLocalName(state, func_index, local_index, name);
}

Result LoggingBinaryReader::OnInitExprF32ConstExpr(const State& state,
                                                   uint32_t index,
                                                   uint32_t value_bits) {
  float value;
  memcpy(&value, &value_bits, sizeof(value));
  LOGF("OnInitExprF32ConstExpr(index: %u, value: %g (0x04%x))\n", index, value,
       value_bits);
  return reader->OnInitExprF32ConstExpr(state, index, value_bits);
}

Result LoggingBinaryReader::OnInitExprF64ConstExpr(const State& state,
                                                   uint32_t index,
                                                   uint64_t value_bits) {
  double value;
  memcpy(&value, &value_bits, sizeof(value));
  LOGF("OnInitExprF64ConstExpr(index: %u value: %g (0x08%" PRIx64 "))\n", index,
       value, value_bits);
  return reader->OnInitExprF64ConstExpr(state, index, value_bits);
}

Result LoggingBinaryReader::OnInitExprI32ConstExpr(const State& state,
                                                   uint32_t index,
                                                   uint32_t value) {
  LOGF("OnInitExprI32ConstExpr(index: %u, value: %u)\n", index, value);
  return reader->OnInitExprI32ConstExpr(state, index, value);
}

Result LoggingBinaryReader::OnInitExprI64ConstExpr(const State& state,
                                                   uint32_t index,
                                                   uint64_t value) {
  LOGF("OnInitExprI64ConstExpr(index: %u, value: %" PRIu64 ")\n", index, value);
  return reader->OnInitExprI64ConstExpr(state, index, value);
}

Result LoggingBinaryReader::OnRelocCount(const State& state,
                                         uint32_t count,
                                         BinarySection section_code,
                                         StringSlice section_name) {
  LOGF("OnRelocCount(count: %d, section: %s, section_name: " PRIstringslice
       ")\n",
       count, get_section_name(section_code),
       WABT_PRINTF_STRING_SLICE_ARG(section_name));
  return reader->OnRelocCount(state, count, section_code, section_name);
}

Result LoggingBinaryReader::OnReloc(const State& state,
                                    RelocType type,
                                    uint32_t offset,
                                    uint32_t index,
                                    int32_t addend) {
  LOGF("OnReloc(type: %s, offset: %u, index: %u, addend: %d)\n",
       get_reloc_type_name(type), offset, index, addend);
  return reader->OnReloc(state, type, offset, index, addend);
}

#define DEFINE_BEGIN(name)                                              \
  Result LoggingBinaryReader::name(const State& state, uint32_t size) { \
    LOGF(#name "(%u)\n", size);                                         \
    Indent();                                                           \
    return reader->name(state, size);                                   \
  }

#define DEFINE_END(name)                                 \
  Result LoggingBinaryReader::name(const State& state) { \
    Dedent();                                            \
    LOGF(#name "\n");                                    \
    return reader->name(state);                          \
  }

#define DEFINE_UINT32(name)                                              \
  Result LoggingBinaryReader::name(const State& state, uint32_t value) { \
    LOGF(#name "(%u)\n", value);                                         \
    return reader->name(state, value);                                   \
  }

#define DEFINE_UINT32_DESC(name, desc)                                   \
  Result LoggingBinaryReader::name(const State& state, uint32_t value) { \
    LOGF(#name "(" desc ": %u)\n", value);                               \
    return reader->name(state, value);                                   \
  }

#define DEFINE_UINT32_UINT32(name, desc0, desc1)                        \
  Result LoggingBinaryReader::name(const State& state, uint32_t value0, \
                                   uint32_t value1) {                   \
    LOGF(#name "(" desc0 ": %u, " desc1 ": %u)\n", value0, value1);     \
    return reader->name(state, value0, value1);                         \
  }

#define DEFINE_OPCODE(name)                                             \
  Result LoggingBinaryReader::name(const State& state, Opcode opcode) { \
    LOGF(#name "(\"%s\" (%u))\n", get_opcode_name(opcode),              \
         static_cast<unsigned>(opcode));                                \
    return reader->name(state, opcode);                                 \
  }

#define DEFINE0(name)                                    \
  Result LoggingBinaryReader::name(const State& state) { \
    LOGF(#name "\n");                                    \
    return reader->name(state);                          \
  }

DEFINE_END(EndModule)

DEFINE_END(EndCustomSection)

DEFINE_BEGIN(BeginTypeSection)
DEFINE_UINT32(OnTypeCount)
DEFINE_END(EndTypeSection)

DEFINE_BEGIN(BeginImportSection)
DEFINE_UINT32(OnImportCount)
DEFINE_END(EndImportSection)

DEFINE_BEGIN(BeginFunctionSection)
DEFINE_UINT32(OnFunctionCount)
DEFINE_UINT32_UINT32(OnFunction, "index", "sig_index")
DEFINE_END(EndFunctionSection)

DEFINE_BEGIN(BeginTableSection)
DEFINE_UINT32(OnTableCount)
DEFINE_END(EndTableSection)

DEFINE_BEGIN(BeginMemorySection)
DEFINE_UINT32(OnMemoryCount)
DEFINE_END(EndMemorySection)

DEFINE_BEGIN(BeginGlobalSection)
DEFINE_UINT32(OnGlobalCount)
DEFINE_UINT32(BeginGlobalInitExpr)
DEFINE_UINT32(EndGlobalInitExpr)
DEFINE_UINT32(EndGlobal)
DEFINE_END(EndGlobalSection)

DEFINE_BEGIN(BeginExportSection)
DEFINE_UINT32(OnExportCount)
DEFINE_END(EndExportSection)

DEFINE_BEGIN(BeginStartSection)
DEFINE_UINT32(OnStartFunction)
DEFINE_END(EndStartSection)

DEFINE_BEGIN(BeginCodeSection)
DEFINE_UINT32(OnFunctionBodyCount)
DEFINE_UINT32(BeginFunctionBody)
DEFINE_UINT32(EndFunctionBody)
DEFINE_UINT32(OnLocalDeclCount)
DEFINE_OPCODE(OnBinaryExpr)
DEFINE_UINT32_DESC(OnCallExpr, "func_index")
DEFINE_UINT32_DESC(OnCallIndirectExpr, "sig_index")
DEFINE_OPCODE(OnCompareExpr)
DEFINE_OPCODE(OnConvertExpr)
DEFINE0(OnCurrentMemoryExpr)
DEFINE0(OnDropExpr)
DEFINE0(OnElseExpr)
DEFINE0(OnEndExpr)
DEFINE_UINT32_DESC(OnGetGlobalExpr, "index")
DEFINE_UINT32_DESC(OnGetLocalExpr, "index")
DEFINE0(OnGrowMemoryExpr)
DEFINE0(OnNopExpr)
DEFINE0(OnReturnExpr)
DEFINE0(OnSelectExpr)
DEFINE_UINT32_DESC(OnSetGlobalExpr, "index")
DEFINE_UINT32_DESC(OnSetLocalExpr, "index")
DEFINE_UINT32_DESC(OnTeeLocalExpr, "index")
DEFINE0(OnUnreachableExpr)
DEFINE_OPCODE(OnUnaryExpr)
DEFINE_END(EndCodeSection)

DEFINE_BEGIN(BeginElemSection)
DEFINE_UINT32(OnElemSegmentCount)
DEFINE_UINT32_UINT32(BeginElemSegment, "index", "table_index")
DEFINE_UINT32(BeginElemSegmentInitExpr)
DEFINE_UINT32(EndElemSegmentInitExpr)
DEFINE_UINT32_UINT32(OnElemSegmentFunctionIndexCount, "index", "count")
DEFINE_UINT32_UINT32(OnElemSegmentFunctionIndex, "index", "func_index")
DEFINE_UINT32(EndElemSegment)
DEFINE_END(EndElemSection)

DEFINE_BEGIN(BeginDataSection)
DEFINE_UINT32(OnDataSegmentCount)
DEFINE_UINT32_UINT32(BeginDataSegment, "index", "memory_index")
DEFINE_UINT32(BeginDataSegmentInitExpr)
DEFINE_UINT32(EndDataSegmentInitExpr)
DEFINE_UINT32(EndDataSegment)
DEFINE_END(EndDataSection)

DEFINE_BEGIN(BeginNamesSection)
DEFINE_UINT32(OnFunctionNamesCount)
DEFINE_UINT32(OnLocalNameFunctionCount)
DEFINE_UINT32_UINT32(OnLocalNameLocalCount, "index", "count")
DEFINE_END(EndNamesSection)

DEFINE_BEGIN(BeginRelocSection)
DEFINE_END(EndRelocSection)
DEFINE_UINT32_UINT32(OnInitExprGetGlobalExpr, "index", "global_index")

// We don't need to log these (the individual opcodes are logged instead), but
// we still need to forward the calls.
Result LoggingBinaryReader::OnOpcode(const State& state, Opcode opcode) {
  return reader->OnOpcode(state, opcode);
}

Result LoggingBinaryReader::OnOpcodeBare(const State& state) {
  return reader->OnOpcodeBare(state);
}

Result LoggingBinaryReader::OnOpcodeUint32(const State& state, uint32_t value) {
  return reader->OnOpcodeUint32(state, value);
}

Result LoggingBinaryReader::OnOpcodeUint32Uint32(const State& state,
                                                 uint32_t value,
                                                 uint32_t value2) {
  return reader->OnOpcodeUint32Uint32(state, value, value2);
}

Result LoggingBinaryReader::OnOpcodeUint64(const State& state, uint64_t value) {
  return reader->OnOpcodeUint64(state, value);
}

Result LoggingBinaryReader::OnOpcodeF32(const State& state, uint32_t value) {
  return reader->OnOpcodeF32(state, value);
}

Result LoggingBinaryReader::OnOpcodeF64(const State& state, uint64_t value) {
  return reader->OnOpcodeF64(state, value);
}

Result LoggingBinaryReader::OnOpcodeBlockSig(const State& state,
                                             uint32_t num_types,
                                             Type* sig_types) {
  return reader->OnOpcodeBlockSig(state, num_types, sig_types);
}

Result LoggingBinaryReader::OnEndFunc(const State& state) {
  return reader->OnEndFunc(state);
}

}  // namespace wabt
