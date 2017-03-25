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

#include "binary-reader-ast.h"

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include <vector>

#include "ast.h"
#include "binary-reader-nop.h"
#include "common.h"

#define CHECK_RESULT(expr) \
  do {                     \
    if (WABT_FAILED(expr)) \
      return Result::Error;   \
  } while (0)

namespace wabt {

namespace {

struct LabelNode {
  LabelNode(LabelType, Expr** first);

  LabelType label_type;
  Expr** first;
  Expr* last;
};

LabelNode::LabelNode(LabelType label_type, Expr** first)
    : label_type(label_type), first(first), last(nullptr) {}

class BinaryReaderAST : public BinaryReaderNop {
 public:
  BinaryReaderAST(Module* out_module, BinaryErrorHandler* error_handler);

  virtual bool OnError(const State&, const char* message);

  virtual Result OnTypeCount(const State&, uint32_t count);
  virtual Result OnType(const State&,
                        uint32_t index,
                        uint32_t param_count,
                        Type* param_types,
                        uint32_t result_count,
                        Type* result_types);

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

  virtual Result OnFunctionCount(const State&, uint32_t count);
  virtual Result OnFunction(const State&, uint32_t index, uint32_t sig_index);

  virtual Result OnTableCount(const State&, uint32_t count);
  virtual Result OnTable(const State&,
                         uint32_t index,
                         Type elem_type,
                         const Limits* elem_limits);

  virtual Result OnMemoryCount(const State&, uint32_t count);
  virtual Result OnMemory(const State&, uint32_t index, const Limits* limits);

  virtual Result OnGlobalCount(const State&, uint32_t count);
  virtual Result BeginGlobal(const State&,
                             uint32_t index,
                             Type type,
                             bool mutable_);
  virtual Result BeginGlobalInitExpr(const State&, uint32_t index);
  virtual Result EndGlobalInitExpr(const State&, uint32_t index);

  virtual Result OnExportCount(const State&, uint32_t count);
  virtual Result OnExport(const State&,
                          uint32_t index,
                          ExternalKind kind,
                          uint32_t item_index,
                          StringSlice name);

  virtual Result OnStartFunction(const State&, uint32_t func_index);

  virtual Result OnFunctionBodyCount(const State&, uint32_t count);
  virtual Result BeginFunctionBody(const State&, uint32_t index);
  virtual Result OnLocalDecl(const State&,
                             uint32_t decl_index,
                             uint32_t count,
                             Type type);

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
  virtual Result OnDropExpr(const State&);
  virtual Result OnElseExpr(const State&);
  virtual Result OnEndExpr(const State&);
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
  virtual Result OnCurrentMemoryExpr(const State&);
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

  virtual Result OnFunctionNamesCount(const State&, uint32_t num_functions);
  virtual Result OnFunctionName(const State&,
                                uint32_t function_index,
                                StringSlice function_name);
  virtual Result OnLocalNameLocalCount(const State&,
                                       uint32_t function_index,
                                       uint32_t num_locals);
  virtual Result OnLocalName(const State&,
                             uint32_t function_index,
                             uint32_t local_index,
                             StringSlice local_name);

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
  bool HandleError(uint32_t offset, const char* message);
  void PrintError(const char* format, ...);
  void PushLabel(LabelType label_type, Expr** first);
  Result PopLabel();
  Result GetLabelAt(LabelNode** label, uint32_t depth);
  Result TopLabel(LabelNode** label);
  Result AppendExpr(Expr* expr);

  BinaryErrorHandler* error_handler = nullptr;
  Module* module = nullptr;

