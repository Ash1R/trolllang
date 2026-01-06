#include "../include/CodeGenerator.h"
#include <iostream>
#include <system_error>
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("troll_module", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    
    // Initialize TrollValue Struct Type: { i32 type, double num, ptr ptr }
    std::vector<llvm::Type*> elements;
    elements.push_back(builder->getInt32Ty()); // type
    elements.push_back(builder->getDoubleTy()); // num
    elements.push_back(llvm::PointerType::getUnqual(*context)); // ptr
    valueStructType = llvm::StructType::create(*context, elements, "TrollValue");
    
    setupExternalFunctions();
}

void CodeGenerator::setupExternalFunctions() {
    // int printf(const char*, ...)
    std::vector<llvm::Type*> args;
    args.push_back(llvm::PointerType::getUnqual(*context)); 
    llvm::FunctionType* printfType = llvm::FunctionType::get(builder->getInt32Ty(), args, true);
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module.get());
    
    // void* troll_create_array(int size)
    std::vector<llvm::Type*> createArgs;
    createArgs.push_back(builder->getInt32Ty());
    llvm::FunctionType* createType = llvm::FunctionType::get(llvm::PointerType::getUnqual(*context), createArgs, false);
    llvm::Function::Create(createType, llvm::Function::ExternalLinkage, "troll_create_array", module.get());
    
    // void troll_print_value(int type, double num, void* ptr)
    std::vector<llvm::Type*> printArgs;
    printArgs.push_back(builder->getInt32Ty());
    printArgs.push_back(builder->getDoubleTy());
    printArgs.push_back(llvm::PointerType::getUnqual(*context));
    llvm::FunctionType* printType = llvm::FunctionType::get(builder->getVoidTy(), printArgs, false);
    llvm::Function::Create(printType, llvm::Function::ExternalLinkage, "troll_print_value", module.get());
    
    // void troll_array_set(void* arr, int index, double value)
    std::vector<llvm::Type*> setArgs;
    setArgs.push_back(llvm::PointerType::getUnqual(*context));
    setArgs.push_back(builder->getInt32Ty());
    setArgs.push_back(builder->getDoubleTy());
    llvm::FunctionType* setType = llvm::FunctionType::get(builder->getVoidTy(), setArgs, false);
    llvm::Function::Create(setType, llvm::Function::ExternalLinkage, "troll_array_set", module.get());

    // double troll_array_get(void* arr, int index)
    std::vector<llvm::Type*> getArgs;
    getArgs.push_back(llvm::PointerType::getUnqual(*context));
    getArgs.push_back(builder->getInt32Ty());
    llvm::FunctionType* getType = llvm::FunctionType::get(builder->getDoubleTy(), getArgs, false);
    llvm::Function::Create(getType, llvm::Function::ExternalLinkage, "troll_array_get", module.get());
}

llvm::Value* CodeGenerator::createNumber(double val) {
    llvm::Value* alloc = builder->CreateAlloca(valueStructType);
    llvm::Value* typePtr = builder->CreateStructGEP(valueStructType, alloc, 0);
    llvm::Value* numPtr = builder->CreateStructGEP(valueStructType, alloc, 1);
    
    builder->CreateStore(builder->getInt32(TYPE_NUMBER), typePtr);
    builder->CreateStore(llvm::ConstantFP::get(*context, llvm::APFloat(val)), numPtr);
    
    // Load as value (first class aggregate) or keep as pointer?
    // Let's stick to passing Load (SSA struct value)
    return builder->CreateLoad(valueStructType, alloc);
}

llvm::Value* CodeGenerator::createBool(bool val) {
    // Bool is treated as number 0.0 or 1.0 but with TYPE_BOOL (2)
    llvm::Value* alloc = builder->CreateAlloca(valueStructType);
    llvm::Value* typePtr = builder->CreateStructGEP(valueStructType, alloc, 0);
    llvm::Value* numPtr = builder->CreateStructGEP(valueStructType, alloc, 1);
    
    builder->CreateStore(builder->getInt32(TYPE_BOOL), typePtr);
    builder->CreateStore(llvm::ConstantFP::get(*context, llvm::APFloat(val ? 1.0 : 0.0)), numPtr);
    
    return builder->CreateLoad(valueStructType, alloc);
}

