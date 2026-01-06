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
  %a = alloca %TrollValue, align 8
  store %TrollValue %15, ptr %a, align 8
  %a3 = load %TrollValue, ptr %a, align 8
  %16 = extractvalue %TrollValue %a3, 0
  %17 = extractvalue %TrollValue %a3, 1
  %18 = extractvalue %TrollValue %a3, 2
  call void @troll_print_value(i32 %16, double %17, ptr %18)
  %arrPtr4 = call ptr @troll_create_array(i32 1)
  %19 = alloca %TrollValue, align 8
  %20 = getelementptr inbounds nuw %TrollValue, ptr %19, i32 0, i32 0
  %21 = getelementptr inbounds nuw %TrollValue, ptr %19, i32 0, i32 1
  store i32 0, ptr %20, align 4
  store double 5.000000e+00, ptr %21, align 8
  %22 = load %TrollValue, ptr %19, align 8
  %rawNum5 = extractvalue %TrollValue %22, 1
  call void @troll_array_set(ptr %arrPtr4, i32 0, double %rawNum5)
  %23 = alloca %TrollValue, align 8
  %24 = getelementptr inbounds nuw %TrollValue, ptr %23, i32 0, i32 0
  %25 = getelementptr inbounds nuw %TrollValue, ptr %23, i32 0, i32 2
  store i32 1, ptr %24, align 4
  store ptr %arrPtr4, ptr %25, align 8
  %26 = load %TrollValue, ptr %23, align 8
  %b = alloca %TrollValue, align 8
  store %TrollValue %26, ptr %b, align 8
  %b6 = load %TrollValue, ptr %b, align 8
  %27 = extractvalue %TrollValue %b6, 0
  %28 = extractvalue %TrollValue %b6, 1
  %29 = extractvalue %TrollValue %b6, 2
  call void @troll_print_value(i32 %27, double %28, ptr %29)
  ret i32 0
}
