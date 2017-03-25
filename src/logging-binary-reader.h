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

#ifndef WABT_LOGGING_BINARY_READER_H_
#define WABT_LOGGING_BINARY_READER_H_

#include "binary-reader.h"

namespace wabt {

struct Stream;

class LoggingBinaryReader : public BinaryReader {
 public:
  LoggingBinaryReader(Stream*, BinaryReader* forward);

  virtual bool OnError(const State&, const char* message);

  virtual Result BeginModule(const State&, uint32_t version);
  virtual Result EndModule(const State&);

  virtual Result BeginSection(const State&,
                              BinarySection section_type,
                              uint32_t size);

  virtual Result BeginCustomSection(const State&,
                                    uint32_t size,
                                    StringSlice section_name);
  virtual Result EndCustomSection(const State&);

  virtual Result BeginTypeSection(const State&, uint32_t size);
  virtual Result OnTypeCount(const State&, uint32_t count);
  virtual Result OnType(const State&,
                        uint32_t index,
                        uint32_t param_count,
                        Type* param_types,
                        uint32_t result_count,
                        Type* result_types);
  virtual Result EndTypeSection(const State&);

  virtual Result BeginImportSection(const State&, uint32_t size);
  virtual Result OnImportCount(const State&, uint32_t count);
  virtual Result OnImport(const State&,
                          uint32_t index,
                          StringSlice module_name,
                          StringSlice field_name);
  virtual Result OnImportFunc(const State&,
                              uint32_t import_index,
                              StringSlice module_name,
                              StringSlice field_name,
                              uint32_t func_index,
                              uint32_t sig_index);
  virtual Result OnImportTable(const State&,
                               uint32_t import_index,
                               StringSlice module_name,
                               StringSlice field_name,
                               uint32_t table_index,
                               Type elem_type,
                               const Limits* elem_limits);
  virtual Result OnImportMemory(const State&,
                                uint32_t import_index,
                                StringSlice module_name,
                                StringSlice field_name,
                                uint32_t memory_index,
                                const Limits* page_limits);
  virtual Result OnImportGlobal(const State&,
                                uint32_t import_index,
                                StringSlice module_name,
                                StringSlice field_name,
                                uint32_t global_index,
                                Type type,
                                bool mutable_);
  virtual Result EndImportSection(const State&);

  virtual Result BeginFunctionSection(const State&, uint32_t size);
  virtual Result OnFunctionCount(const State&, uint32_t count);
  virtual Result OnFunction(const State&, uint32_t index, uint32_t sig_index);
  virtual Result EndFunctionSection(const State&);

  virtual Result BeginTableSection(const State&, uint32_t size);
  virtual Result OnTableCount(const State&, uint32_t count);
  virtual Result OnTable(const State&,
                         uint32_t index,
                         Type elem_type,
                         const Limits* elem_limits);
  virtual Result EndTableSection(const State&);

  virtual Result BeginMemorySection(const State&, uint32_t size);
  virtual Result OnMemoryCount(const State&, uint32_t count);
  virtual Result OnMemory(const State&, uint32_t index, const Limits* limits);
  virtual Result EndMemorySection(const State&);

  virtual Result BeginGlobalSection(const State&, uint32_t size);
  virtual Result OnGlobalCount(const State&, uint32_t count);
  virtual Result BeginGlobal(const State&,
                             uint32_t index,
                             Type type,
                             bool mutable_);
  virtual Result BeginGlobalInitExpr(const State&, uint32_t index);
  virtual Result EndGlobalInitExpr(const State&, uint32_t index);
  virtual Result EndGlobal(const State&, uint32_t index);
  virtual Result EndGlobalSection(const State&);

  virtual Result BeginExportSection(const State&, uint32_t size);
  virtual Result OnExportCount(const State&, uint32_t count);
  virtual Result OnExport(const State&,
                          uint32_t index,
                          ExternalKind kind,
                          uint32_t item_index,
                          StringSlice name);
  virtual Result EndExportSection(const State&);

  virtual Result BeginStartSection(const State&, uint32_t size);
  virtual Result OnStartFunction(const State&, uint32_t func_index);
  virtual Result EndStartSection(const State&);

  virtual Result BeginCodeSection(const State&, uint32_t size);
  virtual Result OnFunctionBodyCount(const State&, uint32_t count);
  virtual Result BeginFunctionBody(const State&, uint32_t index);
  virtual Result OnLocalDeclCount(const State&, uint32_t count);
  virtual Result OnLocalDecl(const State&,
                             uint32_t decl_index,
                             uint32_t count,
                             Type type);