llvm::Value* CodeGenerator::createArray(llvm::Value* ptr) {
    llvm::Value* alloc = builder->CreateAlloca(valueStructType);
    llvm::Value* typePtr = builder->CreateStructGEP(valueStructType, alloc, 0);
    llvm::Value* ptrPtr = builder->CreateStructGEP(valueStructType, alloc, 2);
    
    builder->CreateStore(builder->getInt32(TYPE_ARRAY), typePtr);
    builder->CreateStore(ptr, ptrPtr); // Store the array pointer
    
    return builder->CreateLoad(valueStructType, alloc);
}

llvm::Value* CodeGenerator::unpackNumber(llvm::Value* trollVal) {
    // Extract double num. Assume it's a number/bool.
    // We could optimize by checking type but for now just extract field 1.
    return builder->CreateExtractValue(trollVal, 1, "rawNum");
}

llvm::Value* CodeGenerator::createNumberFromValue(llvm::Value* val) {
    llvm::Value* alloc = builder->CreateAlloca(valueStructType);
    llvm::Value* typePtr = builder->CreateStructGEP(valueStructType, alloc, 0);
    llvm::Value* numPtr = builder->CreateStructGEP(valueStructType, alloc, 1);
    
    builder->CreateStore(builder->getInt32(TYPE_NUMBER), typePtr);
    builder->CreateStore(val, numPtr);
    
    return builder->CreateLoad(valueStructType, alloc);
}

void CodeGenerator::generateCode(const std::vector<std::shared_ptr<Stmt>>& statements) {
    // Create main function: int main()
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder->getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module.get());
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", mainFunc);
    builder->SetInsertPoint(entry);

    for (const auto& stmt : statements) {
        // We only support ExprStmt (expressions) and PrintStmt for now in this restricted main
        stmt->accept(this);
    }

    // Return 0
    builder->CreateRet(builder->getInt32(0));

    // Verify
    llvm::verifyFunction(*mainFunc);
}

void CodeGenerator::saveModule(const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    if (EC) {
        std::cerr << "Could not open file: " << EC.message() << "\n";
        return;
    }
    module->print(dest, nullptr);
}

llvm::Value* CodeGenerator::evaluate(std::shared_ptr<Expr> expr) {
    if (!expr) return nullptr;
    // std::cout << "Evaluating " << typeid(*expr).name() << std::endl;
    std::any res = expr->accept(this);
    if (!res.has_value()) {
         std::cerr << "Fatal: Visitor returned empty any for " << typeid(*expr).name() << std::endl;
         exit(1);
    }
    try {
        return std::any_cast<llvm::Value*>(res);
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Fatal: Bad any cast for " << typeid(*expr).name() << ". Type: " << res.type().name() << std::endl;
        exit(1);
    }
}

// Visitors

std::any CodeGenerator::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) {
    if (std::holds_alternative<double>(expr->value)) {
        return createNumber(std::get<double>(expr->value));
    }
    if (std::holds_alternative<int>(expr->value)) {
        return createNumber((double)std::get<int>(expr->value));
    }
    if (std::holds_alternative<bool>(expr->value)) {
        return createBool(std::get<bool>(expr->value));
    }
    return (llvm::Value*)nullptr;
}

