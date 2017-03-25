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

#ifndef WABT_BINARY_READER_NOP_H_
#define WABT_BINARY_READER_NOP_H_

#include "binary-reader.h"

namespace wabt {

class BinaryReaderNop : public BinaryReader {
 public:
  virtual bool OnError(const State&, const char* message) { return false; }

  /* Module */
  virtual Result BeginModule(const State&, uint32_t version) {
    return Result::Ok;
  }
  virtual Result EndModule(const State&) { return Result::Ok; }

  virtual Result BeginSection(const State&,
                              BinarySection section_type,
                              uint32_t size) {
    return Result::Ok;
  }

  /* Custom section */
  virtual Result BeginCustomSection(const State&,
                                    uint32_t size,
                                    StringSlice section_name) {
    return Result::Ok;
  }
  virtual Result EndCustomSection(const State&) { return Result::Ok; }

  /* Type section */
  virtual Result BeginTypeSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnTypeCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnType(const State&,
                        uint32_t index,
                        uint32_t param_count,
                        Type* param_types,
                        uint32_t result_count,
                        Type* result_types) {
    return Result::Ok;
  }
  virtual Result EndTypeSection(const State&) { return Result::Ok; }

  /* Import section */
  virtual Result BeginImportSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnImportCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnImport(const State&,
                          uint32_t index,
                          StringSlice module_name,
                          StringSlice field_name) {
    return Result::Ok;
  }
  virtual Result OnImportFunc(const State&,
                              uint32_t import_index,
                              StringSlice module_name,
                              StringSlice field_name,
                              uint32_t func_index,
                              uint32_t sig_index) {
    return Result::Ok;
  }
  virtual Result OnImportTable(const State&,
                               uint32_t import_index,
                               StringSlice module_name,
                               StringSlice field_name,
                               uint32_t table_index,
                               Type elem_type,
                               const Limits* elem_limits) {
    return Result::Ok;
  }
  virtual Result OnImportMemory(const State&,
                                uint32_t import_index,
                                StringSlice module_name,
                                StringSlice field_name,
                                uint32_t memory_index,
                                const Limits* page_limits) {
    return Result::Ok;
  }
  virtual Result OnImportGlobal(const State&,
                                uint32_t import_index,
                                StringSlice module_name,
                                StringSlice field_name,
                                uint32_t global_index,
                                Type type,
                                bool mutable_) {
    return Result::Ok;
  }
  virtual Result EndImportSection(const State&) { return Result::Ok; }

  /* Function section */
  virtual Result BeginFunctionSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnFunctionCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnFunction(const State&, uint32_t index, uint32_t sig_index) {
    return Result::Ok;
  }
  virtual Result EndFunctionSection(const State&) { return Result::Ok; }

  /* Table section */
  virtual Result BeginTableSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnTableCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnTable(const State&,
                         uint32_t index,
                         Type elem_type,
                         const Limits* elem_limits) {
    return Result::Ok;
  }
  virtual Result EndTableSection(const State&) { return Result::Ok; }

  /* Memory section */
  virtual Result BeginMemorySection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnMemoryCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnMemory(const State&, uint32_t index, const Limits* limits) {
    return Result::Ok;
  }
  virtual Result EndMemorySection(const State&) { return Result::Ok; }

