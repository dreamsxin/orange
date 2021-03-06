/*
 ** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
 ** directory of this distribution.
 **
 ** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
 ** may not be copied, modified, or distributed except according to those terms.
 */

#include <test/TestLib.h>
#include <test/Comparisons.h>

#include <llvm/IR/Instruction.h>

#include <grove/types/Type.h>
#include <grove/types/ArrayType.h>
#include <grove/types/BoolType.h>
#include <grove/types/DoubleType.h>
#include <grove/types/FloatType.h>
#include <grove/types/EnumType.h>
#include <grove/types/FunctionType.h>
#include <grove/types/IntType.h>
#include <grove/types/LocalNamedType.h>
#include <grove/types/NamedType.h>
#include <grove/types/PointerType.h>
#include <grove/types/UIntType.h>
#include <grove/types/VoidType.h>

#include <grove/Comparison.h>

#define CAST_TEST(name, from, to, expect) \
ADD_TEST(name, #name);\
int name(){\
auto f = from; auto t = to;\
return cmpEq(f->castOperation(t), expect);}

#define PREC_TEST(name, source, target, expect) \
ADD_TEST(name, #name);\
int name(){ return cmpEq(Type::compare(source, target), expect); }

START_TEST_MODULE();

#define C(castType) llvm::Instruction::CastOps::castType

//////
// Cast tests
//////

// Int to Int
CAST_TEST(TestInt8ToInt8,   IntType::get(8),  IntType::get(8),  NO_CAST);
CAST_TEST(TestInt8ToInt16,  IntType::get(8),  IntType::get(16), C(SExt));
CAST_TEST(TestInt8ToInt32,  IntType::get(8),  IntType::get(32), C(SExt));
CAST_TEST(TestInt8ToInt64,  IntType::get(8),  IntType::get(64), C(SExt));
CAST_TEST(TestInt16ToInt8,  IntType::get(16), IntType::get(8),  C(Trunc));
CAST_TEST(TestInt16ToInt16, IntType::get(16), IntType::get(16), NO_CAST);
CAST_TEST(TestInt16ToInt32, IntType::get(16), IntType::get(32), C(SExt));
CAST_TEST(TestInt16ToInt64, IntType::get(16), IntType::get(64), C(SExt));
CAST_TEST(TestInt32ToInt8,  IntType::get(32), IntType::get(8),  C(Trunc));
CAST_TEST(TestInt32ToInt16, IntType::get(32), IntType::get(16), C(Trunc));
CAST_TEST(TestInt32ToInt32, IntType::get(32), IntType::get(32), NO_CAST);
CAST_TEST(TestInt32ToInt64, IntType::get(32), IntType::get(64), C(SExt));
CAST_TEST(TestInt64ToInt8,  IntType::get(64), IntType::get(8),  C(Trunc));
CAST_TEST(TestInt64ToInt16, IntType::get(64), IntType::get(16), C(Trunc));
CAST_TEST(TestInt64ToInt32, IntType::get(64), IntType::get(32), C(Trunc));
CAST_TEST(TestInt64ToInt64, IntType::get(64), IntType::get(64), NO_CAST);

// UInt to UInt
CAST_TEST(TestUInt8ToUInt8,   UIntType::get(8),  UIntType::get(8),  NO_CAST);
CAST_TEST(TestUInt8ToUInt16,  UIntType::get(8),  UIntType::get(16), C(ZExt));
CAST_TEST(TestUInt8ToUInt32,  UIntType::get(8),  UIntType::get(32), C(ZExt));
CAST_TEST(TestUInt8ToUInt64,  UIntType::get(8),  UIntType::get(64), C(ZExt));
CAST_TEST(TestUInt16ToUInt8,  UIntType::get(16), UIntType::get(8),  C(Trunc));
CAST_TEST(TestUInt16ToUInt16, UIntType::get(16), UIntType::get(16), NO_CAST);
CAST_TEST(TestUInt16ToUInt32, UIntType::get(16), UIntType::get(32), C(ZExt));
CAST_TEST(TestUInt16ToUInt64, UIntType::get(16), UIntType::get(64), C(ZExt));
CAST_TEST(TestUInt32ToUInt8,  UIntType::get(32), UIntType::get(8),  C(Trunc));
CAST_TEST(TestUInt32ToUInt16, UIntType::get(32), UIntType::get(16), C(Trunc));
CAST_TEST(TestUInt32ToUInt32, UIntType::get(32), UIntType::get(32), NO_CAST);
CAST_TEST(TestUInt32ToUInt64, UIntType::get(32), UIntType::get(64), C(ZExt));
CAST_TEST(TestUInt64ToUInt8,  UIntType::get(64), UIntType::get(8),  C(Trunc));
CAST_TEST(TestUInt64ToUInt16, UIntType::get(64), UIntType::get(16), C(Trunc));
CAST_TEST(TestUInt64ToUInt32, UIntType::get(64), UIntType::get(32), C(Trunc));
CAST_TEST(TestUInt64ToUInt64, UIntType::get(64), UIntType::get(64), NO_CAST);

// Int to UInt. Should SExt to keep negative bit.
CAST_TEST(TestInt8ToUInt8,   IntType::get(8),  UIntType::get(8),  NO_CAST);
CAST_TEST(TestInt8ToUInt16,  IntType::get(8),  UIntType::get(16), C(SExt));
CAST_TEST(TestInt8ToUInt32,  IntType::get(8),  UIntType::get(32), C(SExt));
CAST_TEST(TestInt8ToUInt64,  IntType::get(8),  UIntType::get(64), C(SExt));
CAST_TEST(TestInt16ToUInt8,  IntType::get(16), UIntType::get(8),  C(Trunc));
CAST_TEST(TestInt16ToUInt16, IntType::get(16), UIntType::get(16), NO_CAST);
CAST_TEST(TestInt16ToUInt32, IntType::get(16), UIntType::get(32), C(SExt));
CAST_TEST(TestInt16ToUInt64, IntType::get(16), UIntType::get(64), C(SExt));
CAST_TEST(TestInt32ToUInt8,  IntType::get(32), UIntType::get(8),  C(Trunc));
CAST_TEST(TestInt32ToUInt16, IntType::get(32), UIntType::get(16), C(Trunc));
CAST_TEST(TestInt32ToUInt32, IntType::get(32), UIntType::get(32), NO_CAST);
CAST_TEST(TestInt32ToUInt64, IntType::get(32), UIntType::get(64), C(SExt));
CAST_TEST(TestInt64ToUInt8,  IntType::get(64), UIntType::get(8),  C(Trunc));
CAST_TEST(TestInt64ToUInt16, IntType::get(64), UIntType::get(16), C(Trunc));
CAST_TEST(TestInt64ToUInt32, IntType::get(64), UIntType::get(32), C(Trunc));
CAST_TEST(TestInt64ToUInt64, IntType::get(64), UIntType::get(64), NO_CAST);

// UInt to Int. Should ZExt.
CAST_TEST(TestUInt8ToInt8,   UIntType::get(8),  IntType::get(8),  NO_CAST);
CAST_TEST(TestUInt8ToInt16,  UIntType::get(8),  IntType::get(16), C(ZExt));
CAST_TEST(TestUInt8ToInt32,  UIntType::get(8),  IntType::get(32), C(ZExt));
CAST_TEST(TestUInt8ToInt64,  UIntType::get(8),  IntType::get(64), C(ZExt));
CAST_TEST(TestUInt16ToInt8,  UIntType::get(16), IntType::get(8),  C(Trunc));
CAST_TEST(TestUInt16ToInt16, UIntType::get(16), IntType::get(16), NO_CAST);
CAST_TEST(TestUInt16ToInt32, UIntType::get(16), IntType::get(32), C(ZExt));
CAST_TEST(TestUInt16ToInt64, UIntType::get(16), IntType::get(64), C(ZExt));
CAST_TEST(TestUInt32ToInt8,  UIntType::get(32), IntType::get(8),  C(Trunc));
CAST_TEST(TestUInt32ToInt16, UIntType::get(32), IntType::get(16), C(Trunc));
CAST_TEST(TestUInt32ToInt32, UIntType::get(32), IntType::get(32), NO_CAST);
CAST_TEST(TestUInt32ToInt64, UIntType::get(32), IntType::get(64), C(ZExt));
CAST_TEST(TestUInt64ToInt8,  UIntType::get(64), IntType::get(8),  C(Trunc));
CAST_TEST(TestUInt64ToInt16, UIntType::get(64), IntType::get(16), C(Trunc));
CAST_TEST(TestUInt64ToInt32, UIntType::get(64), IntType::get(32), C(Trunc));
CAST_TEST(TestUInt64ToInt64, UIntType::get(64), IntType::get(64), NO_CAST);

// Int to Float.
CAST_TEST(TestInt8ToFloat,  IntType::get(8),  FloatType::get(), C(SIToFP));
CAST_TEST(TestInt16ToFloat, IntType::get(16), FloatType::get(), C(SIToFP));
CAST_TEST(TestInt32ToFloat, IntType::get(32), FloatType::get(), C(SIToFP));
CAST_TEST(TestInt64ToFloat, IntType::get(64), FloatType::get(), C(SIToFP));

// UInt to Float.
CAST_TEST(TestUInt8ToFloat,  UIntType::get(8),  FloatType::get(), C(UIToFP));
CAST_TEST(TestUInt16ToFloat, UIntType::get(16), FloatType::get(), C(UIToFP));
CAST_TEST(TestUInt32ToFloat, UIntType::get(32), FloatType::get(), C(UIToFP));
CAST_TEST(TestUInt64ToFloat, UIntType::get(64), FloatType::get(), C(UIToFP));

// Int to Double.
CAST_TEST(TestInt8ToDouble,  IntType::get(8),  DoubleType::get(), C(SIToFP));
CAST_TEST(TestInt16ToDouble, IntType::get(16), DoubleType::get(), C(SIToFP));
CAST_TEST(TestInt32ToDouble, IntType::get(32), DoubleType::get(), C(SIToFP));
CAST_TEST(TestInt64ToDouble, IntType::get(64), DoubleType::get(), C(SIToFP));

// UInt to Double.
CAST_TEST(TestUInt8ToDouble,  UIntType::get(8),  DoubleType::get(), C(UIToFP));
CAST_TEST(TestUInt16ToDouble, UIntType::get(16), DoubleType::get(), C(UIToFP));
CAST_TEST(TestUInt32ToDouble, UIntType::get(32), DoubleType::get(), C(UIToFP));
CAST_TEST(TestUInt64ToDouble, UIntType::get(64), DoubleType::get(), C(UIToFP));

// Int to Pointer.
CAST_TEST(TestInt8ToPointer,  IntType::get(8),  PointerType::get(BoolType::get()), C(IntToPtr));
CAST_TEST(TestInt16ToPointer, IntType::get(16), PointerType::get(BoolType::get()), C(IntToPtr));
CAST_TEST(TestInt32ToPointer, IntType::get(32), PointerType::get(BoolType::get()), C(IntToPtr));
CAST_TEST(TestInt64ToPointer, IntType::get(64), PointerType::get(BoolType::get()), C(IntToPtr));

// UInt to Pointer.
CAST_TEST(TestUInt8ToPointer,  UIntType::get(8),  PointerType::get(BoolType::get()), C(IntToPtr));
CAST_TEST(TestUInt16ToPointer, UIntType::get(16), PointerType::get(BoolType::get()), C(IntToPtr));
CAST_TEST(TestUInt32ToPointer, UIntType::get(32), PointerType::get(BoolType::get()), C(IntToPtr));
CAST_TEST(TestUInt64ToPointer, UIntType::get(64), PointerType::get(BoolType::get()), C(IntToPtr));

// Int to Bool
CAST_TEST(TestInt8ToBool,  IntType::get(8),  BoolType::get(), C(Trunc));
CAST_TEST(TestInt16ToBool, IntType::get(16), BoolType::get(), C(Trunc));
CAST_TEST(TestInt32ToBool, IntType::get(32), BoolType::get(), C(Trunc));
CAST_TEST(TestInt64ToBool, IntType::get(64), BoolType::get(), C(Trunc));

// UInt to Bool
CAST_TEST(TestUInt8ToBool,  UIntType::get(8),  BoolType::get(), C(Trunc));
CAST_TEST(TestUInt16ToBool, UIntType::get(16), BoolType::get(), C(Trunc));
CAST_TEST(TestUInt32ToBool, UIntType::get(32), BoolType::get(), C(Trunc));
CAST_TEST(TestUInt64ToBool, UIntType::get(64), BoolType::get(), C(Trunc));

// Bool to Int
CAST_TEST(TestBoolToInt8,  BoolType::get(), IntType::get(8),  C(ZExt));
CAST_TEST(TestBoolToInt16, BoolType::get(), IntType::get(16), C(ZExt));
CAST_TEST(TestBoolToInt32, BoolType::get(), IntType::get(32), C(ZExt));
CAST_TEST(TestBoolToInt64, BoolType::get(), IntType::get(64), C(ZExt));

// Bool to UInt
CAST_TEST(TestBoolToUInt8,  BoolType::get(), UIntType::get(8),  C(ZExt));
CAST_TEST(TestBoolToUInt16, BoolType::get(), UIntType::get(16), C(ZExt));
CAST_TEST(TestBoolToUInt32, BoolType::get(), UIntType::get(32), C(ZExt));
CAST_TEST(TestBoolToUInt64, BoolType::get(), UIntType::get(64), C(ZExt));

// FP to FP.
CAST_TEST(TestFloatToFloat,   FloatType::get(),  FloatType::get(),  NO_CAST);
CAST_TEST(TestFloatToDouble,  FloatType::get(),  DoubleType::get(), C(FPExt));

CAST_TEST(TestDoubleToDouble, DoubleType::get(), DoubleType::get(), NO_CAST);
CAST_TEST(TestDoubleToFloat,  DoubleType::get(), FloatType::get(),  C(FPTrunc));

// Float to Int
CAST_TEST(TestFloatToInt8,  FloatType::get(), IntType::get(8),  C(FPToSI));
CAST_TEST(TestFloatToInt16, FloatType::get(), IntType::get(16), C(FPToSI));
CAST_TEST(TestFloatToInt32, FloatType::get(), IntType::get(32), C(FPToSI));
CAST_TEST(TestFloatToInt64, FloatType::get(), IntType::get(64), C(FPToSI));

// Float to UInt
CAST_TEST(TestFloatToUInt8,  FloatType::get(), UIntType::get(8),  C(FPToUI));
CAST_TEST(TestFloatToUInt16, FloatType::get(), UIntType::get(16), C(FPToUI));
CAST_TEST(TestFloatToUInt32, FloatType::get(), UIntType::get(32), C(FPToUI));
CAST_TEST(TestFloatToUInt64, FloatType::get(), UIntType::get(64), C(FPToUI));

// Double to Int.
CAST_TEST(TestDoubleToInt8,  DoubleType::get(), IntType::get(8),  C(FPToSI));
CAST_TEST(TestDoubleToInt16, DoubleType::get(), IntType::get(16), C(FPToSI));
CAST_TEST(TestDoubleToInt32, DoubleType::get(), IntType::get(32), C(FPToSI));
CAST_TEST(TestDoubleToInt64, DoubleType::get(), IntType::get(64), C(FPToSI));

// Double to UInt.
CAST_TEST(TestDoubleToUInt8,  DoubleType::get(), UIntType::get(8),  C(FPToUI));
CAST_TEST(TestDoubleToUInt16, DoubleType::get(), UIntType::get(16), C(FPToUI));
CAST_TEST(TestDoubleToUInt32, DoubleType::get(), UIntType::get(32), C(FPToUI));
CAST_TEST(TestDoubleToUInt64, DoubleType::get(), UIntType::get(64), C(FPToUI));

// Pointer to Int.
CAST_TEST(TestPointerToInt8,  PointerType::get(BoolType::get()), IntType::get(8),  C(PtrToInt));
CAST_TEST(TestPointerToInt16, PointerType::get(BoolType::get()), IntType::get(16), C(PtrToInt));
CAST_TEST(TestPointerToInt32, PointerType::get(BoolType::get()), IntType::get(32), C(PtrToInt));
CAST_TEST(TestPointerToInt64, PointerType::get(BoolType::get()), IntType::get(64), C(PtrToInt));

// Pointer to UInt.
CAST_TEST(TestPointerToUInt8,  PointerType::get(BoolType::get()), UIntType::get(8),  C(PtrToInt));
CAST_TEST(TestPointerToUInt16, PointerType::get(BoolType::get()), UIntType::get(16), C(PtrToInt));
CAST_TEST(TestPointerToUInt32, PointerType::get(BoolType::get()), UIntType::get(32), C(PtrToInt));
CAST_TEST(TestPointerToUInt64, PointerType::get(BoolType::get()), UIntType::get(64), C(PtrToInt));

// Pointer to Pointer.
CAST_TEST(TestBoolPointerToBoolPointer, PointerType::get(BoolType::get()), PointerType::get(BoolType::get()), NO_CAST);
CAST_TEST(TestBoolPointerToFloatPointer, PointerType::get(BoolType::get()), PointerType::get(FloatType::get()), C(BitCast));

// Enums should inherit casts
CAST_TEST(EnumIntToFloat, EnumType::get(IntType::get(64)), FloatType::get(), C(SIToFP));

//////
// Cast tests
//////

// Int to Int
PREC_TEST(CompInt8ToInt8,   IntType::get(8),  IntType::get(8),  EQUAL);
PREC_TEST(CompInt8ToInt16,  IntType::get(8),  IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompInt8ToInt32,  IntType::get(8),  IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompInt8ToInt64,  IntType::get(8),  IntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompInt16ToInt8,  IntType::get(16), IntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToInt16, IntType::get(16), IntType::get(16), EQUAL);
PREC_TEST(CompInt16ToInt32, IntType::get(16), IntType::get(32), LOWER_PRECEDENCE	);
PREC_TEST(CompInt16ToInt64, IntType::get(16), IntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompInt32ToInt8,  IntType::get(32), IntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToInt16, IntType::get(32), IntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToInt32, IntType::get(32), IntType::get(32), EQUAL);
PREC_TEST(CompInt32ToInt64, IntType::get(32), IntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompInt64ToInt8,  IntType::get(64), IntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToInt16, IntType::get(64), IntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToInt32, IntType::get(64), IntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToInt64, IntType::get(64), IntType::get(64), EQUAL);

// UInt to UInt
PREC_TEST(CompUInt8ToUInt8,   UIntType::get(8),  UIntType::get(8),  EQUAL);
PREC_TEST(CompUInt8ToUInt16,  UIntType::get(8),  UIntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompUInt8ToUInt32,  UIntType::get(8),  UIntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompUInt8ToUInt64,  UIntType::get(8),  UIntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToUInt8,  UIntType::get(16), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompUInt16ToUInt16, UIntType::get(16), UIntType::get(16), EQUAL);
PREC_TEST(CompUInt16ToUInt32, UIntType::get(16), UIntType::get(32), LOWER_PRECEDENCE	);
PREC_TEST(CompUInt16ToUInt64, UIntType::get(16), UIntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToUInt8,  UIntType::get(32), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompUInt32ToUInt16, UIntType::get(32), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt32ToUInt32, UIntType::get(32), UIntType::get(32), EQUAL);
PREC_TEST(CompUInt32ToUInt64, UIntType::get(32), UIntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToUInt8,  UIntType::get(64), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompUInt64ToUInt16, UIntType::get(64), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt64ToUInt32, UIntType::get(64), UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt64ToUInt64, UIntType::get(64), UIntType::get(64), EQUAL);

// Int to UInt. Should SExt to keep negative bit.
PREC_TEST(CompInt8ToUInt8,   IntType::get(8),  UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt8ToUInt16,  IntType::get(8),  UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompInt8ToUInt32,  IntType::get(8),  UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompInt8ToUInt64,  IntType::get(8),  UIntType::get(64), HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToUInt8,  IntType::get(16), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToUInt16, IntType::get(16), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToUInt32, IntType::get(16), UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToUInt64, IntType::get(16), UIntType::get(64), HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToUInt8,  IntType::get(32), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToUInt16, IntType::get(32), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToUInt32, IntType::get(32), UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToUInt64, IntType::get(32), UIntType::get(64), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToUInt8,  IntType::get(64), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToUInt16, IntType::get(64), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToUInt32, IntType::get(64), UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToUInt64, IntType::get(64), UIntType::get(64), HIGHER_PRECEDENCE);

// UInt to Int. Should ZExt.
PREC_TEST(CompUInt8ToInt8,   UIntType::get(8),  IntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompUInt8ToInt16,  UIntType::get(8),  IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompUInt8ToInt32,  UIntType::get(8),  IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompUInt8ToInt64,  UIntType::get(8),  IntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToInt8,  UIntType::get(16), IntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToInt16, UIntType::get(16), IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToInt32, UIntType::get(16), IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToInt64, UIntType::get(16), IntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToInt8,  UIntType::get(32), IntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToInt16, UIntType::get(32), IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToInt32, UIntType::get(32), IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToInt64, UIntType::get(32), IntType::get(64), LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToInt8,  UIntType::get(64), IntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToInt16, UIntType::get(64), IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToInt32, UIntType::get(64), IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToInt64, UIntType::get(64), IntType::get(64), LOWER_PRECEDENCE);

// Int to Float.
PREC_TEST(CompInt8ToFloat,  IntType::get(8),  FloatType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompInt16ToFloat, IntType::get(16), FloatType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompInt32ToFloat, IntType::get(32), FloatType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompInt64ToFloat, IntType::get(64), FloatType::get(), LOWER_PRECEDENCE);

// UInt to Float.
PREC_TEST(CompUInt8ToFloat,  UIntType::get(8),  FloatType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToFloat, UIntType::get(16), FloatType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToFloat, UIntType::get(32), FloatType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToFloat, UIntType::get(64), FloatType::get(), LOWER_PRECEDENCE);

// Int to Double.
PREC_TEST(CompInt8ToDouble,  IntType::get(8),  DoubleType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompInt16ToDouble, IntType::get(16), DoubleType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompInt32ToDouble, IntType::get(32), DoubleType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompInt64ToDouble, IntType::get(64), DoubleType::get(), LOWER_PRECEDENCE);

// UInt to Double.
PREC_TEST(CompUInt8ToDouble,  UIntType::get(8),  DoubleType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompUInt16ToDouble, UIntType::get(16), DoubleType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompUInt32ToDouble, UIntType::get(32), DoubleType::get(), LOWER_PRECEDENCE);
PREC_TEST(CompUInt64ToDouble, UIntType::get(64), DoubleType::get(), LOWER_PRECEDENCE);

// Int to Pointer.
PREC_TEST(CompInt8ToPointer,  IntType::get(8),  PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToPointer, IntType::get(16), PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToPointer, IntType::get(32), PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToPointer, IntType::get(64), PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);

// UInt to Pointer.
PREC_TEST(CompUInt8ToPointer,  UIntType::get(8),  PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt16ToPointer, UIntType::get(16), PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt32ToPointer, UIntType::get(32), PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt64ToPointer, UIntType::get(64), PointerType::get(BoolType::get()), HIGHER_PRECEDENCE);

// Int to Bool
PREC_TEST(CompInt8ToBool,  IntType::get(8),  BoolType::get(), HIGHER_PRECEDENCE);
PREC_TEST(CompInt16ToBool, IntType::get(16), BoolType::get(), HIGHER_PRECEDENCE);
PREC_TEST(CompInt32ToBool, IntType::get(32), BoolType::get(), HIGHER_PRECEDENCE);
PREC_TEST(CompInt64ToBool, IntType::get(64), BoolType::get(), HIGHER_PRECEDENCE);

// UInt to Bool
PREC_TEST(CompUInt8ToBool,  UIntType::get(8),  BoolType::get(), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt16ToBool, UIntType::get(16), BoolType::get(), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt32ToBool, UIntType::get(32), BoolType::get(), HIGHER_PRECEDENCE);
PREC_TEST(CompUInt64ToBool, UIntType::get(64), BoolType::get(), HIGHER_PRECEDENCE);

// Bool to Int
PREC_TEST(CompBoolToInt8,  BoolType::get(), IntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompBoolToInt16, BoolType::get(), IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompBoolToInt32, BoolType::get(), IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompBoolToInt64, BoolType::get(), IntType::get(64), LOWER_PRECEDENCE);

// Bool to UInt
PREC_TEST(CompBoolToUInt8,  BoolType::get(), UIntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompBoolToUInt16, BoolType::get(), UIntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompBoolToUInt32, BoolType::get(), UIntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompBoolToUInt64, BoolType::get(), UIntType::get(64), LOWER_PRECEDENCE);


// FP to FP.
PREC_TEST(CompFloatToFloat,   FloatType::get(),  FloatType::get(),  EQUAL);
PREC_TEST(CompFloatToDouble,  FloatType::get(),  DoubleType::get(), LOWER_PRECEDENCE);

PREC_TEST(CompDoubleToDouble, DoubleType::get(), DoubleType::get(), EQUAL);
PREC_TEST(CompDoubleToFloat,  DoubleType::get(), FloatType::get(),  HIGHER_PRECEDENCE);

// Float to Int
PREC_TEST(CompFloatToInt8,  FloatType::get(), IntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompFloatToInt16, FloatType::get(), IntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompFloatToInt32, FloatType::get(), IntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompFloatToInt64, FloatType::get(), IntType::get(64), HIGHER_PRECEDENCE);

// Float to UInt
PREC_TEST(CompFloatToUInt8,  FloatType::get(), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompFloatToUInt16, FloatType::get(), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompFloatToUInt32, FloatType::get(), UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompFloatToUInt64, FloatType::get(), UIntType::get(64), HIGHER_PRECEDENCE);

// Double to Int.
PREC_TEST(CompDoubleToInt8,  DoubleType::get(), IntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompDoubleToInt16, DoubleType::get(), IntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompDoubleToInt32, DoubleType::get(), IntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompDoubleToInt64, DoubleType::get(), IntType::get(64), HIGHER_PRECEDENCE);

// Double to UInt.
PREC_TEST(CompDoubleToUInt8,  DoubleType::get(), UIntType::get(8),  HIGHER_PRECEDENCE);
PREC_TEST(CompDoubleToUInt16, DoubleType::get(), UIntType::get(16), HIGHER_PRECEDENCE);
PREC_TEST(CompDoubleToUInt32, DoubleType::get(), UIntType::get(32), HIGHER_PRECEDENCE);
PREC_TEST(CompDoubleToUInt64, DoubleType::get(), UIntType::get(64), HIGHER_PRECEDENCE);

// Pointer to Int.
PREC_TEST(CompPointerToInt8,  PointerType::get(BoolType::get()), IntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompPointerToInt16, PointerType::get(BoolType::get()), IntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompPointerToInt32, PointerType::get(BoolType::get()), IntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompPointerToInt64, PointerType::get(BoolType::get()), IntType::get(64), LOWER_PRECEDENCE);

// Pointer to UInt.
PREC_TEST(CompPointerToUInt8,  PointerType::get(BoolType::get()), UIntType::get(8),  LOWER_PRECEDENCE);
PREC_TEST(CompPointerToUInt16, PointerType::get(BoolType::get()), UIntType::get(16), LOWER_PRECEDENCE);
PREC_TEST(CompPointerToUInt32, PointerType::get(BoolType::get()), UIntType::get(32), LOWER_PRECEDENCE);
PREC_TEST(CompPointerToUInt64, PointerType::get(BoolType::get()), UIntType::get(64), LOWER_PRECEDENCE);

// Pointer to Pointer.
PREC_TEST(CompBoolPointerToBoolPointer, PointerType::get(BoolType::get()), PointerType::get(BoolType::get()), EQUAL);
PREC_TEST(CompBoolPointerToFloatPointer, PointerType::get(BoolType::get()), PointerType::get(FloatType::get()), INCOMPATIBLE);


RUN_TESTS();