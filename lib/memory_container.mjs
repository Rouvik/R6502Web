class MemoryContainerError extends Error {
    constructor(message) {
        super(`[MemoryContainerError] ${message}`);
    }
}

export default class MemoryContainer {
    constructor(container, buffer = [], dimentions = {}) {
        this.buffer = buffer;
        this.dimentions = Object.assign({
            rows: 16,
            cols: 16
        }, dimentions);
        this.currentRow = 0;

        // add container and set its CSS class
        this.container = container;
        this.container.classList.add("memory_container");

        this.rowOffsetInput = document.createElement("input");
        this.rowOffsetInput.value = "0x00";
        this.rowOffsetInput.min = 0;
        this.rowOffsetInput.addEventListener("input", this.refreshArrayView.bind(this));

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
        const rowInputOffsetVal = parseInt(this.rowOffsetInput.value, 16) || 0;
        const offset = rowInputOffsetVal * this.dimentions.cols;
        let index = 0;

        for (let i = 0; i < this.dimentions.rows; i++) {
            this.rowIndexes[i].textContent = `0x${((rowInputOffsetVal + i) * this.dimentions.cols).toString(16).toUpperCase().padStart(4, '0')}`;
        }

        for (let i = 0; i < this.dimentions.rows; i++) {
            for (let j = 0; j < this.dimentions.cols; j++) {
                const val = this.buffer[offset + index];
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
}