  /* Global section */
  virtual Result BeginGlobalSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnGlobalCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result BeginGlobal(const State&,
                             uint32_t index,
                             Type type,
                             bool mutable_) {
    return Result::Ok;
  }
  virtual Result BeginGlobalInitExpr(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndGlobalInitExpr(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndGlobal(const State&, uint32_t index) { return Result::Ok; }
  virtual Result EndGlobalSection(const State&) { return Result::Ok; }

  /* Exports section */
  virtual Result BeginExportSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnExportCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnExport(const State&,
                          uint32_t index,
                          ExternalKind kind,
                          uint32_t item_index,
                          StringSlice name) {
    return Result::Ok;
  }
  virtual Result EndExportSection(const State&) { return Result::Ok; }

  /* Start section */
  virtual Result BeginStartSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnStartFunction(const State&, uint32_t func_index) {
    return Result::Ok;
  }
  virtual Result EndStartSection(const State&) { return Result::Ok; }

  /* Code section */
  virtual Result BeginCodeSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnFunctionBodyCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result BeginFunctionBody(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result OnLocalDeclCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnLocalDecl(const State&,
                             uint32_t decl_index,
                             uint32_t count,
                             Type type) {
    return Result::Ok;
  }

  /* Function expressions; called between BeginFunctionBody and
   EndFunctionBody */
  virtual Result OnOpcode(const State&, Opcode Opcode) { return Result::Ok; }
  virtual Result OnOpcodeBare(const State&) { return Result::Ok; }
  virtual Result OnOpcodeUint32(const State&, uint32_t value) {
    return Result::Ok;
  }
  virtual Result OnOpcodeUint32Uint32(const State&,
                                      uint32_t value,
                                      uint32_t value2) {
    return Result::Ok;
  }
  virtual Result OnOpcodeUint64(const State&, uint64_t value) {
    return Result::Ok;
  }
  virtual Result OnOpcodeF32(const State&, uint32_t value) {
    return Result::Ok;
  }
  virtual Result OnOpcodeF64(const State&, uint64_t value) {
    return Result::Ok;
  }
  virtual Result OnOpcodeBlockSig(const State&,
                                  uint32_t num_types,
                                  Type* sig_types) {
    return Result::Ok;
  }
  virtual Result OnBinaryExpr(const State&, Opcode opcode) {
    return Result::Ok;
  }
  virtual Result OnBlockExpr(const State&,
                             uint32_t num_types,
                             Type* sig_types) {
    return Result::Ok;
  }
  virtual Result OnBrExpr(const State&, uint32_t depth) { return Result::Ok; }
  virtual Result OnBrIfExpr(const State&, uint32_t depth) { return Result::Ok; }
  virtual Result OnBrTableExpr(const State&,
                               uint32_t num_targets,
                               uint32_t* target_depths,
                               uint32_t default_target_depth) {
    return Result::Ok;
  }
  virtual Result OnCallExpr(const State&, uint32_t func_index) {
    return Result::Ok;
  }
  virtual Result OnCallIndirectExpr(const State&, uint32_t sig_index) {
    return Result::Ok;
  }
  virtual Result OnCompareExpr(const State&, Opcode opcode) {
    return Result::Ok;
  }
  virtual Result OnConvertExpr(const State&, Opcode opcode) {
    return Result::Ok;
  }
  virtual Result OnCurrentMemoryExpr(const State&) { return Result::Ok; }
  virtual Result OnDropExpr(const State&) { return Result::Ok; }
  virtual Result OnElseExpr(const State&) { return Result::Ok; }
  virtual Result OnEndExpr(const State&) { return Result::Ok; }
  virtual Result OnEndFunc(const State&) { return Result::Ok; }
  virtual Result OnF32ConstExpr(const State&, uint32_t value_bits) {
    return Result::Ok;
  }
  virtual Result OnF64ConstExpr(const State&, uint64_t value_bits) {
    return Result::Ok;
  }
  virtual Result OnGetGlobalExpr(const State&, uint32_t global_index) {
    return Result::Ok;
  }
  virtual Result OnGetLocalExpr(const State&, uint32_t local_index) {
    return Result::Ok;
  }
  virtual Result OnGrowMemoryExpr(const State&) { return Result::Ok; }
  virtual Result OnI32ConstExpr(const State&, uint32_t value) {
    return Result::Ok;
  }
  virtual Result OnI64ConstExpr(const State&, uint64_t value) {
    return Result::Ok;
  }
  virtual Result OnIfExpr(const State&, uint32_t num_types, Type* sig_types) {
    return Result::Ok;
  }
  virtual Result OnLoadExpr(const State&,
                            Opcode opcode,
                            uint32_t alignment_log2,
                            uint32_t offset) {
    return Result::Ok;
  }
  virtual Result OnLoopExpr(const State&, uint32_t num_types, Type* sig_types) {
    return Result::Ok;
  }
  virtual Result OnNopExpr(const State&) { return Result::Ok; }
  virtual Result OnReturnExpr(const State&) { return Result::Ok; }
  virtual Result OnSelectExpr(const State&) { return Result::Ok; }
  virtual Result OnSetGlobalExpr(const State&, uint32_t global_index) {
    return Result::Ok;
  }
  virtual Result OnSetLocalExpr(const State&, uint32_t local_index) {
    return Result::Ok;
  }
  virtual Result OnStoreExpr(const State&,
                             Opcode opcode,
                             uint32_t alignment_log2,
                             uint32_t offset) {
    return Result::Ok;
  }
  virtual Result OnTeeLocalExpr(const State&, uint32_t local_index) {
    return Result::Ok;
  }
  virtual Result OnUnaryExpr(const State&, Opcode opcode) { return Result::Ok; }
  virtual Result OnUnreachableExpr(const State&) { return Result::Ok; }
  virtual Result EndFunctionBody(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndCodeSection(const State&) { return Result::Ok; }

  /* Elem section */
  virtual Result BeginElemSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnElemSegmentCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result BeginElemSegment(const State&,
                                  uint32_t index,
                                  uint32_t table_index) {
    return Result::Ok;
  }
  virtual Result BeginElemSegmentInitExpr(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndElemSegmentInitExpr(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result OnElemSegmentFunctionIndexCount(const State&,
                                                 uint32_t index,
                                                 uint32_t count) {
    return Result::Ok;
  }
  virtual Result OnElemSegmentFunctionIndex(const State&,
                                            uint32_t index,
                                            uint32_t func_index) {
    return Result::Ok;
  }
  virtual Result EndElemSegment(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndElemSection(const State&) { return Result::Ok; }

  /* Data section */
  virtual Result BeginDataSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnDataSegmentCount(const State&, uint32_t count) {
    return Result::Ok;
  }
  virtual Result BeginDataSegment(const State&,
                                  uint32_t index,
                                  uint32_t memory_index) {
    return Result::Ok;
  }
  virtual Result BeginDataSegmentInitExpr(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndDataSegmentInitExpr(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result OnDataSegmentData(const State&,
                                   uint32_t index,
                                   const void* data,
                                   uint32_t size) {
    return Result::Ok;
  }
  virtual Result EndDataSegment(const State&, uint32_t index) {
    return Result::Ok;
  }
  virtual Result EndDataSection(const State&) { return Result::Ok; }

  /* Names section */
  virtual Result BeginNamesSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnFunctionNameSubsection(const State&,
                                          uint32_t index,
                                          uint32_t name_type,
                                          uint32_t subsection_size) {
    return Result::Ok;
  }
  virtual Result OnFunctionNamesCount(const State&, uint32_t num_functions) {
    return Result::Ok;
  }
  virtual Result OnFunctionName(const State&,
                                uint32_t function_index,
                                StringSlice function_name) {
    return Result::Ok;
  }
  virtual Result OnLocalNameSubsection(const State&,
                                       uint32_t index,
                                       uint32_t name_type,
                                       uint32_t subsection_size) {
    return Result::Ok;
  }
  virtual Result OnLocalNameFunctionCount(const State&,
                                          uint32_t num_functions) {
    return Result::Ok;
  }
  virtual Result OnLocalNameLocalCount(const State&,
                                       uint32_t function_index,
                                       uint32_t num_locals) {
    return Result::Ok;
  }
  virtual Result OnLocalName(const State&,
                             uint32_t function_index,
                             uint32_t local_index,
                             StringSlice local_name) {
    return Result::Ok;
  }
  virtual Result EndNamesSection(const State&) { return Result::Ok; }

  /* Reloc section */
  virtual Result BeginRelocSection(const State&, uint32_t size) {
    return Result::Ok;
  }
  virtual Result OnRelocCount(const State&,
                              uint32_t count,
                              BinarySection section_code,
                              StringSlice section_name) {
    return Result::Ok;
  }
  virtual Result OnReloc(const State&,
                         RelocType type,
                         uint32_t offset,
                         uint32_t index,
                         int32_t addend) {
    return Result::Ok;
  }
  virtual Result EndRelocSection(const State&) { return Result::Ok; }

  /* InitExpr - used by elem, data and global sections; these functions are
   * only called between calls to Begin*InitExpr and End*InitExpr */
  virtual Result OnInitExprF32ConstExpr(const State&,
                                        uint32_t index,
                                        uint32_t value) {
    return Result::Ok;
  }
  virtual Result OnInitExprF64ConstExpr(const State&,
                                        uint32_t index,
                                        uint64_t value) {
    return Result::Ok;
  }
  virtual Result OnInitExprGetGlobalExpr(const State&,
                                         uint32_t index,
                                         uint32_t global_index) {
    return Result::Ok;
  }
  virtual Result OnInitExprI32ConstExpr(const State&,
                                        uint32_t index,
                                        uint32_t value) {
    return Result::Ok;
  }
  virtual Result OnInitExprI64ConstExpr(const State&,
                                        uint32_t index,
                                        uint64_t value) {
    return Result::Ok;
  }
};

}  // namespace wabt

#endif /* WABT_BINARY_READER_H_ */
