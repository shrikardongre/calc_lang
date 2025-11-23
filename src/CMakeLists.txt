add_executable (calc
  calc.cpp
  codegen.cpp
  Lexer.cpp
  Parser.cpp
  sema.cpp
  )
target_link_libraries(calc PRIVATE ${llvm_libs})