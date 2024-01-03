var i = 0; // 设置全局变量,定位目前所显示数据的index

// 读取JSON文件中的索引为index的对象的数据
function readJson(index) {
    var file = document.getElementById("file").files[0];
    var reader = new FileReader();
    reader.readAsText(file);
    reader.onload = function () {
        var itistext = reader.result;
        itisjson = JSON.parse(itistext); // convert it to an object
        var number = JSONLength(itisjson[index].MEM);
        populateData(itisjson[index], number);
    }
}

// 获得JSON元素个数的元素
function JSONLength(obj) {
    var size = 0, key;
    for (key in obj) {
        if (obj.hasOwnProperty(key)) size++;
    }
    return size;
};

// 定位数据
function populateData(obj, number) {
    // 通过id获取元素
    const PC = document.getElementById("PC");
    const STAT = document.getElementById("STAT");
    const ZF = document.getElementById("ZF");
    const SF = document.getElementById("SF");
    const OF = document.getElementById("OF");
    const MEMO0 = document.getElementById("MEM0");
    const MEM8 = document.getElementById("MEM8");
    const MEM16 = document.getElementById("MEM16");
    const MEM24 = document.getElementById("MEM24");
    const MEM32 = document.getElementById("MEM32");
    const MEM40 = document.getElementById("MEM40");
    const MEM48 = document.getElementById("MEM48");
    const MEM56 = document.getElementById("MEM56");
    const RAX = document.getElementById("RAX");
    const RCX = document.getElementById("RCX");
    const RDX = document.getElementById("RDX");
    const RBX = document.getElementById("RBX");
    const RSP = document.getElementById("RSP");
    const RBP = document.getElementById("RBP");
    const RSI = document.getElementById("RSI");
    const RDI = document.getElementById("RDI");
    const R8 = document.getElementById("R8");
    const R9 = document.getElementById("R9");
    const R10 = document.getElementById("R10");
    const R11 = document.getElementById("R11");
    const R12 = document.getElementById("R12");
    const R13 = document.getElementById("R13");
    const R14 = document.getElementById("R14");
    const eviction = document.getElementById("eviction");
    const hit = document.getElementById("hit");
    const miss = document.getElementById("miss");

    eviction.textContent = `${obj.Cache.eviction}`;
    hit.textContent = `${obj.Cache.hit}`;
    miss.textContent = `${obj.Cache.miss}`;
    PC.textContent = `${obj['PC']}`;
    STAT.textContent = `${obj.STAT}`;
    ZF.textContent = `${obj.CC.ZF}`;
    SF.textContent = `${obj.CC.SF}`;
    OF.textContent = `${obj.CC.OF}`;
    MEMO0.textContent = `${obj.MEM['0']}`;
    MEM8.textContent = `${obj.MEM['8']}`;
    MEM16.textContent = `${obj.MEM['16']}`;
    MEM24.textContent = `${obj.MEM['24']}`;
    MEM32.textContent = `${obj.MEM['32']}`;
    MEM40.textContent = `${obj.MEM['40']}`;
    MEM48.textContent = `${obj.MEM['48']}`;
    MEM56.textContent = `${obj.MEM['56']}`;
    RAX.textContent = `${obj.REG.rax}`;
    RCX.textContent = `${obj.REG.rcx}`;
    RDX.textContent = `${obj.REG.rdx}`;
    RBX.textContent = `${obj.REG.rbx}`;
    RSP.textContent = `${obj.REG.rsp}`;
    RBP.textContent = `${obj.REG.rbp}`;
    RSI.textContent = `${obj.REG.rsi}`;
    RDI.textContent = `${obj.REG.rdi}`;
    R8.textContent = `${obj.REG.r8}`;
    R9.textContent = `${obj.REG.r9}`;
    R10.textContent = `${obj.REG.r10}`;
    R11.textContent = `${obj.REG.r11}`;
    R12.textContent = `${obj.REG.r12}`;
    R13.textContent = `${obj.REG.r13}`;
    R14.textContent = `${obj.REG.r14}`;
}

const nextpage = document.getElementById("next");
const lastpage = document.getElementById("last");
const resetdata = document.getElementById("reset");

// 下一页
nextpage.addEventListener('click', () => {
    if (i === 1000) alert("后面的区域之后再探索吧!"); // 已到最后一页
    else {
        i++;
        readJson(i);
    }
});

// 上一页
lastpage.addEventListener('click', () => {
    if (i === 0) alert("前面的区域之后再探索吧!"); // 已到第一页
    else {
        i--;
        readJson(i);
    }
});

// 重置
resetdata.addEventListener('click', () => {
    i = 0; // 重置为第一页
    readJson(i);
});

function searchMEM() {
    var searchInput = document.getElementById("searchInput").value;

    if (!isNaN(searchInput)) {
        var searchNumber = parseFloat(searchInput);
        searchAndDisplayMEM(searchNumber);
    } else {
        alert("Please enter a valid number to search for in MEM.");
    }
}

function searchAndDisplayMEM(searchNumber) {
    var file = document.getElementById("file").files[0];
    var reader = new FileReader();

    reader.readAsText(file);

    reader.onload = function () {
        var jsonData = JSON.parse(reader.result);
        var found = false;

        for (var i = 0; i < jsonData.length; i++) {
            var memValue = jsonData[i].MEM[searchNumber.toString()];

            if (memValue !== undefined) {
                // 使用类似的方式更新元素值
                updateElementValue("MEM" + searchNumber.toString(), memValue);

                // 执行搜索成功时的其他操作
                // additionalActionOnSearchSuccess(jsonData[i]);

                found = true;
                break;
            }
        }

        if (!found) {
            alert("MEM value for " + searchNumber + " is 0(或者没定义).");
        } else {
            alert("MEM value for " + searchNumber + " is " + memValue);
        }
    };
}

// 辅助函数，用于更新元素值
function updateElementValue(elementId, value) {
    var element = document.getElementById(elementId);
    if (element) {
        element.textContent = value;
    } else {
        console.error("Element with ID " + elementId + " not found.");
    }
}

