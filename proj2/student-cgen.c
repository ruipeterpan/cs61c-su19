#include "additional-instructions.h"
#include "instructions.h"
#include "student-cgen.h"
#include "cgen.h"
#include "cgen-helpers.h"
#include "decorate-builtins.h"
#include "utils.h"

/*
 *  Code used to generate the negate expression. This is complete and is
 *  included to provide you an example of code generation.
 *
 *  Note how it calls dispatch on its child to recurse into generate code
 *  that needs to be negated. Then it cause apply its operation to the value
 *  of register S1 because we assume our most recent output is always in S1.
 *  Finally it places it own result in S1.
 */
void ProcessExprPrefixNegate(DAST* dast,
                             char* startLabel,
                             char* endLabel, int regBytes) {
  DAST* child1 = dast->children[0];

  /* cgen and load arguments */
  Dispatch(child1, startLabel, endLabel, regBytes);
  /* Negate by flipping all the bits and add 1. */
  EmitXORI(S1, S1, -1);
  EmitADDI(S1, S1, 1);

  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Helper function used to ease maintaining the our latest value in S1 invariant
 *  for binary expression. It returns an enum value which contains the value of
 *  the register used to hold a value the first of the two expresions,
 *  (the second is defined to be in s1).
 *
 *  You should use the enum values in instructions.h and be sure and restore the
 *  stack to its previous value upon returning from the function
 *
 *   Hint: When using lw/sw the argument order is Reg, imm, Reg and it matches
 *
 *   lw register immediate(register)
 */
enum reg SetupBinaryInvariant(DAST* dast, char* startLabel, char* endLabel, int regBytes) {
  DAST* child1 = dast->children[0]; // In firstReg/S2
  DAST* child2 = dast->children[1]; // In S1
  /* STUDENT CODE GOES HERE. */
  Dispatch(child1, startLabel, endLabel, regBytes);
  // Now S1 stores the value of the first expression
  // Prologue
  EmitADDI(SP, SP, -4);
  // Temporarily stores the value of the first expression on the stack
  EmitSW(S1, 0, SP);
  // Now S1 holds the value of the second expression
  Dispatch(child2, startLabel, endLabel, regBytes);
  // Epilogue
  // Store the value of the first expression into S2
  EmitLW(S2, 0, SP);
  // Move the stack pointer back
  EmitADDI(SP, SP, 4);
  /*
   *  Replace this return value with the register you load the first arg into.
   */
  return S2;
}

/*
 *  Function used to handle code generation for all & expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  No type checking should be necessary for this operator.
 */
void ProcessExprBinaryBitAnd(DAST* dast,
                             char* startLabel,
                             char* endLabel, int regBytes) {

  DAST* child1 = dast->children[0];
  DAST* child2 = dast->children[1];
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  EmitAND(S1, S1, firstReg);
  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Function used to handle code generation for all | expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  No type checking should be necessary for this operator.
 */
void ProcessExprBinaryBitOr(DAST* dast,
                            char* startLabel,
                            char* endLabel, int regBytes) {

  DAST* child1 = dast->children[0];
  DAST* child2 = dast->children[1];
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  EmitOR(S1, S1, firstReg);
  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Function used to handle code generation for all ^ expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  No type checking should be necessary for this operator.
 */
void ProcessExprBinaryBitXor(DAST* dast,
                             char* startLabel,
                             char* endLabel, int regBytes) {

  DAST* child1 = dast->children[0];
  DAST* child2 = dast->children[1];
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  EmitXOR(S1, S1, firstReg);
  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Function used to handle code generation for all && expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  An operand is considered to have a true value if it is nonzero and a false
 *  value if it is 0.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryLogicAnd(DAST* dast,
                               char* startLabel,
                               char* endLabel, int regBytes) {

  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* STUDENT CODE HERE. */
  char* notTrue = GenerateLocalLabel();
  char* quit = GenerateLocalLabel();
  EmitBEQ(S1, X0, notTrue);
  EmitBEQ(firstReg, X0, notTrue);
  EmitADDI(S1, X0, 1);
  EmitJAL(X0, quit);
  EmitLABEL(notTrue);
  EmitADDI(S1, X0, 0);
  EmitLABEL(quit);

  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Function used to handle code generation for all || expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  An operand is considered to have a true value if it is nonzero and a false
 *  value if it is 0.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryLogicOr(DAST* dast,
                              char* startLabel,
                              char* endLabel, int regBytes) {

  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* STUDENT CODE HERE. */
  char* isTrue = GenerateLocalLabel();
  char* quit = GenerateLocalLabel();
  EmitBNE(S1, X0, isTrue);
  EmitBNE(firstReg, X0, isTrue);
  EmitADDI(S1, X0, 0);
  EmitJAL(X0, quit);
  EmitLABEL(isTrue);
  EmitADDI(S1, X0, 1);
  EmitLABEL(quit);

  free(isTrue);
  free(quit);
}

/*
 *  Function used to handle code generation for all == expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  For equality you should not need to consider the types of the operands at all
 *  and simply need to determine if the results are equal.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryEq(DAST* dast,
                         char* startLabel,
                         char* endLabel, int regBytes) {

  DAST* child1 = dast->children[0];
  DAST* child2 = dast->children[1];
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  char* equal = GenerateLocalLabel();
  char* quit = GenerateLocalLabel();
  EmitBEQ(S1, firstReg, equal);
  EmitADDI(S1, X0, 0);
  EmitJAL(X0, quit);
  EmitLABEL(equal);
  EmitADDI(S1, X0, 1);
  EmitLABEL(quit);

  free(equal);
  free(quit);
}

/*
 *  Function used to handle code generation for all != expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  For equality you should not need to consider the types of the operands at all
 *  and simply need to determine if the results are equal.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryNotEq(DAST* dast,
                            char* startLabel,
                            char* endLabel, int regBytes) {

  DAST* child1 = dast->children[0];
  DAST* child2 = dast->children[1];
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  char* equal = GenerateLocalLabel();
  char* quit = GenerateLocalLabel();
  EmitBEQ(S1, firstReg, equal);
  EmitADDI(S1, X0, 1);
  EmitJAL(X0, quit);
  EmitLABEL(equal);
  EmitADDI(S1, X0, 0);
  EmitLABEL(quit);

  free(equal);
  free(quit);
}

/*
 *  Function used to handle code generation for all add expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  Add needs to handle both normal arithmetic and pointer arithmetic. As a
 *  refresher in C adding to a pointer increments the address by one
 *  element in size. For example if we have
 *
 *  x + 1
 *
 *  where x is an int *, then x + 1 produces x + sizeof (int) * 1 in assembly.
 *
 *  To handle this case you should use c1PtrTotal and c2PtrTotal to determine
 *  if either child is a pointer (only 1 will ever be a pointer and you do not
 *  need to check this condition). Then you will need to determine the sizeof
 *  amount. Our compiler organizes types as having a typeDecl, which contains
 *  a base type (like int), and a pointer counter, which roughly indicates how
 *  many *s there are. To find the size of the base type, you should look at
 *  the child1/2->typeDecl->dataSize.
 */
void ProcessExprBinaryAdd(DAST* dast,
                          char* startLabel,
                          char* endLabel, int regBytes) {

  DAST* child1   = dast->children[0];
  int c1PtrTotal = child1->typeDeclArrCount + child1->typeDeclPtrCount;
  DAST* child2   = dast->children[1];
  int c2PtrTotal = child2->typeDeclArrCount + child2->typeDeclPtrCount;
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  if (c1PtrTotal) { // Pointer arithmetic case 1
    // x + 3
    int size = child1->typeDecl->dataSize;
    EmitADDI(T0, X0, size);
    EmitMUL(S1, S1, T0);
    // EmitSLLI(S1, S1, size); // probably shouldn't be using slli
    EmitADD(S1, firstReg, S1);
  } else if (c2PtrTotal) { // Pointer arithmetic case 2
    // 1 + x
    int size = child2->typeDecl->dataSize;
    EmitADDI(T0, X0, size);
    EmitMUL(firstReg, firstReg, T0);
    // EmitSLLI(firstReg, firstReg, size);
    EmitADD(S1, firstReg, S1);
  } else { // No pointer; just plain arithmetic
    EmitADD(S1, firstReg, S1);
  }
  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Function used to handle code generation for all sub expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  Sub needs to handle both normal subtraction, pointer arithmetic and
 *  subtracting one pointer from another. As a refresher in C adding to a pointer
 *  decrements the address by one element in size. For example if we have
 *
 *  x - 1
 *
 *  where x is an int *, then x - 1 produces x - sizeof (int) * 1 in assembly.
 *
 *  To handle this case you should use c1PtrTotal and c2PtrTotal to determine
 *  if either child is a pointer. Then you will need to determine the sizeof
 *  amount. Our compiler organizes types as having a typeDecl, which contains
 *  a base type (like int), and a pointer counter, which roughly indicates how
 *  many *s there are. To find the size of the base type, you should look at
 *  the child1->typeDecl->dataSize.
 *
 *  In addition you also need to consider the case where two pointers are
 *  being subtracted from each other. For example:
 *
 *  (x + 2) - x = 2
 *
 *  regardless of the actual distance between the addresses in x and x + 2.
 *  Once again you will need the pointer count and/or the dataSize to complete
 *  this step.
 */
void ProcessExprBinarySub(DAST* dast,
                          char* startLabel,
                          char* endLabel, int regBytes) { // child1 - child2

  DAST* child1   = dast->children[0];
  int c1PtrTotal = child1->typeDeclArrCount + child1->typeDeclPtrCount;
  DAST* child2   = dast->children[1];
  int c2PtrTotal = child2->typeDeclArrCount + child2->typeDeclPtrCount;
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  if (c1PtrTotal && !c2PtrTotal) { // Pointer arithmetic case 1, like x - 1
    // x - 1
    int size = -1 * child1->typeDecl->dataSize;
    EmitADDI(T0, X0, size);
    EmitMUL(S1, S1, T0);
    // EmitSLLI(S1, S1, size);
    EmitADD(S1, firstReg, S1);
  } else if (c2PtrTotal && !c1PtrTotal) { // Pointer arithmetic case 2. But 1 - x doesn't make sense
    // s2 is a pointer, s1 is not: makes no sense
  } else if (c1PtrTotal && c2PtrTotal) { // both are pointers
    // TODO: Potential erroneous code
    int size = child1->typeDecl->dataSize;
    EmitADDI(T0, X0, size);
    EmitMUL(S1, S1, T0);
    EmitMUL(firstReg, firstReg, T0);
    // EmitSLLI(S1, S1, size);
    // EmitSLLI(firstReg, firstReg, size);
    EmitADD(S1, firstReg, S1);
    EmitDIV(S1, S1, T0);
    // int size2 = -1 * child2->typeDecl->dataSize;
    // EmitADDI(S1, S1*size1, firstReg*size2);
    // EmitSUB(S1, firstReg/size, S1/size);
  } else { // No pointer; just plain arithmetic
    EmitSUB(S1, firstReg, S1);
  }
  /* Make sure we squash any overflow for future operations. */
  ApplyTypeRules (dast, S1);
}

/*
 *  Function used to handle code generation for all >= expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  For comparison operations the type of the operator is important for selecting
 *  the correct comparison instruction. In particular we are concerned with two
 *  types of comparisons:
 * signed
 * unsigned
 *
 *  To determine when to select which type of comparison, we have two conditions.
 *  First if we are working with pointers, both pointers must be the same type
 *  and c1PtrTotal/c2PtrTotal will be > 0. All pointer comparisons are unsigned.
 *  The other possibility is that we are working with an integer type. There are
 *  5 integer types and they can be determined by comparing child1/2->typedecl
 *  directly with some global variables (use ==). These types are:
 *
 *   1. unsigned int (global variable: uintType), unsigned
 *   2. int (global variable: intType), signed
 *   3. unsigned char (global variable: ucharType), unsigned
 *   4. char (global variable: charType), signed
 *   5. NULL (global variable: nullType), signed/unsigned (doesn't matter)
 *
 *  For our comparisons we select the signedness (signed or unsigned) of the
 *  highest priority operand (unsigned int is highest, NULL is lowest). So for
 *  example if we had:
 *
 *  x >= y
 *
 *  where x is a char and y is an unsigned int, then we would do an unsigned
 *  comparison because unsigned int is higher priority than char. Do not worry
 *  about making the sizes of the operands the same, this is handled for you by
 *  the starter code.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryGTEq(DAST* dast,
                           char* startLabel,
                           char* endLabel, int regBytes) {

  DAST* child1   = dast->children[0];
  int c1PtrTotal = child1->typeDeclArrCount + child1->typeDeclPtrCount;
  DAST* child2   = dast->children[1];
  int c2PtrTotal = child2->typeDeclArrCount + child2->typeDeclPtrCount;
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  if (c1PtrTotal && c2PtrTotal) { // pointer comparison, unsigned integer comparison
    if (child1->typeDecl != child2->typeDecl || c1PtrTotal < 0 || c2PtrTotal < 0)
      // Check if the pointers are of the same type and if they are both positive
      return;
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == uintType || child2->typeDecl == uintType) {
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == intType || child2->typeDecl == intType) {
    EmitSLT(S1, firstReg, S1);
  } else if (child1->typeDecl == ucharType || child2->typeDecl == ucharType) {
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == charType || child2->typeDecl == charType) {
    EmitSLT(S1, firstReg, S1);
  } else if (child1->typeDecl == nullType || child2->typeDecl == nullType) {
    EmitSLT(S1, firstReg, S1);
  }
  EmitXORI(S1, S1, 1);
}

/*
 *  Function used to handle code generation for all > expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  For comparison operations the type of the operator is important for selecting
 *  the correct comparison instruction. In particular we are concerned with two
 *  types of comparisons:
 * signed
 * unsigned
 *
 *  To determine when to select which type of comparison, we have two conditions.
 *  First if we are working with pointers, both pointers must be the same type
 *  and c1PtrTotal/c2PtrTotal will be > 0. All pointer comparisons are unsigned.
 *  The other possibility is that we are working with an integer type. There are
 *  5 integer types and they can be determined by comparing child1/2->typedecl
 *  directly with some global variables (use ==). These types are:
 *
 *   1. unsigned int (global variable: uintType), unsigned
 *   2. int (global variable: intType), signed
 *   3. unsigned char (global variable: ucharType), unsigned
 *   4. char (global variable: charType), signed
 *   5. NULL (global variable: nullType), signed/unsigned (doesn't matter)
 *
 *  For our comparisons we select the signedness (signed or unsigned) of the
 *  highest priority operand (unsigned int is highest, NULL is lowest). So for
 *  example if we had:
 *
 *  x > y
 *
 *  where x is a char and y is an unsigned int, then we would do an unsigned
 *  comparison because unsigned int is higher priority than char. Do not worry
 *  about making the sizes of the operands the same, this is handled for you by
 *  the starter code.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryGT(DAST* dast,
                         char* startLabel,
                         char* endLabel, int regBytes) {

  DAST* child1   = dast->children[0];
  int c1PtrTotal = child1->typeDeclArrCount + child1->typeDeclPtrCount;
  DAST* child2   = dast->children[1];
  int c2PtrTotal = child2->typeDeclArrCount + child2->typeDeclPtrCount;
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  char* equal = GenerateLocalLabel();
  char* quit = GenerateLocalLabel();

  EmitBEQ(S1, firstReg, equal);

  if (c1PtrTotal && c2PtrTotal) { // pointer comparison, unsigned
    if (child1->typeDecl != child2->typeDecl || c1PtrTotal <= 0 || c2PtrTotal <= 0)
      // Check if the pointers are of the same type and if they are both positive
      return;
    EmitSLTU(S1, S1, firstReg);
  } else if (child1->typeDecl == uintType || child2->typeDecl == uintType) {
    EmitSLTU(S1, S1, firstReg);
  } else if (child1->typeDecl == intType || child2->typeDecl == intType) {
    EmitSLT(S1, S1, firstReg);
  } else if (child1->typeDecl == ucharType || child2->typeDecl == ucharType) {
    EmitSLTU(S1, S1, firstReg);
  } else if (child1->typeDecl == charType || child2->typeDecl == charType) {
    EmitSLT(S1, S1, firstReg);
  } else if (child1->typeDecl == nullType || child2->typeDecl == nullType) {
    EmitSLT(S1, S1, firstReg);
  }

  EmitJAL(X0, quit);
  EmitLABEL(equal);
  EmitADDI(S1, X0, 0);
  EmitLABEL(quit);

  free(equal);
  free(quit);
}

/*
 *  Function used to handle code generation for all <= expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  For comparison operations the type of the operator is important for selecting
 *  the correct comparison instruction. In particular we are concerned with two
 *  types of comparisons:
 * signed
 * unsigned
 *
 *  To determine when to select which type of comparison, we have two conditions.
 *  First if we are working with pointers, both pointers must be the same type
 *  and c1PtrTotal/c2PtrTotal will be > 0. All pointer comparisons are unsigned.
 *  The other possibility is that we are working with an integer type. There are
 *  5 integer types and they can be determined by comparing child1/2->typedecl
 *  directly with some global variables (use ==). These types are:
 *
 *   1. unsigned int (global variable: uintType), unsigned
 *   2. int (global variable: intType), signed
 *   3. unsigned char (global variable: ucharType), unsigned
 *   4. char (global variable: charType), signed
 *   5. NULL (global variable: nullType), signed/unsigned (doesn't matter)
 *
 *  For our comparisons we select the signedness (signed or unsigned) of the
 *  highest priority operand (unsigned int is highest, NULL is lowest). So for
 *  example if we had:
 *
 *  x <= y
 *
 *  where x is a char and y is an unsigned int, then we would do an unsigned
 *  comparison because unsigned int is higher priority than char. Do not worry
 *  about making the sizes of the operands the same, this is handled for you by
 *  the starter code.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryLTEq(DAST* dast,
                           char* startLabel,
                           char* endLabel, int regBytes) {

  DAST* child1   = dast->children[0];
  int c1PtrTotal = child1->typeDeclArrCount + child1->typeDeclPtrCount;
  DAST* child2   = dast->children[1];
  int c2PtrTotal = child2->typeDeclArrCount + child2->typeDeclPtrCount;
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */

  char* equal = GenerateLocalLabel();
  char* quit = GenerateLocalLabel();
  EmitBEQ(S1, firstReg, equal);

  if (c1PtrTotal && c2PtrTotal) { // pointer comparison, unsigned
    if (child1->typeDecl != child2->typeDecl || c1PtrTotal <= 0 || c2PtrTotal <= 0)
      // Check if the pointers are of the same type and if they are both positive
      return;
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == uintType || child2->typeDecl == uintType) {
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == intType || child2->typeDecl == intType) {
    EmitSLT(S1, firstReg, S1);
  } else if (child1->typeDecl == ucharType || child2->typeDecl == ucharType) {
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == charType || child2->typeDecl == charType) {
    EmitSLT(S1, firstReg, S1);
  } else if (child1->typeDecl == nullType || child2->typeDecl == nullType) {
    EmitSLT(S1, firstReg, S1);
  }

  EmitJAL(X0, quit);
  EmitLABEL(equal);
  EmitADDI(S1, X0, 1);
  EmitLABEL(quit);

  free(equal);
  free(quit);
}

/*
 *  Function used to handle code generation for all < expressions. As with all
 *  binary expressions it relies on the binary invariant completed in part 1.
 *
 *  For comparison operations the type of the operator is important for selecting
 *  the correct comparison instruction. In particular we are concerned with two
 *  types of comparisons:
 * signed
 * unsigned
 *
 *  To determine when to select which type of comparison, we have two conditions.
 *  First if we are working with pointers, both pointers must be the same type
 *  and c1PtrTotal/c2PtrTotal will be > 0. All pointer comparisons are unsigned.
 *  The other possibility is that we are working with an integer type. There are
 *  5 integer types and they can be determined by comparing child1/2->typedecl
 *  directly with some global variables (use ==). These types are:
 *
 *   1. unsigned int (global variable: uintType), unsigned
 *   2. int (global variable: intType), signed
 *   3. unsigned char (global variable: ucharType), unsigned
 *   4. char (global variable: charType), signed
 *   5. NULL (global variable: nullType), signed/unsigned (doesn't matter)
 *
 *  For our comparisons we select the signedness (signed or unsigned) of the
 *  highest priority operand (unsigned int is highest, NULL is lowest). So for
 *  example if we had:
 *
 *  x < y
 *
 *  where x is a char and y is an unsigned int, then we would do an unsigned
 *  comparison because unsigned int is higher priority than char. Do not worry
 *  about making the sizes of the operands the same, this is handled for you by
 *  the starter code.
 *
 *  If the condition is met then you should produce 1. If the condition is not
 *  met not then you should produce 0. Failure to return either of these two
 *  values exactly may result in a loss of credit, regardless of the truthiness
 *  of the values returned being correct.
 */
void ProcessExprBinaryLT(DAST* dast,
                         char* startLabel,
                         char* endLabel, int regBytes) {

  DAST* child1   = dast->children[0];
  int c1PtrTotal = child1->typeDeclArrCount + child1->typeDeclPtrCount;
  DAST* child2   = dast->children[1];
  int c2PtrTotal = child2->typeDeclArrCount + child2->typeDeclPtrCount;
  /* First apply our invariant. */
  enum reg firstReg = SetupBinaryInvariant (dast, startLabel, endLabel, regBytes);
  /* Make sure type sizes match. */
  MatchSizes (child1, firstReg, child2, S1);
  /* STUDENT CODE HERE. */
  
  if (c1PtrTotal && c2PtrTotal) { // pointer comparison, unsigned
    if (child1->typeDecl != child2->typeDecl || c1PtrTotal <= 0 || c2PtrTotal <= 0)
      // Check if the pointers are of the same type and if they are both positive
      return;
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == uintType || child2->typeDecl == uintType) {
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == intType || child2->typeDecl == intType) {
    EmitSLT(S1, firstReg, S1);
  } else if (child1->typeDecl == ucharType || child2->typeDecl == ucharType) {
    EmitSLTU(S1, firstReg, S1);
  } else if (child1->typeDecl == charType || child2->typeDecl == charType) {
    EmitSLT(S1, firstReg, S1);
  } else if (child1->typeDecl == nullType || child2->typeDecl == nullType) {
    EmitSLT(S1, firstReg, S1);
  }
}

/*
 *  Function used to generate call expressions. It decrements the stack to
 *  allocate space for the arguments, places the arguments on the stack and
 *  then switches control to the function it calls. On return it moves the
 *  output to our S1, our invariant register and restores the stack to its
 *  previous state.
 *
 *  To determine where to place arugments you should use the values in offsets
 *  and to determine how large of a value to store (either word or byte) you
 *  should use the results in sizes.
 */
void ProcessExprCall(DAST* dast, char* startLabel, char* endLabel, int regBytes) { // This somehow works
  DAST* funcID    = dast->children[0];
  DAST* params    = dast->children[1];
  char* funcLabel = funcID->data.identifier;
  int decr        = 0;
  /* move stack for all children, if they exist */
  if (params->size > 0) {

    /* First allocate memory to compute locations. */
    size_t* offsets = malloc (sizeof(size_t) * params->size);
    if (offsets == NULL) {
      AllocationFailed ();
    }
    size_t* sizes = malloc (sizeof(size_t) * params->size);
    if (sizes == NULL) {
      AllocationFailed ();
    }

    decr = ComputeArgLocations (funcID->varDecl, offsets, sizes);
    /* Decrement the stack. */
    EmitADDI(SP, SP, -1 * decr);
    for (int i = 0; i < params->size; i++) {
      /* STUDENT CODE HERE */
      // Store each argument on the stack.
      // The location is given by the offsets array and the size by the sizes array.
      // Note the size determines which store instruction you use: either word or byte.
      // call dispatch on all the children

      Dispatch(params->children[i], startLabel, endLabel, regBytes);

      if (sizes[i] == 4) { // word
        EmitSW(S1, offsets[i], SP);
      } else if (sizes[i] == 1) { // byte
        EmitSB(S1, offsets[i], SP);
      }
    }
    /* Free the memory. */
    free (offsets);
    free (sizes);
  }

  /* STUDENT CODE HERE */
  // Switch control to the function identified by funcLabel.
  EmitJAL(RA, funcLabel);
  // Move the return value from a0 to S1 to uphold our invariant.
  EmitMV(S1, A0);
  /* Restore the stack */
  if (params->size > 0) {
    EmitADDI(SP, SP, decr);
  }
}

/*
 *  Helper function for decrementing the stack and storing any registers
 *  that need to be saved. NOTE that you DO NOT know what code will be
 *  Generated after the function declaration and in fact we have hid some
 *  of the code from you.
 *
 *  As a result, you should save every register that calling convention
 *  dictates may need to be restored (look at lecture slides if you
 *  are not sure which registers may need to be restored).
 *
 *  The starter code has already saved the Frame Pointer (S0/FP), so you
 *  do NOT need to save that register.
 *
 *  This function returns the number of bytes the stack was decremented
 *  by. It should always return a non-negative value.
 */
size_t SaveRegisters() { // These two works
  /* STUDENT CODE HERE */
  /* Replace me with number of bytes the SP moves. */
  // s0-s11, sp?
  EmitADDI(SP, SP, -48);

  EmitSW(S1, 0, SP);
  EmitSW(S2, 4, SP);
  EmitSW(S3, 8, SP);
  EmitSW(S4, 12, SP);
  EmitSW(S5, 16, SP);
  EmitSW(S6, 20, SP);
  EmitSW(S7, 24, SP);
  EmitSW(S8, 28, SP);
  EmitSW(S9, 32, SP);
  EmitSW(S10, 36, SP);
  EmitSW(S11, 40, SP);
  EmitSW(RA, 44, SP);

  return 48;
}

/*
 *  Helper function for restoring all saved registers. You may assume that the
 *  stack pointer is currently located at the value it was set to when modified
 *  in SaveRegisters ().
 */
void RestoreRegisters() {
  /* STUDENT CODE HERE */
  EmitLW(RA, 44, SP);
  EmitLW(S11, 40, SP);
  EmitLW(S10, 36, SP);
  EmitLW(S9, 32, SP);
  EmitLW(S8, 28, SP);
  EmitLW(S7, 24, SP);
  EmitLW(S6, 20, SP);
  EmitLW(S5, 16, SP);
  EmitLW(S4, 12, SP);
  EmitLW(S3, 8, SP);
  EmitLW(S2, 4, SP);
  EmitLW(S1, 0, SP);

  EmitADDI(SP, SP, 48);
}

/*
 *  Function used for generating code for all if/else expressions. This code
 *  produces labels that should be used to navigate which code block to execute
 *  as a result of the condition provided. None of these labels should change
 *  the start or end label because those values are used for the starts and
 *  ends of loops.
 */
void ProcessIfElse(DAST* dast, char* startLabel, char* endLabel, int regBytes) { // This works
  char* condEnd   = GenerateLocalLabel();  // same as ifStart
  char* elseStart = GenerateLocalLabel();
  DAST* condition = dast->children[0];
  DAST* ifBody    = dast->children[1];
  DAST* elseBody  = NULL;
  if (dast->size == 3) {
    elseBody = dast->children[2];
  }
  /* STUDENT CODE HERE. */
  if (elseBody == NULL) { // only an if statement
    Dispatch(condition, startLabel, endLabel, regBytes); // S1 now stores if condition evaluates to true. 1 is true, 0 is false
    EmitBEQ(S1, X0, elseStart); // if condition is not true
    EmitLABEL(condEnd);
    Dispatch(ifBody, startLabel, endLabel, regBytes); 
    EmitLABEL(elseStart);
  } else { // both if and else statement
    char* quit = GenerateLocalLabel();
    Dispatch(condition, startLabel, endLabel, regBytes); // S1 now stores if condition evaluates to true. 1 is true, 0 is false
    EmitBEQ(S1, X0, elseStart);
    EmitLABEL(condEnd);
    Dispatch(ifBody, startLabel, endLabel, regBytes);
    EmitJAL(X0, quit);
    EmitLABEL(elseStart);
    Dispatch(elseBody, startLabel, endLabel, regBytes);
    EmitLABEL(quit);
  }
  /* Free the labels used. */
  free (condEnd);
  free (elseStart);
}

/*
 *  Function used for generating code for all while expressions. This code
 *  produces labels that should be used to navigate which code block to execute
 *  as a result of the condition provided return to the condition when the end
 *  of the loop is reached. In any recursive calls you should replace startLabel
 *  with the label used at the start of the loop and endWhile with the label
 *  used at the end of the loop.
 */
void ProcessWhile(DAST* dast, char* startLabel, char* endLabel, int regBytes) { // Buggy, dunno where the bug is
  char* startWhile = GenerateLocalLabel();
  char* endWhile   = GenerateLocalLabel();
  DAST* condition  = dast->children[0];
  DAST* whileBody  = dast->children[1];

  /* STUDENT CODE HERE. */
  EmitLABEL(startWhile);
  Dispatch(condition, startLabel, endLabel, regBytes); // S1 now stores if condition evaluates to true. 1 is true, 0 is false
  EmitBEQ(S1, X0, endWhile); // if condition is not true
  Dispatch(whileBody, startWhile, endWhile, regBytes);
  EmitJAL(X0, startWhile);
  EmitLABEL(endWhile);

  /* Free the labels used. */
  free (startWhile);
  free (endWhile);
}

