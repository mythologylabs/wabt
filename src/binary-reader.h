/*
 * Copyright 2016 WebAssembly Community Group participants
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

#ifndef WABT_BINARY_READER_H_
#define WABT_BINARY_READER_H_

#include <stddef.h>
#include <stdint.h>

#include "binary.h"
#include "common.h"

#define WABT_READ_BINARY_OPTIONS_DEFAULT \
  { nullptr, false }

namespace wabt {

struct ReadBinaryOptions {
  struct Stream* log_stream;
  bool read_debug_names;
};

class BinaryReader {
 public:
  struct State {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  virtual ~BinaryReader() {}

  virtual bool OnError(const State&, const char* message) = 0;

  /* Module */
  virtual Result BeginModule(const State&, uint32_t version) = 0;
  virtual Result EndModule(const State&) = 0;

  virtual Result BeginSection(const State&,
                              BinarySection section_type,
                              uint32_t size) = 0;

  /* Custom section */
  virtual Result BeginCustomSection(const State&,
                                    uint32_t size,
                                    StringSlice section_name) = 0;
  virtual Result EndCustomSection(const State&) = 0;

  /* Type section */
  virtual Result BeginTypeSection(const State&, uint32_t size) = 0;
  virtual Result OnTypeCount(const State&, uint32_t count) = 0;
  virtual Result OnType(const State&,
                        uint32_t index,
                        uint32_t param_count,
                        Type* param_types,
                        uint32_t result_count,
                        Type* result_types) = 0;
  virtual Result EndTypeSection(const State&) = 0;

  /* Import section */
  virtual Result BeginImportSection(const State&, uint32_t size) = 0;
  virtual Result OnImportCount(const State&, uint32_t count) = 0;
  virtual Result OnImport(const State&,
                          uint32_t index,
                          StringSlice module_name,
                          StringSlice field_name) = 0;
  virtual Result OnImportFunc(const State&,
                              uint32_t import_index,
                              StringSlice module_name,
                              StringSlice field_name,
                              uint32_t func_index,
                              uint32_t sig_index) = 0;
  virtual Result OnImportTable(const State&,
                               uint32_t import_index,
                               StringSlice module_name,
                               StringSlice field_name,
                               uint32_t table_index,
                               Type elem_type,
                               const Limits* elem_limits) = 0;
  virtual Result OnImportMemory(const State&,
                                uint32_t import_index,
                                StringSlice module_name,
                                StringSlice field_name,
                                uint32_t memory_index,
                                const Limits* page_limits) = 0;
  virtual Result OnImportGlobal(const State&,
                                uint32_t import_index,
                                StringSlice module_name,
                                StringSlice field_name,
                                uint32_t global_index,
                                Type type,
                                bool mutable_) = 0;
  virtual Result EndImportSection(const State&) = 0;

  /* Function section */
  virtual Result BeginFunctionSection(const State&, uint32_t size) = 0;
  virtual Result OnFunctionCount(const State&, uint32_t count) = 0;
  virtual Result OnFunction(const State&,
                            uint32_t index,
                            uint32_t sig_index) = 0;
  virtual Result EndFunctionSection(const State&) = 0;

  /* Table section */
  virtual Result BeginTableSection(const State&, uint32_t size) = 0;
  virtual Result OnTableCount(const State&, uint32_t count) = 0;
  virtual Result OnTable(const State&,
                         uint32_t index,
                         Type elem_type,
                         const Limits* elem_limits) = 0;
  virtual Result EndTableSection(const State&) = 0;

  /* Memory section */
  virtual Result BeginMemorySection(const State&, uint32_t size) = 0;
  virtual Result OnMemoryCount(const State&, uint32_t count) = 0;
  virtual Result OnMemory(const State&,
                          uint32_t index,
                          const Limits* limits) = 0;
  virtual Result EndMemorySection(const State&) = 0;

  /* Global section */
  virtual Result BeginGlobalSection(const State&, uint32_t size) = 0;
  virtual Result OnGlobalCount(const State&, uint32_t count) = 0;
  virtual Result BeginGlobal(const State&,
                             uint32_t index,
                             Type type,
                             bool mutable_) = 0;
  virtual Result BeginGlobalInitExpr(const State&, uint32_t index) = 0;
  virtual Result EndGlobalInitExpr(const State&, uint32_t index) = 0;
  virtual Result EndGlobal(const State&, uint32_t index) = 0;
  virtual Result EndGlobalSection(const State&) = 0;

  /* Exports section */
  virtual Result BeginExportSection(const State&, uint32_t size) = 0;
  virtual Result OnExportCount(const State&, uint32_t count) = 0;
  virtual Result OnExport(const State&,
                          uint32_t index,
                          ExternalKind kind,
                          uint32_t item_index,
                          StringSlice name) = 0;
  virtual Result EndExportSection(const State&) = 0;

  /* Start section */
  virtual Result BeginStartSection(const State&, uint32_t size) = 0;
  virtual Result OnStartFunction(const State&, uint32_t func_index) = 0;
  virtual Result EndStartSection(const State&) = 0;

  /* Code section */
  virtual Result BeginCodeSection(const State&, uint32_t size) = 0;
  virtual Result OnFunctionBodyCount(const State&, uint32_t count) = 0;
  virtual Result BeginFunctionBody(const State&, uint32_t index) = 0;
  virtual Result OnLocalDeclCount(const State&, uint32_t count) = 0;
  virtual Result OnLocalDecl(const State&,
                             uint32_t decl_index,
                             uint32_t count,
                             Type type) = 0;

  /* Function expressions; called between BeginFunctionBody and
   EndFunctionBody */
  virtual Result OnOpcode(const State&, Opcode Opcode) = 0;
  virtual Result OnOpcodeBare(const State&) = 0;
  virtual Result OnOpcodeUint32(const State&, uint32_t value) = 0;
  virtual Result OnOpcodeUint32Uint32(const State&,
                                      uint32_t value,
                                      uint32_t value2) = 0;
  virtual Result OnOpcodeUint64(const State&, uint64_t value) = 0;
  virtual Result OnOpcodeF32(const State&, uint32_t value) = 0;
  virtual Result OnOpcodeF64(const State&, uint64_t value) = 0;
  virtual Result OnOpcodeBlockSig(const State&,
                                  uint32_t num_types,
                                  Type* sig_types) = 0;
  virtual Result OnBinaryExpr(const State&, Opcode opcode) = 0;
  virtual Result OnBlockExpr(const State&,
                             uint32_t num_types,
                             Type* sig_types) = 0;
  virtual Result OnBrExpr(const State&, uint32_t depth) = 0;
  virtual Result OnBrIfExpr(const State&, uint32_t depth) = 0;
  virtual Result OnBrTableExpr(const State&,
                               uint32_t num_targets,
                               uint32_t* target_depths,
                               uint32_t default_target_depth) = 0;
  virtual Result OnCallExpr(const State&, uint32_t func_index) = 0;
  virtual Result OnCallIndirectExpr(const State&, uint32_t sig_index) = 0;
  virtual Result OnCompareExpr(const State&, Opcode opcode) = 0;
  virtual Result OnConvertExpr(const State&, Opcode opcode) = 0;
  virtual Result OnCurrentMemoryExpr(const State&) = 0;
  virtual Result OnDropExpr(const State&) = 0;
  virtual Result OnElseExpr(const State&) = 0;
  virtual Result OnEndExpr(const State&) = 0;
  virtual Result OnEndFunc(const State&) = 0;
  virtual Result OnF32ConstExpr(const State&, uint32_t value_bits) = 0;
  virtual Result OnF64ConstExpr(const State&, uint64_t value_bits) = 0;
  virtual Result OnGetGlobalExpr(const State&, uint32_t global_index) = 0;
  virtual Result OnGetLocalExpr(const State&, uint32_t local_index) = 0;
  virtual Result OnGrowMemoryExpr(const State&) = 0;
  virtual Result OnI32ConstExpr(const State&, uint32_t value) = 0;
  virtual Result OnI64ConstExpr(const State&, uint64_t value) = 0;
  virtual Result OnIfExpr(const State&,
                          uint32_t num_types,
                          Type* sig_types) = 0;
  virtual Result OnLoadExpr(const State&,
                            Opcode opcode,
                            uint32_t alignment_log2,
                            uint32_t offset) = 0;
  virtual Result OnLoopExpr(const State&,
                            uint32_t num_types,
                            Type* sig_types) = 0;
  virtual Result OnNopExpr(const State&) = 0;
  virtual Result OnReturnExpr(const State&) = 0;
  virtual Result OnSelectExpr(const State&) = 0;
  virtual Result OnSetGlobalExpr(const State&, uint32_t global_index) = 0;
  virtual Result OnSetLocalExpr(const State&, uint32_t local_index) = 0;
  virtual Result OnStoreExpr(const State&,
                             Opcode opcode,
                             uint32_t alignment_log2,
                             uint32_t offset) = 0;
  virtual Result OnTeeLocalExpr(const State&, uint32_t local_index) = 0;
  virtual Result OnUnaryExpr(const State&, Opcode opcode) = 0;
  virtual Result OnUnreachableExpr(const State&) = 0;
  virtual Result EndFunctionBody(const State&, uint32_t index) = 0;
  virtual Result EndCodeSection(const State&) = 0;

  /* Elem section */
  virtual Result BeginElemSection(const State&, uint32_t size) = 0;
  virtual Result OnElemSegmentCount(const State&, uint32_t count) = 0;
  virtual Result BeginElemSegment(const State&,
                                  uint32_t index,
                                  uint32_t table_index) = 0;
  virtual Result BeginElemSegmentInitExpr(const State&, uint32_t index) = 0;
  virtual Result EndElemSegmentInitExpr(const State&, uint32_t index) = 0;
  virtual Result OnElemSegmentFunctionIndexCount(const State&,
                                                 uint32_t index,
                                                 uint32_t count) = 0;
  virtual Result OnElemSegmentFunctionIndex(const State&,
                                            uint32_t index,
                                            uint32_t func_index) = 0;
  virtual Result EndElemSegment(const State&, uint32_t index) = 0;
  virtual Result EndElemSection(const State&) = 0;

  /* Data section */
  virtual Result BeginDataSection(const State&, uint32_t size) = 0;
  virtual Result OnDataSegmentCount(const State&, uint32_t count) = 0;
  virtual Result BeginDataSegment(const State&,
                                  uint32_t index,
                                  uint32_t memory_index) = 0;
  virtual Result BeginDataSegmentInitExpr(const State&, uint32_t index) = 0;
  virtual Result EndDataSegmentInitExpr(const State&, uint32_t index) = 0;
  virtual Result OnDataSegmentData(const State&,
                                   uint32_t index,
                                   const void* data,
                                   uint32_t size) = 0;
  virtual Result EndDataSegment(const State&, uint32_t index) = 0;
  virtual Result EndDataSection(const State&) = 0;

  /* Names section */
  virtual Result BeginNamesSection(const State&, uint32_t size) = 0;
  virtual Result OnFunctionNameSubsection(const State&,
                                          uint32_t index,
                                          uint32_t name_type,
                                          uint32_t subsection_size) = 0;
  virtual Result OnFunctionNamesCount(const State&, uint32_t num_functions) = 0;
  virtual Result OnFunctionName(const State&,
                                uint32_t function_index,
                                StringSlice function_name) = 0;
  virtual Result OnLocalNameSubsection(const State&,
                                       uint32_t index,
                                       uint32_t name_type,
                                       uint32_t subsection_size) = 0;
  virtual Result OnLocalNameFunctionCount(const State&,
                                          uint32_t num_functions) = 0;
  virtual Result OnLocalNameLocalCount(const State&,
                                       uint32_t function_index,
                                       uint32_t num_locals) = 0;
  virtual Result OnLocalName(const State&,
                             uint32_t function_index,
                             uint32_t local_index,
                             StringSlice local_name) = 0;
  virtual Result EndNamesSection(const State&) = 0;

  /* Reloc section */
  virtual Result BeginRelocSection(const State&, uint32_t size) = 0;
  virtual Result OnRelocCount(const State&,
                              uint32_t count,
                              BinarySection section_code,
                              StringSlice section_name) = 0;
  virtual Result OnReloc(const State&,
                         RelocType type,
                         uint32_t offset,
                         uint32_t index,
                         int32_t addend) = 0;
  virtual Result EndRelocSection(const State&) = 0;

  /* InitExpr - used by elem, data and global sections; these functions are
   * only called between calls to Begin*InitExpr and End*InitExpr */
  virtual Result OnInitExprF32ConstExpr(const State&,
                                        uint32_t index,
                                        uint32_t value) = 0;
  virtual Result OnInitExprF64ConstExpr(const State&,
                                        uint32_t index,
                                        uint64_t value) = 0;
  virtual Result OnInitExprGetGlobalExpr(const State&,
                                         uint32_t index,
                                         uint32_t global_index) = 0;
  virtual Result OnInitExprI32ConstExpr(const State&,
                                        uint32_t index,
                                        uint32_t value) = 0;
  virtual Result OnInitExprI64ConstExpr(const State&,
                                        uint32_t index,
                                        uint64_t value) = 0;
};

Result read_binary(const void* data,
                   size_t size,
                   BinaryReader* reader,
                   const ReadBinaryOptions* options);

size_t read_u32_leb128(const uint8_t* ptr,
                       const uint8_t* end,
                       uint32_t* out_value);

size_t read_i32_leb128(const uint8_t* ptr,
                       const uint8_t* end,
                       uint32_t* out_value);

}  // namespace wabt

#endif /* WABT_BINARY_READER_H_ */
