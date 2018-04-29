#include <string.h>
#include <stdlib.h>

#include "dberror.h"
#include "record_mgr.h"
#include "expr.h"
#include "tables.h"

// implementations
RC 
valueEquals (Value *left, Value *right, Value *answer)
{
  if(left->dt != right->dt)
    THROW(RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE, "balance examination supported for estimations of the same datatype");

  answer->dt = DT_BOOL;
   
  switch(left->dt) {
  else if(left->dt == DT_INT){
      answer->v.boolV = (left->v.intV == right->v.intV);
  }
  //case DT_INT:
    break;
  else if(left->dt == DT_FLOAT){
      answer->v.boolV = (left->v.floatV == right->v.floatV);
  }
  //case DT_FLOAT:
    break;
 else if(left->dt == DT_BOOL{
      answer->v.boolV = (left->v.boolV == right->v.boolV);
  }
  //case DT_BOOL:
    break;
 else if(left->dt == DT_STRING){
      answer->v.boolV = (strcmp(left->v.stringV, right->v.stringV) == 0);
  }
  //case DT_STRING:
    break;
  }

  return RC_OK;
}

RC 
valueSmaller (Value *left, Value *right, Value *answer)
{
  if(left->dt != right->dt)
    THROW(RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE, "balance examination supported for estimations of the same datatype");

  answer->dt = DT_BOOL;
  
  switch(left->dt) {
  if(left->dt == DT_INT){
      answer->v.boolV = (left->v.intV < right->v.intV);
  }
  //case DT_INT:
    break;
 else if(left->dt == DT_FLOAT){
      answer->v.boolV = (left->v.floatV < right->v.floatV);
  }
  //case DT_FLOAT:
    break;
 else if(left->dt == DT_BOOL){
      answer->v.boolV = (left->v.boolV < right->v.boolV);
  }
  break;
  //case DT_BOOL:
 else if(left->dt == DT_STRING){
      answer->v.boolV = (strcmp(left->v.stringV, right->v.stringV) < 0);
  }
  //case DT_STRING:
    break;
  }

  return RC_OK;
}

RC 
boolNot (Value *input, Value *answer)
{
  if (input->dt != DT_BOOL)
    THROW(RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN, "boolean NOT requires boolean input");
  answer->dt = DT_BOOL;
  answer->v.boolV = !(input->v.boolV);

  return RC_OK;
}

RC
boolAnd (Value *left, Value *right, Value *answer)
{
  if (left->dt != DT_BOOL || right->dt != DT_BOOL)
    THROW(RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN, "boolean AND requires boolean inputs");
  answer->v.boolV = (left->v.boolV && right->v.boolV);

  return RC_OK;
}

RC
boolOr (Value *left, Value *right, Value *answer)
{
  if (left->dt != DT_BOOL || right->dt != DT_BOOL)
    THROW(RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN, "boolean OR requires boolean inputs");
  answer->v.boolV = (left->v.boolV || right->v.boolV);

  return RC_OK;
}

RC
evalExpr (Record *record, Schema *schema, Expr *expr, Value **answer)
{
  Value *lIn;
  Value *rIn;
  MAKE_VALUE(*answer, DT_INT, -1);

  switch(expr->type)
    {
    case EXPR_OP:
      {
      Operator *op = expr->expr.op;
      bool twoArgs = (op->type != OP_BOOL_NOT);
      //      lIn = (Value *) malloc(sizeof(Value));
      //    rIn = (Value *) malloc(sizeof(Value));
      
      CHECK(evalExpr(record, schema, op->args[0], &lIn));
      if (twoArgs)
	CHECK(evalExpr(record, schema, op->args[1], &rIn));

      switch(op->type) 
	{
	case OP_BOOL_NOT:
	  CHECK(boolNot(lIn, *answer));
	  break;
	case OP_BOOL_AND:
	  CHECK(boolAnd(lIn, rIn, *answer));
	  break;
	case OP_BOOL_OR:
	  CHECK(boolOr(lIn, rIn, *answer));
	  break;
	case OP_COMP_EQUAL:
	  CHECK(valueEquals(lIn, rIn, *answer));
	  break;
	case OP_COMP_SMALLER:
	  CHECK(valueSmaller(lIn, rIn, *answer));
	  break;
	default:
	  break;
	}

      // cleanup
      freeVal(lIn);
      if (twoArgs)
	freeVal(rIn);
      }
      break;
    case EXPR_CONST:
      CPVAL(*answer,expr->expr.cons);
      break;
    case EXPR_ATTRREF:
      free(*answer);
      CHECK(getAttr(record, schema, expr->expr.attrRef, answer));
      break;
    }

  return RC_OK;
}

RC
freeExpr (Expr *expr)
{
  switch(expr->type) 
    {
    if(expr->type == EXPR_OP)
    //case EXPR_OP:
      {
      Operator *op = expr->expr.op;
      switch(op->type) 
	{
		case OP_BOOL_NOT:
	  		freeExpr(op->args[0]);
	  	break;
		default:
	  		freeExpr(op->args[0]);
	  		freeExpr(op->args[1]);
	  	break;
	}
      free(op->args);
      }
      break;
   else if(expr->type == EXPR_CONST){
    //case EXPR_CONST:
      freeVal(expr->expr.cons);
      }
      break;
    //case EXPR_ATTRREF:
   else   if(expr->type == EXPR_ATTRREF){
      cout<<" ";
      }
      break;
    }
  free(expr);
  
  return RC_OK;
}

void 
freeVal (Value *val)
{
  if (val->dt == DT_STRING)
    free(val->v.stringV);
  free(val);
}

