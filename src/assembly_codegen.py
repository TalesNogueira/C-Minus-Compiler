from dataclasses import dataclass
from typing import List, Optional

traceAssembly = True

@dataclass
class Quadruple:
    op: str
    addr_src: str
    addr_tgt: str
    addr_dst: str

@dataclass
class Instruction:
    instr: str
    addr_src: str
    addr_tgt: str
    addr_dst: str

def traceAssembler(items: List):
    if traceAssembly:
        print("\n> Assembly Code Tracing ----------------------------------------------------")
        print("----------------------------------------------------------------------------")
        for i, item in enumerate(items):
            if isinstance(item, Quadruple):
                print(f"        > Quad. [{i:02}]: ({item.op}, {item.addr_src}, {item.addr_tgt}, {item.addr_dst})")
            elif isinstance(item, Instruction):
                print(f"        > Instr. [{i:02}]: ({item.instr}, {item.addr_src}, {item.addr_tgt}, {item.addr_dst})")


def midcodeTranslate(path: str) -> List[Quadruple]:
    global source
    
    quads = []
    
    with open(path, 'r') as midcode:
        source = next(midcode).strip()
        
        for line in midcode:
            parts = line.strip().split('|')
            if len(parts) == 4:
                quad = Quadruple(*[p.strip() for p in parts])
                quads.append(quad)
    traceAssembler(quads)
    return quads

