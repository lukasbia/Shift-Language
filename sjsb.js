class ShiftJSBridge {
    constructor() {
        this.wasmInstance = null;
        this.elements = new Map();
    }

    async init(wasmPath) {
        const imports = {
            sjsb: {
                render_element: (idPtr, typePtr, valPtr) => this.renderElement(idPtr, typePtr, valPtr),
                update_element: (idPtr, valPtr) => this.updateElement(idPtr, valPtr)
            }
        };
        const response = await fetch(wasmPath);
        const bytes = await response.arrayBuffer();
        const results = await WebAssembly.instantiate(bytes, imports);
        this.wasmInstance = results.instance;
    }

    renderElement(id, type, value) {
        const el = document.createElement(type);
        el.textContent = value;
        document.body.appendChild(el);
        this.elements.set(id, el);
    }

    updateElement(id, value) {
        if (this.elements.has(id)) {
            this.elements[id].textContent = value;
        }
    }
}

if (typeof module !== 'undefined') {
    module.exports = ShiftJSBridge;
}