  virtual Result OnOpcode(const State&, Opcode opcode);
  virtual Result OnOpcodeBare(const State&);
  virtual Result OnOpcodeUint32(const State&, uint32_t value);
  virtual Result OnOpcodeUint32Uint32(const State&,
                                      uint32_t value,
                                      uint32_t value2);
  virtual Result OnOpcodeUint64(const State&, uint64_t value);
  virtual Result OnOpcodeF32(const State&, uint32_t value);
  virtual Result OnOpcodeF64(const State&, uint64_t value);
  virtual Result OnOpcodeBlockSig(const State&,
                                  uint32_t num_types,
                                  Type* sig_types);
  virtual Result OnBinaryExpr(const State&, Opcode opcode);
  virtual Result OnBlockExpr(const State&, uint32_t num_types, Type* sig_types);
  virtual Result OnBrExpr(const State&, uint32_t depth);
  virtual Result OnBrIfExpr(const State&, uint32_t depth);
  virtual Result OnBrTableExpr(const State&,
                               uint32_t num_targets,
                               uint32_t* target_depths,
                               uint32_t default_target_depth);
  virtual Result OnCallExpr(const State&, uint32_t func_index);
  virtual Result OnCallIndirectExpr(const State&, uint32_t sig_index);
  virtual Result OnCompareExpr(const State&, Opcode opcode);
  virtual Result OnConvertExpr(const State&, Opcode opcode);
  virtual Result OnCurrentMemoryExpr(const State&);
  virtual Result OnDropExpr(const State&);
  virtual Result OnElseExpr(const State&);
  virtual Result OnEndExpr(const State&);
  virtual Result OnEndFunc(const State&);
  virtual Result OnF32ConstExpr(const State&, uint32_t value_bits);
  virtual Result OnF64ConstExpr(const State&, uint64_t value_bits);
  virtual Result OnGetGlobalExpr(const State&, uint32_t global_index);
  virtual Result OnGetLocalExpr(const State&, uint32_t local_index);
  virtual Result OnGrowMemoryExpr(const State&);
  virtual Result OnI32ConstExpr(const State&, uint32_t value);
  virtual Result OnI64ConstExpr(const State&, uint64_t value);
  virtual Result OnIfExpr(const State&, uint32_t num_types, Type* sig_types);
  virtual Result OnLoadExpr(const State&,
                            Opcode opcode,
                            uint32_t alignment_log2,
                            uint32_t offset);
  virtual Result OnLoopExpr(const State&, uint32_t num_types, Type* sig_types);
  virtual Result OnNopExpr(const State&);
  virtual Result OnReturnExpr(const State&);
  virtual Result OnSelectExpr(const State&);
  virtual Result OnSetGlobalExpr(const State&, uint32_t global_index);
  virtual Result OnSetLocalExpr(const State&, uint32_t local_index);
  virtual Result OnStoreExpr(const State&,
                             Opcode opcode,
                             uint32_t alignment_log2,
                             uint32_t offset);
  virtual Result OnTeeLocalExpr(const State&, uint32_t local_index);
  virtual Result OnUnaryExpr(const State&, Opcode opcode);
  virtual Result OnUnreachableExpr(const State&);
  virtual Result EndFunctionBody(const State&, uint32_t index);
  virtual Result EndCodeSection(const State&);

  virtual Result BeginElemSection(const State&, uint32_t size);
  virtual Result OnElemSegmentCount(const State&, uint32_t count);
  virtual Result BeginElemSegment(const State&,
                                  uint32_t index,
                                  uint32_t table_index);
  virtual Result BeginElemSegmentInitExpr(const State&, uint32_t index);
  virtual Result EndElemSegmentInitExpr(const State&, uint32_t index);
  virtual Result OnElemSegmentFunctionIndexCount(const State&,
                                                 uint32_t index,
                                                 uint32_t count);
  virtual Result OnElemSegmentFunctionIndex(const State&,
                                            uint32_t index,
                                            uint32_t func_index);
  virtual Result EndElemSegment(const State&, uint32_t index);
  virtual Result EndElemSection(const State&);

  virtual Result BeginDataSection(const State&, uint32_t size);
  virtual Result OnDataSegmentCount(const State&, uint32_t count);
  virtual Result BeginDataSegment(const State&,
                                  uint32_t index,
                                  uint32_t memory_index);
  virtual Result BeginDataSegmentInitExpr(const State&, uint32_t index);
  virtual Result EndDataSegmentInitExpr(const State&, uint32_t index);
  virtual Result OnDataSegmentData(const State&,
                                   uint32_t index,
                                   const void* data,
                                   uint32_t size);
  virtual Result EndDataSegment(const State&, uint32_t index);
  virtual Result EndDataSection(const State&);

  virtual Result BeginNamesSection(const State&, uint32_t size);
  virtual Result OnFunctionNameSubsection(const State&,
                                          uint32_t index,
                                          uint32_t name_type,
                                          uint32_t subsection_size);
  virtual Result OnFunctionNamesCount(const State&, uint32_t num_functions);
  virtual Result OnFunctionName(const State&,
                                uint32_t function_index,
                                StringSlice function_name);
  virtual Result OnLocalNameSubsection(const State&,
                                       uint32_t index,
                                       uint32_t name_type,
                                       uint32_t subsection_size);
  virtual Result OnLocalNameFunctionCount(const State&, uint32_t num_functions);
  virtual Result OnLocalNameLocalCount(const State&,
                                       uint32_t function_index,
                                       uint32_t num_locals);
  virtual Result OnLocalName(const State&,
                             uint32_t function_index,
                             uint32_t local_index,
                             StringSlice local_name);
  virtual Result EndNamesSection(const State&);

  virtual Result BeginRelocSection(const State&, uint32_t size);
  virtual Result OnRelocCount(const State&,
                              uint32_t count,
                              BinarySection section_code,
                              StringSlice section_name);
  virtual Result OnReloc(const State&,
                         RelocType type,
                         uint32_t offset,
                         uint32_t index,
                         int32_t addend);
  virtual Result EndRelocSection(const State&);

  virtual Result OnInitExprF32ConstExpr(const State&,
                                        uint32_t index,
                                        uint32_t value);
  virtual Result OnInitExprF64ConstExpr(const State&,
                                        uint32_t index,
                                        uint64_t value);
  virtual Result OnInitExprGetGlobalExpr(const State&,
                                         uint32_t index,
                                         uint32_t global_index);
  virtual Result OnInitExprI32ConstExpr(const State&,
                                        uint32_t index,
                                        uint32_t value);
  virtual Result OnInitExprI64ConstExpr(const State&,
                                        uint32_t index,
                                        uint64_t value);

 private:
  void Indent();
  void Dedent();
  void WriteIndent();
  void LogTypes(uint32_t type_count, Type* types);

  Stream* stream;
  BinaryReader* reader;
  int indent;
};

}  // namespace wabt

#endif  // WABT_LOGGING_BINARY_READER_H_