std::any CodeGenerator::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) {
    llvm::Value* LStruct = evaluate(expr->left);
    llvm::Value* RStruct = evaluate(expr->right);

    if (!LStruct || !RStruct) return (llvm::Value*)nullptr;
    
    llvm::Value* L = unpackNumber(LStruct);
    llvm::Value* R = unpackNumber(RStruct);
    
    // Math ops return Number (TrollValue)
    if (expr->op.type == TokenType::PLUS) return createNumberFromValue(builder->CreateFAdd(L, R));
    if (expr->op.type == TokenType::MINUS) return createNumberFromValue(builder->CreateFSub(L, R));
    if (expr->op.type == TokenType::STAR) return createNumberFromValue(builder->CreateFMul(L, R));
    if (expr->op.type == TokenType::SLASH) return createNumberFromValue(builder->CreateFDiv(L, R));

    // Comp ops return Bool (TrollValue)
    llvm::Value* cmp = nullptr;
    switch(expr->op.type) {
        case TokenType::LESS: cmp = builder->CreateFCmpOLT(L, R); break;
        case TokenType::GREATER: cmp = builder->CreateFCmpOGT(L, R); break;
        case TokenType::LESS_EQUAL: cmp = builder->CreateFCmpOLE(L, R); break;
        case TokenType::GREATER_EQUAL: cmp = builder->CreateFCmpOGE(L, R); break;
        case TokenType::EQUAL_EQUAL: cmp = builder->CreateFCmpOEQ(L, R); break;
        case TokenType::BANG_EQUAL: cmp = builder->CreateFCmpONE(L, R); break;
        default: return (llvm::Value*)nullptr;
    }
    
    // Create Bool TrollValue from i1 result
    llvm::Value* alloc = builder->CreateAlloca(valueStructType);
    builder->CreateStore(builder->getInt32(TYPE_BOOL), builder->CreateStructGEP(valueStructType, alloc, 0));
    llvm::Value* asDouble = builder->CreateUIToFP(cmp, builder->getDoubleTy());
    builder->CreateStore(asDouble, builder->CreateStructGEP(valueStructType, alloc, 1));
    return (llvm::Value*)builder->CreateLoad(valueStructType, alloc);
}

// Stubs for now
std::any CodeGenerator::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) { return (llvm::Value*)nullptr; }
std::any CodeGenerator::visitVariableExpr(std::shared_ptr<VariableExpr> expr) {
    if (namedValues.find(expr->name.lexeme) == namedValues.end()) {
        std::cerr << "Undefined variable: " << expr->name.lexeme << "\n";
        return (llvm::Value*)nullptr;
    }
    llvm::AllocaInst* alloca = namedValues[expr->name.lexeme];
    // Load value
    return (llvm::Value*)builder->CreateLoad(alloca->getAllocatedType(), alloca, expr->name.lexeme.c_str());
}

std::any CodeGenerator::visitLetStmt(std::shared_ptr<LetStmt> stmt) {
    llvm::Value* initVal;
    if (stmt->initializer) {
        initVal = evaluate(stmt->initializer);
    } else {
        initVal = createNumber(0.0);
    }
    
    // Create alloca for TrollValue (struct)
    llvm::AllocaInst* alloca = builder->CreateAlloca(valueStructType, 0, stmt->name.lexeme.c_str());
    builder->CreateStore(initVal, alloca);
    namedValues[stmt->name.lexeme] = alloca;
    
    return std::any();
}

std::any CodeGenerator::visitPrintStmt(std::shared_ptr<PrintStmt> stmt) {
    llvm::Value* val = evaluate(stmt->expression);
    if (!val) return std::any();
    
    // Extract: type, num, ptr
    llvm::Value* type = builder->CreateExtractValue(val, 0);
    llvm::Value* num = builder->CreateExtractValue(val, 1);
    llvm::Value* ptr = builder->CreateExtractValue(val, 2);
    
    llvm::Function* printFunc = module->getFunction("troll_print_value");
    builder->CreateCall(printFunc, {type, num, ptr});
    
    return std::any();
}
std::any CodeGenerator::visitExprStmt(std::shared_ptr<ExprStmt> stmt) { 
    evaluate(stmt->expression);
    return std::any(); 
}

