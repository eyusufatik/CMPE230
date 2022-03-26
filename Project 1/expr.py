
vars = ["a", "b", "c", "d"]


def match_var(s):
    return s in vars


def match_num(s):
    try:
        x = int(s)
    except:
        return False

    return True


def match_func(s: str):
    if s.endswith(")") and (s.startswith("tr(") or s.startswith("sqrt(") or s.startswith("choose(") or s.startswith("print(") or s.startswith("printsep(")):
        return True
    else:
        return False


def match_expr_in_paran(s: str):
    if s.startswith("(") and s.endswith(")"):
        return True

def match_expr_op_expr(s: str):
    return "+" in s or "-" in s or "*" in s

def convert_expr(expr: str):
    print("converting: " + expr)
    if match_var(expr):
        return expr
    elif match_num(expr):
        return expr
    elif match_func(expr):  #  return should be based on the func
        return expr[0: expr.find("(") + 1] + convert_expr(expr[expr.find("(") + 1:len(expr)-1]) + ")"
    elif match_expr_in_paran(expr):
        return "(" + convert_expr(expr[1:len(expr)-1]) + ")"
    elif match_expr_op_expr(expr):
        level = 0
        for i in range(0, len(expr)):
            c = expr[i]
            if c == "(":
                level += 1
            elif c == ")":
                level -= 1
            
            if level == 0 and (c == "+" or c == "*" or c == "-"):
                return convert_expr(expr[0:i]) + c + convert_expr(expr[i+1: len(expr)])


while True:
    s = input()
    print(convert_expr(s))
