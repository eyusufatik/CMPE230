
vars = ["a", "b", "c", "d", "A", "B", "C"]
scalars = ["a", "b", "c"]
matrices = ["A", "B", "C"]
vectors = ["d"]


def match_indexed_matrix(expr: str):
    expr = expr.split(" [],\n\t")
    if len(expr) == 3:
        return expr[0] in matrices
    else:
        return False
def match_indexed_vector(expr: str):
    expr = expr.split(" []\n\t")
    if len(expr) == 2:
        return expr[0] in matrices
    else:
        return False

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

def is_func(s: str):
    if s.startswith(("tr(", "sqrt(", "choose(", "print(", "printsep(")):
        level = 0
        for i in range(s.find("("), len(s)):
            c = s[i]
            if c == "(":
                level += 1
            elif c == ")":
                level -= 1
            print(c, level)
            if level == 0 and i != len(s)-1:
                return False
        return True
    return False

def match_choose(expr: str):
    pass

def match_expr_in_paran(s: str):
    if s.startswith("(") and s.endswith(")"):
        return True
    else:
        return False

def match_expr_op_expr(s: str):
    return "+" in s or "-" in s or "*" in s and (not s.startswith("(") or not s.endswith(")"))

def match_m_m_mult(expr: str):
    expr = expr.split("*")
    if len(expr) == 2:
        if expr[0] in matrices and expr[1] in matrices:
            return True
        else:
            return False

def match_m_m_sum(expr: str):
    expr = expr.split("+")
    if len(expr) == 2:
        if expr[0] in matrices and expr[1] in matrices:
            return True
        else:
            return False

def match_m_v_sum(expr: str):
    expr = expr.split("+")
    if len(expr) == 2:
        if (expr[0] in matrices and expr[1] in vectors) or (expr[0] in vectors and expr[1] in matrices):
            return True
        else:
            return False

def match_m_v_mult(expr: str):
    expr = expr.split("*")
    if len(expr) == 2:
        if (expr[0] in matrices and expr[1] in vectors) or (expr[0] in vectors and expr[1] in matrices):
            return True
        else:
            return False

def match_m_s_mult(expr: str):
    expr = expr.split("*")
    if len(expr) == 2:
        print(expr)
        if (expr[0] in matrices and expr[1] in scalars) or (expr[0] in scalars and expr[1] in matrices):
            return True
        else:
            return False
def match_v_v_sum(expr: str):
    pass

def match_v_s_mult(expr: str):
    expr = expr.split("*")
    if len(expr) == 2:
        if (expr[0] in vectors and expr[1] in scalars) or (expr[0] in scalars and expr[1] in vectors):
            return True
        else:
            return False
"""
matrix matrix mul
matrix matrix sum
matrix vector sum
matrix vector mul
matrix scalar mul
vector scalar mul
"""
def convert_expr(expr: str):
    print("converting: " + expr)
    
    if match_m_m_mult(expr):
        expr = expr.split("*")
        return "m_m_mult(" + expr[0] + ", " + expr[1] + ")"
    elif match_m_m_sum(expr):
        expr = expr.split("+")
        return "m_m_sum(" + expr[0] + ", " + expr[1] + ")"
    elif match_m_v_sum(expr):
        expr = expr.split("+")
        if expr[0] in matrices:
            return "m_v_sum(" + expr[0] + ", " + expr[1] + ")"
        else:
            return "m_v_sum(" + expr[1] + ", " + expr[0] + ")"
    elif match_m_v_mult(expr):
        expr = expr.split("*")
        if expr[0] in matrices:
            return "m_v_mult(" + expr[0] + ", " + expr[1] + ")"
        else:
            return "m_v_mult(" + expr[1] + ", " + expr[0] + ")"
    elif match_m_s_mult(expr):
        expr = expr.split("*")
        if expr[0] in matrices:
            return "m_s_mult(" + expr[0] + ", " + expr[1] + ")"
        else:
            return "m_s_mult(" + expr[1] + ", " + expr[0] + ")"
    elif match_v_s_mult(expr):
        expr = expr.split("*")
        if expr[0] in vectors:
            return "m_s_mult(" + expr[0] + ", " + expr[1] + ")"
        else:
            return "m_s_mult(" + expr[1] + ", " + expr[0] + ")"
    elif match_var(expr):
        return expr
    elif match_num(expr):
        return expr
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
    elif match_expr_in_paran(expr):
        return "(" + convert_expr(expr[1:len(expr)-1]) + ")"
    elif match_func(expr):  #  return should be based on the func
        return expr[0: expr.find("(") + 1] + convert_expr(expr[expr.find("(") + 1:len(expr)-1]) + ")"


if __name__ == "__main__":
    while True:
        s = input()
        print(convert_expr(s))  
