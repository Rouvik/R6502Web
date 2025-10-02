import r6502Module from "./bin/r6502.mjs";
import asm6Module from "./bin/asm6.mjs";
import MemoryContainer from "./lib/memory_container.mjs";

// global state
let r6502Mod = null;
let r6502CPUPtr = null;
let asm6Mod = null;
let intervalIndex = null;
let editor = null;
const memContainer = new MemoryContainer(document.getElementById("memory-container"));
const cpuState = document.getElementById("cpuState");
const programInput = document.getElementById("programInput");
const romStartOffset = document.getElementById("romStartOffset");
const toastCtx = new ToastContext("right");
const clockInterval = document.getElementById("clockInterval");

const cpuStateReg = {
    ip: document.getElementById("ipVal"),
    stack: document.getElementById("stackVal"),
    xreg: document.getElementById("regXVal"),
    yreg: document.getElementById("regYVal"),
    areg: document.getElementById("regAVal"),
    status: document.getElementById("statusVal")
};

// main function as starting point
window.addEventListener('load', main);

// program ROM input handler
programInput.addEventListener("input", () => {
    const fr = new FileReader();
    fr.readAsArrayBuffer(programInput.files[0]);

    fr.onload = () => {
        const memPtr = r6502Mod.getMemoryPtr(r6502CPUPtr);
        const heapArr = r6502Mod.HEAPU8.subarray(memPtr, memPtr + 0x10000);

        if(!setROMToMemory(fr.result, parseInt(romStartOffset.value, 16))) return;
        r6502Mod.R6502.IP = heapArr[0xfffc] | (heapArr[0xfffd] << 8);
        printCPUState();

        toastCtx.displayMessageToast("ROM loaded successfully!", 4);
        toastCtx.displayMessageToast("Make sure IP and offsets are correct!", 4);
    }
});

function setROMToMemory(rom, offset = 0) {
    const memPtr = r6502Mod.getMemoryPtr(r6502CPUPtr);
    const heapArr = r6502Mod.HEAPU8.subarray(memPtr, memPtr + 0x10000);
    try {
        heapArr.set(new Uint8Array(rom), offset);
    } catch(error)
    {
        toastCtx.displayMessageToast(`Failed to write buffer: ${error}\nOffset to write to: ${offset} and size of source: ${rom.length} and destination is only 0xFFFF bytes long`, 10);
        console.error(error);
        return false;
    }

    memContainer.buffer = heapArr;
    memContainer.refreshArrayView();

    return true;
}

// CPU single step button handler
document.getElementById("clockBtn").addEventListener("click", () => {
    r6502Mod.clockCPUFromJS(r6502CPUPtr);
    memContainer.refreshArrayView();
    printCPUState();
});

// Interrupt Button Handler
document.getElementById("irqBtn").addEventListener("click", () => {
    r6502Mod.callIRQFromJS(r6502CPUPtr);
});

// Non Maskable Interrupt Button Handler
document.getElementById("nmiBtn").addEventListener("click", () => {
    r6502Mod.callNMIFromJS(r6502CPUPtr);
});

function stopIntervalLoop() {
    if (!!intervalIndex) {
        clearInterval(intervalIndex);
    }

    intervalIndex = null;
}

// CPU Reset button handler
document.getElementById("resetBtn").addEventListener("click", () => {
    const memPtr = r6502Mod.getMemoryPtr(r6502CPUPtr);
    const heapArr = r6502Mod.HEAPU8.subarray(memPtr, memPtr + 0x10000);

    r6502Mod.resetCPUFromJS(r6502CPUPtr);
    r6502Mod.R6502.IP = heapArr[0xfffc] | (heapArr[0xfffd] << 8);
    r6502Mod.R6502.running = true;
    printCPUState();
    stopIntervalLoop();
});

// CPU run button handler
document.getElementById("runBtn").addEventListener("click", () => {
    if (intervalIndex) {
        return;
    }

    intervalIndex = setInterval(() => {
        r6502Mod.clockCPUFromJS(r6502CPUPtr);
        memContainer.refreshArrayView();
        printCPUState();

        if (!r6502Mod.R6502.running) {
            stopIntervalLoop();
        }
    }, +clockInterval.value);
});

// CPU Stop button handler
document.getElementById("stopBtn").addEventListener("click", stopIntervalLoop);

function printCPUState() {
    cpuState.value = 
`Total Cycles: ${r6502Mod.R6502.total_cycles}
Ticks: ${r6502Mod.R6502.ticks}
Instruction: ${r6502Mod.getInstructionName(r6502Mod.R6502.instr)}
Running: ${r6502Mod.R6502.running}`;

    cpuStateReg.ip.value = `0x${r6502Mod.R6502.IP.toString(16)}`;
    cpuStateReg.stack.value = `0x${r6502Mod.R6502.reg_Stack.toString(16)}`;
    cpuStateReg.xreg.value = r6502Mod.R6502.reg_X;
    cpuStateReg.yreg.value = r6502Mod.R6502.reg_Y;
    cpuStateReg.areg.value = r6502Mod.R6502.reg_Acc;
    cpuStateReg.status.value = r6502Mod.R6502.reg_Status.toString(2).padStart(8, '0');
}

// listeners for CPU State
cpuStateReg.ip.addEventListener("input", () => {
    r6502Mod.R6502.IP = parseInt(cpuStateReg.ip.value, 16) || 0;
});

cpuStateReg.stack.addEventListener("input", () => {
    r6502Mod.R6502.reg_Stack = parseInt(cpuStateReg.stack.value, 16) || 0;
});

cpuStateReg.xreg.addEventListener("input", () => {
    r6502Mod.R6502.reg_X = +cpuStateReg.xreg.value || 0;
});

cpuStateReg.yreg.addEventListener("input", () => {
    r6502Mod.R6502.reg_Y = +cpuStateReg.yreg.value || 0;
});

cpuStateReg.areg.addEventListener("input", () => {
    r6502Mod.R6502.reg_Acc = +cpuStateReg.areg.value || 0;
});

cpuStateReg.status.addEventListener("input", () => {
    r6502Mod.R6502.reg_Status = parseInt(cpuStateReg.status.value, 2) || 0;
});

// assemble and copy code to buffer button handler
document.getElementById("codeCompileBtn").addEventListener("click", async () => {
    asm6Mod.FS.writeFile("prog.c", editor.getValue());
    if(asm6Mod.callMain(["prog.c", "prog.bin"]))
    {
        toastCtx.displayMessageToast("Error failed to compile assembly, please check console.logs for further info on errors, open console with Ctrl + Shift + I", 10);
        asm6Mod = await asm6Module();   // refresh the module to reset error flags
        return;
    }

    const memPtr = r6502Mod.getMemoryPtr(r6502CPUPtr);
    const heapArr = r6502Mod.HEAPU8.subarray(memPtr, memPtr + 0x10000);

    if(!setROMToMemory(asm6Mod.FS.readFile("prog.bin"), parseInt(romStartOffset.value, 16))) return;
    r6502Mod.R6502.IP = heapArr[0xfffc] | (heapArr[0xfffd] << 8);
    printCPUState();

    toastCtx.displayMessageToast("Successfully assembled and loaded ROM!", 4);
    toastCtx.displayMessageToast("Make sure IP and offsets are correct!", 4);
});

// main driver code
async function main() {
    r6502Mod = await r6502Module();
    r6502CPUPtr = r6502Mod.initialiseCPU();
    asm6Mod = await asm6Module();
        
    editor = ace.edit("editor");
    editor.setTheme("ace/theme/dracula");
    editor.session.setMode("ace/mode/assembly_6502");
}