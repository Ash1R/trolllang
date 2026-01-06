; ModuleID = 'troll_module'
source_filename = "troll_module"

%TrollValue = type { i32, double, ptr }

declare i32 @printf(ptr, ...)

declare ptr @troll_create_array(i32)

declare void @troll_print_value(i32, double, ptr)

declare void @troll_array_set(ptr, i32, double)

declare double @troll_array_get(ptr, i32)

define i32 @main() {
entry:
  %arrPtr = call ptr @troll_create_array(i32 3)
  %0 = alloca %TrollValue, align 8
  %1 = getelementptr inbounds nuw %TrollValue, ptr %0, i32 0, i32 0
  %2 = getelementptr inbounds nuw %TrollValue, ptr %0, i32 0, i32 1
  store i32 0, ptr %1, align 4
  store double 1.000000e+01, ptr %2, align 8
  %3 = load %TrollValue, ptr %0, align 8
  %rawNum = extractvalue %TrollValue %3, 1
  call void @troll_array_set(ptr %arrPtr, i32 0, double %rawNum)
  %4 = alloca %TrollValue, align 8
  %5 = getelementptr inbounds nuw %TrollValue, ptr %4, i32 0, i32 0
  %6 = getelementptr inbounds nuw %TrollValue, ptr %4, i32 0, i32 1
  store i32 0, ptr %5, align 4
  store double 2.000000e+01, ptr %6, align 8
  %7 = load %TrollValue, ptr %4, align 8
  %rawNum1 = extractvalue %TrollValue %7, 1
  call void @troll_array_set(ptr %arrPtr, i32 1, double %rawNum1)
  %8 = alloca %TrollValue, align 8
  %9 = getelementptr inbounds nuw %TrollValue, ptr %8, i32 0, i32 0
  %10 = getelementptr inbounds nuw %TrollValue, ptr %8, i32 0, i32 1
  store i32 0, ptr %9, align 4
  store double 3.000000e+01, ptr %10, align 8
  %11 = load %TrollValue, ptr %8, align 8
  %rawNum2 = extractvalue %TrollValue %11, 1
  call void @troll_array_set(ptr %arrPtr, i32 2, double %rawNum2)
  %12 = alloca %TrollValue, align 8
  %13 = getelementptr inbounds nuw %TrollValue, ptr %12, i32 0, i32 0
  %14 = getelementptr inbounds nuw %TrollValue, ptr %12, i32 0, i32 2
  store i32 1, ptr %13, align 4
  store ptr %arrPtr, ptr %14, align 8
  %15 = load %TrollValue, ptr %12, align 8
  %arr = alloca %TrollValue, align 8
  store %TrollValue %15, ptr %arr, align 8
  %arr3 = load %TrollValue, ptr %arr, align 8
  %16 = alloca %TrollValue, align 8
  %17 = getelementptr inbounds nuw %TrollValue, ptr %16, i32 0, i32 0
  %18 = getelementptr inbounds nuw %TrollValue, ptr %16, i32 0, i32 1
  store i32 0, ptr %17, align 4
  store double 0.000000e+00, ptr %18, align 8
  %19 = load %TrollValue, ptr %16, align 8
  %arrayPtr = extractvalue %TrollValue %arr3, 2
  %rawNum4 = extractvalue %TrollValue %19, 1
  %idxInt = fptosi double %rawNum4 to i32
  %arrayVal = call double @troll_array_get(ptr %arrayPtr, i32 %idxInt)
  %20 = alloca %TrollValue, align 8
  %21 = getelementptr inbounds nuw %TrollValue, ptr %20, i32 0, i32 0
  %22 = getelementptr inbounds nuw %TrollValue, ptr %20, i32 0, i32 1
  store i32 0, ptr %21, align 4
  store double %arrayVal, ptr %22, align 8
  %23 = load %TrollValue, ptr %20, align 8
  %24 = extractvalue %TrollValue %23, 0
  %25 = extractvalue %TrollValue %23, 1
  %26 = extractvalue %TrollValue %23, 2
  call void @troll_print_value(i32 %24, double %25, ptr %26)
  %arr5 = load %TrollValue, ptr %arr, align 8
  %27 = alloca %TrollValue, align 8
  %28 = getelementptr inbounds nuw %TrollValue, ptr %27, i32 0, i32 0
  %29 = getelementptr inbounds nuw %TrollValue, ptr %27, i32 0, i32 1
  store i32 0, ptr %28, align 4
  store double 1.000000e+00, ptr %29, align 8
  %30 = load %TrollValue, ptr %27, align 8
  %31 = alloca %TrollValue, align 8
  %32 = getelementptr inbounds nuw %TrollValue, ptr %31, i32 0, i32 0
  %33 = getelementptr inbounds nuw %TrollValue, ptr %31, i32 0, i32 1
  store i32 0, ptr %32, align 4
  store double 9.900000e+01, ptr %33, align 8
  %34 = load %TrollValue, ptr %31, align 8
  %arrayPtr6 = extractvalue %TrollValue %arr5, 2
  %rawNum7 = extractvalue %TrollValue %30, 1
  %idxInt8 = fptosi double %rawNum7 to i32
  %rawNum9 = extractvalue %TrollValue %34, 1
  call void @troll_array_set(ptr %arrayPtr6, i32 %idxInt8, double %rawNum9)
  %arr10 = load %TrollValue, ptr %arr, align 8
  %35 = alloca %TrollValue, align 8
  %36 = getelementptr inbounds nuw %TrollValue, ptr %35, i32 0, i32 0
  %37 = getelementptr inbounds nuw %TrollValue, ptr %35, i32 0, i32 1
  store i32 0, ptr %36, align 4
  store double 1.000000e+00, ptr %37, align 8
  %38 = load %TrollValue, ptr %35, align 8
  %arrayPtr11 = extractvalue %TrollValue %arr10, 2
  %rawNum12 = extractvalue %TrollValue %38, 1
  %idxInt13 = fptosi double %rawNum12 to i32
  %arrayVal14 = call double @troll_array_get(ptr %arrayPtr11, i32 %idxInt13)
  %39 = alloca %TrollValue, align 8
  %40 = getelementptr inbounds nuw %TrollValue, ptr %39, i32 0, i32 0
  %41 = getelementptr inbounds nuw %TrollValue, ptr %39, i32 0, i32 1
  store i32 0, ptr %40, align 4
  store double %arrayVal14, ptr %41, align 8
  %42 = load %TrollValue, ptr %39, align 8
  %43 = extractvalue %TrollValue %42, 0
  %44 = extractvalue %TrollValue %42, 1
  %45 = extractvalue %TrollValue %42, 2
  call void @troll_print_value(i32 %43, double %44, ptr %45)
  %arr15 = load %TrollValue, ptr %arr, align 8
  %46 = alloca %TrollValue, align 8
  %47 = getelementptr inbounds nuw %TrollValue, ptr %46, i32 0, i32 0
  %48 = getelementptr inbounds nuw %TrollValue, ptr %46, i32 0, i32 1
  store i32 0, ptr %47, align 4
  store double 2.000000e+00, ptr %48, align 8
  %49 = load %TrollValue, ptr %46, align 8
  %arrayPtr16 = extractvalue %TrollValue %arr15, 2
  %rawNum17 = extractvalue %TrollValue %49, 1
  %idxInt18 = fptosi double %rawNum17 to i32
  %arrayVal19 = call double @troll_array_get(ptr %arrayPtr16, i32 %idxInt18)
  %50 = alloca %TrollValue, align 8
  %51 = getelementptr inbounds nuw %TrollValue, ptr %50, i32 0, i32 0
  %52 = getelementptr inbounds nuw %TrollValue, ptr %50, i32 0, i32 1
  store i32 0, ptr %51, align 4
  store double %arrayVal19, ptr %52, align 8
  %53 = load %TrollValue, ptr %50, align 8
  %54 = extractvalue %TrollValue %53, 0
  %55 = extractvalue %TrollValue %53, 1
  %56 = extractvalue %TrollValue %53, 2
  call void @troll_print_value(i32 %54, double %55, ptr %56)
  ret i32 0
}