  Func* current_func = nullptr;
  std::vector<LabelNode> label_stack;
  uint32_t max_depth = 0;
  Expr** current_init_expr = nullptr;
};

BinaryReaderAST::BinaryReaderAST(Module* out_module,
                                 BinaryErrorHandler* error_handler)
    : error_handler(error_handler), module(out_module) {}

void WABT_PRINTF_FORMAT(2, 3) BinaryReaderAST::PrintError(const char* format,
                                                          ...) {
  WABT_SNPRINTF_ALLOCA(buffer, length, format);
  HandleError(WABT_UNKNOWN_OFFSET, buffer);
}

void BinaryReaderAST::PushLabel(LabelType label_type, Expr** first) {
  max_depth++;
  label_stack.emplace_back(label_type, first);
}

Result BinaryReaderAST::PopLabel() {
  if (label_stack.size() == 0) {
    PrintError("popping empty label stack");
    return Result::Error;
  }

  max_depth--;
  label_stack.pop_back();
  return Result::Ok;
}

Result BinaryReaderAST::GetLabelAt(LabelNode** label, uint32_t depth) {
  if (depth >= label_stack.size()) {
    PrintError("accessing stack depth: %u >= max: %" PRIzd, depth,
               label_stack.size());
    return Result::Error;
  }

  *label = &label_stack[label_stack.size() - depth - 1];
  return Result::Ok;
}

Result BinaryReaderAST::TopLabel(LabelNode** label) {
  return GetLabelAt(label, 0);
}

Result BinaryReaderAST::AppendExpr(Expr* expr) {
  LabelNode* label;
  if (WABT_FAILED(TopLabel(&label))) {
    delete expr;
    return Result::Error;
  }
  if (*label->first) {
    label->last->next = expr;
    label->last = expr;
  } else {
    *label->first = label->last = expr;
  }
  return Result::Ok;
}

bool BinaryReaderAST::HandleError(uint32_t offset, const char* message) {
  if (error_handler->on_error) {
    return error_handler->on_error(offset, message, error_handler->user_data);
  }
  return false;
}

bool BinaryReaderAST::OnError(const State& state, const char* message) {
  return HandleError(state.offset, message);
}

Result BinaryReaderAST::OnTypeCount(const State& state, uint32_t count) {
  module->func_types.reserve(count);
  return Result::Ok;
}

Result BinaryReaderAST::OnType(const State& state,
                               uint32_t index,
                               uint32_t param_count,
                               Type* param_types,
                               uint32_t result_count,
                               Type* result_types) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::FuncType;
  field->func_type = new FuncType();

  FuncType* func_type = field->func_type;
  func_type->sig.param_types.assign(param_types, param_types + param_count);
  func_type->sig.result_types.assign(result_types, result_types + result_count);
  module->func_types.push_back(func_type);
  return Result::Ok;
}

Result BinaryReaderAST::OnImportCount(const State& state, uint32_t count) {
  module->imports.reserve(count);
  return Result::Ok;
}

Result BinaryReaderAST::OnImport(const State& state,
                                 uint32_t index,
                                 StringSlice module_name,
                                 StringSlice field_name) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Import;
  field->import = new Import();

  Import* import = field->import;
  import->module_name = dup_string_slice(module_name);
  import->field_name = dup_string_slice(field_name);
  module->imports.push_back(import);
  return Result::Ok;
}

Result BinaryReaderAST::OnImportFunc(const State& state,
                                     uint32_t import_index,
                                     StringSlice module_name,
                                     StringSlice field_name,
                                     uint32_t func_index,
                                     uint32_t sig_index) {
  assert(import_index == module->imports.size() - 1);
  Import* import = module->imports[import_index];

  import->kind = ExternalKind::Func;
  import->func = new Func();
  import->func->decl.has_func_type = true;
  import->func->decl.type_var.type = VarType::Index;
  import->func->decl.type_var.index = sig_index;
  import->func->decl.sig = module->func_types[sig_index]->sig;

  module->funcs.push_back(import->func);
  module->num_func_imports++;
  return Result::Ok;
}

Result BinaryReaderAST::OnImportTable(const State& state,
                                      uint32_t import_index,
                                      StringSlice module_name,
                                      StringSlice field_name,
                                      uint32_t table_index,
                                      Type elem_type,
                                      const Limits* elem_limits) {
  assert(import_index == module->imports.size() - 1);
  Import* import = module->imports[import_index];
  import->kind = ExternalKind::Table;
  import->table = new Table();
  import->table->elem_limits = *elem_limits;
  module->tables.push_back(import->table);
  module->num_table_imports++;
  return Result::Ok;
}

Result BinaryReaderAST::OnImportMemory(const State& state,
                                       uint32_t import_index,
                                       StringSlice module_name,
                                       StringSlice field_name,
                                       uint32_t memory_index,
                                       const Limits* page_limits) {
  assert(import_index == module->imports.size() - 1);
  Import* import = module->imports[import_index];
  import->kind = ExternalKind::Memory;
  import->memory = new Memory();
  import->memory->page_limits = *page_limits;
  module->memories.push_back(import->memory);
  module->num_memory_imports++;
  return Result::Ok;
}

