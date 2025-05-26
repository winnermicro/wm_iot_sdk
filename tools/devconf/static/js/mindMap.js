class MindMap {
    constructor(containerId) {
        this.container = document.getElementById(containerId);
        this.container.style.position = 'relative';
        this.canvas = document.createElement('canvas');
        this.ctx = this.canvas.getContext('2d', { alpha: true, antialias: true });
        this.nodes = [];
        this.connections = [];
        this.dropdowns = {};
        this.nodeTypes = {};
        
        this.init();
        this.createClockTree();
    }

    init() {
        this.canvasWidth = this.container.clientWidth;
        this.canvasHeight = this.container.clientHeight;
        this.resizeCanvas();
        this.container.style.position = 'relative';
        this.canvas.style.position = 'absolute';
        this.canvas.style.top = '0';
        this.canvas.style.left = '0';
        this.canvas.style.zIndex = '1';
        this.container.appendChild(this.canvas);
        
        window.addEventListener('resize', () => {
            this.canvasWidth = this.container.clientWidth;
            this.canvasHeight = this.container.clientHeight;
            this.resizeCanvas();
            this.createClockTree();
        });
    }

    resizeCanvas() {
        const dpr = window.devicePixelRatio || 1;
        const rect = this.container.getBoundingClientRect();
        
        this.canvas.style.width = `${rect.width}px`;
        this.canvas.style.height = `${rect.height}px`;
        
        this.canvas.width = rect.width * dpr;
        this.canvas.height = rect.height * dpr;
        
        this.ctx.scale(dpr, dpr);
        
        this.canvasWidth = rect.width;
        this.canvasHeight = rect.height;
    }

    createDropdown(node, options) {
        const select = document.createElement('select');
        select.style.position = 'absolute';
        select.style.left = `${node.x}px`;
        select.style.top = `${node.y}px`;
        select.style.width = `${node.width}px`;
        select.style.height = `${node.height}px`;
        select.style.backgroundColor = 'rgb(209, 233, 255)';
        select.style.border = '1px solid #dee2e6';
        select.style.borderRadius = '8px';
        select.style.color = '#495057';
        select.style.fontSize = '12px';
        select.style.padding = '0 10px';
        select.style.zIndex = '1000';
        select.style.cursor = 'pointer';
        select.style.outline = 'none';
        select.style.appearance = 'none';
        select.style.webkitAppearance = 'none';
        select.style.mozAppearance = 'none';
        select.style.backgroundImage = 'url("/static/images/caret-down.png")';
        select.style.backgroundRepeat = 'no-repeat';
        select.style.backgroundPosition = 'right 8px center';
        select.style.backgroundSize = '10px';
        select.style.paddingRight = '30px';

        if (node.type === 'div3') {
            select.id = 'clk_div_cpu';
            select.name = 'clk_div_cpu';
            select.setAttribute('data-change-name', 'CPU Freq:');
        } else if (node.type === 'div4') {
            select.id = 'clk_div_wlan';
            select.name = 'clk_div_wlan';
            select.setAttribute('data-change-name', 'WiFi Freq:');
        }

        options.forEach(option => {
            const opt = document.createElement('option');
            opt.value = option;
            opt.text = option;
            if (option === node.text) {
                opt.selected = true;
            }
            select.appendChild(opt);
        });

        select.addEventListener('change', (e) => {
            node.text = e.target.value;
            const divider = parseInt(e.target.value.replace('1/', ''));
            const newFreq = Math.floor(480 / divider);

            if (node.options.includes(clockConfig.dividers.div3.options[0])) {
                this.nodes.forEach(n => {
                    if (n.text === clockConfig.terminalNodes.cpu.text || n.text === clockConfig.terminalNodes.dma.text) {
                        n.frequency = `${newFreq}MHz`;
                    }
                });
            } else if (node.options.includes(clockConfig.dividers.div4.options[0])) {
                this.nodes.forEach(n => {
                    if (n.text === clockConfig.terminalNodes.wifi.text) {
                        n.frequency = `${newFreq}MHz`;
                    } else if ([clockConfig.terminalNodes.sdio_slave.text, 
                                clockConfig.terminalNodes.high_spi_slave.text, 
                                clockConfig.terminalNodes.i2c.text, 
                                clockConfig.terminalNodes.spi.text, 
                                clockConfig.terminalNodes.uart.text, 
                                clockConfig.terminalNodes.card_7816.text, 
                                clockConfig.terminalNodes.gpio.text, 
                                clockConfig.terminalNodes.wdt.text, 
                                clockConfig.terminalNodes.timer.text].includes(n.text)) {
                        var newFreq2 = newFreq / parseInt(clockConfig.middleNodes.div4Second.text.replace('1/', ''));
                        if (newFreq2 >= 1) {
                            n.frequency = `${newFreq2}MHz`;
                        } else {
                            newFreq2 = newFreq2 * 1000;
                            n.frequency = `${newFreq2}KHz`;
                        }
                    }
                });
            }

            this.render();
        });

        this.container.appendChild(select);
        this.dropdowns[node.id] = select;
    }

    clearDropdowns() {
        Object.values(this.dropdowns).forEach(select => {
            select.remove();
        });
        this.dropdowns = {};
    }

    createClockTree() {
        this.clearDropdowns();
        this.nodes = [];
        this.connections = [];

        const totalWidth = 1400;
        const totalHeight = 1000;
        const scaleX = this.canvasWidth / totalWidth;
        const scaleY = this.canvasHeight / totalHeight;
        const scale = Math.min(scaleX, scaleY, 1);

        const startX = (this.canvasWidth - totalWidth * scale) / 2 + 0 * scale;
        const startY = (this.canvasHeight - totalHeight * scale) / 2 + 483 * scale;

        const scalePos = (x, y) => ({
            x: startX + x * scale,
            y: startY + y * scale
        });

        const terminalX = 1100;
        const verticalSpacing = 50 * scale;
        const terminalNodes = Object.values(clockConfig.terminalNodes);
        const totalNodesHeight = verticalSpacing * (terminalNodes.length - 1);
        const terminalStartY = -totalNodesHeight / 2;
        const terminalNodeIds = {};
        const terminalNodeObjs = [];
        
        this.nodeTypes = {
            pll: 'pll',
            div3: 'div3',
            div4: 'div4',
            div3Second: 'div3Second',
            div3Sub1: 'div3Sub1',
            div3Sub2: 'div3Sub2',
            div3Sub3: 'div3Sub3',
            div1First: 'div1First',
            div4Second: 'div4Second',
            div40: 'div40',
            empty1: 'empty1',
            apb: 'apb'
        };
        
        terminalNodes.forEach((node, index) => {
            const pos = scalePos(terminalX, terminalStartY / scale + index * (verticalSpacing / scale));
            const nodeId = Date.now() + Math.random();
            terminalNodeIds[node.text] = nodeId;
            terminalNodeObjs.push({
                id: nodeId,
                text: node.text,
                type: node.text,
                x: pos.x,
                y: pos.y - (5 * scale),
                width: 190 * scale,
                height: 40 * scale,
                parentId: null,
                frequency: node.freq,
                isSpecial: node.special
            });
        });

        const nodeMap = {};
        const columnX = {
            pll: 0,
            div: 350 * scale,
            mid: 700 * scale,
            apb: 900 * scale,
            terminal: 1100 * scale
        };
        const pllPos = scalePos(columnX.pll, 0);
        const pllId = Date.now() + Math.random();
        nodeMap[this.nodeTypes.pll] = { 
            id: pllId, 
            text: clockConfig.pll.text,
            type: this.nodeTypes.pll,
            x: pllPos.x, 
            y: pllPos.y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        const div3Id = Date.now() + Math.random();
        const div4Id = Date.now() + Math.random();
        const div3SecondId = Date.now() + Math.random();
        const div3Pos = scalePos(columnX.div, 0);
        const div4Pos = scalePos(columnX.div, 100);
        nodeMap[this.nodeTypes.div3] = { 
            id: div3Id, 
            text: clockConfig.dividers.div3.text,
            type: this.nodeTypes.div3,
            x: div3Pos.x, 
            y: div3Pos.y - (5 * scale), 
            width: 120 * scale,
            height: 40 * scale,
            isDropdown: true,
            options: clockConfig.dividers.div3.options
        };
        nodeMap[this.nodeTypes.div4] = { 
            id: div4Id, 
            text: clockConfig.dividers.div4.text,
            type: this.nodeTypes.div4,
            x: div4Pos.x, 
            y: div4Pos.y - (5 * scale), 
            width: 120 * scale,
            height: 40 * scale,
            isDropdown: true,
            options: clockConfig.dividers.div4.options
        };
        nodeMap[this.nodeTypes.div3Second] = { 
            id: div3SecondId, 
            text: clockConfig.dividers.div3Second.text, 
            x: scalePos(columnX.div, 250).x, 
            y: scalePos(columnX.div, 250).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        const div3Sub1Id = Date.now() + Math.random();
        const div3Sub2Id = Date.now() + Math.random();
        const div3Sub3Id = Date.now() + Math.random();
        nodeMap[this.nodeTypes.div3Sub1] = { 
            id: div3Sub1Id, 
            text: clockConfig.middleNodes.div3Sub1.text, 
            x: scalePos(columnX.mid, 200).x, 
            y: scalePos(columnX.mid, 200).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        nodeMap[this.nodeTypes.div3Sub2] = { 
            id: div3Sub2Id, 
            text: clockConfig.middleNodes.div3Sub2.text, 
            x: scalePos(columnX.mid, 250).x, 
            y: scalePos(columnX.mid, 250).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        nodeMap[this.nodeTypes.div3Sub3] = { 
            id: div3Sub3Id, 
            text: clockConfig.middleNodes.div3Sub3.text, 
            x: scalePos(columnX.mid, 300).x, 
            y: scalePos(columnX.mid, 300).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        const div1FirstId = Date.now() + Math.random();
        const div4SecondId = Date.now() + Math.random();
        nodeMap[this.nodeTypes.div1First] = { 
            id: div1FirstId, 
            text: clockConfig.middleNodes.div1First.text, 
            x: scalePos(columnX.mid, 50).x, 
            y: scalePos(columnX.mid, 50).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        nodeMap[this.nodeTypes.div4Second] = { 
            id: div4SecondId, 
            text: clockConfig.middleNodes.div4Second.text, 
            x: scalePos(columnX.mid, 100).x, 
            y: scalePos(columnX.mid, 100).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };
        
        const div40Id = Date.now() + Math.random();
        nodeMap[this.nodeTypes.div40] = { 
            id: div40Id, 
            text: clockConfig.middleNodes.div40.text, 
            x: scalePos(900, 330).x,
            y: scalePos(700, 330).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };

        const empty1Id = Date.now() + Math.random();
        nodeMap[this.nodeTypes.empty1] = { 
            id: empty1Id, 
            text: clockConfig.middleNodes.empty1.text, 
            x: scalePos(850, 250).x, 
            y: scalePos(700, 250).y - (5 * scale), 
            width: 0 * scale, 
            height: 40 * scale 
        };

        const apbId = Date.now() + Math.random();
        nodeMap[this.nodeTypes.apb] = { 
            id: apbId, 
            text: clockConfig.middleNodes.apb.text, 
            x: scalePos(columnX.apb, 100).x, 
            y: scalePos(columnX.apb, 100).y - (5 * scale), 
            width: 90 * scale, 
            height: 40 * scale 
        };

        terminalNodeObjs.forEach(n => {
            if (n.type === clockConfig.terminalNodes.cpu.text || n.type === clockConfig.terminalNodes.dma.text) {
                n.parentId = div3Id;
            }
            else if (n.type === clockConfig.terminalNodes.rsa.text || 
                     n.type === clockConfig.terminalNodes.gpsec.text || 
                     n.type === clockConfig.terminalNodes.i2s.text) {
                n.parentId = div3Sub1Id;
            }
            else if (n.type === clockConfig.terminalNodes.wifi.text) {
                n.parentId = div1FirstId;
            }
            else if (n.type === clockConfig.terminalNodes.qflash.text) {
                n.parentId = div3Sub2Id;
            }
            else if (n.type === clockConfig.terminalNodes.touch_sensor.text || 
                     n.type === clockConfig.terminalNodes.pwm.text) {
                n.parentId = empty1Id;
            }
            else if (n.type === clockConfig.terminalNodes.adc.text) {
                n.parentId = div40Id;
            }
            else {
                n.parentId = apbId;
            }
        });
        
        nodeMap[this.nodeTypes.div3].parentId = pllId;
        nodeMap[this.nodeTypes.div4].parentId = pllId;
        nodeMap[this.nodeTypes.div3Second].parentId = pllId;
        nodeMap[this.nodeTypes.div3Sub1].parentId = div3SecondId;
        nodeMap[this.nodeTypes.div3Sub2].parentId = div3SecondId;
        nodeMap[this.nodeTypes.div3Sub3].parentId = div3SecondId;
        nodeMap[this.nodeTypes.div1First].parentId = div4Id;
        nodeMap[this.nodeTypes.div4Second].parentId = div4Id;
        nodeMap[this.nodeTypes.apb].parentId = div4SecondId;
        nodeMap[this.nodeTypes.div40].parentId = div3Sub3Id;
        nodeMap[this.nodeTypes.empty1].parentId = div3Sub3Id;

        const allNodes = [
            ...Object.values(nodeMap),
            ...terminalNodeObjs
        ];
        function adjustY(node, visited = new Set()) {
            if (visited.has(node.id)) {
                return node.y + node.height / 2;
            }
            visited.add(node.id);
            
            const children = allNodes.filter(n => n.parentId === node.id);
            if (children.length === 0) return node.y + node.height / 2;
            
            const childCenters = children.map(child => adjustY(child, visited));
            
            const centerY = (Math.min(...childCenters) + Math.max(...childCenters)) / 2;
            node.y = centerY - node.height / 2;
            return node.y + node.height / 2;
        }

        function adjustY2(node, visited = new Set()) {
            if (visited.has(node.id)) {
                return node.y + node.height / 2;
            }
            visited.add(node.id);
            
            const children = allNodes.filter(n => n.parentId === node.id);
            if (children.length === 0) return node.y + node.height / 2;
            
            const childCenters = children.map(child => adjustY(child, visited));
            
            const centerY = (Math.min(...childCenters) + Math.max(...childCenters)) / 2;
            node.y = centerY - node.height / 1.54;


            return node.y + node.height / 2;
            
        }
        adjustY(nodeMap[this.nodeTypes.pll]);
        adjustY2(nodeMap[this.nodeTypes.div3Second]);

        const wifiNode = terminalNodeObjs.find(n => n.type === clockConfig.terminalNodes.wifi.text);
        const wifi1_1Node = Object.values(nodeMap).find(n => n.id === div1FirstId);
        if (wifiNode && wifi1_1Node) {
            wifi1_1Node.y = wifiNode.y;
        }

        this.nodes = allNodes;
        this.connections = [];
        allNodes.forEach(n => {
            if (n.parentId) {
                this.connections.push({ from: n.parentId, to: n.id });
            }
        });

        this.render();
        
        this.nodes.forEach(node => {
            if (node.isDropdown) {
                this.createDropdown(node, node.options);
            }
        });
    }

    addNode(text, x, y, parentId = null, frequency = '', scale = 1, isSpecial = false) {
        const extraWidth = 30 * scale;
        const nodeId = Date.now() + Math.random();
        const node = {
            id: nodeId,
            text,
            x,
            y,
            width: (text.length * 10 + 40 + extraWidth) * scale,
            height: 30 * scale,
            parentId,
            frequency,
            isSpecial
        };

        this.nodes.push(node);

        if (parentId) {
            this.connections.push({
                from: parentId,
                to: nodeId
            });
        }

        return nodeId;
    }

    render() {
        const dpr = window.devicePixelRatio || 1;
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        
        this.ctx.imageSmoothingEnabled = true;
        this.ctx.imageSmoothingQuality = 'high';
        this.ctx.lineJoin = 'round';
        this.ctx.lineCap = 'round';
        
        const parentToChildren = {};
        this.connections.forEach(conn => {
            if (!parentToChildren[conn.from]) parentToChildren[conn.from] = [];
            parentToChildren[conn.from].push(conn.to);
        });

        const standardHorizontalGap = 60;

        Object.keys(parentToChildren).forEach(parentId => {
            const childrenIds = parentToChildren[parentId];
            if (childrenIds.length <= 1) return;
            const parentNode = this.nodes.find(n => n.id === Number(parentId));
            const children = childrenIds.map(cid => this.nodes.find(n => n.id === cid));
            
            if (parentNode.type === this.nodeTypes.div3Sub3) {
                const startX = parentNode.x + parentNode.width;
                const startY = parentNode.y + parentNode.height / 2;
                const leftmostChildX = Math.min(...children.map(n => n.x));
                
                const horizontalLineLength = 50;
                const midX = leftmostChildX - standardHorizontalGap;
                const branchStartX = startX + horizontalLineLength;
                
                this.ctx.beginPath();
                this.ctx.moveTo(startX, startY);
                this.ctx.lineTo(branchStartX, startY);
                this.ctx.strokeStyle = '#ffffff';
                this.ctx.lineWidth = 2;
                this.ctx.stroke();
                
                const minY = Math.min(...children.map(n => n.y + n.height / 2));
                const maxY = Math.max(...children.map(n => n.y + n.height / 2));
                
                this.ctx.beginPath();
                this.ctx.moveTo(branchStartX, minY);
                this.ctx.lineTo(branchStartX, maxY);
                this.ctx.stroke();
                
                children.forEach(child => {
                    const endY = child.y + child.height / 2;
                    this.ctx.beginPath();
                    this.ctx.moveTo(branchStartX, endY);
                    this.ctx.lineTo(child.x, endY);
                    this.ctx.stroke();
                    
                    if (child.text) {
                        this.drawArrow(child.x, endY);
                    }
                });
                return;
            }
            
            const minY = Math.min(...children.map(n => n.y + n.height / 2));
            const maxY = Math.max(...children.map(n => n.y + n.height / 2));
            const startX = parentNode.x + parentNode.width;
            const startY = parentNode.y + parentNode.height / 2;

            const leftmostChildX = Math.min(...children.map(n => n.x));
            const midX = leftmostChildX - standardHorizontalGap;

            this.ctx.beginPath();
            this.ctx.moveTo(startX, startY);
            this.ctx.lineTo(midX, startY);
            this.ctx.strokeStyle = '#fff';
            this.ctx.lineWidth = 2;
            this.ctx.stroke();

            if (Math.abs(maxY - minY) > 1) {
                this.ctx.beginPath();
                this.ctx.moveTo(midX, minY);
                this.ctx.lineTo(midX, maxY);
                this.ctx.stroke();
            }

            children.forEach(child => {
                const endY = child.y + child.height / 2;
                this.ctx.beginPath();
                this.ctx.moveTo(midX, endY);
                this.ctx.lineTo(child.x, endY);
                this.ctx.stroke();
                
                if (child.text) {
                    this.drawArrow(child.x, endY);
                }
            });
        });

        this.connections.forEach(conn => {
            const fromNode = this.nodes.find(n => n.id === conn.from);
            const toNode = this.nodes.find(n => n.id === conn.to);
            if (fromNode && toNode) {
                const siblings = this.connections.filter(c => c.from === conn.from);
                if (siblings.length <= 1) {
                    this.drawSingleConnection(fromNode, toNode, standardHorizontalGap);
                }
            }
        });

        this.nodes.forEach(node => {
            this.drawNode(node);
        });
    }

    drawNode(node) {
        if (node.isDropdown) {
            return;
        }
        
        if (!node.text) {
            return;
        }
        if (node.isSpecial) {
            this.ctx.fillStyle = 'rgb(255, 241, 184)';
        } else {
            this.ctx.fillStyle = 'rgb(209, 233, 255)';
        }
        
        this.ctx.strokeStyle = '#dee2e6';
        this.ctx.lineWidth = 1;
        
        const radius = 8;
        this.roundRect(node.x, node.y, node.width, node.height, radius);
        this.ctx.fill();
        this.ctx.stroke();

        this.ctx.fillStyle = '#495057';
        const fontSize = Math.max(11 * (node.height / 30), 11);
        this.ctx.font = `${fontSize}px Arial, sans-serif`;
        this.ctx.textAlign = 'center';
        this.ctx.textBaseline = 'middle';
        this.ctx.letterSpacing = '1px';
        
        const textX = node.x + node.width / 2;
        const textY = node.y + node.height / 2;
        
        this.ctx.fillText(node.text, textX, textY + fontSize * 0.1);

        if (node.frequency) {
            const freqFontSize = Math.max(14 * (node.height / 30), 14);
            this.ctx.font = `${freqFontSize}px Arial, sans-serif`;
            this.ctx.letterSpacing = '1px';
            this.ctx.textAlign = 'left';
            const freqX = node.x + node.width + 10;
            const freqY = node.y + node.height / 2;

            if (node.type === clockConfig.terminalNodes.wifi.text) {
                const freqWidth = this.ctx.measureText(node.frequency).width;
                const padding = 5;
                
                const currentFreq = parseInt(node.frequency);
                if (currentFreq < 160) {
                    this.ctx.save();
                    this.ctx.fillStyle = '#ff4d4f';
                    this.ctx.beginPath();
                    this.roundRect(freqX - padding, freqY - freqFontSize/2 - padding, freqWidth + padding*2, freqFontSize + padding*2, 4);
                    this.ctx.fill();
                    this.ctx.restore();

                    this.ctx.beginPath();
                    this.ctx.moveTo(freqX, freqY);
                    this.ctx.lineTo(freqX + freqWidth, freqY);
                    this.ctx.strokeStyle = '#ffffff';
                    this.ctx.lineWidth = 1;
                    this.ctx.stroke();
                }
                
                this.ctx.fillStyle = '#ffffff';
                this.ctx.fillText(node.frequency, freqX, freqY);
            } else {
                this.ctx.fillStyle = '#ffffff';
                this.ctx.fillText(node.frequency, freqX, freqY);
            }
        }
    }

    drawConnection(fromNode, toNode) {
        const startX = fromNode.x + fromNode.width;
        const startY = fromNode.y + fromNode.height / 2;
        const endX = toNode.x;
        const endY = toNode.y + fromNode.height / 2;

        const horizontalLineX = endX - 30;
        
        this.ctx.beginPath();
        this.ctx.moveTo(startX, startY);
        
        this.ctx.lineTo(horizontalLineX, startY);
        
        this.ctx.lineTo(horizontalLineX, endY);
        
        this.ctx.lineTo(endX, endY);

        this.ctx.strokeStyle = '#fff';
        this.ctx.lineWidth = 2;
        this.ctx.stroke();

        const arrowSize = 6;
        const angle = Math.PI / 6;

        this.ctx.beginPath();
        this.ctx.moveTo(endX, endY);
        this.ctx.lineTo(endX - arrowSize * Math.cos(angle), endY - arrowSize * Math.sin(angle));
        this.ctx.lineTo(endX - arrowSize * Math.cos(angle), endY + arrowSize * Math.sin(angle));
        this.ctx.closePath();

        this.ctx.fillStyle = '#fff';
        this.ctx.fill();
    }

    roundRect(x, y, width, height, radius) {
        this.ctx.beginPath();
        this.ctx.moveTo(x + radius, y);
        this.ctx.arcTo(x + width, y, x + width, y + height, radius);
        this.ctx.arcTo(x + width, y + height, x, y + height, radius);
        this.ctx.arcTo(x, y + height, x, y, radius);
        this.ctx.arcTo(x, y, x + width, y, radius);
        this.ctx.closePath();
    }

    drawArrow(x, y) {
        const arrowSize = 6;
        const angle = Math.PI / 6;

        this.ctx.beginPath();
        this.ctx.moveTo(x, y);
        this.ctx.lineTo(x - arrowSize * Math.cos(angle), y - arrowSize * Math.sin(angle));
        this.ctx.lineTo(x - arrowSize * Math.cos(angle), y + arrowSize * Math.sin(angle));
        this.ctx.closePath();
        this.ctx.fillStyle = '#fff';
        this.ctx.fill();
    }

    drawSingleConnection(fromNode, toNode, standardHorizontalGap) {
        const startX = fromNode.x + fromNode.width;
        const startY = fromNode.y + fromNode.height / 2;
        const endX = toNode.x;
        const endY = toNode.y + toNode.height / 2;
        
        const midX = endX - standardHorizontalGap;
        
        this.ctx.beginPath();
        this.ctx.moveTo(startX, startY);
        this.ctx.lineTo(midX, startY);
        this.ctx.lineTo(midX, endY);
        this.ctx.lineTo(endX, endY);
        
        this.ctx.strokeStyle = '#fff';
        this.ctx.lineWidth = 2;
        this.ctx.stroke();

        if (toNode.text) {
            this.drawArrow(endX, endY);
        }
    }
} 