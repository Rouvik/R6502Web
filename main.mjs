import r6502Module from "./bin/r6502.mjs";
import asm6Module from "./bin/asm6.mjs";
import MemoryContainer from "./lib/memory_container.mjs";

window.addEventListener('load', main);

let r6502Mod = null;
let r6502CPUPtr = null;

let asm6Mod = null;
let intervalIndex = null;

let editor = null;

const memContainer = new MemoryContainer(document.getElementById("memory-container"));

const cpuState = document.getElementById("cpuState");

const programInput = document.getElementById("programInput");
programInput.addEventListener("input", () => {
    const fr = new FileReader();
    fr.readAsArrayBuffer(programInput.files[0]);

    fr.onload = () => {
        const memPtr = r6502Mod.getMemoryPtr(r6502CPUPtr);
        const heapArr = r6502Mod.HEAPU8.subarray(memPtr, memPtr + 0x10000);
        heapArr.set(new Uint8Array(fr.result), 0x600);

        r6502Mod.R6502.IP = 0x600;
        heapArr[0xFFFA] = 0x11;
        heapArr[0xFFFB] = 0x06;
        heapArr[0xFFFE] = 0x0A;
        heapArr[0xFFFF] = 0x06;

        memContainer.buffer = heapArr;
        memContainer.refreshArrayView();
        printCPUState();
    }
});

document.getElementById("clockBtn").addEventListener("click", () => {
    r6502Mod.clockCPUFromJS(r6502CPUPtr);
    memContainer.refreshArrayView();
    printCPUState();
});

document.getElementById("irqBtn").addEventListener("click", () => {
    r6502Mod.callIRQFromJS(r6502CPUPtr);
});

document.getElementById("nmiBtn").addEventListener("click", () => {
    r6502Mod.callNMIFromJS(r6502CPUPtr);
});

document.getElementById("resetBtn").addEventListener("click", () => {
    r6502Mod.resetCPUFromJS(r6502CPUPtr);
    r6502Mod.R6502.IP = 0x600;
    r6502Mod.R6502.running = true;
    printCPUState();
});

document.getElementById("runBtn").addEventListener("click", () => {
    if (intervalIndex) {
        return;
    }

    intervalIndex = setInterval(() => {
        r6502Mod.clockCPUFromJS(r6502CPUPtr);
        memContainer.refreshArrayView();
        printCPUState();
    }, 50);
});

document.getElementById("stopBtn").addEventListener("click", () => {
    if (!!intervalIndex) {
        clearInterval(intervalIndex);
    }

    intervalIndex = null;
});

function printCPUState() {
    cpuState.value = 
`Total Cycles: ${r6502Mod.R6502.total_cycles}
Ticks: ${r6502Mod.R6502.ticks}
IP: 0x${r6502Mod.R6502.IP.toString(16)} (${r6502Mod.R6502.IP})
Instruction: ${r6502Mod.getInstructionName(r6502Mod.R6502.instr)}
Running: ${r6502Mod.R6502.running}
Stack: 0x${r6502Mod.R6502.reg_Stack.toString(16)} (${r6502Mod.R6502.reg_Stack})
X register: ${r6502Mod.R6502.reg_X}
Y register: ${r6502Mod.R6502.reg_Y}
A register (Accumulator): ${r6502Mod.R6502.reg_Acc}
Status register: ${r6502Mod.R6502.reg_Status.toString(2).padStart(8, '0')}`;
}

document.getElementById("codeCompileBtn").addEventListener("click", () => {
    asm6Mod.FS.writeFile("prog.c", editor.getValue());
    if(asm6Mod.callMain(["prog.c", "prog.bin"]))
    {
        alert("Error failed to compile assembly, please check console.logs for further info on errors, open console with Ctrl + Shift + I");
        return;
    }

    const memPtr = r6502Mod.getMemoryPtr(r6502CPUPtr);
    const heapArr = r6502Mod.HEAPU8.subarray(memPtr, memPtr + 0x10000);
    heapArr.set(new Uint8Array(asm6Mod.FS.readFile("prog.bin")), 0x600);

    r6502Mod.R6502.IP = 0x600;
    heapArr[0xFFFA] = 0x11;
    heapArr[0xFFFB] = 0x06;
    heapArr[0xFFFE] = 0x0A;
    heapArr[0xFFFF] = 0x06;

    memContainer.buffer = heapArr;
    memContainer.refreshArrayView();
    printCPUState();
});

async function main() {
    r6502Mod = await r6502Module();
    r6502CPUPtr = r6502Mod.initialiseCPU();
    asm6Mod = await asm6Module();
        
    editor = ace.edit("editor");
    editor.setTheme("ace/theme/dracula");
    editor.session.setMode("ace/mode/assembly_6502");    
}