Result BinaryReaderAST::OnImportGlobal(const State& state,
                                       uint32_t import_index,
                                       StringSlice module_name,
                                       StringSlice field_name,
                                       uint32_t global_index,
                                       Type type,
                                       bool mutable_) {
  assert(import_index == module->imports.size() - 1);
  Import* import = module->imports[import_index];
  import->kind = ExternalKind::Global;
  import->global = new Global();
  import->global->type = type;
  import->global->mutable_ = mutable_;
  module->globals.push_back(import->global);
  module->num_global_imports++;
  return Result::Ok;
}

Result BinaryReaderAST::OnFunctionCount(const State& state, uint32_t count) {
  module->funcs.reserve(module->num_func_imports + count);
  return Result::Ok;
}

Result BinaryReaderAST::OnFunction(const State& state,
                                   uint32_t index,
                                   uint32_t sig_index) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Func;
  field->func = new Func();

  Func* func = field->func;
  func->decl.has_func_type = true;
  func->decl.type_var.type = VarType::Index;
  func->decl.type_var.index = sig_index;
  func->decl.sig = module->func_types[sig_index]->sig;

  module->funcs.push_back(func);
  return Result::Ok;
}

Result BinaryReaderAST::OnTableCount(const State& state, uint32_t count) {
  module->tables.reserve(module->num_table_imports + count);
  return Result::Ok;
}

Result BinaryReaderAST::OnTable(const State& state,
                                uint32_t index,
                                Type elem_type,
                                const Limits* elem_limits) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Table;
  field->table = new Table();
  field->table->elem_limits = *elem_limits;
  module->tables.push_back(field->table);
  return Result::Ok;
}

Result BinaryReaderAST::OnMemoryCount(const State& state, uint32_t count) {
  module->memories.reserve(module->num_memory_imports + count);
  return Result::Ok;
}

Result BinaryReaderAST::OnMemory(const State& state,
                                 uint32_t index,
                                 const Limits* page_limits) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Memory;
  field->memory = new Memory();
  field->memory->page_limits = *page_limits;
  module->memories.push_back(field->memory);
  return Result::Ok;
}

Result BinaryReaderAST::OnGlobalCount(const State& state, uint32_t count) {
  module->globals.reserve(module->num_global_imports + count);
  return Result::Ok;
}

Result BinaryReaderAST::BeginGlobal(const State& state,
                                    uint32_t index,
                                    Type type,
                                    bool mutable_) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Global;
  field->global = new Global();
  field->global->type = type;
  field->global->mutable_ = mutable_;
  module->globals.push_back(field->global);
  return Result::Ok;
}

Result BinaryReaderAST::BeginGlobalInitExpr(const State& state,
                                            uint32_t index) {
  assert(index == module->globals.size() - 1);
  Global* global = module->globals[index];
  current_init_expr = &global->init_expr;
  return Result::Ok;
}

Result BinaryReaderAST::EndGlobalInitExpr(const State& state, uint32_t index) {
  current_init_expr = nullptr;
  return Result::Ok;
}

Result BinaryReaderAST::OnExportCount(const State& state, uint32_t count) {
  module->exports.reserve(count);
  return Result::Ok;
}

Result BinaryReaderAST::OnExport(const State& state,
                                 uint32_t index,
                                 ExternalKind kind,
                                 uint32_t item_index,
                                 StringSlice name) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Export;
  field->export_ = new Export();

  Export* export_ = field->export_;
  export_->name = dup_string_slice(name);
  switch (kind) {
    case ExternalKind::Func:
      assert(item_index < module->funcs.size());
      break;
    case ExternalKind::Table:
      assert(item_index < module->tables.size());
      break;
    case ExternalKind::Memory:
      assert(item_index < module->memories.size());
      break;
    case ExternalKind::Global:
      assert(item_index < module->globals.size());
      break;
  }
  export_->var.type = VarType::Index;
  export_->var.index = item_index;
  export_->kind = kind;
  module->exports.push_back(export_);
  return Result::Ok;
}

Result BinaryReaderAST::OnStartFunction(const State& state,
                                        uint32_t func_index) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::Start;

  field->start.type = VarType::Index;
  assert(func_index < module->funcs.size());
  field->start.index = func_index;

  module->start = &field->start;
  return Result::Ok;
}

Result BinaryReaderAST::OnFunctionBodyCount(const State& state,
                                            uint32_t count) {
  assert(module->num_func_imports + count == module->funcs.size());
  return Result::Ok;
}

Result BinaryReaderAST::BeginFunctionBody(const State& state, uint32_t index) {
  current_func = module->funcs[index];
  PushLabel(LabelType::Func, &current_func->first_expr);
  return Result::Ok;
}