def assemblyCodeGenerate(quads: List[Quadruple]) -> List[Instruction]:
    global_variables = 0
    global_variable = False
    
    current_function = None
    variable_offsets = {}
    local_offset = 0
    global_offset = 0
    
    labels = {}
    
    registers = []
     
    instructions = []
    
    for quad in quads:
        operator = quad.op.upper()
        src = quad.addr_src
        tgt = quad.addr_tgt
        dst = quad.addr_dst
        
        match operator:
            case "ADD":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("addi", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("addi", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("addi", tgt, dst, src))
                else:
                    instructions.append(Instruction("add", src, tgt, dst))
                    
            case "SUB":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("subi", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("subi", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("subi", tgt, dst, src))
                else:
                    instructions.append(Instruction("sub", src, tgt, dst))
                    
            case "MUL":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("muli", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("muli", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("muli", tgt, dst, src))
                else:
                    instructions.append(Instruction("mul", src, tgt, dst))
                    
            case "DIV":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("divi", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("divi", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("divi", tgt, dst, src))
                else:
                    instructions.append(Instruction("div", src, tgt, dst))
                    
            case "SGT":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("sgti", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("sgti", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("sgti", tgt, dst, src))
                else:
                    instructions.append(Instruction("sgt", src, tgt, dst))
            
            case "SLT":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("slti", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("slti", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("slti", tgt, dst, src))
                else:
                    instructions.append(Instruction("slt", src, tgt, dst))
            
            case "SGET":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("segti", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("segti", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("segti", tgt, dst, src))
                else:
                    instructions.append(Instruction("segt", src, tgt, dst))
            
            case "SLET":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("selti", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("selti", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("selti", tgt, dst, src))
                else:
                    instructions.append(Instruction("selt", src, tgt, dst))
            
            case "SET":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("seti", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("seti", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("seti", tgt, dst, src))
                else:
                    instructions.append(Instruction("set", src, tgt, dst))
            
            case "SDT":
                if (src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("movei", src, "-", dst))
                    instructions.append(Instruction("sdti", dst, dst, tgt))
                elif (not src.isdigit() and tgt.isdigit()):
                    instructions.append(Instruction("sdti", src, dst, tgt))
                elif (src.isdigit() and not tgt.isdigit()):
                    instructions.append(Instruction("sdti", tgt, dst, src))
                else:
                    instructions.append(Instruction("sdt", src, tgt, dst))
        
            case "ALLOCVAR":
                if src == "global":
                    variable_offsets.setdefault("global", {})[tgt] = global_offset
                    global_offset += 1
                    instructions.append(Instruction("addi", "$gp", "$gp", "1"))
                    global_variables += 1
                else:
                    variable_offsets.setdefault(current_function, {})[tgt] = local_offset
                    local_offset += 1
                    instructions.append(Instruction("addi", "$sp", "$sp", "1"))
                    
            case "ALLOCARRAY":
                array_size = int(dst) + 1
                
                if src == "global":
                    variable_offsets.setdefault("global", {})[tgt] = global_offset
                    instructions.append(Instruction("addi", "$gp", "$aux", "1"))
                    instructions.append(Instruction("store", "$gp", "$aux", str(global_offset)))
                    instructions.append(Instruction("addi", "$gp", "$gp", str(array_size)))
                    global_offset += array_size
                    global_variables += 3
                else:
                    variable_offsets.setdefault(current_function, {})[tgt] = local_offset
                    if (dst == "0"):
                        instructions.append(Instruction("addi", "$sp", "$sp", str(array_size)))
                    else: 
                        instructions.append(Instruction("addi", "$fp", "$aux", "1"))
                        instructions.append(Instruction("store", "$fp", "$aux", str(local_offset)))
                        instructions.append(Instruction("addi", "$sp", "$sp", str(array_size)))
                    local_offset += array_size
                
            case "STOREVAR":
                if dst in variable_offsets.get("global", {}):
                    offset = variable_offsets["global"][dst]
                    if (src.isdigit()):
                        instructions.append(Instruction("movei", src, "-", "$aux"))
                        instructions.append(Instruction("store", "$gp", "$aux", str(offset)))
                    else:
                        instructions.append(Instruction("store", "$gp", src, str(offset)))
                    global_variable = False
                else:
                    offset = variable_offsets[tgt][dst]
                    if (src.isdigit()):
                        instructions.append(Instruction("movei", src, "-", "$aux"))
                        instructions.append(Instruction("store", "$fp", "$aux", str(offset)))
                    else:
                        instructions.append(Instruction("store", "$fp", src, str(offset)))
                
            case "STOREARRAY":
                if (global_variable):
                    if (src.isdigit()):
                        instructions.append(Instruction("movei", src, "-", "$aux"))
                        instructions.append(Instruction("store", dst, "$aux", "0"))
                    else:
                        instructions.append(Instruction("store", dst, src, "0"))
                    global_variable = False
                else:
                    if (src.isdigit()):
                        instructions.append(Instruction("movei", src, "-", "$aux"))
                        instructions.append(Instruction("store", dst, "$aux", "0"))
                    else:
                        instructions.append(Instruction("store", dst, src, "0"))
            
            case "LOADVAR":
                if tgt in variable_offsets.get("global", {}):
                    offset = variable_offsets["global"][tgt]
                    instructions.append(Instruction("load", "$gp", dst, str(offset)))
                    global_variable = True
                else:
                    offset = variable_offsets[src][tgt]
                    instructions.append(Instruction("load", "$fp", dst, str(offset)))
            
            case "LOADARRAY":
                if (global_variable):
                    instructions.append(Instruction("load", tgt, dst, "0"))
                    global_variable = True
                else:
                    instructions.append(Instruction("load", tgt, dst, "0"))
        
            case "IFFALSE":
                instructions.append(Instruction("beq", src, "$zero", tgt))
                continue
            
            case "LABEL":
                if "main" in labels:
                    labels[src] = len(instructions)
                else:
                    labels[src] = len(instructions) + 1
        
            case "JUMP":
                instructions.append(Instruction("j", src, "-", "-"))
                
            case "FUNBGN":
                if (src.lower() == "main"):
                    labels[src.lower()] = len(instructions) + 1
                    if (labels[src.lower()] > 1):
                        instructions.insert(global_variables, Instruction("j", labels[src.lower()], "-", "-"))
                    local_offset = 0
                else:
                    labels[src] = len(instructions) + 1
                    instructions.append(Instruction("store", "$fp", "$ra", "1"))
                    instructions.append(Instruction("addi", "$sp", "$sp", "1"))
                    local_offset = 2
    
                current_function = src
                variable_offsets[current_function] = {}
            
            case "FUNEND":
                if (src.lower() != "main"):
                    instructions.append(Instruction("load", "$fp", "$ra", "1"))
                    instructions.append(Instruction("jr", "$ra", "-", "-"))
                
            case "PARAM":
                registers.append(src)
                continue
                
            case "CALL":
                if (src.lower() == "input"):
                    instructions.append(Instruction("in", "-", "-", "r2"))
                elif (src.lower() == "output"):
                    instructions.append(Instruction("out", registers[-1], "-", "-"))  
                else:
                    instructions.append(Instruction("store", "$sp", "$fp", "0"))
                    instructions.append(Instruction("addi", "$sp", "$fp", "0"))
                    instructions.append(Instruction("addi", "$sp", "$sp", "1"))
                    for index, reg in enumerate(registers):
                        instructions.append(Instruction("store", "$fp", reg, str(2+index)))
                    instructions.append(Instruction("jal", src, "-", "-"))
                    instructions.append(Instruction("addi", "$fp", "$sp", "0"))
                    instructions.append(Instruction("load", "$fp", "$fp", "0"))
                    
            case "MOVE" | "RETURN":
                if (operator == "MOVE"):
                    if (src.isdigit()):
                        instructions.append(Instruction("movei", src, "-", tgt))
                    else:
                        instructions.append(Instruction("move", src, "-", tgt))
                
            case "PUSH":
                if (src != "r3"):
                    instructions.append(Instruction("store", "$sp", src, "0"))
                    instructions.append(Instruction("addi", "$sp", "$sp", "1"))
                
            case "POP":
                registers.pop()
                
                if (src != "r3"):
                    instructions.append(Instruction("subi", "$sp", "$sp", "1"))   
                
            case "HALT":
                instructions.append(Instruction("halt", "-", "-", "-"))
                
            case _:
                instructions.append(Instruction("UNKNOWN", "-", "-", "-"))
    
    for instr in instructions:
        if (isinstance(instr, Instruction)):
            if instr.addr_src in labels:
                instr.addr_src = str(labels[instr.addr_src])
            if instr.addr_tgt in labels:
                instr.addr_tgt = str(labels[instr.addr_tgt])
            if instr.addr_dst in labels:
                instr.addr_dst = str(labels[instr.addr_dst])
            
            if (instr.addr_src == "r2"):
                instr.addr_src = "$io"
            if (instr.addr_tgt == "r2"):
                instr.addr_tgt = "$io"
            if (instr.addr_dst == "r2"):
                instr.addr_dst = "$io"
                
            if (instr.addr_src == "r3"):
                instr.addr_src = "$rfA"
            if (instr.addr_tgt == "r3"):
                instr.addr_tgt = "$rfA"
            if (instr.addr_dst == "r3"):
                instr.addr_dst = "$rfA"
                
            if (instr.addr_src == "r4"):
                instr.addr_src = "$rfB"
            if (instr.addr_tgt == "r4"):
                instr.addr_tgt = "$rfB"
            if (instr.addr_dst == "r4"):
                instr.addr_dst = "$rfB"
    
    traceAssembler(instructions)
    return instructions

def assemblySave(path: str, instructions: List[Instruction]):
    with open(path, 'w') as output:
        output.write(source+"\n")
        for index, instr in enumerate(instructions):
            line = f"[{index}] {instr.instr} {instr.addr_src} {instr.addr_tgt} {instr.addr_dst}\n"
            output.write(line)
            
def main():
    path_midcode = "outputs/midcode.txt"
    path_assembly = "outputs/assembly.txt"
    
    quadruples = midcodeTranslate(path_midcode)
    instructions = assemblyCodeGenerate(quadruples)
    assemblySave(path_assembly, instructions)
    
    print(f"\n> Assembly code generated... → [{source}]\n")

if __name__ == "__main__":
    main()