// Missing Stubs
std::any CodeGenerator::visitCallExpr(std::shared_ptr<CallExpr> expr) {
    // Look up function name
    // For now, handle 'print' explicitly if it wasn't a statement? No, PrintStmt handles statement print.
    // This is for function calls.
    std::shared_ptr<VariableExpr> calleeVar = std::dynamic_pointer_cast<VariableExpr>(expr->callee);
    if (!calleeVar) {
        std::cerr << "Only support calling named functions for now.\n";
        return (llvm::Value*)nullptr;
    }
    
    llvm::Function* calleeF = module->getFunction(calleeVar->name.lexeme);
    if (!calleeF) {
        std::cerr << "Unknown function: " << calleeVar->name.lexeme << "\n";
        return (llvm::Value*)nullptr;
    }
    
    // Check arg count etc...
    
    std::vector<llvm::Value*> argsV;
    for (auto& arg : expr->arguments) {
        argsV.push_back(evaluate(arg));
    }
    
    return (llvm::Value*)builder->CreateCall(calleeF, argsV, "calltmp");
}
std::any CodeGenerator::visitGetExpr(std::shared_ptr<GetExpr> expr) { return (llvm::Value*)nullptr; }
std::any CodeGenerator::visitAssignmentExpr(std::shared_ptr<AssignmentExpr> expr) {
    llvm::Value* val = evaluate(expr->value);
    if (!val) return (llvm::Value*)nullptr;
    
    if (namedValues.find(expr->name.lexeme) == namedValues.end()) {
        std::cerr << "Undefined variable: " << expr->name.lexeme << "\n";
        return (llvm::Value*)nullptr;
    }
    
    llvm::AllocaInst* alloca = namedValues[expr->name.lexeme];
    builder->CreateStore(val, alloca);
    return val;
}
std::any CodeGenerator::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) { return (llvm::Value*)nullptr; }
std::any CodeGenerator::visitArrayLiteralExpr(std::shared_ptr<ArrayLiteralExpr> expr) {
    // 1. Create array
    int size = expr->elements.size();
    llvm::Function* createFunc = module->getFunction("troll_create_array");
    llvm::Value* rawPtr = builder->CreateCall(createFunc, {builder->getInt32(size)}, "arrPtr");
    
    // 2. Populate
    llvm::Function* setFunc = module->getFunction("troll_array_set");
    for (int i = 0; i < size; ++i) {
         llvm::Value* eleStruct = evaluate(expr->elements[i]);
         llvm::Value* val = unpackNumber(eleStruct); // Assume double for now
         
         builder->CreateCall(setFunc, {rawPtr, builder->getInt32(i), val});
    }
    
    // 3. Return TrollValue (Type=Array, Ptr=rawPtr)
    return createArray(rawPtr);
}

std::any CodeGenerator::visitIndexExpr(std::shared_ptr<IndexExpr> expr) {
    llvm::Value* objStruct = evaluate(expr->object);
    llvm::Value* idxStruct = evaluate(expr->index);
    if (!objStruct || !idxStruct) return (llvm::Value*)nullptr;

    // Extract pointer
    llvm::Value* ptr = builder->CreateExtractValue(objStruct, 2, "arrayPtr");
    
    // Extract Index (double -> int)
    llvm::Value* idxDbl = unpackNumber(idxStruct);
    llvm::Value* idxInt = builder->CreateFPToSI(idxDbl, builder->getInt32Ty(), "idxInt");
    
    llvm::Function* getFunc = module->getFunction("troll_array_get");
    llvm::Value* val = builder->CreateCall(getFunc, {ptr, idxInt}, "arrayVal");
    
    return createNumberFromValue(val);
}

std::any CodeGenerator::visitArrayAssignmentExpr(std::shared_ptr<ArrayAssignmentExpr> expr) {
    llvm::Value* objStruct = evaluate(expr->object);
    llvm::Value* idxStruct = evaluate(expr->index);
    llvm::Value* valStruct = evaluate(expr->value);
    
    if (!objStruct || !idxStruct || !valStruct) return (llvm::Value*)nullptr;
    
    llvm::Value* ptr = builder->CreateExtractValue(objStruct, 2, "arrayPtr");
    llvm::Value* idxDbl = unpackNumber(idxStruct);
    llvm::Value* idxInt = builder->CreateFPToSI(idxDbl, builder->getInt32Ty(), "idxInt");
    llvm::Value* valRaw = unpackNumber(valStruct);
    
    llvm::Function* setFunc = module->getFunction("troll_array_set");
    builder->CreateCall(setFunc, {ptr, idxInt, valRaw});
    
    return valStruct; // Return the assigned TrollValue
}