Result BinaryReaderAST::OnLocalDecl(const State& state,
                                    uint32_t decl_index,
                                    uint32_t count,
                                    Type type) {
  TypeVector& types = current_func->local_types;
  types.reserve(types.size() + count);
  for (size_t i = 0; i < count; ++i)
    types.push_back(type);
  return Result::Ok;
}

Result BinaryReaderAST::OnBinaryExpr(const State& state, Opcode opcode) {
  Expr* expr = Expr::CreateBinary(opcode);
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnBlockExpr(const State& state,
                                    uint32_t num_types,
                                    Type* sig_types) {
  Expr* expr = Expr::CreateBlock(new Block());
  expr->block->sig.assign(sig_types, sig_types + num_types);
  AppendExpr(expr);
  PushLabel(LabelType::Block, &expr->block->first);
  return Result::Ok;
}

Result BinaryReaderAST::OnBrExpr(const State& state, uint32_t depth) {
  Expr* expr = Expr::CreateBr(Var(depth));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnBrIfExpr(const State& state, uint32_t depth) {
  Expr* expr = Expr::CreateBrIf(Var(depth));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnBrTableExpr(const State& state,
                                      uint32_t num_targets,
                                      uint32_t* target_depths,
                                      uint32_t default_target_depth) {
  VarVector* targets = new VarVector();
  targets->resize(num_targets);
  for (uint32_t i = 0; i < num_targets; ++i) {
    (*targets)[i] = Var(target_depths[i]);
  }
  Expr* expr = Expr::CreateBrTable(targets, Var(default_target_depth));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnCallExpr(const State& state, uint32_t func_index) {
  assert(func_index < module->funcs.size());
  Expr* expr = Expr::CreateCall(Var(func_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnCallIndirectExpr(const State& state,
                                           uint32_t sig_index) {
  assert(sig_index < module->func_types.size());
  Expr* expr = Expr::CreateCallIndirect(Var(sig_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnCompareExpr(const State& state, Opcode opcode) {
  Expr* expr = Expr::CreateCompare(opcode);
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnConvertExpr(const State& state, Opcode opcode) {
  Expr* expr = Expr::CreateConvert(opcode);
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnCurrentMemoryExpr(const State& state) {
  Expr* expr = Expr::CreateCurrentMemory();
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnDropExpr(const State& state) {
  Expr* expr = Expr::CreateDrop();
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnElseExpr(const State& state) {
  LabelNode* label;
  CHECK_RESULT(TopLabel(&label));
  if (label->label_type != LabelType::If) {
    PrintError("else expression without matching if");
    return Result::Error;
  }

  LabelNode* parent_label;
  CHECK_RESULT(GetLabelAt(&parent_label, 1));
  assert(parent_label->last->type == ExprType::If);

  label->label_type = LabelType::Else;
  label->first = &parent_label->last->if_.false_;
  label->last = nullptr;
  return Result::Ok;
}

Result BinaryReaderAST::OnEndExpr(const State& state) {
  return PopLabel();
}

Result BinaryReaderAST::OnF32ConstExpr(const State& state,
                                       uint32_t value_bits) {
  Expr* expr = Expr::CreateConst(Const(Const::F32(), value_bits));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnF64ConstExpr(const State& state,
                                       uint64_t value_bits) {
  Expr* expr = Expr::CreateConst(Const(Const::F64(), value_bits));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnGetGlobalExpr(const State& state,
                                        uint32_t global_index) {
  Expr* expr = Expr::CreateGetGlobal(Var(global_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnGetLocalExpr(const State& state,
                                       uint32_t local_index) {
  Expr* expr = Expr::CreateGetLocal(Var(local_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnGrowMemoryExpr(const State& state) {
  Expr* expr = Expr::CreateGrowMemory();
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnI32ConstExpr(const State& state, uint32_t value) {
  Expr* expr = Expr::CreateConst(Const(Const::I32(), value));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnI64ConstExpr(const State& state, uint64_t value) {
  Expr* expr = Expr::CreateConst(Const(Const::I64(), value));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnIfExpr(const State& state,
                                 uint32_t num_types,
                                 Type* sig_types) {
  Expr* expr = Expr::CreateIf(new Block());
  expr->if_.true_->sig.assign(sig_types, sig_types + num_types);
  expr->if_.false_ = nullptr;
  AppendExpr(expr);
  PushLabel(LabelType::If, &expr->if_.true_->first);
  return Result::Ok;
}

Result BinaryReaderAST::OnLoadExpr(const State& state,
                                   Opcode opcode,
                                   uint32_t alignment_log2,
                                   uint32_t offset) {
  Expr* expr = Expr::CreateLoad(opcode, 1 << alignment_log2, offset);
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnLoopExpr(const State& state,
                                   uint32_t num_types,
                                   Type* sig_types) {
  Expr* expr = Expr::CreateLoop(new Block());
  expr->loop->sig.assign(sig_types, sig_types + num_types);
  AppendExpr(expr);
  PushLabel(LabelType::Loop, &expr->loop->first);
  return Result::Ok;
}

Result BinaryReaderAST::OnNopExpr(const State& state) {
  Expr* expr = Expr::CreateNop();
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnReturnExpr(const State& state) {
  Expr* expr = Expr::CreateReturn();
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnSelectExpr(const State& state) {
  Expr* expr = Expr::CreateSelect();
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnSetGlobalExpr(const State& state,
                                        uint32_t global_index) {
  Expr* expr = Expr::CreateSetGlobal(Var(global_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnSetLocalExpr(const State& state,
                                       uint32_t local_index) {
  Expr* expr = Expr::CreateSetLocal(Var(local_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnStoreExpr(const State& state,
                                    Opcode opcode,
                                    uint32_t alignment_log2,
                                    uint32_t offset) {
  Expr* expr = Expr::CreateStore(opcode, 1 << alignment_log2, offset);
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnTeeLocalExpr(const State& state,
                                       uint32_t local_index) {
  Expr* expr = Expr::CreateTeeLocal(Var(local_index));
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnUnaryExpr(const State& state, Opcode opcode) {
  Expr* expr = Expr::CreateUnary(opcode);
  return AppendExpr(expr);
}

Result BinaryReaderAST::OnUnreachableExpr(const State& state) {
  Expr* expr = Expr::CreateUnreachable();
  return AppendExpr(expr);
}

Result BinaryReaderAST::EndFunctionBody(const State& state, uint32_t index) {
  CHECK_RESULT(PopLabel());
  current_func = nullptr;
  return Result::Ok;
}

Result BinaryReaderAST::OnElemSegmentCount(const State& state, uint32_t count) {
  module->elem_segments.reserve(count);
  return Result::Ok;
}

Result BinaryReaderAST::BeginElemSegment(const State& state,
                                         uint32_t index,
                                         uint32_t table_index) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::ElemSegment;
  field->elem_segment = new ElemSegment();
  field->elem_segment->table_var.type = VarType::Index;
  field->elem_segment->table_var.index = table_index;
  module->elem_segments.push_back(field->elem_segment);
  return Result::Ok;
}

Result BinaryReaderAST::BeginElemSegmentInitExpr(const State& state,
                                                 uint32_t index) {
  assert(index == module->elem_segments.size() - 1);
  ElemSegment* segment = module->elem_segments[index];
  current_init_expr = &segment->offset;
  return Result::Ok;
}

Result BinaryReaderAST::EndElemSegmentInitExpr(const State& state,
                                               uint32_t index) {
  current_init_expr = nullptr;
  return Result::Ok;
}

Result BinaryReaderAST::OnElemSegmentFunctionIndexCount(const State& state,
                                                        uint32_t index,
                                                        uint32_t count) {
  assert(index == module->elem_segments.size() - 1);
  ElemSegment* segment = module->elem_segments[index];
  segment->vars.reserve(count);
  return Result::Ok;
}

Result BinaryReaderAST::OnElemSegmentFunctionIndex(const State& state,
                                                   uint32_t index,
                                                   uint32_t func_index) {
  assert(index == module->elem_segments.size() - 1);
  ElemSegment* segment = module->elem_segments[index];
  segment->vars.emplace_back();
  Var* var = &segment->vars.back();
  var->type = VarType::Index;
  var->index = func_index;
  return Result::Ok;
}

Result BinaryReaderAST::OnDataSegmentCount(const State& state, uint32_t count) {
  module->data_segments.reserve(count);
  return Result::Ok;
}

Result BinaryReaderAST::BeginDataSegment(const State& state,
                                         uint32_t index,
                                         uint32_t memory_index) {
  ModuleField* field = append_module_field(module);
  field->type = ModuleFieldType::DataSegment;
  field->data_segment = new DataSegment();
  field->data_segment->memory_var.type = VarType::Index;
  field->data_segment->memory_var.index = memory_index;
  module->data_segments.push_back(field->data_segment);
  return Result::Ok;
}

Result BinaryReaderAST::BeginDataSegmentInitExpr(const State& state,
                                                 uint32_t index) {
  assert(index == module->data_segments.size() - 1);
  DataSegment* segment = module->data_segments[index];
  current_init_expr = &segment->offset;
  return Result::Ok;
}

Result BinaryReaderAST::EndDataSegmentInitExpr(const State& state,
                                               uint32_t index) {
  current_init_expr = nullptr;
  return Result::Ok;
}

Result BinaryReaderAST::OnDataSegmentData(const State& state,
                                          uint32_t index,
                                          const void* data,
                                          uint32_t size) {
  assert(index == module->data_segments.size() - 1);
  DataSegment* segment = module->data_segments[index];
  segment->data = new char[size];
  segment->size = size;
  memcpy(segment->data, data, size);
  return Result::Ok;
}

Result BinaryReaderAST::OnFunctionNamesCount(const State& state,
                                             uint32_t count) {
  if (count > module->funcs.size()) {
    PrintError("expected function name count (%u) <= function count (%" PRIzd
               ")",
               count, module->funcs.size());
    return Result::Error;
  }
  return Result::Ok;
}

Result BinaryReaderAST::OnFunctionName(const State& state,
                                       uint32_t index,
                                       StringSlice name) {
  if (string_slice_is_empty(&name))
    return Result::Ok;

  module->func_bindings.emplace(string_slice_to_string(name), Binding(index));
  Func* func = module->funcs[index];
  func->name = dup_string_slice(name);
  return Result::Ok;
}

Result BinaryReaderAST::OnLocalNameLocalCount(const State& state,
                                              uint32_t index,
                                              uint32_t count) {
  assert(index < module->funcs.size());
  Func* func = module->funcs[index];
  uint32_t num_params_and_locals = get_num_params_and_locals(func);
  if (count > num_params_and_locals) {
    PrintError("expected local name count (%d) <= local count (%d)", count,
               num_params_and_locals);
    return Result::Error;
  }
  return Result::Ok;
}

Result BinaryReaderAST::OnInitExprF32ConstExpr(const State& state,
                                               uint32_t index,
                                               uint32_t value) {
  *current_init_expr = Expr::CreateConst(Const(Const::F32(), value));
  return Result::Ok;
}

Result BinaryReaderAST::OnInitExprF64ConstExpr(const State& state,
                                               uint32_t index,
                                               uint64_t value) {
  *current_init_expr = Expr::CreateConst(Const(Const::F64(), value));
  return Result::Ok;
}

Result BinaryReaderAST::OnInitExprGetGlobalExpr(const State& state,
                                                uint32_t index,
                                                uint32_t global_index) {
  *current_init_expr = Expr::CreateGetGlobal(Var(global_index));
  return Result::Ok;
}

Result BinaryReaderAST::OnInitExprI32ConstExpr(const State& state,
                                               uint32_t index,
                                               uint32_t value) {
  *current_init_expr = Expr::CreateConst(Const(Const::I32(), value));
  return Result::Ok;
}

Result BinaryReaderAST::OnInitExprI64ConstExpr(const State& state,
                                               uint32_t index,
                                               uint64_t value) {
  *current_init_expr = Expr::CreateConst(Const(Const::I64(), value));
  return Result::Ok;
}

Result BinaryReaderAST::OnLocalName(const State& state,
                                    uint32_t func_index,
                                    uint32_t local_index,
                                    StringSlice name) {
  if (string_slice_is_empty(&name))
    return Result::Ok;

  Func* func = module->funcs[func_index];
  uint32_t num_params = get_num_params(func);
  BindingHash* bindings;
  uint32_t index;
  if (local_index < num_params) {
    /* param name */
    bindings = &func->param_bindings;
    index = local_index;
  } else {
    /* local name */
    bindings = &func->local_bindings;
    index = local_index - num_params;
  }
  bindings->emplace(string_slice_to_string(name), Binding(index));
  return Result::Ok;
}

}  // namespace

Result read_binary_ast(const void* data,
                       size_t size,
                       const ReadBinaryOptions* options,
                       BinaryErrorHandler* error_handler,
                       struct Module* out_module) {
  BinaryReaderAST reader(out_module, error_handler);
  Result result = read_binary(data, size, &reader, options);
  return result;
}

}  // namespace wabt
