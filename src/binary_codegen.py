from dataclasses import dataclass
from typing import List

traceBinary = True
addressRange = 256

registers = {
    "$zero": "00000",
    "$one":  "00001",
    "$io":   "00010",
    "$rf":   "00011",
    "$aux":  "00100",
    "r5":    "00101",
    "r6":    "00110",
    "r7":    "00111",
    "r8":    "01000",
    "r9":    "01001",
    "r10":   "01010",
    "r11":   "01011",
    "r12":   "01100",
    "r13":   "01101",
    "r14":   "01110",
    "r15":   "01111",
    "r16":   "10000",
    "r17":   "10001",
    "r18":   "10010",
    "r19":   "10011",
    "r20":   "10100",
    "r21":   "10101",
    "r22":   "10110",
    "r23":   "10111",
    "r24":   "11000",
    "r25":   "11001",
    "r26":   "11010",
    "r27":   "11011",
    "$gp":   "11100",
    "$fp":   "11101",
    "$sp":   "11110",
    "$ra":   "11111",
}

@dataclass
class Instruction:
    instr: str
    addr_src: str
    addr_tgt: str
    addr_dst: str

instructions = []

def assemblyTranslate(path: str) -> List[Instruction]:
   global source
   
   with open(path, 'r') as assembly:
        source = next(assembly).strip()
        
        for line in assembly:
            parts = line.strip().split(' ')[1:]
            if len(parts) == 4:
                inst = Instruction(*[p.strip() for p in parts])
                instructions.append(inst)
        return instructions

def valueToBinary(value: str, address: bool) -> str:
    num = int(value)
    
    if num < 0:
        num = (1 << 16) + num
    
    if (address):
        return format(num & 0x3FFFFFF, '026b')
    else:
        return format(num & 0xFFFF, '016b')
   
def binaryCodeGenerate(instructions: List[Instruction]) -> List[str]:
    binary = []

    for inst in instructions:
        instr = inst.instr
        src = inst.addr_src
        tgt = inst.addr_tgt
        dst = inst.addr_dst
        
        match instr:
            # IO-Type
            case "in":
                opcode = "000000"
                funct = "000001"
                binary.append(opcode+"0000000000"+"00010"+"00000"+funct)
            
            case "out":
                opcode = "000000"
                funct = "000010"
                binary.append(opcode+registers[src]+"000000000000000"+funct)
            
            # # R-Type
            case "move":
                opcode = "000000"
                funct = "000011"
                shamt = "00000"
                binary.append(opcode+registers[src]+"00000"+registers[dst]+shamt+funct)
            
            case "add":
                opcode = "000000"
                funct = "000100"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "sub":
                opcode = "000000"
                funct = "000101"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "mul":
                opcode = "000000"
                funct = "000110"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "div":
                opcode = "000000"
                funct = "000111"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            # case "and":
            #     continue
            
            # case "or":
            #     continue
            
            # case "not":
            #     continue
            
            # case "xor":
            #     continue
            
            case "sll":
                opcode = "000000"
                funct = "001100"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "srl":
                opcode = "000000"
                funct = "001101"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "set":
                opcode = "000000"
                funct = "010000"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "sdt":
                opcode = "000000"
                funct = "010001"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "sgt":
                opcode = "000000"
                funct = "010010"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "slt":
                opcode = "000000"
                funct = "010011"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "segt":
                opcode = "000000"
                funct = "010100"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "selt":
                opcode = "000000"
                funct = "010101"
                shamt = "00000"
                binary.append(opcode+registers[src]+registers[tgt]+registers[dst]+shamt+funct)
            
            case "jr":
                opcode = "000000"
                funct = "100000"
                shamt = "00000"
                binary.append(opcode+registers[src]+"00000"+"00000"+shamt+funct)
            
            # # I-Type
            case "load":
                opcode = "000001"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "store":
                opcode = "000010"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "movei":
                opcode = "000011"
                binary.append(opcode+"00000"+registers[dst]+valueToBinary(src, False))
            
            case "addi":
                opcode = "000100"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "subi":
                opcode = "000101"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "muli":
                opcode = "000110"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "divi":
                opcode = "000111"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            # case "andi":
            #     continue
            
            # case "ori":
            #     continue
            
            case "beq":
                opcode = "001100"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            # case "bne":
            #     continue
            
            # case "lui":
            #     continue
            
            case "seti":
                opcode = "010000"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "sdti":
                opcode = "010001"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "sgti":
                opcode = "010010"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "slti":
                opcode = "010011"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "segti":
                opcode = "010100"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            case "selti":
                opcode = "010101"
                binary.append(opcode+registers[src]+registers[tgt]+valueToBinary(dst, False))
            
            # # J-Type
            case "nop":
                binary.append("00000000000000000000000000000000")
            
            case "j":
                opcode = "100000"
                binary.append(opcode+valueToBinary(src, True))
            
            case "jal":
                opcode = "100001"
                binary.append(opcode+valueToBinary(src, True))
            
            case "halt":
                opcode = "100010"
                binary.append(opcode+"00000000000000000000000000")
            
            case _:
                binary.append(f"---UNKNOWN---")
    return binary

def binarySave(path: str, bin: List[str]):
    with open(path, 'w') as output:
        if (traceBinary):
            print("\n> Binary Code Tracing ------------------------------------------------------")
            print("----------------------------------------------------------------------------")
        for index, (bin_line, inst_line) in enumerate(zip(bin, instructions)):
            line = f"{bin_line}\n"
            
            if (traceBinary):
                print(f"    > [{index}] {inst_line.instr} {inst_line.addr_src} {inst_line.addr_tgt} {inst_line.addr_dst}\n        {line}", end = "")
            output.write(line)
        if addressRange > len(bin):
            for index in range(len(bin), addressRange):
                output.write("00000000000000000000000000000000\n")

def main():
    path_assembly = "outputs/assembly.txt"
    path_binary = "outputs/binary.txt"
    
    assembly = assemblyTranslate(path_assembly)
    binary = binaryCodeGenerate(assembly)
    binarySave(path_binary, binary)
    
    print(f"\n> Binary code generated... → [{source}]\n")
    
if __name__ == "__main__":
    main()