std::any CodeGenerator::visitBlockStmt(std::shared_ptr<BlockStmt> stmt) { 
    for (const auto& s : stmt->statements) {
        s->accept(this);
    }
    return std::any(); 
}

std::any CodeGenerator::visitIfStmt(std::shared_ptr<IfStmt> stmt) {
    llvm::Value* condV = evaluate(stmt->condition); // TrollValue
    if (!condV) return std::any();
    
    // Unpack num (boolean logic uses the number field for 0.0 check)
    llvm::Value* num = unpackNumber(condV);
    
    // is != 0.0?
    llvm::Value* condBool = builder->CreateFCmpONE(num, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "ifcond");
    
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else", function);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont", function);

    builder->CreateCondBr(condBool, thenBB, elseBB);

    // THEN
    builder->SetInsertPoint(thenBB);
    stmt->thenBranch->accept(this);
    if (!thenBB->getTerminator()) builder->CreateBr(mergeBB);
    
    // ELSE
    builder->SetInsertPoint(elseBB);
    if (stmt->elseBranch) {
        stmt->elseBranch->accept(this);
    }
    if (!elseBB->getTerminator()) builder->CreateBr(mergeBB);

    // MERGE
    builder->SetInsertPoint(mergeBB);
    return std::any();
}


std::any CodeGenerator::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "loopcond", function);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "loopbody", function);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "loopend", function);
    
    builder->CreateBr(condBB);

    // COND
    builder->SetInsertPoint(condBB);
    llvm::Value* condV = evaluate(stmt->condition); // TrollValue
    llvm::Value* num = unpackNumber(condV);
    
    llvm::Value* condBool = builder->CreateFCmpONE(num, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "loopcond");
    builder->CreateCondBr(condBool, bodyBB, afterBB);

    // BODY
    builder->SetInsertPoint(bodyBB);
    stmt->body->accept(this);
    builder->CreateBr(condBB); 

    // AFTER
    builder->SetInsertPoint(afterBB);
    return std::any();
}

std::any CodeGenerator::visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
    if (stmt->value) {
        llvm::Value* retval = evaluate(stmt->value);
        builder->CreateRet(retval);
    } else {
        builder->CreateRet(createNumber(0.0));
    }
    return std::any();
}

std::any CodeGenerator::visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) {
    llvm::BasicBlock* oldInsertBlock = builder->GetInsertBlock();
    auto oldNamedValues = namedValues;
    namedValues.clear();

    // Function signature uses TrollValue Struct (passed by value for now)
    // Actually, passing struct by value in LLVM IR often requires 'byval' or expands it.
    // Let's just use what LLVM defaults to for the StructType.
    std::vector<llvm::Type*> args(stmt->params.size(), valueStructType);
    llvm::FunctionType* ft = llvm::FunctionType::get(valueStructType, args, false);
    
    llvm::Function* function = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, stmt->name.lexeme, module.get());
    
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(bb);
    
    unsigned idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(stmt->params[idx].lexeme);
        
        // Create alloca (TrollValue)
        llvm::AllocaInst* alloca = builder->CreateAlloca(valueStructType, 0, arg.getName());
        builder->CreateStore(&arg, alloca);
        
        namedValues[std::string(arg.getName())] = alloca;
        idx++;
    }
    
    for (auto& s : stmt->body) {
        s->accept(this);
    }
    
    if (!bb->getTerminator()) {
        builder->CreateRet(createNumber(0.0));
    }
    
    builder->SetInsertPoint(oldInsertBlock);
    namedValues = oldNamedValues;
    llvm::verifyFunction(*function);
    return std::any();
}
std::any CodeGenerator::visitModelStmt(std::shared_ptr<ModelStmt> stmt) { return std::any(); }
