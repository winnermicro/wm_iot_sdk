const mapLis = document.querySelectorAll('.device-item-header');
const mapImages = document.querySelectorAll('.sub-items');
const arrows = document.querySelectorAll('.device-item-header svg');

const changeToggle = document.getElementById('changeToggle');
const changesDropdown = document.getElementById('changesDropdown');
const closeDropdown = document.getElementById('closeDropdown');

document.addEventListener('DOMContentLoaded', () => {
    const pinmuxWarning = document.querySelector('.pinmux-warning');
    const pinmuxWarn = document.querySelector('.pinmux—warn');
    const closeHide = document.getElementById('close-hide');
    
    if(pinmuxWarning && pinmuxWarn) {
        pinmuxWarning.addEventListener('click', () => {
            pinmuxWarn.classList.add('active');
        });
    }
    
    if(closeHide && pinmuxWarn) {
        closeHide.addEventListener('click', () => {
            pinmuxWarn.classList.remove('active');
        });
    }
    
    const toggleControls = document.querySelectorAll('.toggle-control input[type="checkbox"]');
    toggleControls.forEach(toggle => {
        const parentControl = toggle.closest('.property-control');
        if (parentControl) {
            const propertyList = parentControl.nextElementSibling;
            if (propertyList && propertyList.classList.contains('property-list')) {
                if (!toggle.checked) {
                    disablePropertyList(propertyList);
                }
            }
        }
        
        toggle.addEventListener('change', function() {
            const parentControl = this.closest('.property-control');
            if (parentControl) {
                const propertyList = parentControl.nextElementSibling;
                if (propertyList && propertyList.classList.contains('property-list')) {
                    if (this.checked) {
                        enablePropertyList(propertyList);
                    } else {
                        disablePropertyList(propertyList);
                    }
                }
            }
        });
    });
});

function disablePropertyList(propertyList) {
    const selects = propertyList.querySelectorAll('select');
    selects.forEach(select => {
        select.disabled = true;
        select.style.cursor = 'not-allowed';
    });
}

function enablePropertyList(propertyList) {
    const selects = propertyList.querySelectorAll('select');
    selects.forEach(select => {
        select.disabled = false;
        select.style.cursor = 'pointer';
    });
}

if(closeDropdown) {
    closeDropdown.addEventListener('click', () => {
        const warningPanel = document.querySelector('.pinmux—warn');
        if(warningPanel) {
            warningPanel.style.display = 'none';
        }
    });
}

if(changeToggle && changesDropdown) {
    changeToggle.addEventListener('click', () => {
        let changes = '';
        const changedElements = findChangedElements();
        changedElements.forEach(changed => {
            changes += '<div class="change-item"><span class="change-item-label">' + changed.itemName + '</span><span class="change-item-old">' + changed.initialValue + '</span><span class="change-item-arrow">&#8594;</span><span class="change-item-new">' + changed.currentValue + '</span></div>';
        });
        const div = document.querySelector('.changes-content');
        div.innerHTML = changes;
        ZhenshangyinAnimation(changesDropdown).fadeIn();
    });

    closeDropdown.addEventListener('click', () => {
        ZhenshangyinAnimation(changesDropdown).fadeOut();
    });

    document.addEventListener('click', (event) => {
        if (!changesDropdown.contains(event.target) && event.target !== changeToggle) {
            ZhenshangyinAnimation(changesDropdown).fadeOut();
        }
    });
}

function showEndDevices(index) {
    if ((index + 1) == mapImages.length) {
        setTimeout(() => {
            const scrollElement = document.querySelector('.devices-panel-header');
            const maxScrollTop = scrollElement.scrollHeight - scrollElement.clientHeight;
            scrollElement.scrollTo({ top: maxScrollTop, behavior: 'smooth' });
          }, 300);
    }
}

mapImages.forEach((img, index) => {
    if (img.id && window.GLOBAL_CONFIG.wm_devName.startsWith(img.id)) {
        ZhenshangyinAnimation(img).slideDown();
        arrows[index].style.transform = 'rotate(180deg)';
        document.querySelector("#" + img.id).scrollIntoView();
        showEndDevices(index);
    } else {
        ZhenshangyinAnimation(img).slideUp();
        arrows[index].style.transform = 'rotate(0deg)';
    }
});

mapLis.forEach((li, index) => {
    li.addEventListener('click', () => {
        mapImages.forEach((img, i) => {
            if (i === index) {
                if (img.style.display === 'none' || img.style.display === '') {
                    ZhenshangyinAnimation(img).slideDown();
                    arrows[i].style.transform = 'rotate(180deg)';
                    showEndDevices(index);
                } else {
                    ZhenshangyinAnimation(img).slideUp();
                    arrows[i].style.transform = 'rotate(0deg)';
                }
            } else {
                ZhenshangyinAnimation(img).slideUp();
                arrows[i].style.transform = 'rotate(0deg)';
            }
        });
    });
});


let mindMap;

document.addEventListener('DOMContentLoaded', () => {
    mindMap = new MindMap('configDiagram');
});

function createMindMapForDevice(deviceName, subItems) {
    if (!mindMap) return;
    
    mindMap = new MindMap('configDiagram');
    
    const rootX = mindMap.canvas.width / 2 - 60;
    const rootY = 50;
    const rootId = mindMap.addNode(deviceName, rootX, rootY);
    
    const numChildren = subItems.length;
    const spacing = mindMap.canvas.height / (numChildren + 1);
    
    subItems.forEach((item, index) => {
        const childX = rootX + 200;
        const childY = spacing * (index + 1) - 20;
        mindMap.addNode(item, childX, childY, rootId);
    });
}


function togglePriorityInput() {
    var select = document.getElementById('initlevel');
    var input = document.getElementById('initpriority');
    if (select.value === "0") {
        input.disabled = true;
        input.style.backgroundColor = "rgb(217, 217, 217)";
    } else {
        input.disabled = false;
        input.style.backgroundColor = "rgb(230, 247, 255)";
    }
}
document.addEventListener('DOMContentLoaded', togglePriorityInput);


function copyText() {
    const text = document.getElementById('device_name').value;
    navigator.clipboard.writeText(text).then(() => {
        new ZhenshangyinMessage({
            message: 'Copy successfully',
            type: 'success',
            duration: 2000
        });
    }).catch(err => {
        new ZhenshangyinMessage({
            message: 'Copy failed',
            type: 'error',
            duration: 2000
        });
    });
}

class CustomTooltip {
    constructor() {
        this.tooltip = document.createElement('div');
        this.tooltip.className = 'custom-tooltip';
        document.body.appendChild(this.tooltip);
    }

    show(text, x, y) {
        this.tooltip.textContent = text;
        this.tooltip.style.left = x + 12 + 'px';
        this.tooltip.style.top = y + 12 + 'px';
        this.tooltip.classList.add('show');
    }

    hide() {
        this.tooltip.classList.remove('show');
    }
}

document.addEventListener('DOMContentLoaded', () => {
    const tooltip = new CustomTooltip();
    document.querySelectorAll('.device-select p , .device-placeholder p').forEach(p => {
        p.addEventListener('mouseenter', e => {
            tooltip.show(p.textContent, e.clientX, e.clientY);
        });
        p.addEventListener('mousemove', e => {
            tooltip.show(p.textContent, e.clientX, e.clientY);
        });
        p.addEventListener('mouseleave', () => {
            tooltip.hide();
        });
    });
});