import xterm_colors from "./xterm_colors.mjs";

class MemoryError extends Error
{
    constructor(message)
    {
        super(`[Memory Error] ${message}`);
    }
}

export class MemoryCanvas {
    static colors = xterm_colors.colors;

    constructor(canvas, offsetElement, dimXElement, dimYElement, dimentions = {}, offset = 0) {
        this.dimentions = Object.assign({
            rows: 16,
            cols: 16,
            width: 400,
            height: 300
        }, dimentions);

        this.numElements = this.dimentions.rows * this.dimentions.cols;
        this.offset = offset;

        this.offsetElement = offsetElement;
        this.offsetElement.type = "text";
        this.offsetElement.addEventListener("input", this.handleOffsetUpdates.bind(this));
        this.offsetElement.value = `0x${this.offset.toString(16)}`;

        this.dimXElement = dimXElement;
        this.dimXElement.type = "number";
        this.dimXElement.addEventListener("input", this.handleDimXUpdate.bind(this));
        this.dimXElement.value = this.dimentions.cols;

        this.dimYElement = dimYElement;
        this.dimYElement.type = "number";
        this.dimYElement.addEventListener("input", this.handleDimYUpdate.bind(this));
        this.dimYElement.value = this.dimentions.rows;

        this.sc = canvas;
        this.sc.style.imageRendering = "pixelated";
        this.sc.style.width = `${this.dimentions.width}px`;
        this.sc.style.height = `${this.dimentions.height}px`;
        this.sc.width = this.dimentions.cols;
        this.sc.height = this.dimentions.rows;

        this.cxt = this.sc.getContext("2d", { alpha: false });
        this.cxt.imageSmoothingEnabled = false;

        this.memContainer = null;   // must be set by attaching using attachMemContainer
    }

    attachMemContainer(memContainer)
    {
        if (!(memContainer instanceof MemoryContainer)) {
            throw new MemoryError("Error failed to attachMemContainer not instance of MemoryContainer");
        }

        this.memContainer = memContainer;
        memContainer.screen = this;
    }

    handleDimXUpdate()
    {
        this.dimentions.cols = +this.dimXElement.value || 0;
        this.sc.width = this.dimentions.cols;
        this.updateCanvas();
    }

    handleDimYUpdate()
    {
        this.dimentions.rows = +this.dimYElement.value || 0;
        this.sc.height = this.dimentions.rows;
        this.updateCanvas();
    }

    handleOffsetUpdates() {
        this.offset = parseInt(this.offsetElement.value, 16) || 0;
        this.updateCanvas();
    }

    updateCanvas() {
        for (let i = 0; i < this.numElements; i++) {
            const color = MemoryCanvas.colors[this.memContainer.buffer[i + this.offset] || 0];
            this.cxt.fillStyle = color;
            this.cxt.fillRect(i % this.dimentions.cols, i / this.dimentions.cols | 0, 1, 1);
        }
    }

    updateCanvasIndexed(index, value) {
        const ori_index = index - this.offset;
        if (ori_index < 0) return;
        if (ori_index > this.numElements) return;

        this.cxt.fillStyle = MemoryCanvas.colors[value];
        this.cxt.fillRect(ori_index % this.dimentions.cols, ori_index / this.dimentions.cols | 0, 1, 1);
    }

    resetCanvas() {
        this.cxt.clearRect(0, 0, this.sc.width, this.sc.height);
    }
}

export default class MemoryContainer {
    constructor(container, buffer = [], dimentions = {}) {
        this.screen = null; // set by memCanvas when it attaches to it
        this.buffer = buffer;
        this.dimentions = Object.assign({
            rows: 16,
            cols: 16
        }, dimentions);
        this.currentRow = 0;
        this.currentRowOffset = 0;
        this.maxOffset = this.dimentions.rows * this.dimentions.cols;

        // add container and set its CSS class
        this.container = container;
        this.container.classList.add("memory_container");

        this.rowOffsetInput = document.createElement("input");
        this.rowOffsetInput.value = "0x00";
        this.rowOffsetInput.min = 0;
        this.rowOffsetInput.addEventListener("input", () => {
            this.currentRow = parseInt(this.rowOffsetInput.value, 16) || 0;
            this.currentRowOffset = this.currentRow * this.dimentions.cols;
            this.refreshArrayView();
        });

        const rowOffsetSpan = document.createElement("span");
        rowOffsetSpan.textContent = "Set the offset to read rows from: ";
        this.container.appendChild(rowOffsetSpan);
        this.container.appendChild(this.rowOffsetInput);

        // append table in container to display memory stuff
        this.tableContainer = document.createElement("table");
        this.container.appendChild(this.tableContainer);

        // header line to display top memory index line (cols)
        const tableHeader = document.createElement("thead");
        this.tableContainer.appendChild(tableHeader);
        this.tableHeaderRow = document.createElement("tr");
        tableHeader.appendChild(this.tableHeaderRow);

        // add the table header col indexes to show up
        this.tableHeaderRow.appendChild(document.createElement("td"));
        for (let i = 0; i < this.dimentions.cols; i++) {
            const col = document.createElement("td");
            col.textContent = `0x${i.toString(16).toUpperCase()}`;
            this.tableHeaderRow.appendChild(col);
        }

        let elemIndex = 0;
        this.containerElements = [];
        this.rowIndexes = [];
        for (let i = 0; i < this.dimentions.rows; i++) {
            // the row element
            const containerRow = [];
            const tableRow = document.createElement("tr");
            this.tableContainer.appendChild(tableRow);

            // the first index element to display column
            const rowIndex = document.createElement("td");
            rowIndex.textContent = `0x${(i * this.dimentions.cols).toString(16).toUpperCase().padStart(4, '0')}`;
            tableRow.appendChild(rowIndex);
            this.rowIndexes.push(rowIndex);

            // insert input elements
            for (let j = 0; j < this.dimentions.cols; j++) {
                const td = document.createElement("td");
                const input = document.createElement("input");
                input.type = "number";
                input.index = elemIndex++;
                input.disabled = true;
                input.addEventListener("input", this.inputUpdateHandler.bind(this));

                td.appendChild(input);

                containerRow.push(input);
                tableRow.appendChild(td);
            }

            this.containerElements.push(containerRow);
        }
    }

    refreshArrayView() {
        for (let i = 0; i < this.dimentions.rows; i++) {
            this.rowIndexes[i].textContent = `0x${((this.currentRow + i) * this.dimentions.cols).toString(16).toUpperCase().padStart(4, '0')}`;
        }

        let index = 0;
        for (let i = 0; i < this.dimentions.rows; i++) {
            for (let j = 0; j < this.dimentions.cols; j++) {
                const val = this.buffer[this.currentRowOffset + index];
                this.containerElements[i][j].disabled = (val == undefined);
                this.containerElements[i][j].value = val;
                index++;
            }
        }
    }

    inputUpdateHandler(evt) {
        const offset = parseInt(this.rowOffsetInput.value, 16) * this.dimentions.cols;
        this.buffer[offset + evt.target.index] = +evt.target.value;
    }

    updateSpecificViewElement(index, value) {
        this.screen.updateCanvasIndexed(index, value, this);

        const ori_index = index - this.currentRowOffset;

        if (ori_index < 0) return;
        if (ori_index >= this.maxOffset) return;

        const cols = this.dimentions.cols;
        this.containerElements[ori_index / cols | 0][ori_index % cols].value = value;
    }
}